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
#ifndef NODE_DISPLAY_H
#define NODE_DISPLAY_H

namespace TreeDisplay
{
	inline int DepthFinder(TreeBase& tb,int d=0)
		{ if (tb.parent) d=DepthFinder(*tb.parent,d+1); return d; }

	struct XPoints : vector<XPoint>
	{
		XPoints() : xpoints(NULL) {}
		virtual ~XPoints() { if (xpoints) delete[] xpoints; }
		virtual operator XPoint& () const
		{
			if (xpoints) delete[] xpoints;
			xpoints=new XPoint[size()];	
			const vector<XPoint>& me(*this);
			for (int j=0;j<size();j++) { xpoints[j].x=me[j].x; xpoints[j].y=me[j].y; }
			return *xpoints;
		}
		private:
		mutable XPoint* xpoints;
	};

	struct NodeBase : map<string,string>
	{
		NodeBase() : SW(0),SH(0),X(0),Y(0),PX(0),PY(0),parented(false),coverlastlin(false),moved(true),motion(100,100),Remove(false),Removing(100),Removed(false),font_info(NULL) {}
		NodeBase(const int _SW,const int _SH) : SW(_SW),SH(_SH),X(_SW/4),Y(10),PX(0),PY(0),parented(false),coverlastlin(false),moved(true), motion(X,Y), color(0X003333),Remove(false),Removing(100), Removed(false),font_info(NULL) { }
		NodeBase(const NodeBase& a) : text(a.text),SW(a.SW),SH(a.SH),CW(a.CW),CH(a.CH),PX(a.PX),PY(a.PY),X(a.X),Y(a.Y),parented(false),coverlastlin(false),moved(a.moved), motion(a.X,a.Y),color(a.color),Remove(a.Remove),Removing(a.Removing), Removed(a.Removed),font_info(NULL)  {}
		const bool Moved() const {return moved;}
		bool undisplay()
		{
			if (!Remove) Removing=0XFF;
			Remove=true;
			return Removed;
		}
		protected: 
		bool moved;
		void operator()(Invalid& invalid,Window& window,Display* display,GC& gc,Pixmap& bitmap);
		Motion motion;
		string text;
		const int SW,SH;
		double X,Y,PX,PY,LSX,LSY,LPX,LPY;
		bool parented,coverlastlin;
		unsigned long color;
		int CW,CH;
		int DCW,DCH;
		private:
		XPoints lastpoints;
		bool Remove,Removed;
		int Removing;
		XFontStruct* font_info;
		void SetFont(Display* display,GC& gc)
		{
			if (font_info) return;
			char* font_name = "*-courier-*-8-*";
			font_info = XLoadQueryFont(display, font_name);
			if (!font_info) 
			{
					cout<<"Cannot load "<<font_name<<endl;
					return;
			}
			XSetFont(display, gc, font_info->fid);
		}
	};

	template<typename KT>
		struct TreeNode : NodeBase
	{
		TreeNode() {}
		TreeNode(const int _SW,const int _SH) : NodeBase(_SW,_SH) { BoxSize(); }
		TreeNode(const TreeNode& a) : NodeBase(a)  {}
		void operator()(unsigned long long _color) { color=_color; }
		void operator()(KT _k,TreeBase& node,TreeBase* parent,bool erasing=false)
		{
			k=_k;
			if (!parent) 
			{
				double x=(SW/2)-(CW/2); double y=(CH*3);
				motion(x,y);
				Text(k,k);
				parented=false;
			} else {
				Bst<KT,TreeNode<KT> >& parentnode(static_cast<Bst<KT,TreeNode<KT> >&>(*parent));
				TreeNode<KT>& pn(parentnode.Data());
				//if (pn.moved) return;
				KT pk(parentnode);
				double px(pn.X);
				double py(pn.Y);
				parented=true;
				PX=pn.X;
				PY=pn.Y;
				const double sw(SW); double sh(SH);
				double y;
				if (k<pk) y=py+(CH*3);
				else y=py+(CH*3)-CH;
				double x;
				if (parent->parent)
				{
					Bst<KT,TreeNode<KT> >& grandparentnode(static_cast<Bst<KT,TreeNode<KT> >&>(*parent->parent));
					TreeNode<KT>& gpn(grandparentnode.Data());
					if (gpn.moved) return;
					double gpx(gpn.X);
					double dx(gpn.X-pn.X);
					dx/=16; dx*=7; // 7/16s of the difference between parent and grand-parent
					if (dx<0) dx*=-1;
					if (k<pk) x=px-dx; else x=px+dx; 

					int depth(DepthFinder(node)+3);
					y=CH*((depth*depth)/3);
					if (k<pk) y+=CH;

					motion(x,y);
					Text(k,pk);
				} else {
					if (k<pk) x=px/2; else x=(sw+px)/2; // half the difference between root and left or right edge
					motion(x,y);
					Text(k,pk);
				}
			}
		}
		void operator()(Invalid& invalid,Window& window,Display* display,GC& gc,Pixmap& bitmap)
			{ NodeBase::operator()(invalid,window,display,gc,bitmap); }
		private:
		KT k;
		void Text(KT k,KT pk,string txt="") {stringstream ss; ss<<k<<","<<pk<<" "<<txt; text=ss.str().c_str();}
		void BoxSize() {CW=50; CH=14;}
	};

	template <> inline void TreeNode<int>::Text(int k,int pk,string txt)
	{
		stringstream ss;
		ss<<k; if (!txt.empty()) ss<<" "<<txt;
		text=ss.str();
	}

	template <> inline void TreeNode<int>::BoxSize() { CW=20; CH=10; }

	template <> inline void TreeNode<double>::Text(double k,double pk,string txt)
	{
		stringstream ss;
		ss<<k; if (!txt.empty()) ss<<" "<<txt;
		text=ss.str();
	}

	template <> inline void TreeNode<double>::BoxSize() { CW=40; CH=10; }
	template <> inline void TreeNode<string>::Text(string k,string pk,string txt)
	{
		stringstream ss;
		ss<<k; if (!txt.empty()) ss<<" "<<txt;
		text=ss.str();
	}
	template <> inline void TreeNode<string>::BoxSize() { CW=40; CH=10; }
} //namespace TreeDisplay

#endif // NODE_DISPLAY_H

