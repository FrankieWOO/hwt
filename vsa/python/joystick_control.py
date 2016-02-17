## \file joystick_control.py
#  \author Alex Dimaras (AD), alexandros.dimaras@kcl.ac.uk , adimaras@gmail.com
#  \brief Control Maccepa by joystick.
#  \Code adapted from Brian D. Wendt, http://principialabs.com/

import sys
import time
import math
# from numpy import *
import pyrex_maccepa as maccepa
import pygame

# TODO: remove hardcoded limits for commands and get them from defines
# Same for clockwise/counterclockwise motion of drum

def joystick_control():
	"""Control motors from joystick."""

	# Reading command limits through class HardwareInterface in pyrex_maccepa.pyx
	# Class contains u_ulim[3], u_llim[3] arrays
	# Next step is to map those to joystick axis [-1,1] space
	print "Reading robot command limits." 
	uulim = robot.u_ulim
	lulim = robot.u_llim
	print "Max u [equ stif damp]: [ %1.2f, %1.2f, %1.2f]" % (uulim[0], uulim[1], uulim[2])
	print "Min u [equ stif damp]: [%1.2f, %1.2f, %1.2f]" % (lulim[0], lulim[1], lulim[2])
	
	
	# allow multiple joysticks
	joy = []
	 # initialize pygame
	pygame.joystick.init()
	pygame.display.init()
	if not pygame.joystick.get_count():
		print ("\nPlease connect a joystick and run again.\n")
		exit()
	print ("%d joystick(s) detected." % pygame.joystick.get_count())
	
	for i in range(pygame.joystick.get_count()):
		myjoy = pygame.joystick.Joystick(i)
		myjoy.init()
		joy.append(myjoy)
		print ("Joystick %d: " % (i) + joy[i].get_name())
		print ("Press trigger (button 0) to quit.")
	print ("X: equilibrium, Y: stiffness, Throttle: damping")
        # Initialize command
	command = [0,0,0]
	robot.write(command)
	# joystick listener loop
	while True:
		e = pygame.event.wait()
		if (e.type == pygame.JOYAXISMOTION or e.type == pygame.JOYBUTTONDOWN):
			command = handleJoyEvent(e, command)
			robot.write(command)


def handleJoyEvent(e, command):
	out_command = command
	if e.type == pygame.JOYAXISMOTION:		
		# axis = "unknown"
		if (e.dict['axis'] == 0): #X axis
			# axis = "X"
		# if (axis == "X"):
			pos = e.dict['value']
			# print ("X: %1.3f" % (pos) )
			# translate from axis position (-1,1) to commands
			# in this case -pi/2 to pi/2, but should take from model
			out_command[0] = -pos*math.pi/2

		if (e.dict['axis'] == 1): # Y axis
			# axis = "Y"
		# if (axis == "Y"):
			pos = e.dict['value']
			# print ("X: %1.3f" % (pos) )
			# translate from axis position (-1,1) to commands
			# in this case 0 to pi, but should take from model
			out_command[1] = math.fabs(pos)*math.pi

		if (e.dict['axis'] == 2): # Throttle axis
			# axis = "Throttle"
		#if (axis == "Throttle"):
			pos = e.dict['value']
			# print ("Y: %1.3f" % (pos) )
			# translate from axis position (-1,1) to commands
			# in this case 0 to 1, but should take from model
			out_command[2] = 0.5+0.5*pos

		# if (e.dict['axis'] == 3): # Z axis
			# axis = "Z"
		# if (axis != "unknown"):
			# str = "Axis: %s; Value: %f" % (axis, e.dict['value'])
			# output(str, e.dict['joy'])
		
			

	elif e.type == pygame.JOYBUTTONDOWN:
		# str = "Button: %d" % (e.dict['button'])
		# output(str, e.dict['joy'])
		# Button 0 (trigger) to quit
		if (e.dict['button'] == 0):
			print ("Exiting...")
			exit()
			# sys.exit("Exiting...")
	else:
		pass

	return out_command
			
# print the joystick position
def output(line, stick):
	print ("Joystick: %d; %s\n" % (stick, line))


if __name__ == "__main__":
	if len(sys.argv) != 2:
		print "Please specify the USB port to which control board is attached (e.g., /dev/ttyUSB0)."
		exit()
	else:
		robot = maccepa.HardwareInterface(sys.argv[1])
		model = maccepa.ModelInterface()
		joystick_control()

