# EmpireEarthZoomChanger

This tool changes the zoom distance for the games "Empire Earth" and its expansion pack "Empire Earth The Art of Conquest" by dynamically binary patching the executable.
The tool is built with C++ and Qt5.

## Recommended Values
For 1920x1080 resolution, I recommend the following values:
- Zoom: -40.5
- Rear Clipping: 55.0

## Releases
The latest `Github Actions`-based binaries (x32, i686) can be found at the [release page](https://github.com/j-frei/EmpireEarthZoomChanger/releases/latest).

## How To Use
Download the latest release of `EmpireEarthZoomChanger.exe` from the release page and run it.  
**Note:** The SmartScreen from Windows Defender might block the execution initially. You may need to bypass/allow it.

Once you see the program GUI, load the `EmpireEarth.exe` or `EE-AOC.exe` file, update the values, and write the new values.  
**Note:** If the game files are write-protected, the program may be unable to overwrite the files. In this case, either run the `EmpireEarthZoomChanger.exe` as administrator, or copy the `EmpireEarth.exe` or `EE-AOC.exe` file to the Desktop, edit this file with `EmpireEarthZoomChanger.exe`, and copy the modified file back into the game directory.
