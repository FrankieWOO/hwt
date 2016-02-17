.PHONY: default clean

default: doxygen/index.html m2html/index.html epydoc/index.html

doxygen/index.html:
	doxygen Doxyfile

m2html/index.html:
	matlab -nosplash -nodesktop -r "addpath ../external/m2html;build_documentation;exit" || touch $@
	touch $@

epydoc/index.html:
	touch $@
#epydoc/index.html: ../pyrex_gui.so
#	cd .. && epydoc --html --graph all --exclude-introspect=test_* \
#		   pyrex_gui \
#		   -v -o doc/epydoc/

clean:
	rm -rf doxygen/* m2html/* epydoc/*

#.PHONY: epydoc doxygen m2html
#default: epydoc doxygen m2html

#epydoc:
#	epydoc --html --graph all --exclude-introspect=test_* \
#		   udp_da sml robotics SimGUI ode rcb3 DLR Schunk Sensor FlockOfBirds dlrkin TriMeshLoader hand QMI rsi DataCollector SimThread Bagnoli fri Bumblebee \
#		   test_swinger_dynamic_simulation test_swinger \
#		   test_lwr_kinematic_simulation test_lwr \
#		   test_sdh_kinematic_simulation test_sdh \
#	       test_emg test_fob \
#	       test_bagnoli \
#		   -v -o epydoc/
#

