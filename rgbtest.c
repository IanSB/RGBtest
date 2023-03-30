/*
 * RGB test (c) 2023 IanB
 * Based on pico extras scanvideo (c) 2021 Raspberry Pi (Trading) Ltd.
 */
#include <stdio.h>
#include <math.h>

#include "memory.h"
#include "pico.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "pico/sync.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "charset.h"            // The character set
#include "custom_modes.h"
#include "hardware/watchdog.h"


#define MAX_FRAME_BUFFER_WIDTH 640
#define FRAME_BUFFER_HEIGHT 16
#define PICO_SCANVIDEO_PIXEL_FROM_RGB4(r, g, b) (((b)<<(PICO_SCANVIDEO_PIXEL_BSHIFT+1))|((g)<<(PICO_SCANVIDEO_PIXEL_GSHIFT+1))|((r)<<(PICO_SCANVIDEO_PIXEL_RSHIFT+1)))

enum {
  PATTERN_12BIT,
  PATTERN_9BIT,
  PATTERN_9BIT_LOW,
  PATTERN_6BIT,
  PATTERN_3BIT,
  PATTERN_1BIT,
  PATTERN_GRATING,
  PATTERN_CGA,
  PATTERN_CGA_MONO,
  PATTERN_CGA_4COL_PALETTE1,
  PATTERN_CGA_4COL_PALETTE2,
  PATTERN_GREY_BARS,
  PATTERN_GREY_0x0A,

};

static unsigned short bitmap[MAX_FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT];    //small frame buffer to hold text

static semaphore_t video_initted;
static bool invert;
char line1[128];
char line2[128];
uint border_colour = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x4, 0x4, 0x4);
uint active_width;
uint active_height;
uint max_text_chars = 80;
uint mode=0;
scanvideo_mode_t vga_mode;
uint pattern = PATTERN_12BIT;

void *memset16(void *m, unsigned short val, size_t count)
{
    unsigned short *buf = m;

    while(count--) *buf++ = val;
    return m;
}

void cls(unsigned short c) {
    memset16(bitmap, c, MAX_FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT);
}

void print_char(int x, int y, int c, unsigned short bc, unsigned short fc) {
    int char_index;
    unsigned short * ptr;

    if(c >= 32 && c < 128) {
        char_index = (c) * 8;
        ptr = &bitmap[(MAX_FRAME_BUFFER_WIDTH * y + x + 7)];
        for(int row = 0; row < 8; row++) {
            unsigned char data = charset[char_index + row];
            for(int bit = 0; bit < 8; bit ++) {
                *(ptr- bit) = (data & (1 << bit)) ? fc : bc;
            }
            ptr += MAX_FRAME_BUFFER_WIDTH;
        }
    }
}

void print_string(int x, int y, char *s, unsigned short bc, unsigned short fc) {
    if (x < 0) x = 0;
    for(int i = 0; i < strlen(s); i++) {
        print_char(x + i * 8, y, s[i], bc, fc);
    }
}

void set_sys_clock_pll_with_refdiv(uint32_t vco_freq, uint post_div1, uint post_div2, uint refdiv) {
        clock_configure(clk_sys,
                        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                        CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                        48 * MHZ,
                        48 * MHZ);

        pll_init(pll_sys, refdiv, vco_freq, post_div1, post_div2);
        uint32_t freq = vco_freq / (post_div1 * post_div2);

        // Configure clocks
        // CLK_REF = XOSC (12MHz) / 1 = 12MHz
        clock_configure(clk_ref,
                        CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC,
                        0, // No aux mux
                        12 * MHZ,
                        12 * MHZ);

        // CLK SYS = PLL SYS (125MHz) / 1 = 125MHz
        clock_configure(clk_sys,
                        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                        CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                        freq, freq);

        clock_configure(clk_peri,
                        0, // Only AUX mux on ADC
                        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                        48 * MHZ,
                        48 * MHZ);
}


int read_mode() {
    return ((gpio_get_all() >> 19) & 0x0f) ^ 0x0f;
}

int read_sw1() {
    return gpio_get(26);
}

int read_sw2() {
    return gpio_get(27);
}

