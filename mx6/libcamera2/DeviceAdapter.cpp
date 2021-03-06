/*
 * Copyright (C) 2012-2013 Freescale Semiconductor, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <linux/videodev2.h>
#include <linux/mxc_v4l2.h>
#include "DeviceAdapter.h"
#include "UvcDevice.h"
#include "Ov5640Mipi.h"
#include "Ov5642Csi.h"
#include "Ov5640Csi.h"
#include "TVINDevice.h"
#include "mt9p111Csi.h"
#include "Hm5065Csi.h"

#define NO_TRANSITION                   (0)
#define HAL_AFSTATE_INACTIVE            (1)
#define HAL_AFSTATE_NEEDS_COMMAND       (2)
#define HAL_AFSTATE_STARTED             (3)
#define HAL_AFSTATE_SCANNING            (4)
#define HAL_AFSTATE_LOCKED              (5)
#define HAL_AFSTATE_FAILED              (6)
#define HAL_AFSTATE_NEEDS_DETERMINATION (7)
#define HAL_AFSTATE_PASSIVE_FOCUSED     (8)

void DeviceAdapter::OnAfEvent(int state){
	sp<CameraEvent> cameraEvt = new CameraEvent();
    cameraEvt->mEventType = CameraEvent::EVENT_FOCUS;
		cameraEvt->mData = (void*)state;
    dispatchEvent(cameraEvt);
	
}
void DeviceAdapter::OnAfTriggerAutoMacro(int id)
{
    int nextState = NO_TRANSITION;
	

    switch (m_afState) {
    case HAL_AFSTATE_INACTIVE:
    case HAL_AFSTATE_PASSIVE_FOCUSED:
    case HAL_AFSTATE_SCANNING:
        nextState = HAL_AFSTATE_NEEDS_COMMAND;
        m_IsAfTriggerRequired = true;
        break;
    case HAL_AFSTATE_NEEDS_COMMAND:
        nextState = NO_TRANSITION;
        break;
    case HAL_AFSTATE_STARTED:
        nextState = NO_TRANSITION;
        break;
    case HAL_AFSTATE_LOCKED:
        nextState = HAL_AFSTATE_NEEDS_COMMAND;
        m_IsAfTriggerRequired = true;
        break;
    case HAL_AFSTATE_FAILED:
        nextState = HAL_AFSTATE_NEEDS_COMMAND;
        m_IsAfTriggerRequired = true;
        break;
    default:
        break;
    }
    ALOGV("(%s): State (%d) -> (%d)", __FUNCTION__, m_afState, nextState);
    if (nextState != NO_TRANSITION)
        m_afState = nextState;
}

sp<DeviceAdapter>DeviceAdapter::Create(const CameraInfo& info)
{
    sp<DeviceAdapter> devAdapter;
    if (strstr(info.name, UVC_SENSOR_NAME)) {
        FLOGI("DeviceAdapter: Create uvc device");
        devAdapter = new UvcDevice();
    }
    else if (strstr(info.name, OV5640MIPI_SENSOR_NAME)) {
        FLOGI("DeviceAdapter: Create ov5640 mipi device");
        devAdapter = new Ov5640Mipi();
    }
    else if (strstr(info.name, OV5642CSI_SENSOR_NAME)) {
        FLOGI("DeviceAdapter: Create ov5642 csi device");
        devAdapter = new Ov5642Csi();
    }
    else if (strstr(info.name, OV5640CSI_SENSOR_NAME)) {
        FLOGI("DeviceAdapter: Create ov5640 csi device");
        devAdapter = new Ov5640Csi();
    }
    else if (strstr(info.name, ADV7180_TVIN_NAME)) {
        FLOGI("DeviceAdapter: Create adv7180 device");
        devAdapter = new TVINDevice();
    }
    else if (strstr(info.name, MT9P111_SENSOR_NAME)) {//add by allenyao
        FLOGI("DeviceAdapter: Create mt9p111_camera device");
        devAdapter = new mt9p111Csi();
    }else if (strstr(info.name, HM5065_SENSOR_NAME)) {
        FLOGI("DeviceAdapter: Create hm5065_camera device");
        devAdapter = new Hm5065Csi();
    }else {
        devAdapter = new OvDevice();
        FLOGE("sensor %s does not support well now!", info.name);
    }

    return devAdapter;
}

DeviceAdapter::DeviceAdapter()
    : mCameraHandle(-1), mQueued(0),m_afState(HAL_AFSTATE_INACTIVE)
{}

DeviceAdapter::~DeviceAdapter()
{
    // Close the camera handle and free the video info structure
    if (mCameraHandle > 0) {
        close(mCameraHandle);
        mCameraHandle = -1;
    }

    if (mVideoInfo) {
        delete mVideoInfo;
        mVideoInfo = NULL;
    }
}

void DeviceAdapter::setMetadaManager(sp<MetadaManager> &metadaManager)
{
    mMetadaManager = metadaManager;
}

PixelFormat DeviceAdapter::getMatchFormat(int *sfmt, int  slen,
                                         int *dfmt, int  dlen)
{
    if ((sfmt == NULL) || (slen == 0) || (dfmt == NULL) || (dlen == 0)) {
        FLOGE("getMatchFormat invalid parameters");
        return 0;
    }

    PixelFormat matchFormat = 0;
    bool live = true;
    for (int i = 0; i < slen && live; i++) {
        for (int j = 0; j < dlen; j++) {
            if (sfmt[i] == dfmt[j]) {
                matchFormat = dfmt[j];
                live        = false;
                break;
            }
        }
    }

    return matchFormat;
}

void DeviceAdapter::setPreviewPixelFormat()
{
    int vpuFormats[MAX_VPU_SUPPORT_FORMAT];
    memset(vpuFormats, 0, sizeof(vpuFormats));
    int ret = mMetadaManager->getSupportedRecordingFormat(&vpuFormats[0],
                                    MAX_VPU_SUPPORT_FORMAT);
    if (ret != NO_ERROR) {
        FLOGE("getSupportedRecordingFormat failed");
        mPreviewPixelFormat = HAL_PIXEL_FORMAT_YCbCr_420_SP;
        return;
    }

    mPreviewPixelFormat = getMatchFormat(vpuFormats, MAX_VPU_SUPPORT_FORMAT,
                          mAvailableFormats, MAX_SENSOR_FORMAT);
	if(mPreviewPixelFormat==0){ /* Ellie Cao:no matching format found, use input format */
		FLOGW("no matching preview format found,use default");
		mPreviewPixelFormat=mAvailableFormats[0];
	}
}

