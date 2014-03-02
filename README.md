mame-libretro
=============

Mainline MAME (0.152) for libretro (with libco) . WIP. Expect bugs

Source base is mame0.152 official source: 
http://www.mamedev.org/downloader.php?file=releases/mame0152s.zip

BUILD:

* build for pc linux/win:

     make -f Makefile.libretro -j4
     (NB: for 64 bits build export PTR64=1 at least on win64)

* build for android:

     for now you must build in 2 pass 
     make -f Makefile.libretro "NATIVE=1" buildtools
     make -f Makefile.libretro "platform=android" emulator -j4

CONTROLS: 

	RETRO_DEVICE_ID_JOYPAD_L 		[KEY_BUTTON_5]
	RETRO_DEVICE_ID_JOYPAD_R		[KEY_BUTTON_6]
	RETRO_DEVICE_ID_JOYPAD_R2		[KEY_TAB]
	RETRO_DEVICE_ID_JOYPAD_L2;		[KEY_F11]
	RETRO_DEVICE_ID_JOYPAD_R3		[KEY_F2]
	RETRO_DEVICE_ID_JOYPAD_L3;		[KEY_F3]
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

TODO:

* sample rate or refresh rate on the fly
* Map artwork, samples, and any other content to content directory if RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY is defined

Notes:

* Path management has been reworked:

If RetroArch includes RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY it will try to use the path defined in retroarch.cfg. 
If no path is defined or RetroArch doesn't yet include RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY then it will use 
retro_system_directory.

Paths are mapped like this:

- cfg_directory
retro_save_directory\mame\cfg
- nvram_directory
retro_save_directory\mame\nvram
- memcard_directory
retro_save_directory\mame\memcard
- input_directory
retro_save_directory\mame\input
- state_directory (MAME save states, not libretro save states, core doesn't implement those)
retro_save_directory\mame\states
- snapshot_directory
retro_save_directory\mame\snaps
- diff_directory
retro_save_directory\mame\diff

Also there are some optional paths that might hold additional data for mame like artwork, samples, etc.
Only the following have been defined so far:

- samplepath
retro_system_directory\mame\samples
- artpath
retro_system_directory\mame\artwork
- cheatpath
retro_system_directory\mame\cheat
- hashpath
retro_system_directory\mame\hash

Cheats need to be extracted, loading from a 7z won't work here