void set_mode(int mode) {
uint pixel_clk;
    #define header "RGBtoHDMI Tester v0.20 (%dx%d@%dHz, %.2fMhz, %s)"
    border_colour = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x4, 0x4, 0x4);
    max_text_chars = 80;
    uint black = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0);
    uint white = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xf, 0xf, 0xf);

    cls(black);

    print_string(0, 0, "ST MONO OK",black, PICO_SCANVIDEO_PIXEL_FROM_RGB5(0x00, 0x01, 0x00));   //note uses otherwise unused 5th green lsb
    print_string(0, 8, "ST BLANK OK",black, PICO_SCANVIDEO_PIXEL_FROM_RGB5(0x00, 0x00, 0x01));  //note uses otherwise unused 5th blue lsb

    switch (mode) {
        default:
        case 0 :
            active_width = 640;
            active_height = 256;
            vga_mode = pal_mode_800x288_50_0_0;
            pattern = PATTERN_12BIT;
            pixel_clk = vga_mode.default_timing->clock_freq;
            sprintf(line1, header, active_width, active_height, 50, (double)pixel_clk / 1000000, "+H+V-C");
            sprintf(line2, "Mode %d: Digital: 12BPP, Analog: 4 Level_Terminated", mode);
            set_sys_clock_khz(pixel_clk * 8 / 1000, true);
            break;
        case 1 :
            active_width = 640;
            active_height = 256;
            vga_mode = pal_mode_800x288_50_1_0;
            pattern = PATTERN_12BIT;
            pixel_clk = vga_mode.default_timing->clock_freq;
            sprintf(line1, header, active_width, active_height, 50, (double)pixel_clk / 1000000, "-H+V+C");
            sprintf(line2, "Mode %d: Digital: 9BPP, Analog: 3 Level Terminated", mode);
            set_sys_clock_khz(pixel_clk * 8 / 1000, true);
            break;
        case 2 :
            active_width = 640;
            active_height = 256;
            vga_mode = pal_mode_800x288_50_1_1;
            pattern = PATTERN_12BIT;
            pixel_clk = vga_mode.default_timing->clock_freq;
            sprintf(line1, header, active_width, active_height, 50, (double)pixel_clk / 1000000, "-H-V+C");
            sprintf(line2, "Mode %d: Digital: 6BPP, Analog: 3 Level Term + Clamp", mode);
            set_sys_clock_khz(pixel_clk * 8 / 1000, true);
            break;
        case 3 :
            active_width = 640;
            active_height = 256;
            vga_mode = pal_mode_800x288_50_0_1;
            pattern = PATTERN_12BIT;
            pixel_clk = vga_mode.default_timing->clock_freq;
            //border_colour = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x8);
            sprintf(line1, header, active_width, active_height, 50, (double)pixel_clk / 1000000, "+H-V-C");
            sprintf(line2, "Mode %d: Digital: 3BPP, Analog: 3 Level UnTerminated", mode);
            set_sys_clock_khz(pixel_clk * 8 / 1000, true);
            break;
        case 4 :
            active_width = 640;
            active_height = 200;
            vga_mode = ntsc_mode_744x240_60;
            pattern = PATTERN_12BIT;
            //pixel clock is 14312500
            pixel_clk = vga_mode.default_timing->clock_freq;
            //set_sys_clock_pll(1374000000, 6, 2);  //114.0Mhz but misread as 114.5 by clock_get_hz(clk_sys) without refdiv set to 2
            //set_sys_clock_pll (1032000000, 3, 3);  //114.666666Mhz so pixel clock is 14.3333333mhz
            set_sys_clock_pll_with_refdiv(1374000000, 6, 2, 2);  //114.5Mhz (8 * 14.3125Mhz)
            sprintf(line1, header, active_width, active_height, 60, (double)pixel_clk / 1000000, "-H-V");
            sprintf(line2, "Mode %d: Digital: 12BPP, Analog: Analog: 4 Level_Terminated", mode);
            break;
        case 5 :
            active_width = 640;
            active_height = 200;
            vga_mode = cga_mode_744x240_60;
            pattern = PATTERN_CGA;
            pixel_clk = vga_mode.default_timing->clock_freq;
            sprintf(line1, header, active_width, active_height, 60, (double)pixel_clk / 1000000, "+H-V");
            sprintf(line2, "Mode %d: Digital: CGA, Analog: CGA", mode);
            set_sys_clock_pll_with_refdiv(1374000000, 6, 2, 2);  //114.5Mhz (8 * 14.3125Mhz)
            break;

        case 11 :
            active_width = 320;
            active_height = 256;
            vga_mode = pal_mode_384x288_60_TS9347_1_1;
            pattern = PATTERN_GREY_BARS;
            pixel_clk = vga_mode.default_timing->clock_freq;
            max_text_chars = 40;
            sprintf(line1, header, active_width, active_height, 60, (double)pixel_clk / 1000000, "+H+V-C");
            sprintf(line2, "Mode %d: Digital: 9BPP, Analog: 8 Level Mono", mode);
            set_sys_clock_pll_with_refdiv(1374000000, 6, 2, 2);  //114.5Mhz (8 * 14.3125Mhz)
            break;
        case 12 :
            active_width = 480;
            active_height = 256;
            vga_mode = pal_mode_576x288_60_TS9347_1_1;
            pattern = PATTERN_GREY_BARS;
            pixel_clk = vga_mode.default_timing->clock_freq;
            max_text_chars = 60;
            sprintf(line1, header, active_width, active_height, 60, (double)pixel_clk / 1000000, "+H+V-C");
            sprintf(line2, "Mode %d: Digital: 9BPP, Analog: 8 Level Mono", mode);
            set_sys_clock_pll_with_refdiv(1374000000, 6, 2, 2);  //114.5Mhz (8 * 14.3125Mhz)
            break;
        case 13 :
            active_width = 480;
            active_height = 256;
            vga_mode = pal_mode_576x288_50_0_0;
            pattern = PATTERN_12BIT;
            pixel_clk = vga_mode.default_timing->clock_freq;
            max_text_chars = 60;
            sprintf(line1, header, active_width, active_height, 60, (double)pixel_clk / 1000000, "+H+V-C");
            sprintf(line2, "Mode %d: (192Mhz CPLD Clock) Digital: 3BPP, Analog: 3BPP", mode);
            set_sys_clock_khz(pixel_clk * 8 / 1000, true);
            break;            
        case 14 :
            active_width = 640;
            active_height = 400;
            vga_mode = atari_mono_mode_704x464_72;
            pattern = PATTERN_GRATING;
            border_colour = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0xf);           
            pixel_clk = vga_mode.default_timing->clock_freq;
            sprintf(line1, header, active_width, active_height, 72, (double)pixel_clk / 1000000, "+H+V-C");
            sprintf(line2, "Mode %d: (Atari Mono) Digital: 3BPP, Analog: 3BPP", mode);
            set_sys_clock_pll_with_refdiv(1800000000, 7, 2, 1);  //128.571428Mhz
            break;                        
        case 15 :
            vga_mode = portrait_mode_864x1024_59;
            // pixel clock is 64000000
            pixel_clk = vga_mode.default_timing->clock_freq;
            //set_sys_clock_pll_refdiv(1, 1920000000, 6, 1);  //320Mhz (5 * 64Mhz)
            set_sys_clock_khz(pixel_clk * 4 / 1000, true);
            border_colour = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0xf);
            active_width = 864;
            active_height = 1024;
            sprintf(line2, "Mode #%d: %dx%d@59Hz, 3BPP, +H+V, %dMhz", mode, active_width, active_height, pixel_clk / 1000000);
            sprintf(line1, header, active_width, active_height, 59, (double)pixel_clk / 1000000, "+H+V");
            sprintf(line2, "Mode %d: (Hi res) Digital: 3BPP, Analog: 3BPP", mode);
            pattern = PATTERN_GRATING;
            break;
    }

    print_string((max_text_chars - strlen(line1))*4, 0, line1, black, white);
    print_string((max_text_chars - strlen(line2))*4, 8, line2, black, white);



}

