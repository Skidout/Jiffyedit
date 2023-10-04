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

  You must have Clang, LLVM, LLD, libc++, & FFmpeg installed. You may uninstall Clang, LLVM, LLD, & libc++ after installation is complete.
  You do not need root permissions for the alternative installations.
  Download all the necessary files.
  Navigate to where you downloaded the files in the terminal.

    clang++ -Ofast -std=c++17 -stdlib=libc++ -fuse-ld=lld jiffyedit-master.cpp -o jiffyedit
    clang++ -Ofast -std=c++17 -stdlib=libc++ -fuse-ld=lld jiffyedit-sr.cpp -o jiffyedit-sr
    clang++ -Ofast -std=c++17 -stdlib=libc++ -fuse-ld=lld jiffyedit-st.cpp -o jiffyedit-st
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

### Roadmap:

- Restructure again with support for shared library plugins and more options.
- Add support for Openshot
- Make GUI
- Fix bugs

### For potential contributors:

The best way you can contribute is to add support for the editor you use or to make your own plugins. I want to keep the number of plugins on this repo to the two that are here, but I have created a new repo for extra plugins here: https://github.com/Skidout/jiffyedit-extra which contains a new plugin for clipping by voice activity. If you have some custom-made plugins you would like to contribute, you can do so there, as long as they are written in C++. If you have your own plugin you want to contribute to this repo, and you are willing to license it under this projects license, please get in touch with me via email at Skidout@proton.me, or make a pull request on the extra repo.

If you want to learn how to make a plugin, see make-a-plugin.md.
