# DTD Engine

## Features

### Entity Component System

Rather than using a traditional oriented system, this engine uses a more efficient [Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system). The implementation of the ECS system is custom to this engine.

### Resource loading

Resources are loaded on startup from the `resource` directory. Currently the following resources are loaded

* `.obj` files (and associated `.mtl` files) are loaded from `resources/models` (and all subdirectories if you wish to organize your models)
* `.png` and `.jpg` files are loaded from `resources/textures`
* `.glsl` shader files are loaded from `resources/shaders/vertex` and `resources/shaders/fragment`
* Skyboxes are loaded from `resources/skyboxes`
* `global_config.json` is loaded from `resources/config`

Processed models are put into a globally accessible `meshes` map where the key is the file name (e.g. `cube.obj` or `houses/house1.obj`). Models are automatically resized to be between [-1, +1] in all dimensions, and are recentered to the origin.

Similarily, processed textures are put into a globally accessible `textures` map.

Audio playback is handled by `Audio.cpp`

Shaders are compiled by calling `Program::init` with the list of shader file names.

### View Frustrum Culling

VFC is a supported optimization out of the box.

### Shadow Maps

Shadow mapping code is included in the project in `src/Systems/SystemShadow.cpp`. The depth map generated in that file is then used by `src/Systems/SystemRender.cpp` to pass depth information along to shaders.

### Immediate mode GUI

ImGui is included in this project to allow for the creation of GUIs.

### 2D and 3D Audio

irrKlang is included in this project, along with a wrapper in `src/Audio.cpp` to call the library.

### Axis Aligned Bounding Box Colliders

An axis aligned bounding box collision system exists to allow for some basic physics reactions. These bounding boxes can be automatically created from an entities model. Using the `resize`, `move`, and `setRotationTo` methods in `src/EntityHelper.cpp` updates the bounding boxes in an efficient manner.

### Instance Rendering

Optionally, entities can be configured to be instanced rendered. This passes the Model matrix of the object as an attribute in the shader.
Additional vec4 attributes can also be passed to an instance rendered object's shader.

### Scene System

Entities are contained within a scene. Scenes also contain information about the update function, and key handlers.
This allows for easy and efficient swapping between different scenes that have entirely different behavior.

### Normal Mapping

The engine allows for a normal / bump map to be configured as part of the renderable component. It will pass along vertex normal / binormal / tangents that were calculated when the model was imported.

## Building The Project

1. Clone the repository
2. Create your build directory `mkdir build`
3. Navigate to your build directory
4. See the steps below about getting irrKlang installed
5. Run `cmake ..` to generate your makefile and `make` to build
6. Start the executable with `./blz-3d-engine`

## Libraries Included

The following libraries are included with the listed licenses from their parent projects

### Glad

[glad](https://github.com/Dav1dde/glad) is used to provide an OpenGL context. It is included as licensed under the MIT license.

### Dear ImGui

[Dear ImGui](https://github.com/ocornut/imgui) is used to create simple user interfaces. It is included as licensed under the MIT license.

### irrKlang

[irrKlang] is used to play back 2D and 3D audio.

It is not open source :(

The license for the software can be found [here](https://www.ambiera.com/irrklang/license.html)

For this reason, it is not included in this repository. You will have to download irrKlang yourself in order to build and run this engine.

Steps for doing so:

#### Linux

1. Download the irrKlang 1.6 SDK from [this page](https://www.ambiera.com/irrklang/index.html)
2. Unzip the download
3. Create an irrKlang folder in the `ext` folder of this repository
4. Copy the `lib`, `include`, and `bin` folders from your irrKlang zip into the `ext/irrKlang` folder
5. Copy `ext/irrKlang/bin/ikpMP3.so` to your build directory
6. Rename `ext/irrKlang/bin/ikpMP3.so` to `ext/irrKlang/bin/libikpMP3.so`
7. Build and run

#### MacOS

Steps are similar

#### Windows

We got it working, but I'm not sure how. I think the 32 bit version of irrKlang was required

### rapidjson

[Rapid JSON](https://github.com/Tencent/rapidjson/) is used to load in JSON resources at engine startup. It is included as licensed under the BSD license.

### stb_image

[stb_image](https://github.com/nothings/stb) is used to load in all image asset files. It is included as a public domain resource.

### tiny obj loader

[tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) is used to load in OBJ model files. It is included as licensed under the MIT license.

## Contributors

This project includes code by

* Shinjiro Sueda
* Zoe Wood
* Ian Dunn

Most of the work on this engine was done by

* Alex Gravenor
* John Potz
* Evan Zhang
* David Heyer
