/*********************************************************************
Underlying C++ class for Scope
Copyright (C) 2007 Stefan Klanke
Contact: s.klanke@ed.ac.uk

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either 
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*********************************************************************/

#include <Fl/Fl_Double_Window.H>
#include <Fl/Fl_Scrollbar.H>
#include <Fl/Fl_Slider.H>
#include <Fl/Fl_Box.H>
#include <Fl/fl_draw.H>
#include <stdio.h>
#include <stdlib.h>

class ScopeWindow : public Fl_Double_Window {
   public:
   
   static const int BW = 100;
   
   ScopeWindow(int numInputs, int X, int Y, int W, int H, const char *L);
      
   virtual ~ScopeWindow() {
      for (int i=0;i<NB;i++) {
         free(data[i]);
      } 
      free(data);
      delete[] colors;
   }
      
   void updateInfoBox() {
      char text[100];
      sprintf(text,"Seqs: %i\nN: %6i",d,N);
      infobox->copy_label(text);
   }
   
   void clear() { 
      N=0; 
      // free all blocks apart from the first
      for (int i=1;i<NB;i++) free(data[i]);
      NB=1;
      redraw();
      maxV = minV = 0;
   }
   
   void add(const double *values);
   int enlarge();
   void changedZoom();
   void changedScroll() { redraw(); }   
   
   void putXTick(int value, int xpos);
   void putYTick(float value, int ypos,int prec);
   
   virtual void draw();

   void setJetScheme();
   void setColor(int index, float r, float g, float b);
   void setBlackBG(int yesno);
   
   double value(int i, int j) {
      int nb = j >> 10;
      int jb = j & 1023;
      
      return data[nb][i+jb*d];
   }
   
   protected:
      
   double *ptr(int j) {
      int nb = j >> 10;
      int jb = j & 1023;
      return data[nb] + jb*d;
   }
   
   Fl_Scrollbar *scrollbar;
   Fl_Slider *zoomer;
   Fl_Box *infobox;
   Fl_Box *canvas;
   
   Fl_Color *colors;
   
   int d, N, NB, NBalloc, blackBG;
   double **data;
   double maxV,minV;
};