uint16_t * write_gradient(uint16_t *p, uint32_t width, uint32_t colour_mask) {
    for (uint bar = 0; bar < 16; bar++) {
        *p++ = COMPOSABLE_COLOR_RUN;
        uint32_t colour = PICO_SCANVIDEO_PIXEL_FROM_RGB4(bar, bar, bar);
        *p++ = colour & colour_mask;
        *p++ = width / 16 - 3;
    }
    return p;
}

uint16_t * write_colour_bars(uint16_t *p, uint32_t width) {
    for (int bar = 7; bar >= 0; bar--) {
        *p++ = COMPOSABLE_COLOR_RUN;
        *p++ = PICO_SCANVIDEO_PIXEL_FROM_RGB4((bar & 0x02) ? 0xf : 0, (bar & 0x04) ? 0xf : 0, (bar & 0x01) ? 0xf : 0);
        *p++ = width / 8 - 3;
    }
    return p;
}

uint16_t * write_grating(uint16_t *p, uint32_t width, uint32_t first_pixel, uint32_t second_pixel) {
    //have to write the grating in 32 bit pairs for speed
    *p++ = COMPOSABLE_RAW_RUN;
    *p++ = first_pixel;
    *p++ = width - 3;
    if (((uint32_t) p) & 2) {  //is 16 bit pointer in the middle of a 32 bit word?
        *p++ = second_pixel;
        uint32_t *ip =  (uint32_t*)p;
        for (uint i = 0; i < (width >> 1) - 1; i++) {
            *ip++ = (second_pixel << 16) | first_pixel;
        }
        p = (uint16_t*)ip;
    } else {
        uint32_t *ip =  (uint32_t*)p;
        for (uint i = 0; i < (width >> 1) - 1; i++) {
            *ip++ = (first_pixel << 16) | second_pixel;
        }
        p = (uint16_t*)ip;
        *p++ = second_pixel;
    }
    return p;
}

