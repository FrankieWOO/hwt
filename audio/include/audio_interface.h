/** \defgroup Audio Soundcard Interface library. 
 *
 * This library provides routines and definitions for reading signals from the
 * system soundcard. The open source PortAudio library is used for real-time
 * signal recording of the audio input. Note that, the system requires a proper
 * stereo line input to work.  
 *
 * To set up and use the Audio Interface system: 
 * -# Build the pyrex_audio module with pyrex_audio.so 
 * -# Start "audacity" and monitor the sound input (click on the arrow close to
 *  the microphone icon in the toolbar). If you get red bars on both channels,
 *  you should be set. If you are not seeing anything, or
 *  the signals are moving in unison (indicating you are only getting mono, not
 *  stereo, input) you can use "alsamixer" to configure the sound options. 
 * -# Use the test program to visualise the signals: "ipython -i test_audio.py". 
 *
 * On Ubuntu, the following packages are required:
 * - portaudio19-dev
 *
 * \sa <a href="http://portaudio.com/">PortAudio Homepage</a>
 * \sa <a href="http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread.h.html">POSIX thread API: \<pthread.h\></a>
 * \sa <a href="http://pubs.opengroup.org/onlinepubs/007908799/xsh/time.h.html">Time API: \<time.h\></a>
 *
 * \author Stefan Klanke, Matthew Howard (mh), matthew.howard@kcl.ac.uk
 */

/**
 * \file audio_interface.h
 * \brief Main header file of the soundcard interface library.
 * \author Stefan Klanke, Matthew Howard (mh), matthew.howard@kcl.ac.uk
 * \ingroup Audio
 */

#ifndef __libaudio_h
#define __libaudio_h

#include <portaudio.h>
#include <math.h>

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#endif

#include <ctype.h>
#include <stdio.h>

#define RMSLEN  4400
#define RB_LEN  10000 /**  */
/** Milliseconds before time out on reading data from the soundcard. */ 
#define AUDIO_TIMEOUT 100

/** \brief Ring buffer class. */
class RingBuffer {
	public:
		/** Current index where incoming data is to be stored in the buffer.  */
		int start;
		/** Number of data points contained in the ring buffer.  */
		int elements;

		/** \brief Clear the buffer. */
		void clear() {
			start = elements = 0;
		}

		/** 
		 * \brief Add data to the buffer. 
		 * \param[in] L Data from left signal
		 * \param[in] R Data from right signal
		 */
		void add(float L, float R) {
			/* If no. elements same as max. length of buffer (max. length)... */
			if (elements==RB_LEN) { 
				left [start] = L;                 /* ...copy data... */
				right[start] = R;
				if (++start == RB_LEN) start = 0; /* ...and reset index to zero, if we are about to reach the end of the buffer. */
			} 
			/* otherwise, continue fill the buffer, until we reach the max. length */
			else {
				int ptr = start + elements; /* compute current index */
				if (ptr>RB_LEN) ptr-=RB_LEN;
				left [ptr] = L;
				right[ptr] = R;
				elements++;
			}
		}

		/** Array for holding left signal data. */
		float left [RB_LEN];
		/** Array for holding right signal data. */
		float right[RB_LEN];
};

class TwoPole {
   public:
   
   static const float QFAC = 1.41421356237310; // 2.0*cos(0.25*M_PI)  or sqrt(2.0)

   float a0, a1, a2, b1, b2;
   float y1, y2, x1, x2;
   
   void clear() {
      y1 = y2 = x1 = x2 = 0.0;
   }
   
   void setHP(float cutoff) {
      double w = tan(M_PI * cutoff);
      
      if (cutoff<1e-4) {
         a0 = 1.0;
         a1 = 0.0;
         a2 = 0.0;
         b1 = 0.0;
         b2 = 0.0;
      } else {
         float b0 = 1.0 + w*QFAC + w*w;
      
         a0 = a2 = 1;
         a1 = -2.0;
         b1 = 2.0*(w*w - 1.0) / b0;
         b2 = (1.0 - w*QFAC + w*w) / b0;
      }
   }
   
