/** 
 * \file scope.cpp
 * \author This version: Matthew Howard (MH), matthew.howard@kcl.ac.uk. Based on an original implementation by Stefan Klanke.
 * \date 01/11/14 21:10:48
 * \brief Underlying C++ class for Scope.
 * 
 */

/*********************************************************************
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*********************************************************************/

#include <Fl/Fl_Double_Window.H>
#include <Fl/Fl_Scrollbar.H>
#include <Fl/Fl_Slider.H>
#include <Fl/Fl_Box.H>
#include <Fl/fl_draw.H>
#include <scope.h>
#include <math.h>
#include <string.h>


static void zoomer_callback(Fl_Widget *widget, void *data) {
   ((ScopeWindow *) data)->changedZoom();
}

static void scroller_callback(Fl_Widget *widget, void *data) {
   ((ScopeWindow *) data)->changedScroll();
}


ScopeWindow::ScopeWindow(int numInputs, int X, int Y, int W, int H, const char *L) : Fl_Double_Window(X,Y,W,H,L) {
   NBalloc = 100;
   NB = 1;
   d = numInputs;
   data = (double **) malloc(NBalloc*sizeof(double *));
   data[0] = (double *) malloc(d*1024*sizeof(double));
   N = 0;

   maxV = minV = 0;

   begin();
   scrollbar = new Fl_Scrollbar(BW,H-20,W-BW,20);
   zoomer = new Fl_Slider(0,H-20,BW,20);
   infobox = new Fl_Box(0,0,BW,H-20);
   canvas = new Fl_Box(BW,0,W-BW,H-20);
   end();

   scrollbar->type(FL_HORIZONTAL);
   scrollbar->callback(scroller_callback, (void *) this);
   scrollbar->bounds(0,0);
   zoomer->type(FL_HOR_NICE_SLIDER);
   zoomer->callback(zoomer_callback, (void *) this);
   zoomer->bounds(1,10);
   zoomer->value(1);
   infobox->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT|FL_ALIGN_TOP);
   updateInfoBox();
   resizable(canvas);

   colors = new Fl_Color[d];
   setJetScheme();

   blackBG = 0;

   show();
}


void ScopeWindow::add(const double *values) {
   double *pn;
   if ((N & 1023) == 1023) {
      // go to next block
      if (NB==NBalloc-1) {
         if (!enlarge()) return;
      }
      data[NB] = (double *) malloc(1024*d*sizeof(double));
      if (data[NB] == NULL) return;
      NB++;
   }
   N++;
   pn = ptr(N);
   for (int i=0;i<d;i++) {
      double vi = values[i];

      pn[i] = vi;
      if (vi>maxV) maxV = vi;
      if (vi<minV) minV = vi;
   }
   updateInfoBox();

   int displayed = (int) ((double) (w()-BW)*zoomer->value());
   int pos = N - displayed;
   if (pos<0) pos=0;
   scrollbar->value(pos,displayed,0,N);
   redraw();
}

int ScopeWindow::enlarge() {
   void *newptr = realloc(data, sizeof(double)*(NBalloc + 100));

   if (newptr!=NULL) {
      data = (double **) newptr;
      NBalloc += 100;
      return 1;
   }

   return 0;
}

void ScopeWindow::changedZoom() {
   int displayed = (int) ((double) (w()-BW)*zoomer->value());

   int pos = scrollbar->value();

   if (pos + displayed > N) {
      pos = N - displayed;
      if (pos<0) pos=0;
   }
   scrollbar->value(pos,displayed,0,N);
   redraw();
}

void ScopeWindow::putXTick(int value, int xpos) {
   char str[30];
   if (blackBG) {
      fl_color(FL_GRAY);
   } else {
      fl_color(FL_BLACK);
   }
   sprintf(str,"%i",value);
   fl_draw(str,xpos,h()-21);
   fl_yxline(xpos,0,h()-20);
}

