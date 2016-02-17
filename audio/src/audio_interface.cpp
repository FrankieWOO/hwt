/**
 * \file audio_interface.cpp
 * \brief Main source file of the soundcard interface library.
 * \author Stefan Klanke, Matthew Howard (MH), matthew.howard@kcl.ac.uk
 * \ingroup Audio
 */

#include "audio_interface.h"


/**
 * \brief Callback function for the PortAudio stream.
 *
 * This function is passed to PortAudio when setting up the stream, and is used
 * for processing the signal data. This is actually just a wrapper for the
 * Audio::callback() function.
 *
 * \returns paContinue (0) to signal that stream should continue to run.
 */
static int audioCallback(const void *input, void *output, unsigned long frameCount, 
                  const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
                  void *userData) {

   const float *inLR;
   float *outLR;

   inLR  = (float *) input;
   outLR = (float *) output;

   if (userData != NULL) {
      ((Audio *) userData)->callback(inLR, outLR, frameCount);
   }

   return paContinue;
}

/**
 * \brief Initialise interface.
 *
 * This function
 * - detects audio devices and selects the one with the lowest latency with at least two i/o channels
 * - calls initProcess() to set up filters and buffers
 * - sets up a PortAudio stream to gather data from the device
 *
 * \returns 1 if successful, 0 otherwise.
 */
int Audio::init() 
{
   int i;

   int numDevices;                       /** no. audio devices available */
   double latency = 1000.0;              /** some 'large' latency value in seconds (used in selecting the best device to use -- see below) */
   int bestDevice = -1;                  /** index of the best device  (used in selecting the best device to use -- see below) */

   PaError err;                          /** PortAudio error codes */
   const PaDeviceInfo *deviceInfo;       /** struct providing information and capabilities of PortAudio devices */
   const PaHostApiInfo *apiInfo;         /** struct containing information about a particular host API */
   PaStreamParameters parIn, parOut;     /** parameters for one direction (input or output) of a stream. */
   double latIn=latency, latOut=latency; /** I/O latency (initialise to 'large' latency) */
   PaHostApiTypeId chosenApi;

   /* initialise PortAudio library */
   err = Pa_Initialize();            
   if( err != paNoError ) {
      fprintf(stderr, "Pa_Initialize() returned error\n");
      return 0;
   }

   /* retrieve the number of available devices */
   numDevices = Pa_GetDeviceCount(); 
   /* if any errors, exit */
   if (numDevices < 0) { /* if error, Pa_GetDeviceCount() returns negative number. */
      fprintf(stderr, "Pa_GetDeviceCount() returned 0x%4x\n", numDevices );
      Pa_Terminate();
      return 0;
   }

   /* for each device */
   for( i=0; i<numDevices; i++ ) {
      double lat_i; /* latency of ith device */

	  /* get device and API info, print stuff out */
      deviceInfo = Pa_GetDeviceInfo( i );
      apiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
      printf("%i: %s\n`-%s  %i inputs, %i outputs, %.0f Hz, %fs / %fs I/O latency\n", i, deviceInfo->name, 
         apiInfo->name, 
         deviceInfo->maxInputChannels,
         deviceInfo->maxOutputChannels,
         deviceInfo->defaultSampleRate,
         deviceInfo->defaultLowInputLatency,
         deviceInfo->defaultLowOutputLatency);
 
	  /* get default latency values for interactive performance */
      lat_i = deviceInfo->defaultLowInputLatency + deviceInfo->defaultLowOutputLatency;

      #ifdef ASIOONLY
      if (apiInfo->type != paASIO) continue;
      #endif

	  /* if latency of ith device less than current minimum latency, and device
	   * has at least two i/o channels, select as best device and record
	   * latency and API type */
      if (lat_i < latency && deviceInfo->maxInputChannels>=2 && deviceInfo->maxOutputChannels>=2) {
         latency = lat_i;
         latIn  = deviceInfo->defaultLowInputLatency;
         latOut = deviceInfo->defaultLowOutputLatency;
         bestDevice = i;
         chosenApi = apiInfo->type;
      }

   }
   /* if no devices are suitable, exit */
   if (bestDevice<0) {
      fprintf(stderr,"No suitable device found\n");
      Pa_Terminate();
      return 0;
   }
   /* print out the chosen device and latency */
   printf("Using device %i with latency %f\n",bestDevice,latency);

   /* save parameters of input/output streams of the best device */
   parIn.device = bestDevice;
   parIn.channelCount = 2;
   parIn.sampleFormat = paFloat32;
   parIn.suggestedLatency = latIn;
   parIn.hostApiSpecificStreamInfo = NULL;

   parOut.device = bestDevice;
   parOut.channelCount = 2;
   parOut.sampleFormat = paFloat32;
   parOut.suggestedLatency = latOut;
   parOut.hostApiSpecificStreamInfo = NULL;

   /* open a stream with the input/output parameters found */
   err = Pa_OpenStream(&stream, &parIn, &parOut, 
		   44100.0,       /* sample rate */
		   0,             /* unspecified no. frames per buffer */
		   paNoFlag, 
		   audioCallback, /* call back function (see below). PortAudio calls this periodically. */
		   this); 
   /* if opening successful get info, print out to user, otherwise exit */
   if (err == paNoError) {
      const PaStreamInfo *info;

      info = Pa_GetStreamInfo(stream);
      printf("Opened audio stream with %f Hz, %fs / %fs IO-latency\n", info->sampleRate, 
               info->inputLatency, info->outputLatency);
   } else {
      Pa_Terminate();
      return 0;
   }
   /* set up filters and buffers (see below) */
   initProcess();
   /* start streaming */
   err = Pa_StartStream(stream);
   
   return 1;
}

