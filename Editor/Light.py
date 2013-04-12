import pygame

NONE        = -1
AMBIENT     = 0
DIRECTIONAL = 1
POINT       = 2

SHADER_DICT = {-1: ("Default.vs", "Default.fs"),    \
                0: ("Default.vs", "AmbientLight.fs"),\
                1: ("Default.vs", "Directional.fs"), \
                2: ("Default.vs", "PointLight.fs")}

class Light:
    def __init__(self):
        self.shaders    = ("", "")

        self.bright     = 1.0
        self.color      = (1.0, 1.0, 1.0)

        self.att        = (0.05, 0.01, 0.0)
        self.position   = (0, 0)
        self.angle      = (0, 359)

        self.type       = NONE
        self.surf       = pygame.Surface((32, 32))
        self.surf.fill((255, 255, 255))

    def Render(self, screen):           screen.blit(self.surf, self.position)
        
    def SetType(self, t): self.type = t;self.shaders = SHADER_DICT[t]

    def SetAttenuation(self, c, l, q):  self.att = (c, l, q)        
    def SetPosition(self, x, y):        self.position = (x, y)        
    def SetBrightness(self, b):         self.bright = b
    def SetColor(self, r, g, b):        self.color = (r/255.0, g/255.0, b/255.0); self.surf.fill((r, g, b))
    def SetMaxAngle(self, t):           self.angle = (self.angle[0], t)
    def SetMinAngle(self, t):           self.angle = (t, self.angle[1])

    def GetVShader(self):               return self.shaders[0]
    def GetFShader(self):               return self.shaders[1]

    def GetPosition(self):              return self.position
    def GetX(self):                     return self.position[0]
    def GetY(self):                     return self.position[1]

    def GetBrightness(self):            return self.bright
    def GetColor(self):                 return self.color
    def GetColorRGB(self):              return (self.color[0]*255, self.color[1]*255, self.color[2]*255)
    def GetMinAngle(self):              return self.angle[0]
    def GetMaxAngle(self):              return self.angle[1]
    def GetAttenuation(self):           return self.att
    def GetType(self):                  return self.type
