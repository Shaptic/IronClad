FILE FORMAT SPECIFICATION FOR ICLvl FILES

Name        : IronClad Level
Extension   : .iclvl
Description : Level files are a high level interface used throughout the IronClad engine.
              They are essentially collections of raw mesh data, and may in the future hold
              essential script or entity data for in-game manipulation.
              These files are usually generated automatically with a custom level editor.
Format      : The .iclvl file format wraps .icmesh files in an entity container, and adds
              an optional light container for controlling lighting.
Example     : // Example level file

              [entity]
              location=100,100
              attribs=1,1,0,0,0,0              
              
              [mesh]
              // A platform 100px by 10px.
              positions=0,0,100,0,100,10,0,10
              texcoords=0,1,5,1,5,0,0,0
              
              [surface]
              texture=Data/Textures/Platform.tga
              indices=0,1,3,3,1,2
              
              [endm]
              
              [light]
              position=12,12
              color=1.0,1.0,0.8
              attenuation=0.01,0.05,0.00
              brightness=0.5
              [endl]
              [end]