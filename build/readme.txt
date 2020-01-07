Welcome to RG350-TI92

Original Author of XTiger

  Tiger is a TI-92 emulator for Linux/x86.  It was originally written by Jonas Minnberg and was closed source.  There were two versions: the SVGA version and an Xlib version.  Jonas Minnberg has not worked on XTiger for quite a while and he gave Misha Nasledov the code and permission to GPL it.

Author of the PSP, GP2X and CAANOO ports versions

  Ludovic.Jacomme (ludovic.jacomme@gmail.com) also known as Zx-81

  Special thanks to CousinWeb for giving me its TI92 during the PSP port dev.

XTiger : TI 92 Emulator for Unix/X11 

  Jonas Minnberg <j.minnberg@upright.se>
  Misha Nasledov <misha@nasledov.com>

  PSPXTI : Porting on PSP

  Ludovic Jacomme (zx81.zx81@gmail.com)

  GP2X-TI92   : Porting on GP2X F100
  WIZ-TI92    : Porting on GP2X WIZ
  Caanoo-TI92 : Porting on Caanoo


# INTRODUCTION

  This is a port on RG350 of the Texas Instrument 92 emulator for X Window Systems : Xtiger.

  This package is under GPL Copyright, read COPYING file for more information about it.


# INSTALLATION

  Unzip the zip file, and copy the content of the directory game to your SD card.

  TI92+ > v1.1 Roms are not compatible with this emulator !

  For any comments or questions on this version, please visit http://zx81.zx81.free.fr or http://www.gp32x.com/


# CONTROL

3.1 - Virtual keyboard

  In the editor window, press Select to open/close the On-Screen keyboard.

  Use Analog stick to choose one of the 9 squares, and use Y, A, Cross and B to choose one of the 4 letters of the highlighted square.

  Select  Disable virtual keyboard
  
  L/R     Navigate between different keyboard panels 

2.2 - Standard keys

  In the TI92 emulator window, there are three different mapping (standard,
  left trigger, and right Trigger mappings).  You can toggle between while
  playing inside the emulator using the two trigger keys.

  -------------------------------------
  RG350        TI-92          (standard)
  
    A           Backspace
    B           Space 
    Y           Escape 
    X           Enter1
    Up          Up
    Down        Down
    Left        Left
    Right       Right

  -------------------------------------
  RG350        TI-92    (left)

    A           Apps   
    B           Diamond 
    Y           Hand   
    X           2nd   
    Up          F1
    Down        F2  
    Left        F3   
    Right       F4   

  -------------------------------------
  RG350        TI-92    (right)

    A           (     
    B           )    
    Y           Diamond
    X           ,     
    Up          Up
    Down        Down
    Left        Left
    Right       Right

  Press Menu   to enter in emulator main menu.

  Press Select open/close the On-Screen keyboard

  In the main menu

    RTrigger   Reset the emulator

    Y   Go Up directory
    X   Valid
    B   Valid
    A   Go Back to the emulator window


3.3 Touch screen

  This version supports the touch screen.

3.3.1 - On the virtual keyboard panel

  Using the touch screen you can click on the letter you want, and drawing a line vertically or horizontally (on the virtual keyboard screen) you can navigate between the different keyboard panels.

3.3.2 - Editor menu 

  The touch screen can be used to move the cursor on a position with a simple click. And then you can :

    draw a line up    : page up
    draw a line down  : page down
    draw a line left  : first column of the current line
    draw a line right : last column of the current line

3.3.3 - Main menu

  You can select the line you want and click to select the option you want.

3.3.4 - File requester menu

  You can use the virtual keyboard as previously described.  If you press the touch screen in the first line, current selection goes up, and if you press the touch screen in the last line, current selection goes down. You can draw a line left / right to do page up / down.

  You can open the file you want by a simple click.

3.3.5 - Help menu

  draw a line up/left     : page up
  draw a line down/right  : page down

3.3.6 - Settings menu

  You can select the line you want and click to select the option you want.

  Just click on left or right part of the menu option name to respectively decrease or increase its value.

3.3.7 - Emulator window

  You can use the virtual keyboard as previously described.


# LOADING TI-92 APPLICATION FILES

  You can save any TI 92 files (with .92* file extension) on your SD card. 

  It may happen that the file is not properly loaded, and so you have to try again.

  To manage applications already loaded in your TI-92, you can use the VLINK menu.

  You can find many applications on http://www.ticalc.org/ and on the official TI ftp site ftp://ftp.ti.com/pub/graph-ti/calc-apps/92/


# LOADING KEY MAPPING FILES

  If you want to modify the default keyboard mapping between RG350 Keys and TI-92 keys you can write your own mapping file. Using notepad for example you can edit a file with the .kbd extension and put it in the kbd directory.

  After writting such keyboard mapping file, you can load them using the keyboard menu.


# COMPILATION

  It has been developped under Linux FC9 using gcc with GCW0 SDK.  All tests have been done using a RG350.  To rebuild the homebrew run the Makefile in the src archive.


  Enjoy,
  
         Zx


