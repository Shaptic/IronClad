IronClad
========

## About
IronClad is a personal project of mine. It is a 2D game engine wrapped around OpenGL, OpenAL, and GLFW.  
I currently have a game work-in-progress called Praecursor being built using this engine.
The binaries *may not* be updated as often as the source code, but feel free to build it yourself,
since the .vcxproj file is included. Alternately, contact me and I'll upload a build immediately.
As of the latest update, IronClad supports audio assets as well, making it closer to a game engine
than a rendering one like never before.

## Instructions
* Clone the repo
* Copy Source/include to your project include directory
* Copy Binaries/IronClad.lib to your project library directory
* Copy Binaries/IronClad.dll to your project binary directory
* In your source, add `#include "IronClad.hpp"`
* Also, link the lib file
* You're set!

## Features
IronClad currently supports the following:
* Full use of modern OpenGL features such as vertex array objects
* Instanced geometry which allows for high-speed, batched rendering of many vertices
* Per-pixel shader-based lighting (currently only point lights)
* Optimized mesh loading
* Entity wrapper for easily controllable mesh instances
* Customized entity animation
* Minimalistic physics system
* Loading of a custom level format for easy scene creation
* Loading and rendering of TrueType fonts
* Simple but flexible GUI creation
* OGG-Vorbis and WAV file audio support

## Planned Features
In the near future, IronClad will support:
* Fully-featured level creation and control
* More robust physics system
* Variation in lighting
* Shadow generation
