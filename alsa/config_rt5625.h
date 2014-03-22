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

/*
 * We don't have any real earpiece output, now mapping earpiece to on board speaker.
 * Because it's unstable while using code vodsp to do echo cancelling,it's disable temporarily.
 * 
 
.vx_main_mic_input	is for onboard speaker and microphone
.vx_hp_mic_input	 	is for headphone without microphone(using on board microphone)
.vx_hs_mic_input 		is for headset with microphone
.vx_bt_mic_input	 	is for bluetooth microphone
*/

enum audio_aec_mode{
	AEC_MODE_DISABLED=0,
	AEC_MODE_PCM_IN_PCM_OUT,
	AEC_MODE_ANALOG_IN_ANALOG_OUT,
	AEC_MODE_DAC_IN_DAC_OUT,
};

/* These are values that never change */
static struct route_setting defaults_rt5625[] = {
    /* general */
    {
        .ctl_name = "SPKOUT Playback Volume",
        .intval = 31,
    },
	{
		.ctl_name = "HPOUT Playback Volume",
		.intval = 31,
	},
	{
		.ctl_name = "AUXOUT Playback Volume",
		.intval = 23,
	},
	{
		.ctl_name = "AUXOUT Mux",
		.intval = 3,//mux source fron nonomixer
	},	
	{
        .ctl_name = "Phone Playback Volume",
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
		.ctl_name = "HPOUT Playback Switch",
		.intval	= 1,
	},
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
        .ctl_name = NULL,
    },
};

static struct route_setting earpiece_output_rt5625[] = {
    {
        .ctl_name = NULL,
    },
};





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
		.intval = 23,
	},
    {
        .ctl_name = NULL,
    },
};

/*
 *
 
|-----------|
|                   |<------------------AuxOut<------------ ---------------|
|Voice Modem|                                                      -------                            |                            
|                   |------> MIC1---->ADCR------->|          |				     |
|-------  ---|-----> PhoneIn------->ADCL-- >|VoDSP |--->DAC---->Mono Mixer                                                     
                                       |                                    |----- |         
				           |	----->SPK Mixer->On board speaker 
					    | ----->HP Mixer-->Headset

Path1(Near end to Far end) : Near end->MICIN -> ADC -> VoDSP -> TXDP( Echo canceling ) ? 
DAC ->AUXOUT ->Far end 
Path2(Far end to Near end) : Far end -> Phone IN -> ADC ->RxDP ( For Echo reference ) -> 
VoDSP ->TxDC ->VoDAC ? Near end 
 
Path in codec : 1.PhoneIN( Differential ) -> ADC record mixer L->ADC L -> RxDP -> VoDSP 
                        2.MIC1IN( Differential ) -> ADC record mixer R-> ADC R->PDM -> VoDSP 
                        3.TxDP -> DAC(16k sample rate) -> mono mixer -> AUXOUT( Differential ) 
                4.TxDC -> VoDAC(16k sample rate) -> SPKmixer -> SPKOUT 					    
*/
static struct route_setting vx_main_mic_input_rt5625_aec[] = {
	{
		.ctl_name = "AEC Mode",
		.intval = AEC_MODE_ANALOG_IN_ANALOG_OUT,
	},	
    {
        .ctl_name = "Left Rec Mixer Phone Capture Switch",
		.intval = 1,
    },
    {
        .ctl_name = "Right Rec Mixer Mic1 Capture Switch",
		.intval = 1,
    },
	{
		.ctl_name = "Left Rec Mixer Mic1 Capture Switch",
		.intval = 0,
	},    
	{
		.ctl_name = "SPK Mixer Phone Playback Switch",
		.intval = 1,
	},	
	{
		.ctl_name = "PCM Capture Volume",
		.intval = 23,
	},    
	{
		.ctl_name = "Mic1 Amp Boost Type",
		.intval = 0,
	},
	{
		.ctl_name = "Mic1 Playback Volume",
		.intval = 25,
	},	
	{
		.ctl_name = "MoNo Mixer Voice DAC Playback Switch",
		.intval = 1,
	},	
	{
		.ctl_name = "AUXOUT Playback Switch",
		.intval = 1,
	},
	{
		.ctl_name = "AUXOUT Playback Volume",
		.intval = 23,
	},
	{
		.ctl_name = "AUXOUT Mux",
		.intval = 3,//mux source fron nonomixer
	},
	{
		.ctl_name = "Phone Playback Volume",
		.intval = 23,
	},
	{
		.ctl_name = "Voice DAC Enable",
		.intval = 1,
	},	
    {
        .ctl_name = NULL,
    },
};

