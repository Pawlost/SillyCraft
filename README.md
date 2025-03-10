# SillyCraft
![SillyCraft](./Screenshots/SillCraft.PNG)

## Description
Unreal Engine Voxel algorithm inspired by Terra created as a part of company project interview. Repurposed as a foundation of my bachelors thesis. Made in Unreal Engine 5.4. Repurposed.

### Prerequisites
Windows 11:
* [Visual Studio Installer](https://visualstudio.microsoft.com/cs/downloads/)
    * [Workloads: Game development with C++, Desktop development with C++, .NET desktop development](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine?application_version=5.4)
    * Individual Components: MSVC v143 - VS 2022 C++ x64/x86 build tools (v14.38-17.8)
* [Jetbrains Rider](https://www.jetbrains.com/rider/) (or alternative IDE able to build and run Unreal Engine)
* [Epic games launcher](https://store.epicgames.com/en-US/download)
    * [Unreal Engine 5.4](https://www.unrealengine.com/en-US/)
    * [Fast Noise Generator Plugin](https://www.fab.com/listings/c1d444fc-54cc-4f11-8a4a-c0c41112a321)
    * [Realtime Mesh - Core](https://www.fab.com/listings/bb2e4fbb-617c-41d3-aac6-e181eddf8b3b)

**Note: Linux environment is not tested**

## Installation

After installing all Prerequisites, restart the computer.

If .uproject files are not associated with unreal engine
1. Copy `C:\Program Files (x86)\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe` to `C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64`
2. Run `UnrealVersionSelector` in the directory.
3. Go to the .uproject file
4. Right click -> open with -> Unreal Engine

## Github Project

1. Copy project files to a directory
2. right click uproject -> show more options -> open with unreal engine -> Generate Visual Studio project files
3. Open solution `.sln` in Rider (or your IDE)
    1. Wait for rider to process solution files
    2. Make sure RiderLink plugin was installed
4. Run the project
For further details see [Usage](#usage).

## Project as plugin

## Usage
### Data table
Example voxel types are stored DT_BlockTable. 
You can create new voxel table from ...

### Blueprints
You can make custom terrain/voxel generators 

Example voxel generators can be found in

Mesher turns voxel grid from voxel generator to mesh.

For chunk spawner you need to specify voxel generator and which voxel it is using.

You can make spawning actor that spawns terrain as its moving or you can make independed generator.

Example of chunk spawning actor can be found in.

### Scenes
Example scenes can be found in.

## Contribution
This project is not meant for contribution.

## Licence
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

## Contact

* Email: [mr.p.balusek@gmail.com](mailto:mr.p.balusek@gmail.com)
* Github: [Pawlost](https://github.com/Pawlost?tab=repositories)