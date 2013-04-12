#!/usr/bin/python

import os

import tkMessageBox
from Tkinter import *

import pygame
import rpErrorHandler

import Light
import EditorFrame

# Exit callback
def leave():
    global quit
    quit = True

try:
    quit = False
    
    if __name__ == '__main__':

        Root = Tk()
        CallWrapper = rpErrorHandler.CallWrapper
        
        App = EditorFrame.EditorGUI(Root)
        App.pack(expand='yes',fill='both')
        App.Run()
        
        mainMenu = Menu(Root)
        
        fileMenu = Menu(mainMenu, tearoff=0)
        fileMenu.add_command(label="New", command=App.New)
        fileMenu.add_command(label="Import", command=App.Import)
        fileMenu.add_command(label="Export", command=App.Export)
        fileMenu.add_separator()
        fileMenu.add_command(label="Quit", command=leave)
        
        lighting_var = BooleanVar()
        lighting_var.set(True)
        viewMenu = Menu(mainMenu, tearoff=0)
        viewMenu.add_checkbutton(label="Toggle Lighting", onvalue=True,
            offvalue=False, variable=lighting_var, command=None)
        
        helpMenu = Menu(mainMenu, tearoff=0)
        helpMenu.add_command(label="About", command=App.About)
        helpMenu.add_command(label="Credits", command=App.Credits)
        
        mainMenu.add_cascade(label="File", menu=fileMenu)
        mainMenu.add_cascade(label="View", menu=viewMenu)
        mainMenu.add_cascade(label="Help", menu=helpMenu)
        
        Root.geometry('1000x605+10+10')
        Root.title('IronLevel')
        Root.config(menu=mainMenu)
        
        Root.protocol("WM_DELETE_WINDOW", leave)        

        ed = App.GetScreen()

        while not quit:
            for evt in pygame.event.get():
                App.HandlePygame(evt)

            App.Pan()

            ed.fill(App.GetAmbientColor())

            for m in App.entities:  m.Render(ed)
            for l in App.lights:
                if(l.GetType() == Light.POINT):
                    pygame.draw.circle(ed, l.GetColorRGB(), l.GetPosition(), 10)

                elif(l.GetType() == Light.DIRECTIONAL):
                    pts = [(l.GetPosition()[0], l.GetPosition()[1]+32), \
                           (l.GetPosition()[0]+16, l.GetPosition()[1]), \
                           (l.GetPosition()[0]+32, l.GetPosition()[1]+32)]
                    pygame.draw.polygon(ed, l.GetColorRGB(), pts)

            if(App.ActiveMesh != None): App.ActiveMesh.Render(ed)
            if(App.ActiveTile != None): App.ActiveTile.Render(ed)

            pygame.display.update()
            Root.update()
            
        os.system("del *.pyc")

except:
    rpErrorHandler.RunError()