static struct route_setting vx_main_mic_input_rt5625[] = {
	{
		.ctl_name = "SPK Mixer Phone Playback Switch",
		.intval = 1,
	},
	{
        .ctl_name = "Phone Playback Volume",
		.intval = 23,
    },
	{
		.ctl_name = "Mic1 Amp Boost Type",
		.intval = 0,
	},
	{
		.ctl_name = "Mic1 Playback Volume",
		.intval = 23,
	},
	{
		.ctl_name = "MoNo Mixer Mic1 Playback Switch",
		.intval = 1,
	},
	{
		.ctl_name = "AUXOUT Playback Switch",
		.intval = 1,
	},
    {
        .ctl_name = NULL,
    },
};


static struct route_setting vx_hp_mic_input_rt5625[] = {
    {
        .ctl_name = "Right HP Mixer Phone Playback Switch",
		.intval = 1,
    },
	{
        .ctl_name = "Left HP Mixer Phone Playback Switch",
		.intval = 1,
    },
	{
        .ctl_name = "Phone Playback Volume",
		.intval = 15,
    },    
	{
		.ctl_name = "Mic1 Amp Boost Type",
		.intval = 0,
	},
	{
		.ctl_name = "Mic1 Playback Volume",
		.intval = 23,
	},	
	{
		.ctl_name = "MoNo Mixer Mic1 Playback Switch",
		.intval = 1,
	},	  
	{
		.ctl_name = "AUXOUT Playback Switch",
		.intval = 1,
	},
    {
        .ctl_name = NULL,
    },
};

/*
 * For headset phone we don't enable AEC mode??


 
|-----------|
|                   |<------------------AuxOut<---------|
|Voice Modem|                                                      ---- |                            
|                   |------> MIC2---->Mono Mixer------->|          
|-------  ---|-----> PhoneIn-------HP Mixer-->Headset  
*/
static struct route_setting vx_hs_mic_input_rt5625[] = {
    {
        .ctl_name = "Right HP Mixer Phone Playback Switch",
		.intval = 1,
    },
	{
        .ctl_name = "Left HP Mixer Phone Playback Switch",
		.intval = 1,
    },
	{
        .ctl_name = "Phone Playback Volume",
		.intval = 15,
    },
	{
		.ctl_name = "Mic2 Amp Boost Type",
		.intval = 0,
	},
	{
		.ctl_name = "Mic2 Playback Volume",
		.intval = 23,
	},
	{
		.ctl_name = "MoNo Mixer Mic2 Playback Switch",
		.intval = 1,
	},
	{
		.ctl_name = "AUXOUT Playback Switch",
		.intval = 1,
	},

	{
        .ctl_name = NULL,
    },
};


static struct route_setting mm_hs_mic_input_rt5625[] = {
    {
        .ctl_name = "Right Rec Mixer Mic2 Capture Switch",
		.intval = 1,
    },
    {
        .ctl_name = "Left Rec Mixer Mic2 Capture Switch",
		.intval = 1,
    },
	{
		.ctl_name = "Mic2 Amp Boost Type",
		.intval = 0,
	},
	{
		.ctl_name = "Mic2 Playback Volume",
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
    .vx_hp_mic_input	 = vx_hp_mic_input_rt5625,
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
