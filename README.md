# Rich Presence for Autodesk Maya

![image](https://github.com/user-attachments/assets/97085773-4c72-4a26-97f2-c11133ac9e9c)

This plug-in will update your status on Discord depending on your scene and project.

Currently has been tested in Maya 2023-2026. Earlier versions could work, but compatibility is not guaranteed and compiled binaries won't be provided.

### V2 Disclaimer
With v2, the plug-in has been updated to make use of Discord's Social Sdk. While this should overall work better and support more features, there have been some minor regressions. Primarily the ability to hide the timestamp is completely gone.
The Maya version is also not displayed anymore as every version requires a separate registered application, so this was simplified.

Latest compiled release available [here](https://github.com/aronamao/Discord-Rich-Presence-For-Maya/releases/latest)

## Installation

Simply download the release .zip and extract the contents to a location of your choice. From there open Maya and drag'n'drop the installer.py file into your Maya window. A new dialog should pop-up:

![image](https://github.com/user-attachments/assets/01c7273b-a153-4cd4-845a-672020dcd3ab)

Simply hit install.

## Uninstalling
Delete everything named DRPForMaya in your maya modules folder (<Documents>/maya/modules).

## Details
Once you enable the "DRPForMaya.mll" plug-in in the plug-in manager your discord status should immediately update.
A new menu will be registered, which you can use to open the settings:

![image](https://github.com/user-attachments/assets/5262e7f8-0a4f-4a92-ac5e-532d6ed54617)

There's a few options that can be changed:

![image](https://github.com/user-attachments/assets/05ef115d-f37d-4bb9-9598-64234a1e773c)

If you want to hide your status you will just need to disable the plug-in.

## Building

The project is built with CMake 3.31+

### Step-By-Step build
As the target location I will use C:\RichPresence

1. Get the maya devkit for and extract it to any location. Where exactly doesn't matter, but it should follow this pattern:
   **path_of_your_choice/version/devkitBase**. So for example: **E:/maya_devkits/2025/devkitBase**
   This is done for convenience, so it's easier to build different versions
2. clone the repo to your target location via `git clone https://github.com/aronamao/Discord-Rich-Presence-For-Maya.git C:\RichPresence`
3. `cd **C:\RichPresence**`
4. `mkdir ./out`
5. `cmake -B out -G "Visual Studio 17 2022"  -DMAYA_VERSION:STRING="2025" -DDEVKIT_LOCATION:STRING="E:/maya_devkits"` You have to provide a devkit location and maya_version.
6. `cmake --build --preset=Release --target install`
7. Repeat step 5 and 6 for all desired versions

Once you run the install, all needed files should be moved into the module folder. Afterwards you can simply run the installer.py by drag-n-dropping it into Maya.
I would usually recommend to build the Release version to avoid some debugging verbosity.