uint16_t * write_cga_bars(uint16_t *p, uint32_t width, uint32_t intensity) {
    for (int bar = 7; bar >= 0; bar--) {
        *p++ = COMPOSABLE_COLOR_RUN;
        *p++ = PICO_SCANVIDEO_PIXEL_FROM_RGB4((bar & 0x02) ? 0x8 : 0, ((bar & 0x04) ? 0x8 : 0), (bar & 0x01) ? 0x8 : 0) | intensity;
        *p++ = width / 8 - 3;
    }
    return p;
}

uint16_t * write_colour(uint16_t *p, uint32_t width, uint32_t colour) {
    *p++ = COMPOSABLE_COLOR_RUN;
    *p++ = colour;
    *p++ = width - 3;
    return p;
}

uint16_t * write_mono_artifacts(uint16_t *p, uint32_t width, uint32_t background_colour, uint32_t foreground_colour) {
    uint phase = 0;
    *p++ = COMPOSABLE_RAW_RUN;
    *p++ = background_colour;
    *p++ = width - 3;
    uint bar_width = width /16;
    //fill in remaining pixels of first block
    for (uint pixel = 0; pixel < bar_width - 1; pixel++) {
        *p++ = background_colour;
    }
    for (uint bar = 1; bar < 16; bar++) {
        for (uint pixel = 0; pixel < (bar_width/4); pixel++) {
            //*p++ = ((bar >> (pixel & 3)) & 1) ? foreground_colour : background_colour;
            //unrolled for speed
            *p++ = ((bar >> ((phase + 3) & 3)) & 1) ? foreground_colour : background_colour;
            *p++ = ((bar >> ((phase + 2) & 3)) & 1) ? foreground_colour : background_colour;
            *p++ = ((bar >> ((phase + 1) & 3)) & 1) ? foreground_colour : background_colour;
            *p++ = ((bar >> ((phase + 0) & 3)) & 1) ? foreground_colour : background_colour;
        }
    }
    return p;
}

uint16_t * write_four_colour_artifacts(uint16_t *p, uint32_t width, uint32_t colour_0, uint32_t colour_1, uint32_t colour_2, uint32_t colour_3) {
    uint phase = 0;
    *p++ = COMPOSABLE_RAW_RUN;
    *p++ = colour_0;
    *p++ = width - 3;
    uint bar_width = width /16;
    //fill in remaining pixels of first block
    for (uint pixel = 0; pixel < bar_width - 1; pixel++) {
        *p++ = colour_0;
    }
    for (uint bar = 1; bar < 16; bar++) {
        for (uint pixel = 0; pixel < (bar_width / 4); pixel++) {
            //unrolled for speed
            switch (bar >> 2 & 3) {
                case 0:
                    *p++ = colour_0;
                    *p++ = colour_0;
                    break;
                case 1:
                    *p++ = colour_1;
                    *p++ = colour_1;
                    break;
                case 2:
                    *p++ = colour_2;
                    *p++ = colour_2;
                    break;
                case 3:
                    *p++ = colour_3;
                    *p++ = colour_3;
                    break;
            }
            switch ((bar >> 0) & 3) {
                case 0:
                    *p++ = colour_0;
                    *p++ = colour_0;
                    break;
                case 1:
                    *p++ = colour_1;
                    *p++ = colour_1;
                    break;
                case 2:
                    *p++ = colour_2;
                    *p++ = colour_2;
                    break;
                case 3:
                    *p++ = colour_3;
                    *p++ = colour_3;
                    break;
            }
        }
    }
    return p;
}