void ScopeWindow::putYTick(float value, int ypos,int prec) {
   char str[30];
   fl_color(FL_BLACK);
   sprintf(str,"%.*f",prec,value);
   fl_draw(str,0,ypos,BW-4,0,FL_ALIGN_RIGHT);
   fl_color(FL_GRAY);
   fl_xyline(BW,ypos,w());
}

void ScopeWindow::draw() {
   float xscale, yscale,range;
   int yoffset,si,ei,displayed;
   double zoomVal = zoomer->value();

   Fl_Window::draw();

   if (blackBG) {
      fl_color(FL_BLACK);
   } else {
      fl_color(FL_WHITE);
   }
   fl_rectf(BW,0,w()-BW,h()-20);

   range = maxV - minV;

   if (range<1e-6) range = 1e-6;

   xscale = 1./zoomVal;

   fl_line_style(FL_DOT);

   if (range<1e-12) {
      yoffset = (h()-20)/2;
      yscale = 0;
      putYTick(maxV, yoffset,1);

   } else {
      yscale = (h()-24)/range;
      yoffset = 2;

      double logRange = floor(log10(range));
      double typical  = pow(10,logRange-1);

      int ystart = (int) ceil(minV/typical);
      int yend   = (int) floor(maxV/typical);
      int prec   = 1-(int) logRange;
      if (prec<0) prec=0;
      int i;

      for (i=0;i<yend;i+=5) {

         putYTick(i*typical, (int) (yoffset + (maxV - (double)i*typical)*yscale), prec);
      }
      if (i==5) putYTick(i*typical, (int) (yoffset + (maxV - (double)yend*typical)*yscale), prec);
      for (i=-5;i>ystart;i-=5) {
         putYTick(i*typical, (int) (yoffset + (maxV - (double)i*typical)*yscale), prec);
      }
      if (i==-10) putYTick(i*typical, (int) (yoffset + (maxV - (double)ystart*typical)*yscale), prec);
   }

   displayed = (int) (zoomVal * double (w()-BW));

   si = scrollbar->value();
   ei = si + displayed;
   if (ei>N) ei = N;


   int incrX;

   if (zoomVal>7) {
      incrX = 1000;
   } else if (zoomVal>3.5) {
      incrX = 500;
   } else if (zoomVal>1.2) {
      incrX = 200;
   } else {
      incrX = 100;
   }

   int firstX = si/incrX + 1;
   int lastX  = ei/incrX;

   for (int iX = firstX;iX<=lastX; iX++) {
      putXTick(iX*incrX, (int) ((float)(iX*incrX-si)*xscale) + BW);
   }

   // draw grid

   fl_line_style(FL_SOLID);



   // draw the sequences
   for (int i=0;i<d;i++) {
      fl_color(colors[i]);
      fl_begin_line();
      for (int j=si;j<ei;j++) {
         fl_vertex((j-si)*xscale+BW,yoffset+(maxV-value(i,j))*yscale);
      }
      fl_end_line();
   }

}

void ScopeWindow::setJetScheme() {
   if (d==1) {
      colors[0]=fl_rgb_color(255,0,0);
   } else {
      float tbd = 2.0/(d-1.0);

      for (int i=0;i<d;i++) {
         float r = sqrtf(fmaxf(0.0f, 1.0f - i*tbd));
         float g = 1.0 - (i*tbd - 1.0f)*(i*tbd - 1.0f);
         float b = sqrtf(fmaxf(0.0f, i*tbd - 1.0f));

         //printf("%f  %f  %f\n",r,g,b);

         colors[i]=fl_rgb_color((uchar) (r*255.0f),(uchar) (g*255.0f), (uchar) (b*255.0f));
      }
   }
}

void ScopeWindow::setColor(int index, float r, float g, float b) {
   if (index<0 || index>=d) return;
   colors[index]=fl_rgb_color((uchar) (r*255.0f),(uchar) (g*255.0f), (uchar) (b*255.0f));
}

void ScopeWindow::setBlackBG(int yesno) {
   blackBG = yesno;
   redraw();
}
