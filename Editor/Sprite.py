from copy import copy
import pygame

class Sprite:
    def __init__(self):
        self.pos    = (0, 0)
        self.surf   = pygame.Surface((1, 1))
        self.rect   = pygame.locals.Rect(0, 0, 1, 1)
        self.fn     = ""

    def Load(self, filename = None, surface = None):
        if filename is not None:
            self.surf = pygame.image.load(filename).convert()
            self.rect = copy(self.surf.get_rect())
            self.fn   = filename

        elif surface is not None:
            self.surf = copy(surface)
            self.rect = copy(self.surf.get_rect())

        return self

    def Move(self, x, y):
        self.pos = (x, y)
        self.rect.topleft = (x, y)

        return self

    def Collide(self, rect):
        return self.rect.colliderect(rect)

    def Update(self, screen):
        screen.blit(self.surf, self.pos)
        return self

    def Blit(self, surface, x, y):
        self.surf.blit(surface, (x, y))
        return self

    def GetW(self): return self.rect.w
    def GetH(self): return self.rect.h
    def GetX(self): return self.pos[0]
    def GetY(self): return self.pos[1]
    def GetRect(self): return self.rect
    def GetFilename(self): return self.fn
    def GetSprite(self): return self.surf

def render_text(text, want_sprite=True):
    font = pygame.font.SysFont("Arial", 12)
    return Sprite().Load(surface=font.render(text, 1, (255, 255, 255))) \
        if want_sprite else font.render(text, 1, (255, 255, 255))