void DeviceAdapter::setPicturePixelFormat()
{
    int picFormats[MAX_PICTURE_SUPPORT_FORMAT];
    memset(picFormats, 0, sizeof(picFormats));
    int ret = mMetadaManager->getSupportedPictureFormat(picFormats,
                                MAX_PICTURE_SUPPORT_FORMAT);
    if (ret != NO_ERROR) {
        FLOGE("getSupportedPictureFormat failed");
        mPicturePixelFormat = HAL_PIXEL_FORMAT_YCbCr_420_SP;
        return;
    }
    mPicturePixelFormat = getMatchFormat(picFormats, MAX_PICTURE_SUPPORT_FORMAT,
                            mAvailableFormats, MAX_SENSOR_FORMAT);
	if(mPicturePixelFormat==0){ /* Ellie Cao:no matching format found, use input format */
		FLOGW("no matching picture format found,use default");		
		mPicturePixelFormat=mAvailableFormats[0];
	}
}

status_t DeviceAdapter::initialize(const CameraInfo& info)
{
    if (info.name == NULL) {
        FLOGE("invalid camera sensor name in initialize");
        return BAD_VALUE;
    }
    if (info.devPath == NULL) {
        FLOGE("invalid camera devpath in initialize");
        return BAD_VALUE;
    }

    if (info.devPath[0] != '\0') {
        mCameraHandle = open(info.devPath, O_RDWR);
    }
    if (mCameraHandle < 0) {
        memset((void*)info.devPath, 0, sizeof(info.devPath));
        GetDevPath(info.name, (char*)info.devPath, CAMAERA_FILENAME_LENGTH);
        if (info.devPath[0] != '\0') {
            mCameraHandle = open(info.devPath, O_RDWR);
            if (mCameraHandle < 0) {
                FLOGE("can not open camera devpath:%s", info.devPath);
                return BAD_VALUE;
            }
        }
        else {
            FLOGE("can not open camera devpath:%s", info.devPath);
            return BAD_VALUE;
        }
    }
    mVideoInfo = new VideoInfo();
    if (mVideoInfo == NULL) {
        close(mCameraHandle);
        FLOGE("new VideoInfo failed");
        return NO_MEMORY;
    }

    int ret = NO_ERROR;
    ret = ioctl(mCameraHandle, VIDIOC_QUERYCAP, &mVideoInfo->cap);
    if (ret < 0) {
        close(mCameraHandle);
        delete mVideoInfo;
        FLOGE("query v4l2 capability failed");
        return BAD_VALUE;
    }
    if ((mVideoInfo->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0)
    {
        close(mCameraHandle);
        delete mVideoInfo;
        FLOGE("v4l2 capability does not support capture");
        return BAD_VALUE;
    }

    initSensorInfo(info);
    setPreviewPixelFormat();
    setPicturePixelFormat();
    // Initialize flags
    mPreviewing            = false;
    mVideoInfo->isStreamOn = false;
    mImageCapture          = false;

    // Ellie added for flash, autofocus and exposure compensation
    mFlashOn = false;
    mAutoFocusing = false;
    mSingleFlashing = false;
    nExpComp = 0;
    return NO_ERROR;
}

status_t DeviceAdapter::setDeviceConfig(int         width,
                                        int         height,
                                        PixelFormat format,
                                        int         fps)
{
    if (mCameraHandle <= 0) {
        FLOGE("setDeviceConfig: DeviceAdapter uninitialized");
        return BAD_VALUE;
    }
    if ((width == 0) || (height == 0)) {
        FLOGE("setDeviceConfig: invalid parameters");
        return BAD_VALUE;
    }

    status_t ret = NO_ERROR;
    int input    = 1;
    ret = ioctl(mCameraHandle, VIDIOC_S_INPUT, &input);
    if (ret < 0) {
        FLOGE("Open: VIDIOC_S_INPUT Failed: %s", strerror(errno));
        return ret;
    }

    int vformat;
    vformat = convertPixelFormatToV4L2Format(format);

    if ((width > 1920) || (height > 1080)) {
        fps = 15;
    }
    FLOGI("Width * Height %d x %d format 0x%x, fps: %d",
          width, height, vformat, fps);

    mVideoInfo->width       = width;
    mVideoInfo->height      = height;
    mVideoInfo->framesizeIn = (width * height << 1);
    mVideoInfo->formatIn    = vformat;

    struct v4l2_streamparm param;
    memset(&param, 0, sizeof(param));
    param.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    param.parm.capture.timeperframe.numerator   = 1;
    param.parm.capture.timeperframe.denominator = fps;
    param.parm.capture.capturemode = getCaptureMode(width, height);
    ret = ioctl(mCameraHandle, VIDIOC_S_PARM, &param);
    if (ret < 0) {
        FLOGE("Open: VIDIOC_S_PARM Failed: %s", strerror(errno));
        return ret;
    }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type                 = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width        = width & 0xFFFFFFF8;
    fmt.fmt.pix.height       = height & 0xFFFFFFF8;
    fmt.fmt.pix.pixelformat  = vformat;
    fmt.fmt.pix.priv         = 0;
    fmt.fmt.pix.sizeimage    = 0;
    fmt.fmt.pix.bytesperline = 0;

    // Special stride alignment for YU12
    if (vformat == v4l2_fourcc('Y', 'U', '1', '2')){
        // Goolge define the the stride and c_stride for YUV420 format
        // y_size = stride * height
        // c_stride = ALIGN(stride/2, 16)
        // c_size = c_stride * height/2
        // size = y_size + c_size * 2
        // cr_offset = y_size
        // cb_offset = y_size + c_size
        // int stride = (width+15)/16*16;
        // int c_stride = (stride/2+16)/16*16;
        // y_size = stride * height
        // c_stride = ALIGN(stride/2, 16)
        // c_size = c_stride * height/2
        // size = y_size + c_size * 2
        // cr_offset = y_size
        // cb_offset = y_size + c_size

        // GPU and IPU take below stride calculation
        // GPU has the Y stride to be 32 alignment, and UV stride to be
        // 16 alignment.
        // IPU have the Y stride to be 2x of the UV stride alignment
        int stride = (width+31)/32*32;
        int c_stride = (stride/2+15)/16*16;
        fmt.fmt.pix.bytesperline = stride;
        fmt.fmt.pix.sizeimage    = stride*height+c_stride * height;
        FLOGI("Special handling for YV12 on Stride %d, size %d",
            fmt.fmt.pix.bytesperline,
            fmt.fmt.pix.sizeimage);
    }

    ret = ioctl(mCameraHandle, VIDIOC_S_FMT, &fmt);
    if (ret < 0) {
        FLOGE("Open: VIDIOC_S_FMT Failed: %s", strerror(errno));
        return ret;
    }

    return ret;
}

int DeviceAdapter::getFrameSize()
{
    return mBufferSize;
}

int DeviceAdapter::getFrameCount()
{
    return mBufferCount;
}

status_t DeviceAdapter::registerCameraBuffers(CameraFrame *pBuffer,
                                             int        & num)
{
    status_t ret = NO_ERROR;

    if ((pBuffer == NULL) || (num <= 0)) {
        FLOGE("requestCameraBuffers invalid pBuffer");
        return BAD_VALUE;
    }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof (req));
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;
    req.count  = num;

    ret = ioctl(mCameraHandle, VIDIOC_REQBUFS, &req);
    if (ret < 0) {
        FLOGE("VIDIOC_REQBUFS failed: %s", strerror(errno));
        return ret;
    }

    struct v4l2_buffer buf;
    for (int i = 0; i < num; i++) {
        CameraFrame *buffer = pBuffer + i;
        memset(&buf, 0, sizeof (buf));
        buf.index    = i;
        buf.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory   = V4L2_MEMORY_USERPTR;
        buf.m.offset = buffer->mPhyAddr;
        buf.length   = buffer->mSize;

        ret = ioctl(mCameraHandle, VIDIOC_QUERYBUF, &buf);
        if (ret < 0) {
            FLOGE("Unable to query buffer (%s)", strerror(errno));
            return ret;
        }

        // Associate each Camera buffer
        buffer->setObserver(this);
        mDeviceBufs[i] = buffer;
    }

    mBufferSize  = pBuffer->mSize;
    mBufferCount = num;

    return ret;
}

