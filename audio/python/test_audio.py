## \file test_audio.py
#  \author Matthew Howard (MH), matthew.howard@kcl.ac.uk
#  \brief Script for testing soundcard data acquisition
#  \ingroup Audio

import time
import math
from numpy import *
import pyrex_gui as gui
import pyrex_audio as audio

# Time constant (ms)
dt = 0.002; 

# Window to plot signals
s = gui.Scope(2,0,0,800,400,'Signals')

# Sliders to adjust filters/smoothing.
slb = gui.createSliderBox(3,'Filters')
slb.setupSlider(0,0,2000,'High Pass',0.1)
slb.setupSlider(1,0,2000,'Low Pass',0.1)
slb.setupSlider(2,0,100 ,'Smoothing',0.1)
slb.setValues([60, 600, 3])

# Audio object
a = audio.AudioInterface()

def show_signal():
    """ Show signals. """
    while slb.checkExit()==0:
       v = slb.getValues()
       a.setHP(v[0])
       a.setLP(v[1])
       a.setSmooth(v[2])
    
       u =  a.getData()

       # visualise
       if len(u)>0:
        s.add([u[1],u[0]])

       time.sleep(dt)

show_signal()
