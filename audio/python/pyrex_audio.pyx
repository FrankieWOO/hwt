## \file audio/python/pyrex_audio.pyx
#  \brief Main source file of the python soundcard interface.
#  \ingroup Audio
#  \author Stefan Klanke, Matthew Howard (mh), matthew.howard@kcl.ac.uk

cdef extern from "audio_interface.h":
    ctypedef struct c_Audio "Audio":
        void setLowPassCutoff(float freq)
        void setHighPassCutoff(float freq)
        void setRmsCutoff(float freq)
        int getData(double *out, int nMax)
        int init()
        void close()
      
    c_Audio *new_Audio "new Audio" ()
    void del_Audio "delete" (c_Audio *AE)

cdef class AudioInterface:
    """Class for reading signals from the system soundcard.
       
       Initialise with 

       audio = pyrex_audio.AudioInterface()

       See test_audio.py for example usage.
    """
    cdef c_Audio *AE
    cdef double myBuffer[1000*2]
    
    def __cinit__(self):
       self.AE = new_Audio()
       self.AE.init()
    
    def __dealloc__(self):
       del_Audio(self.AE)
       
    def setHP(self, double freq):
       self.AE.setHighPassCutoff(freq)
       
    def setLP(self, double freq):
       self.AE.setLowPassCutoff(freq)
       
    def setSmooth(self, double freq):
       self.AE.setRmsCutoff(freq)
       
    def getData(self):
       cdef int n

       # call get data on the interface
       n = self.AE.getData(self.myBuffer, 1000)
       
       # if return value <=0, must have been a timeout
       if n<=0:
          raise IOError('Timeout occured')
       return (self.myBuffer[n-1],self.myBuffer[n])