void draw_test_pattern(scanvideo_scanline_buffer_t *buffer) {
    // figure out 1/32 of the color value
    uint line_num = scanvideo_scanline_number(buffer->scanline_id);
    uint text_height = 16;
    uint border_width  = (vga_mode.width - active_width) >> 1;
    uint border_height = (vga_mode.height - active_height) >> 1;
    uint16_t *p = (uint16_t *) buffer->data;

    if (pattern == PATTERN_GREY_0x0A) {
        {
            p = write_colour(p, vga_mode.width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xA, 0xA, 0xA));
        }
    } else {


        if (border_width) {
            *p++ = COMPOSABLE_COLOR_RUN;
            *p++ = border_colour;
            *p++ = border_width - 3;
        }

        if (line_num < border_height || line_num >= (active_height + border_height)) {
            *p++ = COMPOSABLE_COLOR_RUN;
            *p++ = border_colour;
            *p++ = active_width - 3;
        } else if (line_num < (border_height + text_height)) {

            // = bitmap + ((line_num - border_height) * 640 / 2);

            uint16_t padding = (active_width - max_text_chars * 8) >> 1;
            if (padding > 0) {
                switch (padding) {
                    case 1:
                        *p++ = COMPOSABLE_RAW_1P;
                        *p++ = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0);
                        break;
                    case 2:
                        *p++ = COMPOSABLE_RAW_2P;
                        *p++ = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0);
                        break;
                    default:
                        *p++ = COMPOSABLE_COLOR_RUN;
                        *p++ = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0);
                        *p++ = padding - 3;
                        break;
                }
            }

            unsigned short *buf = &bitmap[MAX_FRAME_BUFFER_WIDTH * (line_num - border_height)];
            *p++ = COMPOSABLE_RAW_RUN;
            //have to copy text image buffer in 32 bit pairs for speed
            if (((uint32_t) p) & 2) {  //is 16 bit pointer in the middle of a 32 bit word?
                *p++ = *buf++;
                *p++ = max_text_chars * 8 - 3;
                *p++ = *buf++;
                uint32_t *ip =  (uint32_t*)p;
                uint32_t *ibuf =  (uint32_t*)buf;
                for (uint i = 0; i < ((max_text_chars * 8 - 2) >> 1); i++) {
                    *ip++ = *ibuf++;
                }
                p = (uint16_t*)ip;
            } else {
                *p++ = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0);   //pad with 0 so p and buf 32 bit aligned
                *p++ = max_text_chars * 8 - 3;
                uint32_t *ip =  (uint32_t*)p;
                uint32_t *ibuf =  (uint32_t*)buf;
                for (uint i = 0; i < ((max_text_chars * 8 - 2) >> 1); i++) {
                    *ip++ = *ibuf++;
                }
                p = (uint16_t*)ip;
                *p++ = *buf++;
            }

            if (padding > 0) {
                switch (padding) {
                    case 1:
                        *p++ = COMPOSABLE_RAW_1P;
                        *p++ = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0);
                        break;
                    case 2:
                        *p++ = COMPOSABLE_RAW_2P;
                        *p++ = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0);
                        break;
                    default:
                        *p++ = COMPOSABLE_COLOR_RUN;
                        *p++ = PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0);
                        *p++ = padding - 3;
                        break;
                }
            }

        } else {
            uint block = (line_num - border_height - text_height) * 12 / (active_height - text_height);
            switch(pattern) {
                default:
                case PATTERN_12BIT:
                {
                    switch (block >> 1) {
                        case 0 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xf, 0x0, 0x0));
                            break;
                        case 1 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0xf, 0x0));
                            break;
                        case 2 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0xf));
                            break;
                        case 3 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xf, 0xf, 0xf));
                            break;
                        case 4 :
                            p = write_colour_bars(p, active_width);
                            break;
                        default:
                        case 5:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xf, 0xf, 0xf), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                    }
                }
                break;
                case PATTERN_9BIT:
                {
                    switch (block >> 1) {
                        case 0 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xe, 0x0, 0x0));
                            break;
                        case 1 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0xe, 0x0));
                            break;
                        case 2 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0xe));
                            break;
                        case 3 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xe, 0xe, 0xe));
                            break;
                        case 4 :
                            p = write_colour_bars(p, active_width);
                            break;
                        default:
                        case 5:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xe, 0xe, 0xe), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                    }
                }
                break;
                case PATTERN_9BIT_LOW:
                {
                    switch (block >> 1) {
                        case 0 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x7, 0x0, 0x0));
                            break;
                        case 1 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x7, 0x0));
                            break;
                        case 2 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x7));
                            break;
                        case 3 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x7, 0x7, 0x7));
                            break;
                        case 4 :
                            p = write_colour_bars(p, active_width);
                            break;
                        default:
                        case 5:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x7, 0x7, 0x7), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                    }
                }
                break;
                case PATTERN_6BIT:
                {
                    switch (block >> 1) {
                        case 0 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xc, 0x0, 0x0));
                            break;
                        case 1 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0xc, 0x0));
                            break;
                        case 2 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0xc));
                            break;
                        case 3 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xc, 0xc, 0xc));
                            break;
                        case 4 :
                            p = write_colour_bars(p, active_width);
                            break;
                        default:
                        case 5:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xc, 0xc, 0xc), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                    }
                }
                break;
                case PATTERN_3BIT:
                {
                    switch (block >> 1) {
                        case 0 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x0, 0x0));
                            break;
                        case 1 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x8, 0x0));
                            break;
                        case 2 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x8));
                            break;
                        case 3 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x8, 0x8));
                            break;
                        case 4 :
                            p = write_colour_bars(p, active_width);
                            break;
                        default:
                        case 5:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x8, 0x8), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                    }
                }
                break;
                case PATTERN_1BIT:
                {
                    switch (block >> 1) {
                        case 0 :
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x8, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                        case 1 :
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x8, 0x0));
                            break;
                        case 2 :
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x8, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                        case 3 :
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x8, 0x0));
                            break;
                        case 4 :
                            p = write_colour_bars(p, active_width);
                            break;
                        default:
                        case 5:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x8, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                    }
                }
                break;


                case PATTERN_GRATING:
                {
                    switch (block >> 1) {
                        case 0:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xf, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                        case 1:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0xf, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                        case 2:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0xf), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                        case 3:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xf, 0xf, 0xf), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0xf));
                            break;
                        case 4:
                            p = write_colour_bars(p, active_width);
                            break;
                        default:
                        case 5:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xf, 0xf, 0xf), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                    }
                }
                break;
                case PATTERN_CGA:
                {
                    switch (block) {
                        case 0 :
                            p = write_mono_artifacts(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x8, 0x8));
                            break;
                        case 1 :
                            p = write_mono_artifacts(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0xc, 0x8));
                            break;
                        case 2 :
                            p = write_four_colour_artifacts(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x8, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x8, 0x0));
                            break;
                        case 3 :
                            p = write_four_colour_artifacts(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0xc, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x4, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0xc, 0x0));
                            break;
                        case 4 :
                            p = write_four_colour_artifacts(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x8, 0x8), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x0, 0x8), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x8, 0x8));
                            break;
                        case 5 :
                            p = write_four_colour_artifacts(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0xc, 0x8), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0x4, 0x8), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0xc, 0x8));
                            break;
                        case 6:
                        case 7:
                            p = write_cga_bars(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                        case 8:
                        case 9:
                            p = write_cga_bars(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x4, 0x0));
                            break;
                        default:
                        case 10:
                        case 11:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x8, 0xc, 0x8), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                    }
                }
                break;

                case PATTERN_GREY_BARS:
                {
                    switch (block >> 1) {


                        case 0 :
                        case 1 :
                        case 2 :
                        case 3 :
                        case 4 :
                            p = write_gradient(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xf, 0xf, 0xf));
                            break;
                        default:
                        case 5:
                            p = write_grating(p, active_width, PICO_SCANVIDEO_PIXEL_FROM_RGB4(0xf, 0xf, 0xf), PICO_SCANVIDEO_PIXEL_FROM_RGB4(0x0, 0x0, 0x0));
                            break;
                    }

                }

            }


        }

        if (border_width) {
            *p++ = COMPOSABLE_COLOR_RUN;
            *p++ = border_colour;
            *p++ = border_width - 3;
        }

    }

    // 32 * 3, so we should be word aligned
    assert(!(3u & (uintptr_t) p));

    // black pixel to end line
    *p++ = COMPOSABLE_RAW_1P;
    *p++ = 0;
    // end of line with alignment padding
    *p++ = COMPOSABLE_EOL_SKIP_ALIGN;
    *p++ = 0;

    buffer->data_used = ((uint32_t *) p) - buffer->data;
    assert(buffer->data_used < buffer->data_max);

    buffer->status = SCANLINE_OK;
}

