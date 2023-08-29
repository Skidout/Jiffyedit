# Jiffyedit: Automate video editing tasks for Shotcut, Pitivi, LosslessCut, and Vidcutter, and directly export with MLT.

Note: Jiffyedit is not meant to completely replace manual editing, only to enhance it.
Also note: This is an early version. Expect a few bugs and for things to change.

## Usage:

  Enter ?, -h, -help, or --help for help, if you need it.
  The command will be all one line, structured like this:

    -i (path of file) [plugin name] editor
    ex. jiffyedit -i myvid.mp4 [sr] [st] shotcut
    or jiffyedit -i /home/user/Videos/myvid.mp4 [sr db 25 mt 0.5 bf 0.4 ] [st dur 0.2 ] pitivi

  Where (path of file) is the path of the video file you want to remove silence from.
  In the square brackets is the call for the plugin you wish to use. This may not match the name of the plugin. Only one clipper may be used at a time.
  Remember to put -i before the file.

  [sr] Silence Remove - cut video to only the parts where the audio is as loud as specified.

    bf # - Set buffer. A buffer at the beginning and end of each clip. Helps avoid jarring audio clipping. Must be a decimal number Default 0.3s.
    mt # - Set minimum clip length. - Having a minimum clip length helps avoid having many tiny clips Must be a decimal Default 0.5s.
    db # - Audio level for when silence ends and clips begin. Must NOT be a decimal. Default -20dB.
    smt # - Set minimum silence time. Helps to avoid having clips that are very close to each other, making them one clip instead. Cannot be less than two times buffer. Default two times buffer.

  [st] Smooth Transitions - add a fade-to-silence transition between clips to help ease the audio awkwardness.

    dur # - Set the duration of the fades. Default the length of two frames. Cannot be less than two frames for Shotcut.

  The resulting file will be in the same folder as your video, with the same name but the extension changed. If that file already exists, you will be asked if you want to overwrite it first. You can use the -overwrite option to specify that you want to overwrite any files, if they exist, so you will not be prompted. This is mostly for the purpose of developing your own plugins or contributing to the existing ones, so I do not recommend using it otherwise.

## Installation Instructions:

