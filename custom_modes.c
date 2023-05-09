#include "pico/scanvideo.h"

const scanvideo_timing_t pal_timing_800x288_50_default_312 =
        {
                .clock_freq = 16000000,

                .h_active = 800,
                .v_active = 288,

                .h_front_porch = 32,
                .h_pulse = 80,
                .h_total = 1024,
                .h_sync_polarity = 0,

                .v_front_porch = 4,
                .v_pulse = 2,
                .v_total = 312,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };

const scanvideo_mode_t pal_mode_800x288_50_312 =
        {
                .default_timing = &pal_timing_800x288_50_default_312,
                .pio_program = &video_24mhz_composable,
                .width = 800,
                .height = 288,
                .xscale = 1,
                .yscale = 1,
        };




const scanvideo_timing_t pal_timing_800x288_50_default_314 =
        {
                .clock_freq = 16000000,

                .h_active = 800,
                .v_active = 288,

                .h_front_porch = 32,
                .h_pulse = 80,
                .h_total = 1024,
                .h_sync_polarity = 0,

                .v_front_porch = 4,
                .v_pulse = 2,
                .v_total = 314,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };

const scanvideo_mode_t pal_mode_800x288_50_314 =
        {
                .default_timing = &pal_timing_800x288_50_default_314,
                .pio_program = &video_24mhz_composable,
                .width = 800,
                .height = 288,
                .xscale = 1,
                .yscale = 1,
        };




const scanvideo_timing_t pal_timing_800x288_50_default_310 =
        {
                .clock_freq = 16000000,

                .h_active = 800,
                .v_active = 288,

                .h_front_porch = 32,
                .h_pulse = 80,
                .h_total = 1024,
                .h_sync_polarity = 0,

                .v_front_porch = 4,
                .v_pulse = 2,
                .v_total = 310,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };

const scanvideo_mode_t pal_mode_800x288_50_310 =
        {
                .default_timing = &pal_timing_800x288_50_default_310,
                .pio_program = &video_24mhz_composable,
                .width = 800,
                .height = 288,
                .xscale = 1,
                .yscale = 1,
        };




const scanvideo_timing_t pal_timing_800x288_50_default_316 =
        {
                .clock_freq = 16000000,

                .h_active = 800,
                .v_active = 288,

                .h_front_porch = 32,
                .h_pulse = 80,
                .h_total = 1024,
                .h_sync_polarity = 0,

                .v_front_porch = 4,
                .v_pulse = 2,
                .v_total = 316,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };



const scanvideo_mode_t pal_mode_800x288_50_316 =
        {
                .default_timing = &pal_timing_800x288_50_default_316,
                .pio_program = &video_24mhz_composable,
                .width = 800,
                .height = 288,
                .xscale = 1,
                .yscale = 1,
        };



 const scanvideo_timing_t c64_timing_744x240_60_default_262 =
        {
                .clock_freq = 16375000,    //closest frequency for 16.363636Mhz

                .h_active = 744,
                .v_active = 240,

                .h_front_porch = 90,
                .h_pulse = 80,
                .h_total = 1040,
                .h_sync_polarity = 0,

                .v_front_porch = 7,
                .v_pulse = 2,
                .v_total = 262,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };


const scanvideo_mode_t c64_mode_744x240_60_262 =
        {
                .default_timing = &c64_timing_744x240_60_default_262,
                .pio_program = &video_24mhz_composable,
                .width = 744,
                .height = 240,
                .xscale = 1,
                .yscale = 1,
        };

 const scanvideo_timing_t apple_timing_744x240_60_default_266 =
        {
                .clock_freq = 14312500,    //14333333,   //closest frequency for 14.318181Mhz

                .h_active = 744,
                .v_active = 240,

                .h_front_porch = 25,
                .h_pulse = 80,
                .h_total = 912,
                .h_sync_polarity = 0,

                .v_front_porch = 7,
                .v_pulse = 2,
                .v_total = 266,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };


const scanvideo_mode_t apple_mode_744x240_60_266 =
        {
                .default_timing = &apple_timing_744x240_60_default_266,
                .pio_program = &video_24mhz_composable,
                .width = 744,
                .height = 240,
                .xscale = 1,
                .yscale = 1,
        };




 const scanvideo_timing_t cga_timing_744x240_60_default_00 =
        {
                .clock_freq = 14312500,    //14333333,   //closest frequency for 14.318181Mhz

                .h_active = 744,
                .v_active = 240,

                .h_front_porch = 25,
                .h_pulse = 80,
                .h_total = 912,
                .h_sync_polarity = 0,

                .v_front_porch = 7,
                .v_pulse = 2,
                .v_total = 260,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };


