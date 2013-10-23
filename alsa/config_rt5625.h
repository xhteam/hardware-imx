/*
 * Copyright (C) 2011 The Android Open Source Project
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
/* Copyright (C) 2012 Freescale Semiconductor, Inc. */

#ifndef ANDROID_INCLUDE_IMX_CONFIG_RT5625_H
#define ANDROID_INCLUDE_IMX_CONFIG_RT5625_H

#include "audio_hardware.h"





/* These are values that never change */
static struct route_setting defaults_rt5625[] = {
    /* general */
    {
        .ctl_name = "SPKOUT Playback Volume",
        .intval = 26,
    },
	{
		.ctl_name = "HPOUT Playback Volume",
		.intval = 23,
	},
    
    {
        .ctl_name = NULL,
    },
};

static struct route_setting bt_output_rt5625[] = {
    {
        .ctl_name = NULL,
    },
};

//default use hp mixer to speaker
static struct route_setting speaker_output_rt5625[] = {
    {
        .ctl_name = "SPKOUT Playback Switch",
        .intval = 1,
    },
    {
        .ctl_name = "SPK Mixer DAC Mixer Playback Switch",
        .intval = 1,
    },
    {
        .ctl_name = "SPKOUT Mux",
        .intval = 2,
    },
    {
        .ctl_name = NULL,
    },
};

static struct route_setting hs_output_rt5625[] = {
    {
        .ctl_name = "Right HP Mixer HIFI DAC Playback Switch",
        .intval = 1,
    },
    {
        .ctl_name = "Left HP Mixer HIFI DAC Playback Switch",
        .intval = 1,
    },    
    {
        .ctl_name = "HPROUT Mux",
        .intval = 1,
    },
	{
		.ctl_name = "HPLOUT Mux",
		.intval = 1,
	},
	{
		.ctl_name = "HPOUT Playback Switch",
		.intval	= 1,
	},
    {
        .ctl_name = NULL,
    },
};

static struct route_setting earpiece_output_rt5625[] = {
    {
        .ctl_name = NULL,
    },
};


//FIXME
static struct route_setting vx_hs_mic_input_rt5625[] = {
    {
        .ctl_name = NULL,
    },
};


//FIXME
static struct route_setting mm_main_mic_input_rt5625[] = {
    {
        .ctl_name = "Right Rec Mixer Mic1 Capture Switch",
		.intval = 1,
    },
    {
        .ctl_name = "Left Rec Mixer Mic1 Capture Switch",
		.intval = 1,
    },
	{
		.ctl_name = "Mic1 Amp Boost Type",
		.intval = 1,
	},
	{
		.ctl_name = "Mic1 Playback Volume",
		.intval = 23,
	},
	{
		.ctl_name = "PCM Capture Volume",
		.intval = 20,
	},
    {
        .ctl_name = NULL,
    },
};

//FIXME
static struct route_setting vx_main_mic_input_rt5625[] = {
    {
        .ctl_name = NULL,
    },
};

static struct route_setting mm_hs_mic_input_rt5625[] = {

    {
        .ctl_name = NULL,
    },
};

static struct route_setting vx_bt_mic_input_rt5625[] = {
    {
        .ctl_name = NULL,
    },
};


static struct route_setting mm_bt_mic_input_rt5625[] = {
    {
        .ctl_name = NULL,
    },
};

/* ALSA cards for IMX, these must be defined according different board / kernel config*/
static struct audio_card  rt5625_card = {
    .name = "rt5625-audio",
    .driver_name = "rt5625-audio",
    .supported_out_devices = (AUDIO_DEVICE_OUT_EARPIECE |
            AUDIO_DEVICE_OUT_SPEAKER |
            AUDIO_DEVICE_OUT_WIRED_HEADSET |
            AUDIO_DEVICE_OUT_WIRED_HEADPHONE |
            AUDIO_DEVICE_OUT_ANLG_DOCK_HEADSET |
            AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET |
            AUDIO_DEVICE_OUT_ALL_SCO |
            AUDIO_DEVICE_OUT_DEFAULT ),
    .supported_in_devices=(AUDIO_DEVICE_IN_COMMUNICATION |
            AUDIO_DEVICE_IN_AMBIENT |
            AUDIO_DEVICE_IN_BUILTIN_MIC |
            AUDIO_DEVICE_IN_WIRED_HEADSET |
            AUDIO_DEVICE_IN_BACK_MIC |
            AUDIO_DEVICE_IN_ALL_SCO |
            AUDIO_DEVICE_IN_DEFAULT),
    .defaults            = defaults_rt5625,
    .bt_output           = bt_output_rt5625,
    .speaker_output      = speaker_output_rt5625,
    .hs_output           = hs_output_rt5625,
    .earpiece_output     = earpiece_output_rt5625,
    .vx_hs_mic_input     = vx_hs_mic_input_rt5625,
    .mm_main_mic_input   = mm_main_mic_input_rt5625,
    .vx_main_mic_input   = vx_main_mic_input_rt5625,
    .mm_hs_mic_input     = mm_hs_mic_input_rt5625,
    .vx_bt_mic_input     = vx_bt_mic_input_rt5625,
    .mm_bt_mic_input     = mm_bt_mic_input_rt5625,
    .card                = 0,
    .out_rate            = 0,
    .out_channels        = 0,
    .out_format          = 0,
    .in_rate             = 0,
    .in_channels         = 0,
    .in_format           = 0,	
};

#endif  /* ANDROID_INCLUDE_IMX_CONFIG_WM8962_H */