   void setLP(float cutoff) {
      double w = tan(M_PI * cutoff);
      float b0 = 1.0 + w*QFAC + w*w;
      
      if (cutoff<1e-4) {
         a0 = 0.0;
         a1 = 0.0;
         a2 = 0.0;
         b1 = 0.0;
         b2 = 0.0;
      } else {
         a0 = a2 = w*w / b0;
         a1 = 2.0*w*w / b0;
         b1 = 2.0*(w*w - 1.0) / b0;
         b2 = (1.0 - w*QFAC + w*w) / b0;
      }
   }
   
   inline float process(float x) {
      float y = a0*x + a1*x1 + a2*x2 - b1*y1 - b2*y2;
      
      if (fabsf(y)<1e-20) y=0.0; // care about denormals
      
      y2 = y1;
      y1 = y;
      x2 = x1;
      x1 = x;
      return y;
   }
};

/** \brief Mutex class. 
 *
 * Handles locking of shared data structures, to ensure they are not accessed
 * by multiple threads at the same time.
 *
 * This is a wrapper that allows Windows/Unix Mutex to be called through the
 * same interface.
 * 
 */
class CriticalSection {
#ifdef WIN32
	protected:
		CRITICAL_SECTION lock; 
	public:
		CriticalSection()  { InitializeCriticalSection(&lock); }
		~CriticalSection() {     DeleteCriticalSection(&lock); }
		void enter()       {      EnterCriticalSection(&lock); }
		void leave()       {      LeaveCriticalSection(&lock); }
#else
	protected:
		pthread_mutex_t lock; /** \brief Mutex */
	public:
		CriticalSection()  { pthread_mutex_init  (&lock, NULL);}
		/** \brief Apply mutex lock */  		
		void enter()       { pthread_mutex_lock  (&lock);      }
		/** \brief Release mutex lock */		
		void leave()       { pthread_mutex_unlock(&lock);      }
#endif
};

/** \brief Audio system class. 
 *
 * Holds the PortAudio stream, and a set of filters/buffers that process the recorded data.
 * 
 */
class Audio {
	protected:
		TwoPole lp1, lp2, hp1, hp2, lpL, lpR, lpLa, lpRa;
		RingBuffer rb;
		/** \brief PortAudio stream struct, for streaming data from soundcard */ PaStream *stream; 
		int framesToGo;
		float sampleRate, hpCut, lpCut, rmsCut, outFreqL, outFreqR;
		bool inProcess;
		float ph1, ph2;
		CriticalSection critSec;

	public:

		Audio() {
			stream = NULL;
			hpCut = 30.0;
			lpCut = 1000.0;
			rmsCut = 30.0;
			outFreqL = 100.0;
			outFreqR = 100.0;
			sampleRate = 44100.0;
		}

		~Audio() {
			close();
		}

		void setLowPassCutoff(float freq) {
			if (freq<0.0) freq = 0.0;
			lp1.setLP(2.0*freq/sampleRate);
			lp2.setLP(2.0*freq/sampleRate);
		}

		void setHighPassCutoff(float freq) {
			if (freq<0.0) freq = 0.0;
			hp1.setHP(2.0*freq/sampleRate);
			hp2.setHP(2.0*freq/sampleRate);
		}

		void setRmsCutoff(float freq) {
			if (freq<0.0) freq = 0.0;
			lpL .setLP(2.0*freq/sampleRate);
			lpR .setLP(2.0*freq/sampleRate);
			lpLa.setLP(2.0*freq/sampleRate);
			lpRa.setLP(2.0*freq/sampleRate);
		}   

		int  init();
		void close();
		void callback(const float *inLR, float *outLR, unsigned long frameCount);
		int  getData(double *out, int nMax);


	protected:
		void initProcess();
};

#endif


