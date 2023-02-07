# Jiffyedit: Remove silence from videos into the Shotcut MLT format +more soon.

Note: Jiffyedit is not meant to completely replace manual editing, only to enhance it.
Also note: This is an early version. Expect a few bugs and for things to change.

## Installation Instructions:

###  Linux:
  
  You must have GCC & FFmpeg installed.
  You do not need root permissions for the alternative installations.
  Download all the necessary files.
  Navigate to where you downloaded the files in the terminal.
  
    sudo install -D jiffyedit -t /usr/bin
    g++ -fsanitize=address -fstack-usage -g jiffyedit.cpp -o jiffyedit
    g++ -fsanitize=address -fstack-usage -g jiffyedit-sr.cpp -o jiffyedit-sr
    sudo install -D jiffyedit -t /usr/lib/jiffyedit
    sudo install -D jiffyedit-sr -t /usr/lib/jiffyedit
    sudo install -D jiffyedit-sr.dat -t /usr/lib/jiffyedit
      
  After that you can run it simply by typing jiffyedit in the terminal
    
  Alternatively, for permissionless install:
    Add the path of the resulting file as an alias for 'jiffyedit' in your bash/zsh/fish config file, ex:
    
      alias jiffyedit='/home/user/programs/jiffyedit'
            
Or you can simply navigate directly to the executable when you want to use it.
If want to use git clone and then enter the cloned folder on your PC, you can compile using the same command, then add it just like it says above.
  
###  Windows:
  
    Find a C++17 or later compiler, compile it, then you're on your own after that. Keep in mind that the program itself requires the full path of the video file to be passed, and for all the clippers to be in the same directory as the master program, and it needs to be called from that directory. You might also need to specify the executable files as .exe where necessary.
    
###  Mac:
  
    Same as windows.
    

### Usage:

  Enter ? for help, if you need it.
  The command will be all one line, structured like this:
  
    (PATH OF FILE) [sr]
    ex. jiffyedit /home/user/Videos/myvid.mp4 [sr]
    or jiffyedit /home/user/Videos/myvid.mp4 [sr db25 mt0.5 bf0.4 ]
   
  Where (PATH OF FILE) is the path of the video file you want to remove silence from.
  [sr] is for silence remove.
  bf# - Set buffer. A buffer at the beginning and end of each clip. Helps avoid jarring audio clipping. Must be a decimal number Default 0.3s.
  mt# - Set minimum clip length. - Having a minimum clip length helps avoid having many tiny clips Must be a decimal Default 0.5s.
  db# - Audio level for when silence ends and clips begin. Must NOT be a decimal. Default -20dB.
  
  The MLT project file will be in the same folder as your video, with the same name. If that file already exists, you will be asked if you want to overwrite it first.
  

### Known bugs and tips:

  If a part of the video is cut out that you want to inculde, if using Shotcut, you can drag from the edges of the clip to recover more of the video to include.
  
### Roadmap:
 - Fix bugs.
 - Add support for Kdenlive.
 - Add support for auto filters.

### For potential contributors:

  The best way you can contribute is to add support for the editor you use or to make your own clippers. I want to keep the number of clippers (and languages) on this repo to a minimum, but I have thought about making another repo that would contain clippers that have full support and are up to date but don't come packaged by default.
 
### Developer Documentation:
  When a user decides to use a clipper, the master program will start the clipper. The first argument passed will be the full path to the video file. After this will be whatever is in between the [clippername and ]. If there is nothing but an empty call, no additional arguments will be passed.
  The master program expects the start of a clip to be a floating point variable output right after "clipstart: " (see jiffyedit-sr.cpp). It will expect the end of a clip to be the same but "clipend: " rather than clipstart.
  If a fatal error occurs, you can tell the master program by starting a line with "Fatal error: ". This line will be passed to the user.
  Any lines that do not fit within these categories will also be passed to the user.
  * The master program does not read this data live, but only after the execution of the clipper has finished.
  
  For the master program to be able to see your clipper, it needs a .dat file in the same directory. The file can be called anything, so long as it ends in .dat. Inside the .dat file, on the first line, should be the type of sub-process this will be. For now, only clippers are supported so put "clipper". If for some reason you need to put a .dat file inside this directory that does not relate to your sub-process, you can put "ignore" anywhere on the first line and it will be ignored. The second line will be the name of the sub-process. The third will be the filename of the executable (not the full path). The fourth line defines the call the user will use to call your sub-process. It also defines the line before the help section. The help section ends when you put the call at the beginning of a line. That whole line will be passed, so you can use it to make an example command like I did (see jiffyedit-sr.dat).
