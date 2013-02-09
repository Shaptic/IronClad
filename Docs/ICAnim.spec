FILE FORMAT SPECIFICATION FOR ICAnim FILES

Name        : IronClad animation sprite sheet
Extension   : .icanim
Description : Animation files are used to (duh) animate sprites.
Format      : The .icanim file is a generic Targa (.tga) image file preceded
              with some basic header information detailing the file dimensions
              and the amount of sprites in each row/column. There is no support
              for comments or any of that useless jazz, it's just a few bytes
              preceding the image data. The below example specifes a 64x64 texture
              containing 4 sprites, 2 in each row (of which there are 2).
              The order is width height columns rows.              
Example     : 64 64 2 2tgadataimmediatelyafterward