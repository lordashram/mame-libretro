MAME.0149-LIBRETRO WIP

Very WIP port MAME 0.149 for libretro using libco.


AND BE AWARE OFF THIS WHEN YOU USE IT (AT YOUR OWN RISK).
 
Source base is mame0.149 official source: http://www.mamedev.org/downloader.php?file=releases/mame0149s.zip

OSD code is highly inspired by source code of the SDL osd of the official mame build and by others mame ports :

- librerto-mame0.78 port : https://github.com/libretro/mame2003-libretro
- ps3 mame0.125 port     : http://www.volny.cz/molej/ps3/mame_ps3.htm
- mame4droid             : http://code.google.com/p/imame4all/source/browse/


Build :

make -f Makefile.libretro

Usage : 

 rompath , inipath and others must be set in mame.ini and the initial inipath is hardcoded.

 for PC        to	   ./
 for ANDROID   to 	   /mnt/sdcard/

 Once mame.ini is in the good place , launch retroarch , select game 
 
 controls are: 

	RETRO_DEVICE_ID_JOYPAD_L 		[KEY_ESCAPE]
	RETRO_DEVICE_ID_JOYPAD_R		[KEY_F11]
	RETRO_DEVICE_ID_JOYPAD_R2		[KEY_TAB]
	RETRO_DEVICE_ID_JOYPAD_L2;		[KEY_ENTER]
	RETRO_DEVICE_ID_JOYPAD_START		[KEY_START]
	RETRO_DEVICE_ID_JOYPAD_SELECT		[KEY_COIN]
	RETRO_DEVICE_ID_JOYPAD_A		[KEY_BUTTON_1]
	RETRO_DEVICE_ID_JOYPAD_B		[KEY_BUTTON_2]
	RETRO_DEVICE_ID_JOYPAD_X		[KEY_BUTTON_3]
	RETRO_DEVICE_ID_JOYPAD_Y		[KEY_BUTTON_4]
	RETRO_DEVICE_ID_JOYPAD_UP		[KEY_JOYSTICK_U]
	RETRO_DEVICE_ID_JOYPAD_DOWN		[KEY_JOYSTICK_D]
	RETRO_DEVICE_ID_JOYPAD_LEFT		[KEY_JOYSTICK_L]
	RETRO_DEVICE_ID_JOYPAD_RIGHT		[KEY_JOYSTICK_R]

        tips: R2 to tab and select newgame from mameui


