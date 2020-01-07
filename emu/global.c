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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#include <sys/stat.h>

#include "global.h"

TI92_t TI92;
int psp_screenshot_mode = 0;

int ti92_save_configuration(void)
{
  char chFileName[MAX_PATH + 1];
  FILE *FileDesc;
  int error = 0;

  strncpy(chFileName, TI92.ti92_home_dir, sizeof(chFileName) - 10);
  strcat(chFileName, "/ti92.cfg");

  FileDesc = fopen(chFileName, "w");
  if (FileDesc != (FILE *)0)
  {

    fprintf(FileDesc, "psp_reverse_analog=%d\n", TI92.psp_reverse_analog);
    fprintf(FileDesc, "ti92_render_mode=%d\n", TI92.ti92_render_mode);
    fprintf(FileDesc, "ti92_view_fps=%d\n", TI92.ti92_view_fps);
    fprintf(FileDesc, "psp_cpu_clock=%d\n", TI92.psp_cpu_clock);
    fprintf(FileDesc, "psp_skip_max_frame=%d\n", TI92.psp_skip_max_frame);
    fclose(FileDesc);
  }
  else
  {
    error = 1;
  }

  return error;
}

void myPowerSetClockFrequency(int cpu_clock)
{
  if (TI92.psp_current_clock != cpu_clock)
  {
    gp2xPowerSetClockFrequency(cpu_clock);
    TI92.psp_current_clock = cpu_clock;
  }
}

int ti92_parse_configuration(void)
{
  char chFileName[MAX_PATH + 1];
  char Buffer[512];
  char *Scan;
  unsigned int Value;
  FILE *FileDesc;

  strncpy(chFileName, TI92.ti92_home_dir, sizeof(chFileName) - 10);
  strcat(chFileName, "/ti92.cfg");

  FileDesc = fopen(chFileName, "r");
  if (FileDesc == (FILE *)0)
    return 1;

  while (fgets(Buffer, 512, FileDesc) != (char *)0)
  {

    Scan = strchr(Buffer, '\n');
    if (Scan)
      *Scan = '\0';
    /* For this #@$% of windows ! */
    Scan = strchr(Buffer, '\r');
    if (Scan)
      *Scan = '\0';
    if (Buffer[0] == '#')
      continue;

    Scan = strchr(Buffer, '=');
    if (!Scan)
      continue;

    *Scan = '\0';
    Value = atoi(Scan + 1);

    if (!strcasecmp(Buffer, "ti92_render_mode"))
      TI92.ti92_render_mode = Value;
    else if (!strcasecmp(Buffer, "ti92_view_fps"))
      TI92.ti92_view_fps = Value;
    else if (!strcasecmp(Buffer, "psp_cpu_clock"))
      TI92.psp_cpu_clock = Value;
    else if (!strcasecmp(Buffer, "psp_skip_max_frame"))
      TI92.psp_skip_max_frame = Value;
    else if (!strcasecmp(Buffer, "psp_reverse_analog"))
      TI92.psp_reverse_analog = Value;
  }

  TI92.psp_skip_cur_frame = 0;

  fclose(FileDesc);

  myPowerSetClockFrequency(TI92.psp_cpu_clock);

  return 0;
}

void ti92_default_settings()
{
  TI92.psp_reverse_analog = 0;
  TI92.ti92_view_fps = 0;
  TI92.psp_cpu_clock = GP2X_DEF_EMU_CLOCK;
  TI92.psp_skip_max_frame = 0;
  TI92.psp_skip_cur_frame = 0;
  TI92.ti92_render_mode = TI92_RENDER_MAX;

  myPowerSetClockFrequency(TI92.psp_cpu_clock);
}

void ti92_update_save_name()
{
  char TmpFileName[MAX_PATH];
  struct stat aStat;
  int index;
  char *SaveName;
  char *Scan1;
  char *Scan2;

  for (index = 0; index < TI92_MAX_SAVE_STATE; index++)
  {
    TI92.ti92_save_state[index].used = 0;
    memset(&TI92.ti92_save_state[index].date, 0, sizeof(time_t));

    snprintf(TmpFileName, MAX_PATH, "%s/save/sav_%d.stz", TI92.ti92_home_dir, index);
    if (!stat(TmpFileName, &aStat))
    {
      TI92.ti92_save_state[index].used = 1;
      TI92.ti92_save_state[index].date = aStat.st_mtime;
    }
  }
}

void ti92_kbd_load(void)
{
  char TmpFileName[MAX_PATH + 1];
  struct stat aStat;

  snprintf(TmpFileName, MAX_PATH, "%s/default.kbd", TI92.ti92_home_dir);
  if (!stat(TmpFileName, &aStat))
  {
    psp_kbd_load_mapping(TmpFileName);
  }
}

int ti92_kbd_save(void)
{
  char TmpFileName[MAX_PATH + 1];
  snprintf(TmpFileName, MAX_PATH, "%s/default.kbd", TI92.ti92_home_dir);
  return (psp_kbd_save_mapping(TmpFileName));
}

char home_path[512];
char psp_screenshot_path[512];

