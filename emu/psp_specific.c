/*
 *  xtiger port on PSP 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <SDL/SDL.h>

#include "sysdeps.h"
#include "config.h"
#include "options.h"
#include "newcpu.h"
#include "keyboard.h"
#include "specific.h"
#include "debug.h"
#include "global.h"
#include "globinfo.h"

# include "psp_kbd.h"
# include "psp_sdl.h"
# include "psp_danzeff.h"
# include "psp_specific.h"

/* Colors */

#define PSP_LCD_BACKGROUND psp_sdl_rgb(181, 211, 242)
#define PSP_LCD_FOREGROUND psp_sdl_rgb(0x0, 0x0, 0x0)

static u16 loc_lcd_background_16 = 0;
static int loc_lcd_background = 0;
static int loc_lcd_foreground = 0;

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;


#define ESTAT_SCREENON 1
#define ESTAT_LINKON 2
#define ESTAT_CMDS 8

int contrast = 16;
/* Planar->Chunky conversion table */
int convtab[512];

ULONG *screenBuf = NULL;
int grayPlanes = 2;
int currPlane = 0;
/* Pointer to TI92 planar screen */

unsigned char *the_screen = NULL;

void 
update_progbar(int size) 
{
}

void 
link_progress(int type, char *name, int size) 
{
}

int 
init_specific(void) 
{
  int i,j,k;
  unsigned char *tmptab = (unsigned char *)convtab;

  for(k = 0, i = 0; i<256; i++) {
    for(j = 7; j>=0; j--)
      if((i>>j)&1) tmptab[k++] = 1;
      else         tmptab[k++] = 0;
  }

  psp_init_keyboard();

  screenBuf=(ULONG*)malloc(TI92_WIDTH * TI92_HEIGHT);

  loc_lcd_background_16 = PSP_LCD_BACKGROUND;

  loc_lcd_background = PSP_LCD_BACKGROUND;
  loc_lcd_background |= loc_lcd_background << 16;
  loc_lcd_foreground = PSP_LCD_FOREGROUND;
  loc_lcd_foreground |= loc_lcd_foreground << 16;

  return(1);
}

#define filter(v, l, h) (v<l ? l : (v>h ? h : v))

int lastc = 0;

void 
set_contrast(int c) 
{
}

void CloseTigerWin(void)
{
}

/** PutImage *************************************************/
/** Put an image on the screen.                             **/
/*************************************************************/

void
ti92_update_fps()
{
  static u32 next_sec_clock = 0;
  static u32 cur_num_frame = 0;
  cur_num_frame++;
  u32 curclock = SDL_GetTicks();
  if (curclock > next_sec_clock) {
    next_sec_clock = curclock + 1000;
    TI92.ti92_current_fps = cur_num_frame;
    cur_num_frame = 0;
  }
}

static void
loc_PutImage_normal()
{
  int x;
  int y;
  int color;
  char *scan_buf = (char *)screenBuf;
  u16 *vram = NULL;
  u16 *scan_vram = NULL;

  psp_sdl_clear_screen(loc_lcd_background_16);

  vram = (u16 *)psp_sdl_get_vram_addr(40, 56);
  scan_vram = vram;
  for (y = 0; y < TI92_HEIGHT; y++) {
    for (x = 0; x < TI92_WIDTH; x++) {
      if (scan_buf[x]) color = loc_lcd_foreground;
      else             color = loc_lcd_background;
      scan_vram[x] = color;
    }
    scan_vram += 320;
    scan_buf  += TI92_WIDTH;
  }
}

static void
loc_PutImage_blit()
{
  int x;
  int y;
  int color;
  char *scan_buf = (char *)screenBuf;
  u16 *vram = NULL;
  u16 *scan_vram = NULL;

  vram = (u16 *)blit_surface->pixels;
  scan_vram = vram;
  for (y = 0; y < TI92_HEIGHT; y++) {
    for (x = 0; x < TI92_WIDTH; x++) {
      if (scan_buf[x]) color = loc_lcd_foreground;
      else             color = loc_lcd_background;
      scan_vram[x] = color;
    }
    scan_vram += TI92_WIDTH;
    scan_buf  += TI92_WIDTH;
  }
}

static void
loc_PutImage_max()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.x = 0;
  srcRect.y = 0;
  srcRect.w = TI92_WIDTH;
  srcRect.h = TI92_HEIGHT;
  dstRect.x = 0;
  dstRect.y = 0;
  dstRect.w = 320;
  dstRect.h = 240;

  loc_PutImage_blit();
  SDL_SoftStretch( blit_surface, &srcRect, screen_surface, &dstRect );
}

