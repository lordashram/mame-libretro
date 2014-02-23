mame-libretro
=============

Mainline MAME (0.152) for libretro (with libco) . WIP. Expect bugs

TODO:

* Change aspect ratio, sample rate or refresh rate on the fly
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