const scanvideo_mode_t cga_mode_744x240_60_00 =
        {
                .default_timing = &cga_timing_744x240_60_default_00,
                .pio_program = &video_24mhz_composable,
                .width = 744,
                .height = 240,
                .xscale = 1,
                .yscale = 1,
        };





 const scanvideo_timing_t cga_timing_744x240_60_default_01 =
        {
                .clock_freq = 14312500,    //14333333,   //closest frequency for 14.318181Mhz

                .h_active = 744,
                .v_active = 240,

                .h_front_porch = 25,
                .h_pulse = 80,
                .h_total = 912,
                .h_sync_polarity = 0,

                .v_front_porch = 7,
                .v_pulse = 2,
                .v_total = 260,
                .v_sync_polarity = 1,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };


const scanvideo_mode_t cga_mode_744x240_60_01 =
        {
                .default_timing = &cga_timing_744x240_60_default_01,
                .pio_program = &video_24mhz_composable,
                .width = 744,
                .height = 240,
                .xscale = 1,
                .yscale = 1,
        };




 const scanvideo_timing_t cga_timing_744x240_60_default_10 =
        {
                .clock_freq = 14312500,    //14333333,   //closest frequency for 14.318181Mhz

                .h_active = 744,
                .v_active = 240,

                .h_front_porch = 25,
                .h_pulse = 80,
                .h_total = 912,
                .h_sync_polarity = 1,

                .v_front_porch = 7,
                .v_pulse = 2,
                .v_total = 260,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };


const scanvideo_mode_t cga_mode_744x240_60_10 =
        {
                .default_timing = &cga_timing_744x240_60_default_10,
                .pio_program = &video_24mhz_composable,
                .width = 744,
                .height = 240,
                .xscale = 1,
                .yscale = 1,
        };





 const scanvideo_timing_t cga_timing_744x240_60_default_11 =
        {
                .clock_freq = 14312500,    //14333333,   //closest frequency for 14.318181Mhz

                .h_active = 744,
                .v_active = 240,

                .h_front_porch = 25,
                .h_pulse = 80,
                .h_total = 912,
                .h_sync_polarity = 1,

                .v_front_porch = 7,
                .v_pulse = 2,
                .v_total = 260,
                .v_sync_polarity = 1,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };


const scanvideo_mode_t cga_mode_744x240_60_11 =
        {
                .default_timing = &cga_timing_744x240_60_default_11,
                .pio_program = &video_24mhz_composable,
                .width = 744,
                .height = 240,
                .xscale = 1,
                .yscale = 1,
        };




 const scanvideo_timing_t atari_mono_timing_704x464_72_default =
        {
                .clock_freq = 32142857,

                .h_active = 704,
                .v_active = 464,

                .h_front_porch = 2,
                .h_pulse = 80,
                .h_total = 896,
                .h_sync_polarity = 1,

                .v_front_porch = 31,
                .v_pulse = 2,
                .v_total = 501,
                .v_sync_polarity = 1,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };


const scanvideo_mode_t atari_mono_mode_704x464_72 =
        {
                .default_timing = &atari_mono_timing_704x464_72_default,
                .pio_program = &video_24mhz_composable,
                .width = 704,
                .height = 464,
                .xscale = 1,
                .yscale = 1,
        };

 const scanvideo_timing_t portrait_timing_864x1024_59_default =
        {
                .clock_freq = 64000000,

                .h_active = 864,
                .v_active = 1024,

                .h_front_porch = 20,
                .h_pulse = 40,
                .h_total = 1024,
                .h_sync_polarity = 0,

                .v_front_porch = 12,
                .v_pulse = 2,
                .v_total = 1056,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };


const scanvideo_mode_t portrait_mode_864x1024_59 =
        {
                .default_timing = &portrait_timing_864x1024_59_default,
                .pio_program = &video_24mhz_composable,
                .width = 864,
                .height = 1024,
                .xscale = 1,
                .yscale = 1,
        };