status_t DeviceAdapter::fillCameraFrame(CameraFrame *frame)
{
    status_t ret = NO_ERROR;

    if (!mVideoInfo->isStreamOn) {
        return NO_ERROR;
    }

    int i = frame->mIndex;
    if (i < 0) {
        return BAD_VALUE;
    }

    struct v4l2_buffer cfilledbuffer;
    memset(&cfilledbuffer, 0, sizeof (struct v4l2_buffer));
    cfilledbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    cfilledbuffer.memory = V4L2_MEMORY_USERPTR;
    cfilledbuffer.index    = i;
    cfilledbuffer.m.offset = frame->mPhyAddr;

    ret = ioctl(mCameraHandle, VIDIOC_QBUF, &cfilledbuffer);
    if (ret < 0) {
        FLOGE("fillCameraFrame: VIDIOC_QBUF Failed");
        return BAD_VALUE;
    }
    android_atomic_inc(&mQueued);

    return ret;
}

status_t DeviceAdapter::startDeviceLocked()
{
    status_t ret = NO_ERROR;
    

    int state;
    struct v4l2_buffer buf;
    for (int i = 0; i < mBufferCount; i++) {
        CameraFrame* frame = mDeviceBufs[i];
        state = frame->getState();
        if (state != CameraFrame::BUFS_FREE) {
            continue;
        }
        frame->setState(CameraFrame::BUFS_IN_DRIVER);

        memset(&buf, 0, sizeof (struct v4l2_buffer));
        buf.index    = i;
        buf.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory   = V4L2_MEMORY_USERPTR;
        buf.m.offset = frame->mPhyAddr;
        ret = ioctl(mCameraHandle, VIDIOC_QBUF, &buf);
        if (ret < 0) {
            FLOGE("VIDIOC_QBUF Failed");
            return BAD_VALUE;
        }

        android_atomic_inc(&mQueued);
    }

    enum v4l2_buf_type bufType;
    if (!mVideoInfo->isStreamOn) {
        bufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ret = ioctl(mCameraHandle, VIDIOC_STREAMON, &bufType);
        if (ret < 0) {
            FLOGE("VIDIOC_STREAMON failed: %s", strerror(errno));
            return ret;
        }

        mVideoInfo->isStreamOn = true;
    }

    mDeviceThread = new DeviceThread(this);

    FLOGI("Created device thread");
    return ret;
}

