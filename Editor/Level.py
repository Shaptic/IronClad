import os
import copy

import Light

LEVEL_HEADER = \
"""// This level was generated with a level editor.
// Editing by hand is not recommended.
// Please see Docs/ICLevel.spec for details on level file formatting.
<level>%s</level>
"""

class LevelBuilder:
    def __init__(self):
        self.level = ""

    def AddEntity(self, ent):
        self.level += "\n<entity>\n";
        
        self.level += "\tx=%0.2f\n"   % float(ent.GetX())
        self.level += "\ty=%0.2f\n\n" % float(ent.GetY())
        
        self.level += "\tvertex=0,0,%d,0,%d,%d,0,%d\n" %    \
            (ent.GetW(), ent.GetW(), ent.GetH(), ent.GetH())
            
        self.level += "\ttexcoords=0,%0.1f,%0.1f,%0.1f,%0.1f,0,0,0\n\n" %   \
             (ent.GetH() / (1.0 * ent.Sprite.GetH()),   \
              ent.GetW() / (1.0 * ent.Sprite.GetW()),   \
              ent.GetH() / (1.0 * ent.Sprite.GetH()),   \
              ent.GetW() / (1.0 * ent.Sprite.GetW()))
        
        self.level += "\t<surface>\n";
        
        if(ent.IsAnimation()):
            self.level += "\t\tanimation=%s\n\n" % ent.GetFilename()
        else:
            self.level += "\t\ttexture=%s\n\n" % ent.GetFilename()
            
        self.level += "\t\tindices=0,1,3,3,1,2\n"

        if(ent.HasShader()):
            self.level += "\t\tvshader=%s\n" % ent.GetVShader()
            self.level += "\t\tfshader=%s\n\n" % ent.GetFShader()

        self.level += "\t</surface>\n\n"

        if(ent.IsSpawn()):
            self.level += "\tisSpawn=1\n\n"

        else:
            self.level += "\tisPhysical=%d\n" % int(ent.IsPhysical())
            self.level += "\tisAnimation=%d\n" % int(ent.IsAnimation())
            self.level += "\tisStatic=%d\n" % int(ent.IsStatic())

        if(ent.GetAnimationRate() > 0.0):
            self.level += "\tanimationRate=%0.3f\n\n" % float(ent.GetAnimationRate())

        self.level += "</entity>\n"

    def AddLight(self, light):
        t = int(light.GetType())

        self.level += "\n<light>\n"
        self.level += "\ttype=%d\n\n" % t
        #self.level += "\tvshader=%s\n\tfshader=%s\n\n" % (light.GetVShader(), light.GetFShader())
        self.level += "\tx=%d\n" % int(light.GetPosition()[0])
        self.level += "\ty=%d\n\n" % int(light.GetPosition()[1])

        self.level += "\tbrightness=%0.4f\n" % light.GetBrightness()
        self.level += "\tcolor=%0.2f,%0.2f,%0.2f\n" %           \
            (light.GetColor()[0], light.GetColor()[1], light.GetColor()[2])

        if(t != Light.AMBIENT):
            self.level += "\tattenuation=%0.4f,%0.4f,%0.4f\n" % \
                (light.GetAttenuation()[0],                   \
                 light.GetAttenuation()[1],                   \
                 light.GetAttenuation()[2])

        if(t == Light.DIRECTIONAL):
            self.level += "\tminAngle=%0.1f\n\tmaxAngle=%0.1f\n" % \
                (light.GetMinAngle(), light.GetMaxAngle())

        self.level += "</light>\n\n"

    def Export(self, f):
        f.write(LEVEL_HEADER % self.level)
        f.flush()

    def Reset(self):
        self.level = ""

class Level:
    def __init__(self):
        self.entities = []
        self.lights   = []

    def AddEntity(self, ent):
        self.entities.append(copy.copy(ent))

    def AddLight(self, light):
        self.lights.append(copy.copy(light))

    def Export(self, filename):
        Builder = LevelBuilder()

        for e in self.entities: Builder.AddEntity(e)
        for l in self.lights:   Builder.AddLight(l)

        Builder.Export(open(filename, "w"))

    def Reset(self):
        self.entities = []
        self.lights   = []

