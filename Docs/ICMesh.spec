FILE FORMAT SPECIFICATION FOR ICMesh FILES

Name        : IronClad mesh
Extension   : .icmesh
Description : Mesh files are used throughout the IronClad engine for loading meshes (go figure).
Format      : The .icmesh file format is quite similar to the WaveFront .obj model format,
              with a twist of .ini files thrown in the mix.
              Each surface must be preceded by a header, followed by comma-separated values.
              Regular parameters can be standalone.
Example     : // Example mesh file
              // Specifies vertex data for a simple textured quad.

              positions=0,0,100,0,100,100,0,100
              // Each pair of texture coordinates must match the above position values.
              texcoords=0,1,1,1,1,0,0,0
              
              [surface]
              // Can be either "texture", "shader", or both.
              texture=Data/Textures/Default1.tga
              shader=Data/Shaders/Default.vs,Data/Shaders/Default.fs
              indices=0,1,3,3,1,2
