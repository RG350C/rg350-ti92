/*
 *  Copyright (C) 2009 Ludovic Jacomme (ludovic.jacomme@gmail.com)
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#include "gp2x_cpu.h"
#include "global.h"
#include "psp_sdl.h"
#include "psp_kbd.h"
#include "psp_menu.h"
#include "psp_fmgr.h"
#include "psp_menu_kbd.h"
#include "psp_menu_set.h"

extern SDL_Surface *back_surface;

# define MENU_ANALOG       0
# define MENU_SET_CLOCK    1
# define MENU_SKIP_FPS     2
# define MENU_VIEW_FPS     3
# define MENU_SET_RENDER   4

# define MENU_SET_SAVE     5
# define MENU_SET_RESET    6
                                 
# define MENU_SET_BACK     7

# define MAX_MENU_SET_ITEM (MENU_SET_BACK + 1)

  static menu_item_t menu_list[] =
  {
    { "Swap Analog/Cursor :" },
    { "Clock frequency    :" },
    { "Skip frame         :"},
    { "Display fps        :"},
    { "Video Mode         :"},

    { "Save settings"        },
    { "Reset settings"       },

    { "Back to Menu"         }
  };

  static int cur_menu_id = MENU_SET_SAVE;

  static int psp_reverse_analog   = 0;
  static int ti92_view_fps        = 0;
  static int psp_skip_fps         = 0;
  static int ti92_render_mode     = 0;
  static int psp_cpu_clock        = GP2X_DEF_EMU_CLOCK;


static void 
psp_display_screen_settings_menu(void)
{
  char buffer[64];
  int menu_id = 0;
  int color   = 0;
  int x       = 0;
  int y       = 0;
  int y_step  = 0;

  psp_sdl_blit_help();
  
  x      = 10;
  y      =  5;
  y_step = 10;
  
  for (menu_id = 0; menu_id < MAX_MENU_SET_ITEM; menu_id++) {
    color = PSP_MENU_TEXT_COLOR;
    if (cur_menu_id == menu_id) color = PSP_MENU_SEL_COLOR;

    psp_sdl_back_print(x, y, menu_list[menu_id].title, color);
# if defined(WIZ_MODE)
    psp_menu_set_mouse_area( &menu_list[menu_id], x, y, y_step );
# endif

    if (menu_id == MENU_ANALOG) {
      if (psp_reverse_analog) strcpy(buffer,"yes");
      else                    strcpy(buffer,"no ");
      string_fill_with_space(buffer, 4);
      psp_sdl_back_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_CLOCK) {
      sprintf(buffer,"%d", psp_cpu_clock);
      string_fill_with_space(buffer, 4);
      psp_sdl_back_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SKIP_FPS) {
      sprintf(buffer,"%d", psp_skip_fps);
      string_fill_with_space(buffer, 4);
      psp_sdl_back_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_RENDER) {

      if (ti92_render_mode == TI92_RENDER_NORMAL) strcpy(buffer, "normal");
      else
      if (ti92_render_mode == TI92_RENDER_WIDTH) strcpy(buffer, "width");
      else
      if (ti92_render_mode == TI92_RENDER_X125) strcpy(buffer, "x125");
      else
      if (ti92_render_mode == TI92_RENDER_MAX) strcpy(buffer, "max");

      string_fill_with_space(buffer, 13);
      psp_sdl_back_print(140, y, buffer, color);
      y += y_step;
    } else
    if (menu_id == MENU_VIEW_FPS) {
      if (ti92_view_fps) strcpy(buffer,"on ");
      else              strcpy(buffer,"off");
      string_fill_with_space(buffer, 4);
      psp_sdl_back_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_RESET) {
      y += y_step;
    }

    y += y_step;
  }
}

static void
psp_settings_menu_clock(int step)
{
  if (step > 0) {
    psp_cpu_clock += 10;
    if (psp_cpu_clock > GP2X_MAX_CLOCK) psp_cpu_clock = GP2X_MAX_CLOCK;
  } else {
    psp_cpu_clock -= 10;
    if (psp_cpu_clock < GP2X_MIN_CLOCK) psp_cpu_clock = GP2X_MIN_CLOCK;
  }
}

static void
psp_settings_menu_skip_fps(int step)
{
  if (step > 0) {
    if (psp_skip_fps < 25) psp_skip_fps++;
  } else {
    if (psp_skip_fps > 0) psp_skip_fps--;
  }
}

static void
psp_settings_menu_render(int step)
{
  if (step > 0) {
    if (ti92_render_mode < TI92_LAST_RENDER) ti92_render_mode++;
    else                                   ti92_render_mode = 0;
  } else {
    if (ti92_render_mode > 0) ti92_render_mode--;
    else                     ti92_render_mode = TI92_LAST_RENDER;
  }
}

static void
psp_settings_menu_init(void)
{
  psp_reverse_analog = TI92.psp_reverse_analog;
  ti92_render_mode    = TI92.ti92_render_mode;
  ti92_view_fps      = TI92.ti92_view_fps;
  psp_skip_fps       = TI92.psp_skip_max_frame;
}

static void
psp_settings_menu_validate(void)
{
  /* Validate */
  TI92.psp_cpu_clock = psp_cpu_clock;
  TI92.psp_reverse_analog = psp_reverse_analog;
  TI92.ti92_render_mode    = ti92_render_mode;
  TI92.ti92_view_fps      = ti92_view_fps;
  TI92.psp_skip_max_frame = psp_skip_fps;
  TI92.psp_skip_cur_frame = 0;
  myPowerSetClockFrequency(TI92.psp_cpu_clock);
}