status_t DeviceAdapter::stopDeviceLocked()
{
    status_t ret = NO_ERROR;
    enum v4l2_buf_type bufType;

    mDeviceThread->requestExitAndWait();
    mDeviceThread.clear();

    if (mVideoInfo->isStreamOn) {
        bufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        ret = ioctl(mCameraHandle, VIDIOC_STREAMOFF, &bufType);
        if (ret < 0) {
            close(mCameraHandle);
            FLOGE("StopStreaming: Unable to stop capture: %s", strerror(errno));
            return ret;
        }

        mVideoInfo->isStreamOn = false;
    }

    mQueued   = 0;
    memset(mDeviceBufs, 0, sizeof(mDeviceBufs));

    return ret;
}

status_t DeviceAdapter::startPreview()
{
    status_t ret = NO_ERROR;

    if (mPreviewing) {
        FLOGE("DeviceAdapter: startPreview but preview running");
        return BAD_VALUE;
    }

    Mutex::Autolock lock(mBufsLock);
    mPreviewing = true;
    ret = startDeviceLocked();

    return ret;
}

status_t DeviceAdapter::stopPreview()
{
    int ret = NO_ERROR;

    if (!mPreviewing) {
        FLOGE("DeviceAdapter: stopPreview but preview not running");
        return NO_INIT;
    }

    Mutex::Autolock lock(mBufsLock);
    mPreviewing = false;
    ret         = stopDeviceLocked();

    return ret;
}