const scanvideo_timing_t pal_timing_576x288_60_TS9347_1_1 =
        {
                .clock_freq = 14312500,

                .h_active = 576,
                .v_active = 288,

                .h_front_porch = 52,
                .h_pulse = 56,
                .h_total = 768,
                .h_sync_polarity = 1,

                .v_front_porch = 4,
                .v_pulse = 2,
                .v_total = 312,
                .v_sync_polarity = 1,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };

const scanvideo_mode_t pal_mode_576x288_60_TS9347_1_1 =
        {
                .default_timing = &pal_timing_576x288_60_TS9347_1_1,
                .pio_program = &video_24mhz_composable,
                .width = 576,
                .height = 288,
                .xscale = 1,
                .yscale = 1,
        };


const scanvideo_timing_t pal_timing_384x288_60_TS9347_1_1 =
        {
                .clock_freq = 19083333, //9541666,

                .h_active = 2*384,
                .v_active = 288,

                .h_front_porch = 2*20,
                .h_pulse = 2*38,
                .h_total = 2*512,
                .h_sync_polarity = 0,

                .v_front_porch = 6,
                .v_pulse = 2,
                .v_total = 312,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };

const scanvideo_mode_t pal_mode_384x288_60_TS9347_1_1 =
        {
                .default_timing = &pal_timing_384x288_60_TS9347_1_1,
                .pio_program = &video_24mhz_composable,
                .width = 384,
                .height = 288,
                .xscale = 2,
                .yscale = 1,
        };

const scanvideo_timing_t pal_timing_576x288_50_0_0 =
        {
                .clock_freq = 12000000,

                .h_active = 576,
                .v_active = 288,

                .h_front_porch = 52,
                .h_pulse = 56,
                .h_total = 768,
                .h_sync_polarity = 0,

                .v_front_porch = 4,
                .v_pulse = 2,
                .v_total = 318,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };

const scanvideo_mode_t pal_mode_576x288_50_0_0 =
        {
                .default_timing = &pal_timing_576x288_50_0_0,
                .pio_program = &video_24mhz_composable,
                .width = 576,
                .height = 288,
                .xscale = 1,
                .yscale = 1,
        };


//************************************************************
//timings below are not used at the moment
/*
const scanvideo_timing_t pal_timing_640x256_50_default =
        {
                .clock_freq = 16000000,

                .h_active = 640,
                .v_active = 256,

                .h_front_porch = 112,
                .h_pulse = 80,
                .h_total = 1024,
                .h_sync_polarity = 0,

                .v_front_porch = 20,
                .v_pulse = 2,
                .v_total = 312,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };

const scanvideo_mode_t pal_mode_320x256_50 =
        {
                .default_timing = &pal_timing_640x256_50_default,
                .pio_program = &video_24mhz_composable,
                .width = 320,
                .height = 256,
                .xscale = 2,
                .yscale = 1,
        };

const scanvideo_mode_t pal_mode_640x256_50 =
        {
                .default_timing = &pal_timing_640x256_50_default,
                .pio_program = &video_24mhz_composable,
                .width = 640,
                .height = 256,
                .xscale = 1,
                .yscale = 1,
        };

const scanvideo_timing_t pal_timing_768x288_50_default =
        {
                .clock_freq = 16000000,

                .h_active = 768,
                .v_active = 288,

                .h_front_porch = 48,
                .h_pulse = 80,
                .h_total = 1024,
                .h_sync_polarity = 0,

                .v_front_porch = 4,
                .v_pulse = 2,
                .v_total = 312,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };

const scanvideo_mode_t pal_mode_768x288_50 =
        {
                .default_timing = &pal_timing_768x288_50_default,
                .pio_program = &video_24mhz_composable,
                .width = 768,
                .height = 288,
                .xscale = 1,
                .yscale = 1,
        };

const scanvideo_timing_t pal_timing_720x288_50_default =
        {
                .clock_freq = 16000000,

                .h_active = 720,
                .v_active = 288,

                .h_front_porch = 72,
                .h_pulse = 80,
                .h_total = 1024,
                .h_sync_polarity = 0,

                .v_front_porch = 4,
                .v_pulse = 2,
                .v_total = 312,
                .v_sync_polarity = 0,

                .enable_clock = 0,
                .clock_polarity = 0,

                .enable_den = 1
        };


const scanvideo_mode_t pal_mode_720x288_50 =
        {
                .default_timing = &pal_timing_720x288_50_default,
                .pio_program = &video_24mhz_composable,
                .width = 720,
                .height = 288,
                .xscale = 1,
                .yscale = 1,
        };
*/