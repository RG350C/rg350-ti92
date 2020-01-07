/*
 *  xtiger port on PSP 
 *
 *  Copyright (C) 2006 Ludovic Jacomme (ludovic.jacomme@gmail.com)
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

#ifndef _GLOBAL_H
#define _GLOBAL_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#include "gp2x_psp.h"
#include "gp2x_cpu.h"
#include <time.h>

# ifndef CLK_TCK
# define CLK_TCK  CLOCKS_PER_SEC
# endif

# define TI92_RENDER_NORMAL     0
# define TI92_RENDER_WIDTH      1
# define TI92_RENDER_X125       2
# define TI92_RENDER_MAX        3
# define TI92_LAST_RENDER       3

# ifndef MAX_PATH
# define MAX_PATH 128
# endif

#define TI92_WIDTH  240
#define TI92_HEIGHT 128

#define TI92_MAX_SAVE_STATE    5

  typedef struct TI92_save_t {

    time_t          date;
    char            used;

  } TI92_save_t;

   typedef struct TI92_t {

     TI92_save_t   ti92_save_state[TI92_MAX_SAVE_STATE];

     char          ti92_install_dir[MAX_PATH];
     char          ti92_home_dir[MAX_PATH];
     char          psp_screenshot_path[MAX_PATH];
     char          edit_filename[MAX_PATH];
     int           psp_screenshot_id;
     int           psp_reverse_analog;
     int           ti92_render_mode;
     int           ti92_view_fps;
     int           psp_cpu_clock;
     int           psp_current_clock;
     int           ti92_current_fps;
     int           psp_skip_max_frame;
     int           psp_skip_cur_frame;
 
   } TI92_t;

  extern TI92_t TI92;
  extern int  psp_screenshot_mode;

  extern int  psp_exit(int);

  extern void ti92_init_resource();
  extern int  ti92_parse_configuration();
  extern int  ti92_save_configuration();

# ifdef DEBUG
  extern void pspDebugPrintf(const char *Format, ...);
# endif

#endif 