/**
 * \brief Initialise signal processing.
 *
 * This function sets up a collection of filters and buffers for processing the signal from the soundcard.
 */
void Audio::initProcess() 
{
	lp1.clear();  // clear low pass filters
	lp2.clear();  //
	hp1.clear();  // clear high pass filters
	hp2.clear();  //
	rb.clear();   // clear ring buffer

	framesToGo = (int) (sampleRate / 100.0);
	inProcess = false;

	ph1 = ph2 = 0.0;

	hp1 .setHP(2.0* hpCut/sampleRate); // set cutoff frequencies
	hp2 .setHP(2.0* hpCut/sampleRate); 
	lp1 .setLP(2.0* lpCut/sampleRate);
	lp2 .setLP(2.0* lpCut/sampleRate);
	lpL .setLP(2.0*rmsCut/sampleRate);
	lpR .setLP(2.0*rmsCut/sampleRate);
	lpLa.setLP(2.0*rmsCut/sampleRate);
	lpRa.setLP(2.0*rmsCut/sampleRate);
}

/**
 * \brief Callback function.
 *
 * This function is passed to the PortAudio stream for processing the signal from the soundcard.
 *
 * \param[in] inLR 
 * \param[in] outLR
 * \param[in] frameCount
 *
 */
void Audio::callback(const float *inLR, float *outLR, unsigned long frameCount) 
{
   unsigned long i;

   /* processing data now */
   inProcess = true;

   /*  */
   float f1 = 2.0*M_PI*outFreqL/sampleRate;
   float f2 = 2.0*M_PI*outFreqR/sampleRate;

   /* for each frame of data */
   for (i=0;i<frameCount;i++) {
      float s1, s2;

      s1 = fabsf(lpLa.process(lpL.process(fabsf(lp1.process( hp1.process( *inLR++ ))))));
      s2 = fabsf(lpRa.process(lpR.process(fabsf(lp2.process( hp2.process( *inLR++ ))))));
      
      
      if (--framesToGo<=0) {
         critSec.enter();
         rb.add(s1,s2);
         critSec.leave();
         framesToGo = 441; // next output in 10 ms
      }
      
      *outLR++ = s1*sin(ph1);
      *outLR++ = s2*sin(ph2);
      ph1+=f1;
      ph2+=f2;
   }
   
   inProcess = false;

}

/**
 * \brief Close interface.
 *
 * Stops and closes the PortAudio stream, deallocates resources used by PortAudio, exits.
 */
void Audio::close() 
{
   PaError err;
   if (stream!=NULL) {
      err = Pa_StopStream(stream);   
      err = Pa_CloseStream(stream);
      err = Pa_Terminate();
      stream = NULL;
   }
}

/**
 * \brief Read from interface.
 * \param[out] out
 * \param[in] nMax
 *
 * \returns no. elements read from ring buffer if successful, -1 otherwise.
 */
	int
Audio::getData(double *out, int nMax) 
{
   int i,n;   
   
   i = AUDIO_TIMEOUT;
   
   /* poll for data in the in ring buffer every second, with timeout after AUDIO_TIMEOUT seconds. */
   while (rb.elements == 0) {
      if (--i<0) return -1; /* if timeout, then return -1. */
      #ifdef WIN32
      Sleep(1);
      #else
      usleep(1000);
      #endif
   }

   /* critical section for reading from stream. lock mutex. */
   critSec.enter();
   
   /* determine number of elements of buffer to read */
   n = nMax; /* either number specified in function argument */
   if (rb.elements<n) n = rb.elements; /* or, if there are not that many elements, then all elements in buffer */

   /* compute pointer */
   int ptr = rb.start + rb.elements - n;
   if (ptr>RB_LEN) ptr-=RB_LEN;

   /* iterate through ring buffer elements, copy to output array */
   for (i=0;i<n;i++) {
      out[2*i  ] = rb.left [ptr];
      out[2*i+1] = rb.right[ptr];
	  /* if we reach the end of the buffer, wrap around to the start */
      if (++ptr == RB_LEN) ptr = 0;
   }

   /* clear ring buffer */
   rb.clear();

   /* leave critical section. unlock mutex. */
   critSec.leave();

   return n;
}

