import Sprite
from pygame.locals import Rect

class Entity:
    def __init__(self):
        self.Sprite = Sprite.Sprite()
        self.start  = (0, 0)
        self.end    = (0, 0)
        self.dim    = Rect(0, 0, 0, 0)
        self.on     = False

        self.shaders  = ("", "")
        self.anim_rate= -1
        self.anim_fn  = ""

        # Various entity attributes
        self.phyz     = False
        self.anim     = False
        self.static   = False
        self.p_spawn  = False
        self.e_spawn  = False

    def Load(self, filename=None, surface=None):
        if(filename): self.Sprite.Load(filename=filename)
        elif surface: self.Sprite.Load(surface=surface)
        return self

    def Update(self, x, y):
        if not self.on: return

        self.Sprite.Move(x, y)
        self.end = (x, y)

    def Pan(self, x, y):
        self.start  = (self.start[0] + x, self.start[1] + y)
        self.end    = (self.end[0] + x, self.end[1] + y)
        self.dim.x += x
        self.dim.y += y

    def Enable(self, x, y):
        self.on     = True
        self.start  = (x, y)
        self.end    = (x, y)
        self.dim.x  = x
        self.dim.y  = y

    def Disable(self, x, y):
        self.on     = False

    def Collide(self, rect):
        return self.dim.colliderect(rect)

    def Render(self, screen):
        # Calculate how many should be rendered.
        count_x = abs(self.start[0] - self.end[0]) / self.Sprite.GetW()
        count_y = abs(self.start[1] - self.end[1]) / self.Sprite.GetH()

        # Fix dimensions.
        self.dim.topleft    = self.start
        self.dim.w          = (count_x + 1) * self.Sprite.GetW()
        self.dim.h          = (count_y + 1) * self.Sprite.GetH()

        # Render count_x * count_y copies.
        for x in xrange(count_x + 1):
            for y in xrange(count_y + 1):
                coord = (self.start[0] + (x * self.Sprite.GetW()),  \
                        (self.start[1] + (y * self.Sprite.GetH())))

                screen.blit(self.Sprite.GetSprite(), coord)

        return self

    def BuildMeshString(self):
        return MESH_FORMAT %                                        \
            (self.GetW(), self.GetW(), self.GetH(), self.GetH(),    \
             self.GetH() / (1.0 * self.Sprite.GetH()),              \
             self.GetW() / (1.0 * self.Sprite.GetW()),              \
             self.GetH() / (1.0 * self.Sprite.GetH()),              \
             self.GetW() / (1.0 * self.Sprite.GetW()),              \
             os.path.splitext(os.path.basename(self.Sprite.GetFilename()))[0] + ".tga")

    def TogglePhysics(self):self.phyz = not self.phyz; return self.phyz
    def ToggleStatic(self): self.static = not self.static; return self.static
    def TogglePSpawn(self): self.p_spawn = not self.p_spawn; return self.p_spawn
    def ToggleESpawn(self): self.e_spawn = not self.e_spawn; return self.e_spawn

    def IsAnimation(self):  return self.anim
    def IsPhysical(self):   return self.phyz
    def IsStatic(self):     return self.static
    def IsSpawn(self):      return self.p_spawn or self.e_spawn

    def HasShader(self):    return self.shaders[0] != "" or self.shaders[1] != ""

    def GetX(self):         return self.start[0]
    def GetY(self):         return self.start[1]
    def GetW(self):         return self.dim.w
    def GetH(self):         return self.dim.h
    def _GetRect(self):     return self.dim

    def GetFilename(self):  return self.Sprite.GetFilename()
    def GetVShader(self):   return self.shaders[0]
    def GetFShader(self):   return self.shaders[1]

    def ToggleAnimation(self):      self.anim = not self.anim; return self.anim
    def GetAnimationFilename(self): return self.anim_fn
    def GetAnimationRate(self):     return self.anim_rate

"""
bool load(const char* fn)
{
    std::ifstream f(fn);
    std::string line;
    std::map<std::string, std::string> d;

    if(!f.is_open()) return true;

    uint32_t num = 0;

    while(std::getline(f, line))
    {
        ++num;

        if(line.empty() || line[0] == '/') continue;

        std::vector<std::string> pair = util::split(line, '=');
        
        if(pair.size() != 2)
        {
            util::g_Log.Flush();
            util::g_Log << "[INFO] Corrupt line on line " << num;
            util::g_Log << "in " << fn << ": " << line << std::endl;
            util::g_Log.PrintLastLog();
        }

        // Valid lines look like this:
        // "position=20,40"
        // So we can put them in a key:value dictionary like so:
        // {"position": "20,40"}
        // Then we can search the dictionary for values by calling 
        // Parser.GetValue("position");
        d[pair[0]] = pair[1];
    }

    f.close();
    return (num > 0);   // Show we actually read some data.
}
"""