status_t DeviceAdapter::startImageCapture()
{
    status_t ret = NO_ERROR;

    if (mImageCapture) {
        FLOGE("DeviceAdapter: startPreview but preview running");
        return BAD_VALUE;
    }

    fAssert(mBufferProvider != NULL);

    Mutex::Autolock lock(mBufsLock);
    mImageCapture = true;
    ret           = startDeviceLocked();

    return ret;
}

status_t DeviceAdapter::stopImageCapture()
{
    int ret = NO_ERROR;

    if (!mImageCapture) {
        FLOGE("DeviceAdapter: stopPreview but preview not running");
        return NO_INIT;
    }

    Mutex::Autolock lock(mBufsLock);
    mImageCapture = false;
    ret           = stopDeviceLocked();

    // Ellie: turn off flash after taking picture if it's on
    if(mSingleFlashing)
    {
        cancelFlash();
        mSingleFlashing=false;
    }

    return ret;
}

CameraFrame * DeviceAdapter::acquireCameraFrame()
{
    int ret;

    struct v4l2_buffer cfilledbuffer;
    memset(&cfilledbuffer, 0, sizeof (cfilledbuffer));
    cfilledbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    cfilledbuffer.memory = V4L2_MEMORY_USERPTR;

    /* DQ */
    ret = ioctl(mCameraHandle, VIDIOC_DQBUF, &cfilledbuffer);
    if (ret < 0) {
        FLOGE("GetFrame: VIDIOC_DQBUF Failed");
        return NULL;
    }
    android_atomic_dec(&mQueued);

    int index = cfilledbuffer.index;
    fAssert(index >= 0 && index < mBufferCount);
    mDeviceBufs[index]->mTimeStamp = systemTime();

    return mDeviceBufs[index];
}