static void
loc_PutImage_fit_width()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.x = 0;
  srcRect.y = 0;
  srcRect.w = TI92_WIDTH;
  srcRect.h = TI92_HEIGHT;
  dstRect.x = 0;
  dstRect.y = 35;
  dstRect.w = 320;
  dstRect.h = 170;

  loc_PutImage_blit();
  psp_sdl_clear_screen(loc_lcd_background_16);
  SDL_SoftStretch( blit_surface, &srcRect, screen_surface, &dstRect );
}

static void
loc_PutImage_x125()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.x = 0;
  srcRect.y = 0;
  srcRect.w = TI92_WIDTH;
  srcRect.h = TI92_HEIGHT;
  dstRect.x = 10;
  dstRect.y = 40;
  dstRect.w = 300;
  dstRect.h = 160;

  loc_PutImage_blit();
  psp_sdl_clear_screen(loc_lcd_background_16);
  SDL_SoftStretch( blit_surface, &srcRect, screen_surface, &dstRect );
}

static void
loc_PutImage_x2()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

//240 x 128 -> 160 x  
  srcRect.x = 0;
  srcRect.y = 4;
  srcRect.w = TI92_WIDTH  - 80;
  srcRect.h = TI92_HEIGHT - 8;
  dstRect.x = 0;
  dstRect.y = 0;
  dstRect.w = 320;
  dstRect.h = 240;

  loc_PutImage_blit();
  SDL_SoftStretch( blit_surface, &srcRect, screen_surface, &dstRect );
}

static void
loc_PutImage_x15()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.x = 0;
  srcRect.y = 0;
  srcRect.w = TI92_WIDTH - 26;
  srcRect.h = TI92_HEIGHT;
  dstRect.x = 0;
  dstRect.y = 0;
  dstRect.w = 320;
  dstRect.h = 192;

  loc_PutImage_blit();
  psp_sdl_clear_screen(loc_lcd_background_16);
  SDL_SoftStretch( blit_surface, &srcRect, screen_surface, &dstRect );
}

static void 
loc_PutImage(void)
{
  if (TI92.psp_skip_cur_frame <= 0) {

    TI92.psp_skip_cur_frame = TI92.psp_skip_max_frame;

    if (TI92.ti92_render_mode == TI92_RENDER_NORMAL) loc_PutImage_normal();
    else
    if (TI92.ti92_render_mode == TI92_RENDER_WIDTH) loc_PutImage_fit_width();
    else
    if (TI92.ti92_render_mode == TI92_RENDER_MAX   ) loc_PutImage_max();
    else
    if (TI92.ti92_render_mode == TI92_RENDER_X125  ) loc_PutImage_x125();

    if (psp_kbd_is_danzeff_mode()) {
      danzeff_moveTo(-85, -55);
      danzeff_render( 1 );
    }

    if (TI92.ti92_view_fps) {
      char buffer[32];
      sprintf(buffer, "%3d", (int)TI92.ti92_current_fps);
      psp_sdl_fill_print(0, 0, buffer, 0x0, loc_lcd_background_16 );
    }
    psp_sdl_flip();

    if (psp_screenshot_mode) {
      psp_screenshot_mode--;
      if (psp_screenshot_mode <= 0) {
        psp_save_screenshot();
        psp_screenshot_mode = 0;
      }
    }
  } else if (TI92.psp_skip_max_frame) {
    TI92.psp_skip_cur_frame--;
  }

  if (TI92.ti92_view_fps) {
    ti92_update_fps();
  }
}

void
psp_save_screenshot(void)
{
  char TmpFileName[MAX_PATH];

	fprintf(stderr, "Save on: %s/scr/screenshot_%d.png", TI92.ti92_home_dir, TI92.psp_screenshot_id++);  
  if (TI92.psp_screenshot_id >= 10) TI92.psp_screenshot_id = 0;
  psp_sdl_save_png(TmpFileName);
}

void 
exit_specific(void) 
{
  CloseTigerWin();
}

void set_screen_ptr(unsigned char *ptr) {
  the_screen = ptr;
}

void 
ti92_update_screen(void) 
{
  unsigned char *ptr = the_screen;
  ULONG* scan_screen = screenBuf;

  if(!grayPlanes || !currPlane) {
    int l = 128 * 30;
    while (l-- > 0) {
      short v = (*ptr++) << 1;
      *scan_screen++ = convtab[v  ];
      *scan_screen++ = convtab[v+1];
    }
  }
  else {
    int l = 128 * 30;
    while (l-- > 0) {
      short v = (*ptr++) << 1;
      *scan_screen++ += convtab[v  ];
      *scan_screen++ += convtab[v+1];
      }
  }

  if(currPlane++ >= grayPlanes) {
    loc_PutImage();
    currPlane = 0;
  }
}

void
ti92_clear_screen()
{
  psp_sdl_clear_screen(loc_lcd_background_16);
  psp_sdl_flip();
  psp_sdl_clear_screen(loc_lcd_background_16);
  psp_sdl_flip();
}
