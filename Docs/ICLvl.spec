FILE FORMAT SPECIFICATION FOR ICLvl FILES

Name        : IronClad level
Extension   : .iclvl
Description : Level files are a high level interface used throughout the IronClad engine.
              They are essentially collections of raw mesh data, and may in the future hold
              essential script or entity data for in-game manipulation.
Format      : The .iclvl file format is currently just .icmesh files wrapped in mesh/endmesh tags.
Example     : // Example level file

              [mesh]
              10.0,400.0
              
              // A platform 100px by 10px.
              positions=0,0,100,0,100,10,0,10
              texcoords=0,1,5,1,5,0,0,0
              
              [surface]
              texture=Data/Textures/Platform.tga
              indices=0,1,3,3,1,2
              
              [end]
              
              
Proposed    : [entity]
              type=1
              physics=0
              controllable=0
              position=1.0,212.0

              [mesh]
              positions=0,0,800,0,800,416,0,416
              texcoords=0,13.0,25.0,13.0,25.0,0,0,0

              [surface]
              texture=Wall.tga
              indices=0,1,3,3,1,2

              [end]
