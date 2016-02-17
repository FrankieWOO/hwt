#include <Python.h>
//#include <ode/ode.h>
//#include <pyode.h>    
#include <FL/Fl.H>
#include <gui.h>

//#include <OdeWindow.hh>
#include <sliderbox.h>
#include <scope.h>
//#include <MatrixDisplay.hh>
//#include <BarDisplay.hh>

#ifdef WIN32

#include <windows.h>
#include <process.h>

static CRITICAL_SECTION threadLock;
static HANDLE guiThread;

#else

#include <pthread.h>

static pthread_mutex_t threadLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t guiThread;

#endif


static int numOpenWindows = 0;
static int threadCommand;  // 0 = nothing (processed), 1 = open a window, 2 = close a window, 3 = wait for input, 4 = export ppm
static void *threadData;
static int threadResult;


void doNothing_CB(Fl_Widget*, void*) {
}

void doCreateWindow(WinCreation *wc) {
/*   if (wc->type==SimGUI_Ode) {
//      OdeWindow *win = new OdeWindow(wc->x,wc->y,wc->w,wc->h,wc->title);
//      win->callback(doNothing_CB);
//
//      wc->object = (void *) win;
//      numOpenWindows++;
//      
//   } else */if (wc->type==SimGUI_Fader) {
      SliderBox *win = new SliderBox(wc->n,wc->x,wc->y,wc->w,wc->h,wc->title);
      win->callback(doNothing_CB);

      wc->object = (void *) win;
      numOpenWindows++;
   
   } else if (wc->type==SimGUI_Scope) {
      ScopeWindow *win = new ScopeWindow(wc->n,wc->x,wc->y,wc->w,wc->h,wc->title);
      win->callback(doNothing_CB);

      wc->object = (void *) win;
      numOpenWindows++;
//   } else if (wc->type==SimGUI_Matrix) {
//      MatrixDisplay *win = new MatrixDisplay(wc->m, wc->n, wc->x, wc->y, wc->w, wc->h, wc->title);
//      win->callback(doNothing_CB);
//
//      wc->object = (void *) win;
//      numOpenWindows++;
//   } else if (wc->type==SimGUI_Bar) {
      //BarDisplay *win = new BarDisplay(wc->m, wc->n, wc->x, wc->y, wc->w, wc->h, wc->title);
      //win->callback(doNothing_CB);

     // wc->object = (void *) win;
     // numOpenWindows++;
   }
   
   wc->error = 1;
}

void doCloseWindow(Fl_Window *win) {
   win->hide();
   Fl::flush();
   delete win;
   numOpenWindows--;
}

#ifdef WIN32

void threadFunc(LPVOID data) {
   //printf("In thread...\n");
   doCreateWindow((WinCreation *) data);
   threadCommand = 0;
   //printf("Window created...\n");
   
   while (numOpenWindows>0) {
      EnterCriticalSection(&threadLock);
      if (threadCommand == 1) {
        doCreateWindow((WinCreation *) threadData);
      } else if (threadCommand == 2) {
        doCloseWindow((Fl_Window *) threadData);
      } else if (threadCommand == 3) {
         SliderBox *slb = (SliderBox *) threadData;
         slb->ack();
         do {
            Fl::wait();
         } while (slb->lastMoved()==-2);
         threadResult = slb->lastMoved();
      } /* else if (threadCommand == 4) {
         SGC_Export *ex = (SGC_Export *) threadData;
         threadResult = ex->win->exportBitmap(ex->filename);
      } else if (threadCommand == 5) {
         OdeWindow *win = (OdeWindow *) threadData;
         win->exportFrame();
      }*/
      threadCommand = 0;
      Fl::wait(0);
      LeaveCriticalSection(&threadLock);   
      Sleep(40);  // ~ 25 Hz Refresh rate
   }
   //printf("End of threadFunc\n");
}

int createWindow(WinCreation *wc) {
   wc->error = 0;
   if (numOpenWindows==0) {
      InitializeCriticalSection(&threadLock);
     //printf("Crit Sec. initialized\n");
     threadCommand = 1;
     guiThread = (HANDLE) _beginthread(threadFunc, 0, wc);
     //printf("Thread created\n");
      do { 
        Sleep(10); 
      } while (threadCommand > 0);
   } else {
      EnterCriticalSection(&threadLock);
      threadData = (void *) wc;
      threadCommand = 1;
      LeaveCriticalSection(&threadLock);
      do {
         Sleep(10);
      } while (threadCommand > 0);
   }
   return (wc->error == 0);
}

