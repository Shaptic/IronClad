#!/usr/bin/python

import os
import copy

import tkMessageBox
import tkFileDialog
from Tkinter import *

import rpErrorHandler
import pygame
import Light

from LightCfgFrame      import LightWindow
#from AnimationCfgFrame  import AnimationWindow

from Entity import Entity
from Sprite import Sprite
from Level  import Level

class GUIMenu(Menu):
    def __init__(self, parent, **kw):
        apply(Menu.__init__, (self, parent), kw)
        self.vars = []

    def add_checkbutton(self, **kw):
        v = IntVar()
        v.set(0)
        self.vars.append(v)
        kw["variable"] = v
        Menu.add_checkbutton(self, **kw)
        return len(self.vars) - 1

    def SetVar(self, i, val):
        if(i >= len(self.vars)): return
        val = int(val)
        print "Setting [%d] to %d" % (i, val)
        self.vars[i].set(val)

    def ToggleVar(self, i):
        if(i >= len(self.vars)):    return
        if(self.GetVar(i) == 0):    self.vars[i].set(1)
        else:                       self.vars[i].set(0)

        print "Toggling [%d] to %d" % (i, self.GetVar(i))

    def GetVar(self, i):
        return int(self.vars[i].get())

class EditorGUI(Frame):
    def __init__(self, Master=None, **kw):
        apply(Frame.__init__, (self, Master), kw)
        self._Setup()

        # Pygame entity stuff
        self.entities   = []
        self.lights     = []

        self.pan_rate   = (0, 0)

        self.Level      = Level()
        self.ActiveTile = Entity()
        self.ActiveLight= Light.Light()
        self.ActiveMesh = None
        self.Changer    = None
        self.LChanger   = None

        self.menu_show  = False
        self.light_mode = False

        self.ActiveLight.SetType(Light.NONE)
        
    def _Setup(self):
        self._LeftFrame         = Frame(self, height=200, width=100)
        self._TLScrollBar       = Scrollbar(self._LeftFrame, orient=VERTICAL)
        self._TextureList       = Listbox(self._LeftFrame, selectmode=SINGLE,
                                          bd=0, yscrollcommand=self._TLScrollBar.set)
        self._LightCheck        = Checkbutton(self._LeftFrame,
                                              text='Light Editing')
        #self._LightingButton    = Button(self._LeftFrame, text="Add Light")
        
        self._RightFrame = Frame(self, borderwidth=1, height=600, width=800)
        
        # This is a drop-down context menu for sprite objects.
        self.menu = GUIMenu(self, tearoff=0, postcommand=self.__MenuPost)
        self.menu.add_command(label="Delete Object", command=self.__MenuEraseLMB_Down)
        self.menu.add_command(label="To Background", command=self.__PlaceOnBottom)
        self.menu.add_command(label="Delete Ambience", command=self.__KillAmbience)
        self.menu.add_separator()
        self.menu.add_checkbutton(label="Physical",     onvalue=1, offvalue=0, command=self.__SetAttribPh)
        self.menu.add_checkbutton(label="Animation",    onvalue=1, offvalue=0, command=self.__SetAttribAn)
        self.menu.add_checkbutton(label="Static",       onvalue=1, offvalue=0, command=self.__SetAttribSt)
        self.menu.add_checkbutton(label="Spawn",        onvalue=1, offvalue=0, command=self.__SetAttribSp)
        
        self.LightWin = LightWindow(self)
        self.AnimWin  = AnimationWindow(self)
        
    def Run(self):
        self._LeftFrame.pack(side='left', expand='yes', fill='both')
        #self._LightingButton.pack(padx=5, pady=5, side='bottom')
        self._TLScrollBar.config(command=self._TextureList.yview)
        self._TLScrollBar.pack(anchor='nw', side='left', fill='y')
        self._TextureList.pack(anchor='nw', pady=0, side='left', fill='y')
        self._LightCheck.pack(anchor='w', pady=5, side='bottom')
        self._RightFrame.pack(expand='yes', fill='both', side='right')
        
        #self._RightFrame.bind('<Motion>', self.__MMotion)
        self._RightFrame.bind('<Button-3>', self.__DropDown)
        self._LightCheck.bind('<ButtonRelease-1>', self.__LightModeLMB_Down)
        self._TextureList.bind('<ButtonRelease-1>', self.__TextureListLMB_Down)

        # Add valid image files to texture list.
        for r, dirs, files in os.walk(os.getcwd()):
            for f in files:
                if(f.endswith('.tga')):
                    self._TextureList.insert(END, os.path.splitext(f)[0])
                
        # Enable the Pygame window as the Tkinter frame
        os.environ["SDL_WINDOWID"] = str(self._RightFrame.winfo_id())
        os.environ["SDL_WINDOWDRIVER"] = "windib"
                
        # Usual pygame initialization
        pygame.display.init()
        self.screen = pygame.display.set_mode((800, 600))
        
    # Start of event handler methods
    def __LightingButtonLMB_Down(self, Event=None):
        ''' For adding a light '''
        self.ActiveTile.Load(surface=pygame.Surface((32, 32)))
        self.ActiveTile.Sprite.surf.fill(self.LightWin.GetColor())
        self.ActiveTile.Enable(Event.x_root, Event.y_root)
        self.light_mode = True

    def __LightModeLMB_Down(self, Event=None):
        if(not self.light_mode):
            self.ActiveTile.Load(surface=pygame.Surface((32, 32)))
            self.ActiveTile.Sprite.surf.fill(self.LightWin.GetColor())
            self.ActiveTile.Enable(Event.x_root, Event.y_root)
        
        self.light_mode = not self.light_mode

    def __TextureListLMB_Down(self, Event=None):
        ''' For choosing active textures '''
        index = self._TextureList.curselection()
        if(len(index) > 0):
            self.ActiveTile.Load(filename=os.path.join("Images", \
                self._TextureList.get(index) + ".tga"))

    def __DropDown(self, Event=None):
        self.__RMB_Down(Event.x_root, Event.y_root, True)

    def __MenuEraseLMB_Down(self, Event=None):
        if(self.Changer != None):   self.entities.remove(self.Changer)
        if(self.LChanger != None):  self.lights.remove(self.LChanger)

    def __MenuPost(self, Event=None):
        self.menu_show = True
        
        for l in self.lights:
            if(l.GetType() == Light.AMBIENT):
                self.menu.entryconfig(2, state=NORMAL)
                return
        self.menu.entryconfig(2, state=DISABLED)

    def __KillAmbience(self, Event=None):
        for l in self.lights:
            if l.GetType() == Light.AMBIENT: self.lights.remove(l)

    def __SetAttribPh(self, Event=None):
        if(self.Changer != None):
            self.Changer.TogglePhysics()
            self.menu.SetVar(0, self.Changer.IsPhysical())

    def __SetAttribAn(self, Event=None):
        if(self.Changer != None):
            self.Changer.ToggleAnimation()
            self.menu.SetVar(1, self.Changer.IsAnimation())
            #self.AnimWin.Entity = self.Changer
            #self.AnimWin.Setup()
            #self.AnimWin.Run()

    def __SetAttribSt(self, Event=None):
        if(self.Changer != None):
            self.Changer.ToggleStatic()
            self.menu.SetVar(2, self.Changer.IsStatic())

    def __SetAttribSp(self, Event=None):
        if(self.Changer != None):
            self.Changer.TogglePSpawn()
            self.menu.SetVar(3, self.Changer.IsSpawn())
            
    def __PlaceOnBottom(self, Event=None):
        if(self.Changer != None):
            self.entities.remove(self.Changer)
            self.entities.insert(0, self.Changer)

    # Pygame events
    def __MMotion(self, x, y):
        if(self.LightWin.active): return

        self.ActiveTile.Enable(x, y)

        rate = [0, 0]

        if x + 50 > 800:    rate[0] = -1
        elif x - 50 < 0:    rate[0] =  1

        if y + 50 > 600:    rate[1] = -1
        elif y - 50 < 0:    rate[1] =  1

        self.pan_rate = tuple(rate)

        if self.ActiveMesh != None:
            self.ActiveMesh.Update(x, y)

    def __RMB_Down(self, x, y, tk=False):
        if(self.LightWin.active): return

        wx = self._RightFrame.winfo_rootx()
        wy = self._RightFrame.winfo_rooty()

        if not tk:  self.menu.post(x + wx, y + wy)
        else:       self.menu.post(x, y)

        # Lights are usually on the highest level, so they are
        # checked for deletion first.
        # TODO

        self.Changer = None
        
        # Check for collisions from top to bottom, as they appear.
        # Rather than as they are placed.
        self.entities.reverse()
        for mesh in self.entities:
            if mesh.Collide(pygame.Rect(x, y, 1, 1)):
                self.Changer = mesh; break
        self.entities.reverse()

        for i in xrange(4): self.menu.SetVar(i, False)

        if(self.Changer != None):
            self.menu.SetVar(0, self.Changer.IsPhysical())
            self.menu.SetVar(1, self.Changer.IsAnimation())
            self.menu.SetVar(2, self.Changer.IsStatic())
            self.menu.SetVar(3, self.Changer.IsSpawn())

    def __LMB_Down(self, x, y):
        if(self.LightWin.active): return
        if(self.menu_show):
            self.menu_show = False

        elif(self.light_mode):
            self.LightWin.MoveLight(x, y)
            self.LightWin.Setup()
            self.LightWin.Run()

        else:
            self.ActiveMesh = Entity().Load(self.ActiveTile.GetFilename())
            self.ActiveMesh.Enable(x, y)

    def __LMB_Up(self, x, y):
        if self.ActiveMesh != None:
            self.ActiveMesh.Disable(x, y)
            self.entities.append(copy.copy(self.ActiveMesh))
            self.ActiveMesh = None

    def HandlePygame(self, evt):
        if(self.LightWin.active): return
        if evt.type == pygame.MOUSEBUTTONDOWN:
            x = evt.pos[0]
            y = evt.pos[1]

            if evt.button == 1:     # Left mouse?
                self.__LMB_Down(x, y)

            # elif evt.button == 2:   # Middle mouse?
            #     self.__MMB_Down(x, y)

            elif evt.button == 3:   # Right mouse?
                self.__RMB_Down(x, y)

        elif evt.type == pygame.MOUSEBUTTONUP:
            if evt.button == 1:     # Left mouse?
                self.__LMB_Up(evt.pos[0], evt.pos[1])

        elif evt.type == pygame.MOUSEMOTION:
            self.__MMotion(evt.pos[0], evt.pos[1])
        
    # Start of non-Rapyd user code
    def About(self):    tkMessageBox.showinfo("About", "IronLevel GUI Editor\nVersion 1.0")    
    def Credits(self):  tkMessageBox.showinfo("Credits", "By George Kudrayvtsev\nCopyright (c) 2013 Zen Panda Interactive")
    def New(self):      self.entities = []; self.lights = []; self.Level.Reset()

    def Export(self):
        # Make left-most entity at x=0
        try: l = self.entities[0].GetX()
        except: return
        
        for s in self.entities:
            if s.GetX() < l: l = s.GetX()

        for s in self.entities: s.Pan(-l, 0)
        for s in self.lights: s.SetPosition(s.GetPosition()[0] - l, s.GetPosition()[1])
        
        # Repeat for y=0
        l = self.entities[0].GetY()
        for s in self.entities:
            if s.GetY() < l: l = s.GetY()
    
        for s in self.entities: s.Pan(0, -l)
        for s in self.lights: s.SetPosition(s.GetPosition()[0], s.GetPosition()[1] - l)
    
        # Find ambient lights and add them to the file first
        # Gawd I love Python list comprehensions
        ambients = [l for l in self.lights if l.GetType() == Light.AMBIENT]
        non      = [l for l in self.lights if l.GetType() != Light.AMBIENT]
        
        for s in self.entities: self.Level.AddEntity(s)
        for l in ambients: self.Level.AddLight(l)
        for l in non: self.Level.AddLight(l)

        # 'Save As...' popup
        fn = tkFileDialog.asksaveasfilename(defaultextension=".iclvl",              \
                filetypes=[('IronClad Level files', '.iclvl'), ('All Files', '.*')],\
                initialfile="IronLevel.iclvl",  parent=self,                        \
                title="Export Level As...")

        self.Level.Export(fn)
        self.Level.Reset()

    def Import(self): pass

    def Pan(self):
        if(self.LightWin.active): return
        for mesh in self.entities:
            mesh.Pan(self.pan_rate[0], self.pan_rate[1])

        for light in self.lights:
            light.SetPosition(light.GetPosition()[0] + self.pan_rate[0],    \
                              light.GetPosition()[1] + self.pan_rate[1])

        if self.ActiveMesh != None:
            self.ActiveMesh.Pan(self.pan_rate[0], self.pan_rate[1])

    def GetAmbientColor(self):
        c = [0, 0, 0]

        for l in self.lights:
            if(l.GetType() == Light.AMBIENT):
                c[0] += l.GetColor()[0] * l.GetBrightness()
                c[1] += l.GetColor()[1] * l.GetBrightness()
                c[2] += l.GetColor()[2] * l.GetBrightness()
        
        for x in xrange(len(c)): c[x] = 255 if c[x]*255 > 255 else int(c[x]*255)
        return tuple(c)


    def GetScreen(self):        return self.screen
    def GetATile(self):         return self.ActiveTile
    def IsLightCfgActive(self): return self.LightWin.active