void ti92_init_resource()
{
  // Get init file directory & name
  strcpy(TI92.ti92_install_dir, ".");
  fprintf(stderr, "START: ti92_install_dir: %s\n", TI92.ti92_install_dir);

  // Get init file directory & name
  snprintf(TI92.ti92_home_dir, sizeof(TI92.ti92_home_dir), "%s/.ti92", getenv("HOME"));
  mkdir(TI92.ti92_home_dir, 0777);
  fprintf(stderr, "START: ti92_home_dir: %s\n", TI92.ti92_home_dir);

  snprintf(TI92.psp_screenshot_path, sizeof(TI92.psp_screenshot_path), "%s/.ti92/scr", getenv("HOME"));
  mkdir(psp_screenshot_path, 0777);
  fprintf(stderr, "START: psp_screenshot_path: %s\n", TI92.psp_screenshot_path);

  char ti92_tempdir[512];
  snprintf(ti92_tempdir, sizeof(ti92_tempdir), "%s/.ti92/save", getenv("HOME"));
  mkdir(ti92_tempdir, 0777);
  fprintf(stderr, "START: psp_save_path: %s\n", ti92_tempdir);

  snprintf(ti92_tempdir, sizeof(ti92_tempdir), "%s/.ti92/kbd", getenv("HOME"));
  mkdir(ti92_tempdir, 0777);
  fprintf(stderr, "START: psp_kbd_path: %s\n", ti92_tempdir);

  char ti92_tempfile_source[512];
  char ti92_tempfile_dest[512];
  snprintf(ti92_tempfile_source, sizeof(ti92_tempfile_source), "./%s", "ti92.mem");
  snprintf(ti92_tempfile_dest, sizeof(ti92_tempfile_dest), "%s/%s", TI92.ti92_home_dir, "ti92.mem");
  fprintf(stderr, "START: ti92_copy_file source: %s\n", ti92_tempfile_source);
  fprintf(stderr, "START: ti92_copy_file dest: %s\n", ti92_tempfile_dest);
  ti92_copy_file(ti92_tempfile_source, ti92_tempfile_dest);

  snprintf(ti92_tempfile_source, sizeof(ti92_tempfile_source), "./%s", "ti92.rom");
  snprintf(ti92_tempfile_dest, sizeof(ti92_tempfile_dest), "%s/%s", TI92.ti92_home_dir, "ti92.rom");
  fprintf(stderr, "START: ti92_copy_file source: %s\n", ti92_tempfile_source);
  fprintf(stderr, "START: ti92_copy_file dest: %s\n", ti92_tempfile_dest);
  ti92_copy_file(ti92_tempfile_source, ti92_tempfile_dest);

  snprintf(ti92_tempfile_source, sizeof(ti92_tempfile_source), "./%s", "ti92.cfg");
  snprintf(ti92_tempfile_dest, sizeof(ti92_tempfile_dest), "%s/%s", TI92.ti92_home_dir, "ti92.cfg");
  fprintf(stderr, "START: ti92_copy_file source: %s\n", ti92_tempfile_source);
  fprintf(stderr, "START: ti92_copy_file dest: %s\n", ti92_tempfile_dest);
  ti92_copy_file(ti92_tempfile_source, ti92_tempfile_dest);

  TI92.psp_screenshot_id = 0;

  // Leemos la configuración por defecto
  ti92_default_settings();
  psp_kbd_default_settings();
  ti92_parse_configuration();
  ti92_update_save_name();

  ti92_kbd_load();
}

int ti92_file_exists(const char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else // -1
        return 0;
}

int ti92_copy_file(const char *source, const char *destination)
{
    if(ti92_file_exists(destination)) {
        fprintf(stderr, "File %s exist.\n", destination);
        return 0;
    }
    else{
      fprintf(stderr, "File %s does not exist.\n", destination);
      int input, output;
      if ((input = open(source, O_RDONLY)) == -1)
      {
        return -1;
      }
      if ((output = creat(destination, 0660)) == -1)
      {
        close(input);
        return -1;
      }

      //sendfile will work with non-socket output (i.e. regular file) on Linux 2.6.33+
      off_t bytesCopied = 0;
      struct stat fileinfo = {0};
      fstat(input, &fileinfo);
      int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);

      close(input);
      close(output);

      return result;
    }   
}

int ti92_snapshot_save_slot(int save_id)
{
  char FileName[MAX_PATH + 1];
  struct stat aStat;
  int error;

  error = 1;

  if (save_id < TI92_MAX_SAVE_STATE)
  {
    snprintf(FileName, MAX_PATH, "%s/save/sav_%d.stz", TI92.ti92_home_dir, save_id);
    error = ti92_save_state(FileName);
    if (!error)
    {
      if (!stat(FileName, &aStat))
      {
        TI92.ti92_save_state[save_id].used = 1;
        TI92.ti92_save_state[save_id].date = aStat.st_mtime;
      }
    }
  }

  return error;
}

int ti92_snapshot_load_slot(int load_id)
{
  char FileName[MAX_PATH + 1];
  int error;

  error = 1;

  if (load_id < TI92_MAX_SAVE_STATE)
  {
    snprintf(FileName, MAX_PATH, "%s/save/sav_%d.stz", TI92.ti92_home_dir, load_id);
    error = ti92_load_state(FileName);
  }
  return error;
}

int ti92_snapshot_del_slot(int save_id)
{
  char FileName[MAX_PATH + 1];
  int error;

  error = 1;

  if (save_id < TI92_MAX_SAVE_STATE)
  {
    snprintf(FileName, MAX_PATH, "%s/save/sav_%d.stz", TI92.ti92_home_dir, save_id);
    error = remove(FileName);
    if (!error)
    {
      TI92.ti92_save_state[save_id].used = 0;
      memset(&TI92.ti92_save_state[save_id].date, 0, sizeof(time_t));
    }
  }

  return error;
}
