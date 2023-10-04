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
