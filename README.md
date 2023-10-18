# Serious Engine

This is a fork of Croteam's Serious Engine 1.10 that aims to provide code that is easier to maintain and build under any sort of configuration and that also includes many quality-of-life improvements over the original code without adding too much.

### Engine components
- `DedicatedServer` - Dedicated server application for hosting multiplayer game
- `Ecc` - Entity Class Compiler for compiling entity source files (`*.es`)
- `Engine` - Serious Engine 1.10
- `EngineGUI` - Common GUI components for game tools
- `EntitiesMP` - All the entity logic
- `GameGUIMP` - Specific GUI components for game tools
- `GameMP` - Module for handling basic game logic
- `SeriousSam` - The main game executable
- `Shaders` - Compiled shaders for SKA models

### Engine tools
- `DecodeReport` - Tool for decoding crash report files (`*.rpt`)
- `Depend` - Tool for generating a list of dependency files based on a list of root files
- `MakeFONT` - Tool for generating font files for the game (`*.fnt`)
- `Modeler` - Serious Modeler application for creating and configuring models with vertex animations
- `RCon` - Remote console application for connecting to servers using an admin password
- `SeriousSkaStudio` - Serious SKA Studio application for creating and configuring models with skeletal animations
- `WorldEditor` - Serious Editor application for creating in-game levels

### Other projects
- `LWSkaExporter` - Exporter of SKA models in ASCII format for use in LightWave
- `GameAgent` - Custom master server emulator written in Python
- `libogg`, `libvorbis` - Third party libraries used for playing OGG-encoded ingame music (see http://www.vorbis.com/ for more information)
- `zlib` - Third party static library for working with ZIP archives (see https://zlib.net/ for more information)

These have been modified to run correctly under the recent versions of Windows. (Tested: Win7 x64, Win8 x64, Win8.1 x64, Win10 x64)

# Building

To build Serious Engine 1, you will need **Visual Studio 2010** or later. The engine was originally written using **Visual C++ 6.0** and released to the public modified to build and run correctly using **Visual Studio 2013**.

Once you've installed **Visual Studio** and (optionally) DirectX8 SDK, you can build the engine solution (`Sources/All.sln`). Press F7 or **Build** -> **Build solution**. Once the project is build, the libraries and executables will be put under the `Bin` directory (or `Bin/Debug` if you are using the Debug configuration).

# Optional features

These features are disabled by default but can be enabled if you wish to extend the capabilities of your Serious Engine build.

- **DirectX:** Download DirectX8 SDK (headers & libraries) ( https://www.microsoft.com/en-us/download/details.aspx?id=6812 ) and then define a `SE1_D3D` macro for all projects in the solution (you can do it by adding it to the `SE1GenericPreproc` property inside `Sources/Properties/Common.props`). You will also need to make sure the DirectX8 headers and libraries are located in the following folders (make the folder structure if it doesn't exist):
  - `Tools.Win32/Libraries/DX8SDK/Include/`
  - `Tools.Win32/Libraries/DX8SDK/Lib/`
- **MP3 playback:** Copy `amp11lib.dll` library into the 'Bin' directory (and 'Bin/Debug' in Debug mode) that used to be distributed with older versions of **Serious Sam Classic: The First Encounter**.
- **3D Exploration**: Support is disabled due to copyright issues. If you need to create new models, you will have to either use editing tools from any of the original games, or write your own code for 3D object import/export.
- **IFeel:** Support is disabled due to copyright issues. If you need IFeel support, you will have to copy `IFC22.dll` and `ImmWrapper.dll` from the original games into the `Bin` folder.

# Running

This version of the engine comes with a set of resources (`SE1_10.gro`) that allow you to freely use the engine without any additional resources required. However if you want to open or modify levels from **Serious Sam Classic: The First Encounter** or **The Second Encounter** (including most user-made levels), you will have to copy the game's resources (`.gro` files) into the game folder.

When running a selected project, make sure that its project settings under **Debugging** are setup correctly:
- Command: `$(PostBuildCopyDir)$(TargetFileName)`
- Working Directory: `$(SolutionDir)..\`

# Common problems

- `SeriousSkaStudio` has some issues with MFC windows that can prevent the main window from being displayed properly.
- Even though **Visual Studio 2010** can be used for building, its compiler seems to be introducing certain bugs that are not ironed out yet, so it's suggested that you use **Visual Studio 2013** or higher. Serious Engine 1.10 was initially released for these versions.

# License

Serious Engine is licensed under the GNU GPL v2 (see `LICENSE` file).

Some of the code included with the engine sources under `Sources/ThirdParty` is not licensed under the GNU GPL v2:

- **zlib** (located in `zlib`) by Jean-loup Gailly and Mark Adler
- **LightWave SDK** (located in `LWSkaExporter/SDK`) by NewTek Inc.
- **libogg** & **libvorbis** (located in `libogg` and `libvorbis`) by Xiph.Org Foundation