static void
psp_settings_menu_save_config()
{
  int error;

  psp_settings_menu_validate();
  error = ti92_save_configuration();

  if (! error) /* save OK */
  {
    psp_display_screen_settings_menu();
    psp_sdl_back_print(170, 110, "File saved !", 
                       PSP_MENU_NOTE_COLOR);
    psp_sdl_flip();
    sleep(1);
  }
  else 
  {
    psp_display_screen_settings_menu();
    psp_sdl_back_print(170, 110, "Can't save file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_sdl_flip();
    sleep(1);
  }
}

int
psp_settings_menu_exit(void)
{
  SceCtrlData c;

  psp_display_screen_settings_menu();
  psp_sdl_back_print(170, 110, "press B to confirm !", PSP_MENU_WARNING_COLOR);
  psp_sdl_flip();

  psp_kbd_wait_no_button();

  do
  {
    gp2xCtrlReadBufferPositive(&c, 1);
    c.Buttons &= PSP_ALL_BUTTON_MASK;

    if (c.Buttons & GP2X_CTRL_CROSS) psp_sdl_exit(0);

  } while (c.Buttons == 0);

  psp_kbd_wait_no_button();

  return 0;
}

static void
psp_settings_menu_save()
{
  int error;

  psp_settings_menu_validate();
  error = ti92_save_configuration();

  if (! error) /* save OK */
  {
    psp_display_screen_settings_menu();
    psp_sdl_back_print(170, 110, "File saved !", 
                       PSP_MENU_NOTE_COLOR);
    psp_sdl_flip();
    sleep(1);
  }
  else 
  {
    psp_display_screen_settings_menu();
    psp_sdl_back_print(170, 110, "Can't save file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_sdl_flip();
    sleep(1);
  }
}

void
psp_settings_menu_reset(void)
{
  psp_display_screen_settings_menu();
  psp_sdl_back_print(170,110, "Reset Settings !", 
                     PSP_MENU_WARNING_COLOR);
  psp_sdl_flip();
  ti92_default_settings();
  psp_settings_menu_init();
  sleep(1);
}

int 
psp_settings_menu(void)
{
  gp2xCtrlData c;
  long        new_pad;
  long        old_pad;
  int         last_time;
  int         end_menu;

  psp_kbd_wait_no_button();

  old_pad   = 0;
  last_time = 0;
  end_menu  = 0;

  psp_settings_menu_init();

  while (! end_menu)
  {
    psp_display_screen_settings_menu();
    psp_sdl_flip();

    while (1) {

      gp2xCtrlPeekBufferPositive(&c, 1);
      new_pad = c.Buttons;
# if defined(WIZ_MODE)
      if (c.Mouse) break;
# endif

      if ((old_pad != new_pad) || ((c.TimeStamp - last_time) > PSP_MENU_MIN_TIME)) {
        last_time = c.TimeStamp;
        old_pad = new_pad;
        break;
      }
    }

# if defined(WIZ_MODE)
    int chk_menu_id = -1;
    for (chk_menu_id = 0; chk_menu_id < MAX_MENU_SET_ITEM; chk_menu_id++) {
      int chk_area = psp_menu_is_mouse_area( &menu_list[chk_menu_id], &c);
      if (chk_area) {
        cur_menu_id = chk_menu_id;
        if (c.Mouse & GP2X_MOUSE_CLICK) {
          if (chk_area < 0) new_pad |= GP2X_CTRL_LEFT;
          else
          if (chk_area > 0) new_pad |= GP2X_CTRL_RIGHT;
        }
        break;
      }
    }
# endif

    if ((c.Buttons & GP2X_CTRL_RTRIGGER) == GP2X_CTRL_RTRIGGER) {
      psp_settings_menu_reset();
      end_menu = 1;
    } else
    if ((new_pad == GP2X_CTRL_LEFT ) || 
        (new_pad == GP2X_CTRL_RIGHT) ||
        (new_pad == GP2X_CTRL_CROSS) || 
        (new_pad == GP2X_CTRL_CIRCLE))
    {
      int step = 0;

      if (new_pad & GP2X_CTRL_RIGHT) {
        step = 1;
      } else
      if (new_pad & GP2X_CTRL_LEFT) {
        step = -1;
      }

      switch (cur_menu_id ) 
      {
        case MENU_ANALOG    : psp_reverse_analog = ! psp_reverse_analog;
        break;              
        case MENU_VIEW_FPS   : ti92_view_fps = ! ti92_view_fps;
        break;              
        case MENU_SKIP_FPS   : psp_settings_menu_skip_fps( step );
        break;              
        case MENU_SET_RENDER     : psp_settings_menu_render( step );
        break;              
        case MENU_SET_CLOCK      : psp_settings_menu_clock( step );
        break;
        case MENU_SET_SAVE       : psp_settings_menu_save();
                                   old_pad = new_pad = 0;
        break;                     
        case MENU_SET_RESET      : psp_settings_menu_reset();
        break;                     
                                   
        case MENU_SET_BACK       : end_menu = 1;
        break;                     
      }

    } else
    if(new_pad & GP2X_CTRL_UP) {

      if (cur_menu_id > 0) cur_menu_id--;
      else                 cur_menu_id = MAX_MENU_SET_ITEM-1;

    } else
    if(new_pad & GP2X_CTRL_DOWN) {

      if (cur_menu_id < (MAX_MENU_SET_ITEM-1)) cur_menu_id++;
      else                                     cur_menu_id = 0;

    } else  
    if(new_pad & GP2X_CTRL_SQUARE) {
      /* Cancel */
      end_menu = -1;
    } else 
    if(new_pad & GP2X_CTRL_SELECT) {
      /* Back to TI92 */
      end_menu = 1;
    }
  }
 
  if (end_menu > 0) {
    psp_settings_menu_validate();
  }

  psp_kbd_wait_no_button();

  psp_sdl_clear_screen( PSP_MENU_BLACK_COLOR );
  psp_sdl_flip();
  psp_sdl_clear_screen( PSP_MENU_BLACK_COLOR );
  psp_sdl_flip();

  return 1;
}