status_t DeviceAdapter::setFlash(bool on)
{
    int ret;
    struct v4l2_control ctrl;
	if(mFlashOn!=on)
		mFlashOn = on;

    ctrl.id = V4L2_CID_MXC_FLASH;
    ctrl.value = mFlashOn?1:0;
    ret = ioctl(mCameraHandle, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0) {
        FLOGE("updateAutoExposure: VIDIOC_S_CTRL Failed: %s", strerror(errno));
        return ret;
    }
    return NO_ERROR;
}


// Ellie added
status_t DeviceAdapter::updateAutoExposure()
{
    int32_t exp_comp;
    int ret;
    struct v4l2_control ctrl;

    ret = mMetadaManager->getExpComp(&exp_comp);
    if (ret != NO_ERROR) {
        FLOGE("%s: getExpComp failed", __FUNCTION__);
        return BAD_VALUE;
    }
    if(nExpComp==exp_comp)
        return NO_ERROR;

    ctrl.id = V4L2_CID_EXPOSURE;
    ctrl.value = exp_comp;
    ret = ioctl(mCameraHandle, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0) {
        FLOGE("updatePQParam: VIDIOC_S_CTRL Failed: %s", strerror(errno));
        return ret;
    }
    nExpComp = exp_comp;
    return NO_ERROR;
}

int DeviceAdapter::deviceThread()
{
    CameraFrame *frame = NULL;
    int res;

    if (mQueued <= 0) {
        FLOGI("no buffer in v4l2, continue");
        usleep(10000); //sleep 10ms
        return NO_ERROR;
    }
    if (!mPreviewing && !mImageCapture) {
        FLOGI("device stop, deviceThread exit");
        return BAD_VALUE;
    }

	// Ellie added: turn on/off flash according to the mode set
    updateAutoExposure();

    frame = acquireCameraFrame();
    if (!frame) {
        FLOGE("device thread exit with frame = null, %d buffers still in v4l",
              mQueued);
        if (mErrorListener != NULL) {
            mErrorListener->handleError(CAMERA2_MSG_ERROR_DEVICE);
        }
        return BAD_VALUE;
    }

    if (mImageCapture) {
        sp<CameraEvent> cameraEvt = new CameraEvent();
        cameraEvt->mEventType = CameraEvent::EVENT_SHUTTER;
        dispatchEvent(cameraEvt);

        frame->mFrameType = CameraFrame::IMAGE_FRAME;
    }
    else {
        frame->mFrameType = CameraFrame::PREVIEW_FRAME;
    }

    dispatchCameraFrame(frame);

    if (!mImageCapture && !mPreviewing) {
        FLOGI("device thread exit...");		
        return ALREADY_EXISTS;
    }

    return NO_ERROR;
}

status_t DeviceAdapter::autoFlash(){
    if (mAutoFlashThread != NULL) {
		mAutoFlashThread->requestExitAndWait();
        mAutoFlashThread.clear();
    }

    mAutoFlashThread = new AutoFlashThread(this);
    if (mAutoFlashThread == NULL) {
        return UNKNOWN_ERROR;
    }
    return NO_ERROR;
}
status_t DeviceAdapter::cancelFlash(){
	mCancelFlash = true;
    return NO_ERROR;
}

// Ellie implemented: turn on flash before starting autofocus if the mode is "single"
status_t DeviceAdapter::autoFocus()
{
	mAutoFocusing = true;
    if (mAutoFocusThread != NULL) {
		mAutoFocusThread->requestExitAndWait();
		mAutoFocusThread.clear();
    }

    mAutoFocusThread = new AutoFocusThread(this);
    if (mAutoFocusThread == NULL) {
        return UNKNOWN_ERROR;
    }
    return NO_ERROR;
}

// Ellie implemented: set a flag so that autoFocusThread will exit
status_t DeviceAdapter::cancelAutoFocus()
{
	Mutex::Autolock lock(mFocusLock);

	mAutoFocusing = false;
	
	struct v4l2_control ctrl;	
	ctrl.id = V4L2_CID_MXC_AUTOFOCUS;
	ctrl.value = 0;
	ioctl(mCameraHandle, VIDIOC_S_CTRL, &ctrl);


	return NO_ERROR;
}