### Linux:

  If using Arch, install via the PKGBUILD. If you had it installed before the filter update, remember to remove it first.

  You must have GCC & FFmpeg installed. You may uninstall GCC after installation is complete.
  You do not need root permissions for the alternative installations.
  Download all the necessary files.
  Navigate to where you downloaded the files in the terminal.

    g++ -Ofast jiffyedit-master.cpp -o jiffyedit
    g++ -Ofast jiffyedit-sr.cpp -o jiffyedit-sr
    g++ -Ofast jiffyedit-st.cpp -o jiffyedit-st
    sudo install -D jiffyedit -t /usr/bin
    sudo install -D jiffyedit-sr -t /usr/lib/jiffyedit
    sudo install -D jiffyedit-st -t /usr/lib/jiffyedit
    sudo install -D jiffyedit-sr.dat -t /usr/lib/jiffyedit
    sudo install -D jiffyedit-st.dat -t /usr/lib/jiffyedit

  After that you can run it as you would other programs.

  Alternatively, for permissionless install, add the path of the resulting file as an alias for 'jiffyedit' in your bash/zsh/fish config file, ex:

    alias jiffyedit='/home/user/programs/jiffyedit'

  And change the path of the plugins in jiffyedit-master.cpp to reflect the actual path of the plugins (before compiling). (See the #ifdef.)

  If want to use git clone and then enter the cloned folder on your PC, you can compile using the same commands, then add it just like it says above.

### Windows:

    Find a C++17 or later compiler, compile it, then you're on your own after that. Keep in mind that the program itself requires the full path of the video file to be passed, and for all the clippers to be in the same directory as the master program, and it needs to be active in that directory. You might also need to specify the executable files as .exe in the .dat files. You will need to add an #ifdef for windows as well.

### Mac:

    Should be similar to Linux, but you may have to change where the plugins are located and change the working directory. If so, please do so using #ifdef and commit the change to this repo.

### Known bugs and tips:

  If a part of the video is cut out that you want to include, if using Shotcut or Pitivi, you can drag from the edges of the clip to recover more of the video to include.

  [sr] may be having issues, or maybe there is some new inconsistency with FFmpeg, possibly relating to the use of AV1 encoding. Don't ask me how that affects detecting silence in audio, but that's what it seems like.

### Roadmap:
Jiffyedit has some minor technical debt which I want to be worked out before I continue development much further. I want to implement shared libraries as an option for plugins to speed things up and there is code that can be reused and general cleanup that should be done. As long as it doesn't take me too long to learn how to use shared libraries, it won't take long after I actually start for that to be finished, but I also have some other things going on right now. For now, Jiffyedit is still useable as it is, so there is no need to rush.

- Add support for Openshot
- Make GUI
- Fix bugs

### For potential contributors:

The best way you can contribute is to add support for the editor you use or to make your own plugins. I want to keep the number of plugins on this repo to the two that are here, but I have created a new repo for extra plugins here: https://github.com/Skidout/jiffyedit-extra which contains a new plugin for clipping by voice activity. If you have some custom-made plugins you would like to contribute, you can do so there, as long as they are written in C++. If you have your own plugin you want to contribute to this repo, and you are willing to license it under this projects license, please get in touch with me via email at Skidout@proton.me, or make a pull request on the extra repo.

### Developer Documentation:

  In order to make and use a plugin, it needs a few things.

  The first is a .dat file in the same directory as the master program. It should look something like this:

    clipper/filterer
    Name
    executable name
    [call]
    (help section)
    [call], [call option example ]

  You can also put "ignore" anywhere on the first line and it should be ignored. See jiffyedit-sr.dat or jiffyedit-st.dat if you still need more info.

  The second is an executable in the same directory as the master program that can output its results in the format required by the master progam.

  For clippers, the format is:

    clipstart: #
    clipend: #

  The number should be floating point in a decimal format, not scientific. There cannot be a second clipstart before a clipend or vice versa.

  For filterers, the format depends on the editor in question.
  For Shotcut:

    # (the amount of filters your program will use, can be 0)
    (filter text)

  For Pitivi:

    (filter text)
    -volume-control: (filter text) - to control the volume of a clip using timestamps and values.
    -alpha control: (filter text) - to control the alpha of a clip using timestamps and values.

  In order to achieve this output, you likely need some information in order to do so. This is provided in the arguments.
  For clippers:

    (path of file) (user args, if any)

  For filterers, it depends on the editor.
  For Shotcut: 

    (path of file) shotcut (beginning timestamp of clip) (ending timestamp of clip) (clip number/total clip number) (filter number) (user args, if any)
    ex. /home/user/Videos/myvid.mp4 shotcut 4.067 8.678 3/50 4

  For Pitivi:

    (path of file) pitivi (beginning timestamp of clip) (ending timestamp of clip) (clip number/total clip number) (user args, if any)
    ex. /home/user/Videos/myvid.mp4 shotcut 4.067 8.678 3/50

For playlistfilters, only Shotcut/MLT is supported currently, and it looks like this:    

```
(path of file) shotcut (filter number) (project length) (project length minus 1 frame) (user args, if any)
ex. /home/user/Videos/myvid.mp4 shotcut 4.067 8.678 3/50
```

Those filters work the same as regular Shotcut filters but they are applied to the whole playlist, rather than each clip individually.

  LosslessCut does not support filters.
  Vidcutter also does not support filters.

  Output from filterers is not read live.

  For both types of plugins, you can put "Fatal error: " at the beginning of any line, followed by an error message, to end the execution of the master program, should you encounter an error which cannot be fixed during execution. This line will be passed to the user.
  Any lines that a clipper outputs that do not use the format should be passed to the user.
  Clippers can put "reset: " at the beginning of any line and the master program will reset to the beginning of the line. Useful for progress.

  Filterers can put "message: " at the beginning of any line to have it passed to the user.

  When making a filterer for Shotcut, there is one thing to know: 
  The first line of output needs to be the amount of filters that your filterer will add to the file. It can be 0 or no output if your filterer will not add any filters that time. This is necessary to keep a count of how many filters there are. The rest of the output will added directly to where filters would normally go in the file.

    A filter for MLT will be the same as Shotcut.

  When making a filterer for Pitivi, there are a couple things to know: 
  Normal output will be added directly to where filters would normally go. Some filters, however, may require that certain values that are usually handled by jiffyedit-master to be controlled by the filterer because Pitivi does not have a built-in way of having that filter. [st] is one such filter. 
  With Pitivi/Gstreamer, you can control audio levels and audio level slopes with keyframes/timestamps. If your plugin needs control of that, you can put "-alpha-control: " at the beginning of a line for video, and "-volume-control: " for audio. The following text will be placed inside where that would normally go. You have control over everything after "source_type='interpolation'" until the "/>".