void core1_func() {
        // initialize video and interrupts on core 1
        scanvideo_setup(&vga_mode);
        scanvideo_timing_enable(true);
        sem_release(&video_initted);
    while (true) {
        scanvideo_scanline_buffer_t *scanline_buffer = scanvideo_begin_scanline_generation(true);
        draw_test_pattern(scanline_buffer);
        scanvideo_end_scanline_generation(scanline_buffer);
    }
}

int main(void) {
    int current_mode;
    stdio_init_all();
    // init GPIO 0 as serial port for debugging
    gpio_set_function(0, GPIO_FUNC_UART);

    gpio_set_function(19, GPIO_FUNC_NULL);
    gpio_set_function(20, GPIO_FUNC_NULL);
    gpio_set_function(21, GPIO_FUNC_NULL);
    gpio_set_function(22, GPIO_FUNC_NULL);
    gpio_set_function(26, GPIO_FUNC_NULL);
    gpio_set_function(27, GPIO_FUNC_NULL);

    gpio_set_dir(19,0);
    gpio_set_dir(20,0);
    gpio_set_dir(21,0);
    gpio_set_dir(22,0);
    gpio_set_dir(26,0);
    gpio_set_dir(27,0);

    gpio_pull_up(19);
    gpio_pull_up(20);
    gpio_pull_up(21);
    gpio_pull_up(22);
    gpio_pull_up(26);
    gpio_pull_up(27);

/*
    uint vco_freq_out;
    uint post_div1_out;
    uint post_div2_out;
    check_sys_clock_khz (vga_mode.default_timing->clock_freq * 8 / 1000, &vco_freq_out, &post_div1_out, &post_div2_out);
    uart_puts(uart1, "HELLO WORLD\r\n");
    printf("freq %d d1=%d, d2=%d\r\n", vco_freq_out, post_div1_out, post_div2_out);
*/

    // create a semaphore to be posted when video init is complete
    sem_init(&video_initted, 0, 1);


    current_mode = read_mode();
    set_mode(current_mode);

    //can't init uart until mode set as that changes sys_clk
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    printf("\r\nPi Pico booted\r\n");
    uint sys_clk = clock_get_hz(clk_sys);
    uint pixel_clk = vga_mode.default_timing->clock_freq;
    uint multiplier = sys_clk / pixel_clk;

    printf("System clock = %dHz\r\n", sys_clk);

    printf("System clock = %dHz, Pixel clock = %dHz, Multiplier = %d\r\n", sys_clk, pixel_clk, multiplier);
    //replicate sanity check in scanvideo code as serial port gets redefined during video initialisation and this message would not be seen
    if (pixel_clk != sys_clk / multiplier) {
        panic("System clock (%d) must be an integer multiple of 2 times the requested pixel clock (%d).", sys_clk, pixel_clk);
    } else {
        printf("%s\r\n", line1);
        printf("%s\r\n", line2);
    }
    sleep_us(10000); //sleep for 10ms to allow any serial debugging output before GPIO 0 gets re-assigned

    // launch all the video on core 1, so it isn't affected by USB handling on core 0
    multicore_launch_core1(core1_func);
    // wait for initialization of video to be complete
    sem_acquire_blocking(&video_initted);
    uint old_pattern = pattern;
    uint old_sw1 = 1;
    while (true) {
        scanvideo_wait_for_vblank();
        int new_mode = read_mode();
        if (new_mode != current_mode) {
            watchdog_enable(1, 1);
            while(1);
        }
        uint sw1 = read_sw1();

        if (sw1 != old_sw1) {
            printf("SW1: %d\r\n", sw1);
            if (sw1 == 0) {
                pattern = PATTERN_GREY_0x0A;
            } else {
                pattern = old_pattern;
            }
            old_sw1 = sw1;
        }
        
        uint sw2 = read_sw2();
        if (sw2 == 0) {
            active_width = vga_mode.width;
            active_height = vga_mode.height;
        }
    }
}
