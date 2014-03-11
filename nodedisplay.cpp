/*
* Copyright (c) Jack M. Thompson WebKruncher.com, exexml.com
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the WebKruncher nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jack M. Thompson ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jack M. Thompson BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "x11grid.h"
using namespace X11Methods;
#include <treeset.h>
using namespace TreeObjects;
#include <trees.h>
using namespace TreeDisplay;
#include <math.h>
#include "nodedisplay.h"

namespace TreeDisplay
{
		//#define GHOSTS
		void NodeBase::operator()(Invalid& invalid,Window& window,Display* display,GC& gc,Pixmap& bitmap)
		{
			XSetForeground(display,gc,0X777777);
			while(!lpsxy.empty())
			{
				pair<pair<double,double>,pair<double,double> > p(lpsxy);
				XDrawLine(display,bitmap,gc,p.first.first,p.first.second,p.second.first,p.second.second);
			}
			SetFont(display,gc);
			//invalid.SetTrace(true);
			moved=false;
			pair<double,double> D(motion.next(X,Y));
			if ((D.first) or (D.second)) moved=true;
			{
				pair<double,double> ul(X-(DCW/2)-1,Y-(DCH/2)-1);
				pair<double,double> lr(ul.first+DCW+2,ul.second+DCH+2);
				Rect iv(ul.first,ul.second,lr.first,lr.second);
				#ifdef GHOSTS
					if (Remove) if (Removing<0X88) {Removed=true; return;}
					invalid.insert(iv);
				#else
					invalid.expand(iv);
				#endif
				if (Remove)
				{
					if (Removing<=0X77) 
					{
						Removed=true;
						XSetForeground(display,gc,0X777777);
					} else {
						unsigned long color((Removing<<0) | (Removing<<8) | (Removing));
						XSetForeground(display,gc,color);
					}
					XPoint& points(iv);
					XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
					if (!Removed)
					{
						XSetForeground(display,gc,0XFFFFFF);
						int yoff(CH);
						XDrawString(display,bitmap,gc,ul.first,ul.second+yoff-2,text.c_str(),text.size());
					}
					Removing-=10;
					return;
				}
			}
			DCH=CH+(size()*CH);
			DCW=CW;
			if (size()) DCW=CW*2;
			X+=D.first; Y+=D.second;
			{
				pair<double,double> ul(X-(DCW/2),Y-(DCH/2));
				pair<double,double> lr(ul.first+DCW,ul.second+DCH);
				Rect iv(ul.first,ul.second,lr.first,lr.second);
				#ifdef GHOSTS
					invalid.insert(iv);
				#else
					invalid.expand(iv);
				#endif
				XSetForeground(display,gc,0XFFFFFF);
				XPoint& points(iv); XPoint* pt=&points;
				for (int j=0;j<4;j++,pt++) lastpoints.push_back(*pt);
				XSetForeground(display,gc,color);
				XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
				XSetForeground(display,gc,0XFFFFFF);
				int yoff(CH);
				XDrawString(display,bitmap,gc,ul.first,ul.second+yoff-2,text.c_str(),text.size());
				for (iterator it=begin();it!=end();it++)
				{
					yoff+=CH;
					stringstream ss; ss<<it->first<<" : "<<it->second;
					XDrawString(display,bitmap,gc,ul.first,ul.second+yoff-2,ss.str().c_str(),ss.str().size());
				}
				if ( (parented) and (!Remove) ) 
				{
					XSetForeground(display,gc,0XAAAACC);
					XDrawLine(display,bitmap,gc,X,Y,PX,PY);
					lpsxy(X,Y,PX,PY);
				}
			}
		}
} // namespace TreeDisplay