int DeviceAdapter::autoFlashThread(){	
	DurationTimer duration;	
	duration.start();

	setFlash(true);

	do {
		usleep(500000);
		duration.stop();
	}while((true!=mCancelFlash)&&(ns2ms(us2ns(duration.durationUsecs()))<10000));

	setFlash(false);
	
	if(true==mCancelFlash)
		mCancelFlash = false;
	

	return UNKNOWN_ERROR;
	
}

// Ellie implemented: keep checking the focus state, when autofocus finishes,either success or fail,send notification and turn off flash if the mode is "single"
int DeviceAdapter::autoFocusThread()
{
	Mutex::Autolock lock(mFocusLock);

	int ret;
	int success=0;
	DurationTimer duration;
	struct v4l2_control ctrl;
	uint8_t fl_mode;
	bool bFlash4Focus=false;

	ret = mMetadaManager->getFlashMode(&fl_mode);
	if(fl_mode==ANDROID_FLASH_SINGLE)
	{
		setFlash(true);
		if(!mSingleFlashing)
			bFlash4Focus=true;
	}


	OnAfEvent(ANDROID_CONTROL_AF_STATE_ACTIVE_SCAN);

	ctrl.id = V4L2_CID_MXC_AUTOFOCUS;
	ctrl.value = 1;
	duration.start();
	ret = ioctl(mCameraHandle, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0) {
		FLOGE("autoFocusThread: VIDIOC_S_CTRL Failed: %s", strerror(errno));
	}

	do {
		usleep(500000);
		ctrl.id = V4L2_CID_MXC_AUTOFOCUS;
		ctrl.value = 0;
		ret = ioctl(mCameraHandle, VIDIOC_G_CTRL, &ctrl);
		if (ret < 0) {
			FLOGE("autoFocusThread: VIDIOC_G_CTRL Failed: %s", strerror(errno));
		}
		else if(ctrl.value){
				success++;
				break;
		}

		duration.stop();
	}while(!success&&mAutoFocusing&&duration.durationUsecs()<2000000000);


	OnAfEvent(success?ANDROID_CONTROL_AF_STATE_FOCUSED_LOCKED:ANDROID_CONTROL_AF_STATE_NOT_FOCUSED_LOCKED);
    // exit the thread.
	if(bFlash4Focus)
	{
		setFlash(false);
	}

	return UNKNOWN_ERROR;
}

// Ellie: turn on flash before image capture if the mode is "single"
status_t DeviceAdapter::precaptureMetering()
{
	int ret;

/* dont' report to CaptureSequence because we are too fast, manageStandardPrecaptureWait is not able to catch both start and end events,
    simply let timed out waiting for precapture metering start, which is 200ms */
#if 0 
	if (mListener != NULL) {
		mListener->handlePrecapture(ANDROID_CONTROL_AE_STATE_PRECAPTURE);
	}
#endif
	uint8_t fl_mode;
	ret = mMetadaManager->getFlashMode(&fl_mode);
	if(fl_mode!=ANDROID_FLASH_OFF) {
		autoFlash();		
		if(fl_mode==ANDROID_FLASH_SINGLE)
		{
			mSingleFlashing=true;
		}
	}

#if 0
	if (mListener != NULL) {
		mListener->handlePrecapture(ANDROID_CONTROL_AE_STATE_INACTIVE);
	}
#endif
	return NO_ERROR;
}

void DeviceAdapter::handleFrameRelease(CameraFrame *buffer)
{
    if (mPreviewing) {
        fillCameraFrame(buffer);
    }
}

void DeviceAdapter::setErrorListener(CameraErrorListener *listener)
{
    mErrorListener = listener;
}

void DeviceAdapter::setCameraBufferProvide(CameraBufferProvider *bufferProvider)
{
    if (bufferProvider != NULL) {
        bufferProvider->addBufferListener(this);
    }
    mBufferProvider = bufferProvider;
}

void DeviceAdapter::onBufferCreat(CameraFrame *pBuffer,
                                  int          num)
{
    registerCameraBuffers(pBuffer, num);
}

void DeviceAdapter::onBufferDestroy()
{
    memset(mDeviceBufs, 0, sizeof(mDeviceBufs));
}

