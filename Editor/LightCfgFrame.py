#!/usr/bin/python
import copy

from Tkinter import *
import tkColorChooser

import rpErrorHandler
import pygame

import Light

class LightWindow(Toplevel):
    def __init__(self, Master=None, **kw):
        kw['borderwidth'] = 5
        
        self.light_col  = (255, 255, 255)
        self.light_hex  = '#ffffff'
        self.screen     = None
        self.Master     = Master
        self.kw         = kw
        self.active     = False

        self.Light      = Light.Light()
        self.vars       = [IntVar(), IntVar(), IntVar()]
        
    def Setup(self):
        apply(Toplevel.__init__, (self, self.Master), self.kw)
    
        for v in self.vars: v.set(0)

        self._Sidebar = Frame(self)
        #self.LightPreviewFrame = Frame(self)
        #self._LightPreview = Canvas(self.LightPreviewFrame)

        self.__Frame5 = Frame(self._Sidebar)
        self._AmbientRB     = Radiobutton(self.__Frame5, justify='left',
                                          variable=self.vars[0],
                                          text='Ambient Light', value=1,
                                          command=self.__AmbientReset)
        self._DirectionalRB = Radiobutton(self.__Frame5, justify='left',
                                          variable=self.vars[1],
                                          text='Directional Lighting', value=2,
                                          command=self.__DirReset)
        self._PointRB       = Radiobutton(self.__Frame5, justify='left',
                                          variable=self.vars[2],
                                          text='Point Light', value=3,
                                          command=self.__PointReset)

        # self.__Frame2 = Frame(self._Sidebar, borderwidth=1)                            
        # self._LivePreviewCheck = Checkbutton(self.__Frame2,
        #                                      text='Live Preview (slow)')

        self.__Frame1 = Frame(self._Sidebar)
        self._BrightnessScale = Scale(self.__Frame1, label='Brightness: ',
                                      length=200, orient='horizontal',
                                      resolution=0.05, sliderrelief='ridge',
                                      to=5.0, width=10, command=self.__ScaleUpdateColor)
        
        self.__Frame6 = Frame(self._Sidebar)
        self._MinAngleLabel = Label(self.__Frame6, text='Minimum Angle: ')
        self._MinAngleEntry = Entry(self.__Frame6)
        
        self.__Frame4 = Frame(self._Sidebar)
        self._MaxAngleLabel = Label(self.__Frame4, text='Maximum angle: ')
        self._MaxAngleEntry = Entry(self.__Frame4)

        self.__Frame7 = Frame(self._Sidebar)
        self._DoneButton = Button(self.__Frame7, text='Done', command=self._Destroy)
        
        self.__Frame3 = Frame(self)        
        self._ColorPreview = Frame(self.__Frame3, background=self.light_hex,
                                   borderwidth=1, height=100, relief='sunken',
                                   takefocus=None, width=100)
        self._ColorChooser = Button(self.__Frame3, text='Choose Color', command=self.__ColorChooserLMB_Down)
                                   
        # Register quit event
        self.protocol("WM_DELETE_WINDOW", self._Destroy)

    def Run(self):
        self.active = True
        self._Sidebar.pack(side='left')
        
        # self.LightPreviewFrame.pack(side='left')
        # self._LightPreview.pack(side='left')
        
        self.__Frame5.pack(side='left')
        self._AmbientRB.pack(anchor='w', padx=5, side='top')
        self._DirectionalRB.pack(anchor='w',padx=5, side='top')
        self._PointRB.pack(anchor='w', padx=5, side='top')
        self.__PointReset()
        
        # self.__Frame2.pack(side='top')
        # self._LivePreviewCheck.pack(anchor='w',pady=5,side='left')
        
        self.__Frame1.pack(pady=10,side='top')
        self._BrightnessScale.pack(anchor='w', side='left')        
        
        self.__Frame6.pack(side='top')
        self._MinAngleLabel.pack(side='left')
        self._MinAngleEntry.pack(side='right')
        
        self.__Frame4.pack(fill='both', pady=5, side='top')
        self._MaxAngleLabel.pack(side='left')
        self._MaxAngleEntry.pack(side='right')

        self.__Frame7.pack(fill='both', pady=10, side='bottom')
        self._DoneButton.pack(side='left')
        
        self.__Frame3.pack(side='top')        
        self._ColorPreview.pack(expand='yes', fill='both', padx=5, pady=5, side='top')
        self._ColorChooser.pack(padx=5, pady=0, side='top')
        self._ColorPreview.bind('<ButtonRelease-1>', self.__ColorChooserLMB_Down)
        
        self._BrightnessScale.set(1.0)
    
    # Start of event handler methods
    def __ColorChooserLMB_Down(self, Event=None):
        (rgb, hx) = tkColorChooser.askcolor(parent=self)
        self._ColorPreview.config(bg=hx)
        self.light_col = rgb
        self.light_hex = hx

    def __ScaleUpdateColor(self, Event=None):
        # Make the color preview show an accurate representation of 
        # the color, based on the brightness.
        br = float(self._BrightnessScale.get())

        r = int(self.light_col[0] * br); r = 255 if r > 255 else r;
        g = int(self.light_col[1] * br); g = 255 if g > 255 else g;
        b = int(self.light_col[2] * br); b = 255 if b > 255 else b;
        h = "#%02x%02x%02x" % (r, g, b)
        
        self.light_hex = h
        self._ColorPreview.config(bg=h)

    def __AmbientReset(self, Event=None):
        [x.set(0) for x in self.vars if x != self.vars[0]]
        self._MaxAngleEntry.config(state=DISABLED)
        self._MinAngleEntry.config(state=DISABLED)

    def __DirReset(self, Event=None):
        [x.set(0) for x in self.vars if x != self.vars[1]]
        self._MaxAngleEntry.config(state=NORMAL)
        self._MinAngleEntry.config(state=NORMAL)

    def __PointReset(self, Event=None):
        [x.set(0) for x in self.vars if x != self.vars[2]]
        self._MaxAngleEntry.config(state=DISABLED)
        self._MinAngleEntry.config(state=DISABLED)

    # Start of non-Rapyd user code
    def _Destroy(self):
        # Set type
        t = [x.get() for x in self.vars]
        for y in t:
            if y: self.Light.SetType(t.index(y))

        if(self.Light.GetType() != Light.NONE):
            # For all lights
            self.Light.SetBrightness(float(self._BrightnessScale.get()))
            self.Light.SetColor(self.light_col[0], self.light_col[1], self.light_col[2])

            # Only for directional lights
            if(self.vars[1].get()):
                self.Light.SetMaxAngle(float(self._MaxAngleEntry.get()))
                self.Light.SetMinAngle(float(self._MinAngleEntry.get()))

            # For point & directional
            #self.Light.SetAttenuation(

            self.Master.lights.append(copy.copy(self.Light))
            self.Light = Light.Light()

        self.active = False
        self.destroy()
        
    def MoveLight(self, x, y):
        self.Light.SetPosition(x, y)

    def GetColor(self): return self.Light.GetColorRGB()