void closeWindow(Fl_Window *win) {
 
   EnterCriticalSection(&threadLock);
   threadData = (void *) win;
   threadCommand = 2;
   LeaveCriticalSection(&threadLock);
   
   do { 
      Sleep(10); 
   } while (threadCommand>0);
   
   //printf("Slept long enough\n");
   
   if (numOpenWindows==0) {
     WaitForSingleObject(guiThread, INFINITE); 
      // printf("GUI thread has stopped!\n");
      DeleteCriticalSection(&threadLock);
   }
}




void lockGUI() {
   EnterCriticalSection(&threadLock);
}

void unlockGUI() {
   LeaveCriticalSection(&threadLock);
}

void shortDelay() {
   Sleep(10); // 10 ms
}

void mediumDelay() {
   Sleep(40); // 40 ms
}


#else

void *threadFunc(void *data) {
   //printf("In thread...\n");
   doCreateWindow((WinCreation *) data);
   threadCommand = 0;
   //printf("Window created...\n");
   
   while (numOpenWindows>0) {
      pthread_mutex_lock(&threadLock);
      if (threadCommand == 1) {
        doCreateWindow((WinCreation *) threadData);
      } else if (threadCommand == 2) {
        doCloseWindow((Fl_Window *) threadData);
      } else if (threadCommand == 3) {
         SliderBox *slb = (SliderBox *) threadData;
         slb->ack();
         while (1) {
//            Fl::wait();
            Fl::wait(0);
            if (slb->lastMoved()!=-2) break;
            usleep(40000);
         } 
         threadResult = slb->lastMoved();
      } /*else if (threadCommand == 4) {
         SGC_Export *ex = (SGC_Export *) threadData;
         threadResult = ex->win->exportBitmap(ex->filename);
      } else if (threadCommand == 5) {
         OdeWindow *win = (OdeWindow *) threadData;
         win->exportFrame();
      }*/
      threadCommand = 0;
      Fl::wait(0);
      pthread_mutex_unlock(&threadLock);
      usleep(40000);  // ~ 25 Hz Refresh rate
   }
   //printf("End of threadFunc\n");
   return NULL;
}

int createWindow(WinCreation *wc) {
   wc->error = 0;
   if (numOpenWindows==0) {
      threadCommand = 1;
      if (pthread_create(&guiThread, NULL, threadFunc, wc)) return 0;
      //printf("Thread created\n");
      do { 
         usleep(10000); 
      } while (threadCommand > 0);
   } else {
      pthread_mutex_lock(&threadLock);
      threadData = (void *) wc;
      threadCommand = 1;
      pthread_mutex_unlock(&threadLock);
      do {
         usleep(10000);
      } while (threadCommand > 0);
   }
   return (wc->error == 0);
}

void closeWindow(Fl_Window *win) {
   pthread_mutex_lock(&threadLock);
   threadData = (void *) win;
   threadCommand = 2;
   pthread_mutex_unlock(&threadLock);
  
   do { 
      usleep(10000); 
   } while (threadCommand>0);
      
   if (numOpenWindows==0) {
      pthread_join(guiThread, NULL);
      // printf("GUI thread has stopped!\n");
   }
}


void lockGUI() {
   pthread_mutex_lock(&threadLock);
}

void unlockGUI() {
   pthread_mutex_unlock(&threadLock);
}

void shortDelay() {
   usleep(10000); // 10 ms
}

void mediumDelay() {
   usleep(40000); // 40 ms
}

#endif


int awaitSlider(SliderBox *slb) {
   lockGUI();
   threadData = (void *) slb;
   threadCommand = 3;
   unlockGUI();
   
   do { 
      shortDelay();
   } while (threadCommand>0);
   
   return threadResult;
}

//int exportBitmap(OdeWindow *win, const char *filename) {
//   SGC_Export ex;
//   
//   ex.win = win;
//   ex.filename = filename;
//  
//   lockGUI();
//   threadCommand = 4;
//   threadData = &ex;
//   unlockGUI();
//
//   do { 
//      shortDelay();
//   } while (threadCommand>0);   
//   
//   return threadResult;  
//}
//
//
//void exportFrame(OdeWindow *win) {
//   lockGUI();
//   threadCommand = 5;
//   threadData = (void *) win;
//   unlockGUI();
//
//   do { 
//      shortDelay();
//   } while (threadCommand>0);   
//}
