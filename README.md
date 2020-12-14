# SillyCraft
Silly attempt to create Minecraft v2 

![SillyCraft](https://github.com/Pawlost/SillyCraft/blob/master/Screenshots/SillCraft.PNG)

Voxel terrain generator made as a test for Grip Digital.
This project has not sufficient quality for the company.

## Company requirements / features
Minecraft functions:
* 4 Types of blocks, color is enough
* In building mode (when user selects one of 4 blocks and wants to place it) will be shown wireframe/half-visible block and the block can only be snapped to a grid, aka can not be placed anywhere
* Blocks can not be placed in the air, one side of the block has to touch a side of different a block or a floor
* User can destroy blocks, each of 4 selectable blocks has its own "hardness", so user has to hold button for destroying blocks
* when map is loaded, a new terrain is automatically generated, where player will move (see perlin noise or some modification for more significant differences in height)
* Texture/color of terrain/blocks will be varying based on a block height (affects only generated terrain, not blocks placed by a player)
* In case a player walks to the end of a map, rest of the map will be generated, idealy deterministic changes (bonus: save changes in the terrain made by the player)
* the game a world state can be saved into a file and loaded (no need of UI, 1 save slot will do)
* think about optimizations and implement them

## Installation
See [releases](https://github.com/Pawlost/SillyCraft/releases) to download exported project for PC.

## Contribution
This project is not meant for contribution.

### Prerequisites
* C++ develepment environment
* [Unreal Engine](https://www.unrealengine.com/en-US/)
    * [Fast Noise Generator Plugin](https://unrealengine.com/marketplace/en-US/product/fast-noise-generator)

## Licence
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details