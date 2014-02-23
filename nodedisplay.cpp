

#include "x11grid.h"
using namespace X11Methods;
#include <tree.h>
using namespace TreeObjects;
#include <trees.h>
using namespace TreeDisplay;
#include <math.h>
#include "nodedisplay.h"

namespace TreeDisplay
{
		void NodeBase::operator()(Invalid& invalid,Window& window,Display* display,GC& gc,Pixmap& bitmap)
		{
			//invalid.SetTrace(true);
			moved=false;
			pair<double,double> D(motion.next(X,Y));
			if ((D.first) or (D.second)) moved=true;
			if (moved)
			{
				pair<double,double> ul(X-(DCW/2)-1,Y-(DCH/2)-1);
				pair<double,double> lr(ul.first+DCW+2,ul.second+DCH+2);
				Rect iv(ul.first,ul.second,lr.first,lr.second);
				invalid.expand(iv);
				//XSetForeground(display,gc,0X777777);
				//invalid.Draw(display,bitmap,window,gc);
				//XPoint& points(iv);
				//XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			}
			DCH=CH+(size()*CH);
			DCW=CW;
			if (size()) DCW=CW*2;
			X+=D.first; Y+=D.second;
			{
				pair<double,double> ul(X-(DCW/2),Y-(DCH/2));
				pair<double,double> lr(ul.first+DCW,ul.second+DCH);
				Rect iv(ul.first,ul.second,lr.first,lr.second);
				invalid.expand(iv);
				XSetForeground(display,gc,0XFFFFFF);
				//invalid.Draw(display,bitmap,window,gc);
				XPoint& points(iv); XPoint* pt=&points;
				for (int j=0;j<4;j++,pt++) lastpoints.push_back(*pt);
				XSetForeground(display,gc,color);
				XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
				XSetForeground(display,gc,0XFFFFFF);
				int yoff(CH);
				XDrawString(display,bitmap,gc,ul.first,ul.second+yoff,text.c_str(),text.size());
				for (iterator it=begin();it!=end();it++)
				{
					yoff+=CH;
					stringstream ss; ss<<it->first<<" : "<<it->second;
					XDrawString(display,bitmap,gc,ul.first,ul.second+yoff,ss.str().c_str(),ss.str().size());
				}
			}
		}
} // namespace TreeDisplay

