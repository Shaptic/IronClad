IronClad
========

## About
IronClad is a personal project of mine. It is a 2D rendering / game engine wrapped around OpenGL.  
I currently have a game work-in-progress called Praecursor being built using this engine.

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
* Minimalistic physics system
* Loading of a custom level format for easy scene creation

## Planned Features
In the near future, IronClad will support:
* Fully-featured level creation and control
* More robust physics system
* Variation in lighting
* Shadow generation
* Simple GUI creation
