/** 
 * \file sliderbox.h
 * \author This version: Matthew Howard (MH), matthew.howard@kcl.ac.uk, based on an original implementation by Stefan Klanke (2007).
 * \date 02/11/14 07:23:03
 * \brief Underlying C++ class for SliderBox
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

#include <Fl/Fl_Window.H>
#include <Fl/Fl_Value_Slider.H>
#include <Fl/Fl_Button.H>

class SliderBox : public Fl_Window {
   public:
   
   SliderBox(int numSliders, int X, int Y, int W, int H, const char *L) : Fl_Window(X,Y,W,H,L) {
      if (numSliders==0) {
         show();
         return;
      }
      int y = 10;
      int h = (H-50)/(numSliders);
      
      begin();
      
      numS = numSliders;
      sliders = new Fl_Value_Slider*[numSliders];
      
      for (int i=0;i<numS;i++) {
         sliders[i] = new Fl_Value_Slider(10,y,W-20,h-10);
         sliders[i]->type(FL_HOR_NICE_SLIDER);
         sliders[i]->align(FL_ALIGN_RIGHT);
         sliders[i]->callback(sliderCallback,i);
         y+=h;
      }
      
      exitButton = new Fl_Button(10,y,120,h-10,"Exit");
      exitButton->callback(sliderCallback,(-1));
      
      hasLabels = 0;
      exitPressed = 0;
      
      requestW = requestH = -1;
      
      end();
      show();
   }
         
   virtual ~SliderBox() {
      delete[] sliders;  
   }
   
   int numSliders() const { return numS; }
   
   void setup(int num, double min, double max, char *label) {
      if (num<0 || num >= numS) return;
      
      if (label!=NULL && !hasLabels) {
         hasLabels = 1;
         requestW = w()+120;
         requestH = h();
      }
            
      sliders[num]->bounds(min,max);
      sliders[num]->copy_label(label);
   }
   
   void setValue(int num, double value) {
      if (num<0 || num >= numS) return;
      sliders[num]->value(sliders[num]->clamp(value));
   }
   
   void setStep(int num, double value) {
      if (num<0 || num >= numS) return;
      sliders[num]->step(value);
   }
   
   double getValue(int num) {
      if (num<0 || num >= numS) return 0;
      return sliders[num]->value();
   }
   
   virtual void draw() {
      if (requestW!=-1) {
         size(requestW,requestH);
         requestW=-1;
      }
      Fl_Window::draw();
   }
   
   static void sliderCallback(Fl_Widget *widget, long data) {
      SliderBox *slb = (SliderBox *) widget->parent();
      slb->lastMove = data;
      
      if (data == -1) {
         slb->exitPressed = 1;
      }
   }
   
   void ack() { lastMove = -2; }
   int lastMoved() { return lastMove; }
   int checkExit() {
      int state = exitPressed;
      exitPressed = 0;
      return state;
   }
   
   protected:
   
   Fl_Value_Slider **sliders;
   Fl_Button *exitButton;
   int numS;
   int hasLabels;
   int requestW, requestH;
   int lastMove;
   int exitPressed;
};
