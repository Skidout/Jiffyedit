Jiffedit: Remove silence from videos into the Shotcut MLT format.

Note: Jiffyedit is not meant to completely replace manual editing, only to enhance it.
Also note: This is an extremely early version. Expect a few bugs and for things to change.

Installation Instructions:

  Linux:
  If using Arch, simply run "sudo pacman -U" on the package (.pkg.tar.zst) and you can use jiffyedit by typing it into the terminal. The package should be getting added to the AUR soon.
  
    You must have GCC & FFmpeg installed.
    You do not need sudo permissions for the alternative installations.
    Download jiffyedit.cpp.
    Navigate to where you downloaded jiffyedit.cpp in the terminal.
    Run 'g++ -fsanitize=address -fstack-usage -g jiffyedit.cpp -o jiffyedit'
    Run 'sudo install -D jiffyedit -t /usr/bin'
    After that you can run it simply by typing jiffyedit in the terminal
    
    Alternatively:
    Add the path of the resulting file as an alias for 'jiffyedit' in your bash/zsh/fish config file, ex:
      alias jiffyedit='/home/user/programs/jiffyedit'
      Video: https://odysee.com/@TOSkidout:d/jiffyedit-guide:6
      
    Or you can simply navigate directly to the executable when you want to use it.
    
    
    If want to use git clone and then enter the cloned folder on your PC, you can compile using the same command, then add it just like it says above.
  
  Windows:
  
    Find a c++17 or later compiler, compile it, then you're on your own after that. I don't use windows, hopefully someone will make better instructions soon.
    
  Mac:
  
    Same as windows.
    

Usage:

  Start the program.
  Enter ? for help, if you need it.
  The command will be all one line, structured like this:
    (PATH OF FILE) -inpf sr sr-bf#
    ex. "/home/user/Videos/MyVideo.mp4 sr sr-bf0.2 sr-db15 sr-mt1.0
    
    Where (PATH OF FILE) is the path of the video file you want to remove silence from.
    -inpf tells the program what the path is. You can use '' or "" if you want, but you don't have to.
    sr is for silence remove.
    sr-bf# specifies the amount of buffer you want to leave at the beginning and end of each clip. With no buffer, some clipping can occor. Default 0.3s.
    sr-db# specifies the max audio level, in decibels, for what will be counted as silence. Default -20 dB. For best results, enter it as positive.
    sr-mt# specifies the minimum length that a clip can be to be included in the final result. Otherwise, you would have a lot of tiny clips. Default 0.5s.
  
  The MLT project file will be in the same folder as your video, with the same name. If that file already exists, you will be asked if you want to overwrite it first.
  

Known bugs and tips:

  Sometimes the very last clip will not be included in the MLT project file. If you need it, you can drag out from end of the last clip in Shotcut and edit it in manually.


For potential contributors:

  Keep in mind that this is a very early version of this software. I currently plan to almost completely restructure it to make it more versatile. If you want to contribute, you are of course welcome to, but your contribution may not make it into the next major version.
  I will also generally clean up the code as time goes on, but I don't suspect that there will be many updates to this early version before the restructuring.
  
Roadmap:
 - Fix bugs.
 - Restructure the program.
 - Add support for longer videos.
 - Add support for auto filters.
 - Add support for Kdenlive.
