/*
 * Copyright (C) 2009-2013 Freescale Semiconductor, Inc.
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

#include "CameraHal.h"
#include "PhysMemAdapter.h"

using namespace android;

CameraHal::CameraHal(int cameraId)
    : mPowerLock(false), mCameraId(cameraId),triggerid_af(0),triggerid_precapture(0)
{

}

CameraHal::~CameraHal()
{
    release();
    mRequestManager.clear();
}

void CameraHal::handleError(int err)
{
    switch (err) {
        case CAMERA2_MSG_ERROR_HARDWARE:
        case CAMERA2_MSG_ERROR_DEVICE:
        case CAMERA2_MSG_ERROR_REQUEST:
        case CAMERA2_MSG_ERROR_FRAME:
        case CAMERA2_MSG_ERROR_STREAM:
        default:
            FLOGE("%s handle error:%d", __FUNCTION__, err);
            mNotifyCb(CAMERA2_MSG_ERROR, err, 0, 0, mNotifyUserPtr);

            break;
    }
}

void CameraHal::handleEvent(sp<CameraEvent>& event)
{
	if(CameraEvent::EVENT_FOCUS==event->mEventType){
		mNotifyCb(CAMERA2_MSG_AUTOFOCUS,(int32_t)event->mData,triggerid_af,0,mNotifyUserPtr);
	}else if(CameraEvent::EVENT_EXPOSURE==event->mEventType){
		mNotifyCb(CAMERA2_MSG_AUTOEXPOSURE,(int32_t)event->mData,triggerid_precapture,0,mNotifyUserPtr);
	}else if(CameraEvent::EVENT_WB==event->mEventType){
		mNotifyCb(CAMERA2_MSG_AUTOWB,(int32_t)event->mData,triggerid_precapture,0,mNotifyUserPtr);
	}
}

int CameraHal::notify_request_queue_not_empty()
{
    FLOGI("%s running", __FUNCTION__);
    return mRequestManager->dispatchRequest();
}

int CameraHal::set_request_queue_src_ops(
    const camera2_request_queue_src_ops_t *request_src_ops)
{
    mRequestQueue = request_src_ops;
    return mRequestManager->setRequestOperation(request_src_ops);
}

int CameraHal::set_frame_queue_dst_ops(
                    const camera2_frame_queue_dst_ops_t *frame_dst_ops)
{
    mFrameQueue = frame_dst_ops;
    return mRequestManager->setFrameOperation(frame_dst_ops);
}

int CameraHal::get_in_progress_count()
{
    return mRequestManager->getInProcessCount();
}

int CameraHal::construct_default_request(
            int request_template, camera_metadata_t **request)
{
    return mRequestManager->CreateDefaultRequest(request_template, request);
}

int CameraHal::allocate_stream(uint32_t width,
        uint32_t height, int format,
        const camera2_stream_ops_t *stream_ops,
        uint32_t *stream_id,
        uint32_t *format_actual,
        uint32_t *usage,
        uint32_t *max_buffers)
{
    return mRequestManager->allocateStream(width, height, format,
                stream_ops, stream_id, format_actual, usage, max_buffers);
}

int CameraHal::register_stream_buffers(
    uint32_t stream_id, int num_buffers,
    buffer_handle_t *buffers)
{
    return mRequestManager->registerStreamBuffers(stream_id, num_buffers, buffers);
}

int CameraHal::release_stream(uint32_t stream_id)
{
    return mRequestManager->releaseStream(stream_id);
}

int CameraHal::allocate_reprocess_stream(
    uint32_t width,
    uint32_t height,
    uint32_t format,
    const camera2_stream_in_ops_t *reprocess_stream_ops,
    uint32_t *stream_id,
    uint32_t *consumer_usage,
    uint32_t *max_buffers)
{
    return INVALID_OPERATION;
}

int CameraHal::release_reprocess_stream(uint32_t stream_id)
{
    return INVALID_OPERATION;
}

int CameraHal::get_metadata_vendor_tag_ops(vendor_tag_query_ops_t **ops)
{
    *ops = NULL;
    return NO_ERROR;
}

int CameraHal::set_notify_callback(camera2_notify_callback notify_cb,
            void *user)
{
    mNotifyCb = notify_cb;
    mNotifyUserPtr = user;
    return NO_ERROR;
}

int CameraHal::trigger_action(uint32_t trigger_id,int32_t ext1,int32_t ext2){
	int ret;
	if(trigger_id==CAMERA2_TRIGGER_AUTOFOCUS){
		triggerid_af = ext1;
		ret = autoFocus();
	}
	else if(trigger_id==CAMERA2_TRIGGER_CANCEL_AUTOFOCUS){
		triggerid_af = ext1;
		ret = cancelAutoFocus();
	}
	else if(trigger_id==CAMERA2_TRIGGER_PRECAPTURE_METERING){
		triggerid_precapture = ext1;
		ret = precaptureMetering();
	}
	return INVALID_OPERATION;
}


status_t CameraHal::initialize(CameraInfo& info)
{
    status_t ret = NO_ERROR;

    FLOG_RUNTIME("initialize name:%s, path:%s", info.name, info.devPath);
    mRequestManager = new RequestManager(mCameraId);
    if (mRequestManager == NULL) {
        FLOGE("CameraHal: DeviceAdapter create failed");
        return BAD_VALUE;
    }

    ret = mRequestManager->initialize(info);
    if (ret) {
        FLOGE("CameraHal: DeviceAdapter initialize failed");
        return ret;
    }

    mRequestManager->setErrorListener(this);
	mRequestManager->setEventListener(this);

    return ret;
}

void CameraHal::release()
{
    mRequestManager->release();
}

void CameraHal::LockWakeLock()
{
    if (!mPowerLock) {
        acquire_wake_lock(PARTIAL_WAKE_LOCK, V4LSTREAM_WAKE_LOCK);
        mPowerLock = true;
    }
}

void CameraHal::UnLockWakeLock()
{
    if (mPowerLock) {
        release_wake_lock(V4LSTREAM_WAKE_LOCK);
        mPowerLock = false;
    }
}

status_t CameraHal::dump(int fd) const
{
    return NO_ERROR;
}

// Ellie added
int CameraHal::autoFocus(void)
{
    return mRequestManager->autoFocus();
}

int CameraHal::cancelAutoFocus(void)
{
    return mRequestManager->cancelAutoFocus();
}

int CameraHal::precaptureMetering(void)
{
    return mRequestManager->precaptureMetering();
}

