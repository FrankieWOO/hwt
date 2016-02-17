#ifndef _SIMGUIX_HH
#define _SIMGUIX_HH

#include <Fl/Fl_Window.H>

class SliderBox;
//class OdeWindow;
//class MatrixDisplay;

enum SimGUI_WinType {
   SimGUI_Unknown = 0,
   SimGUI_Ode,
   SimGUI_Fader,
   SimGUI_Scope,
   SimGUI_Matrix,
   SimGUI_Bar
};

struct WinCreation {
   SimGUI_WinType type;
   
   int x,y,w,h,m,n;
   const char *title;
   
   int error;
   void *object;   
};

//struct SGC_Export {
//   OdeWindow *win;
//   const char *filename;
//};

void lockGUI();
void unlockGUI();
void shortDelay();
void mediumDelay();
int createWindow(WinCreation *wc);
void closeWindow(Fl_Window *win);
int awaitSlider(SliderBox *slb);
//int exportBitmap(OdeWindow *win, const char *filename);
//void exportFrame(OdeWindow *win);

#endif
