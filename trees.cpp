
#include "x11grid.h"
using namespace X11Methods;
#include <trees.h>
#include <tree.h>
#include <math.h>

namespace TreeDisplay
{
	using namespace TreeeObjects;
	struct TestRect : X11Methods::Rect
	{
		TestRect() {}
		TestRect(const int ulx,const int uly,const int brx,const int bry) 
			: Rect(ulx,uly,brx,bry) {}
		TestRect(const TestRect& a) : X11Methods::Rect(a) {}
		TestRect& operator=(const TestRect& a) { X11Methods::Rect::operator=(a); }
		virtual operator XPoint& () 
		{
			if (xpoints) delete[] xpoints;
			xpoints=new XPoint[4];	
			xpoints[0].x=first.first;
			xpoints[0].y=first.second;
			xpoints[1].x=second.first;
			xpoints[1].y=first.second;
			xpoints[2].x=second.first;
			xpoints[2].y=second.second;
			xpoints[3].x=first.first;
			xpoints[3].y=second.second;
			return *xpoints;
		}
	};

	struct Bubble : X11Grid::Card
	{
		Bubble(X11Grid::GridBase& _grid,TreeBase& _val,int _x,int _y,int _rx,int _ry) 
			: grid(_grid), Card(_grid), val(_val),X(_x),Y(_y),tx(_rx),ty(_ry),init(true) {}
		Bubble(X11Grid::GridBase& _grid,TreeBase& _val)
			: grid(_grid), Card(0), val(_val) {}
		virtual ~Bubble() {}
		const bool operator<(const Bubble& _a) const 
			{ return const_cast<TreeBase&>(val)<const_cast<TreeBase&>(_a.val); }
		virtual void cover(Display* display,GC& gc,Pixmap& bitmap,unsigned long color,X11Methods::InvalidBase& _invalid,const int x,const int y) 
		{
			TestRect r(x-50,y-10,x+70,y+10);	
			XPoint& points(r);
			XSetForeground(display,gc,color);
			XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			InvalidArea<TestRect>& invalid(static_cast<InvalidArea<TestRect>&>(_invalid));
			invalid.insert(r);
		}

		virtual void operator()(Pixmap& bitmap,const int x,const int y,Display* display,GC& gc,X11Methods::InvalidBase& _invalid)
		{
			TestRect r(x-50,y-10,x+70,y+10);	
			XPoint& points(r);
			XSetForeground(display,gc,0X0080FF);
			XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			XSetForeground(display,gc,0X8800FF);
			//stringstream ss; ss<<id<<") "<<setprecision(3)<<fixed<<const_cast<TreeBase&>(val);
			stringstream ss; ss<<text;
			XDrawString(display,bitmap,gc,X-40,Y,ss.str().c_str(),ss.str().size());
			InvalidArea<TestRect>& invalid(static_cast<InvalidArea<TestRect>&>(_invalid));
			invalid.insert(r);
		}
		void operator()(int x,int y) const { tx=x; ty=y; }
		void operator()(string _text) const { text=_text;}
		operator const pair<int,int> () const {return make_pair<int,int>(tx,ty);}
		const bool update() const
		{
			X11Grid::GridBase& gb(const_cast<X11Grid::GridBase&>(grid));
			if (init) 
			{
				Point p(X,Y);
				gb[p]+=const_cast<Bubble*>(this);
				init=false;
				return true;
			}
			if ( (tx==X) && (ty==Y) ) return true; 
			Point b(X,Y);
			gb[b]-=const_cast<Bubble*>(this);
			const pair<int,int> motion(move());
			X+=motion.first;
			Y+=motion.second;
			Point p(X,Y);
			gb[p]+=const_cast<Bubble*>(this);
			return true;
		}
		private:
		X11Grid::GridBase& grid;
		const TreeBase& val;
		mutable int X,Y,tx,ty;
		mutable bool init;
		mutable string text;

		const pair<int,int> move() const
		{
			const double distx(tx-X);
			const double disty(ty-Y);
			const double direction(atan2(disty, distx));
			const double distance(sqrt( (distx * distx) + (disty * disty) ) );
			const double force(distance/10);
			const double dx(force*cos(direction));
			const double dy(force*sin(direction));
			pair<int,int> m;
			m.first=floor(dx);
			m.second=floor(dy);
			return m;
		}
	};

	struct Bubbles : set<Bubble>
	{
		Bubbles(X11Grid::GridBase& _grid) : grid(_grid) {} //, rx(300), ry(50) {}
		operator const bool ()
		{
			for (iterator it=begin();it!=end();it++)
			{
				const Bubble& b(*it);	
				if (!b.update()) return false;
			}
			return true;
		}
		void operator()(TreeBase& node,int x,int y)
		{
			Bubble b(grid,node,x,y,0,0);
			iterator it(find(b));
			if (it!=end()) return;
			insert(b);
		}
		private: X11Grid::GridBase& grid;
	};

	template <typename KT,typename VT>
		struct Trees : PlotterBase
	{
		Trees(X11Grid::GridBase& _grid,const int _w,const int _h,const int _cw,const int _ch) 
				: grid(_grid), PlotterBase(_w,_h,_cw,_ch), tick(0),node(0),bubbles(_grid),root(NULL),flip(true) 
			{
				//cout<<"T:"<<typeid(*this).name()<<" ";
				ns.push_back(6);
				ns.push_back(4);
				ns.push_back(5);
				ns.push_back(7);
				ns.push_back(1);
				ns.push_back(2);
				ns.push_back(8);
				ns.push_back(8);
			}
		operator const bool ()
		{
			if (!((tick++)%50)) 
			{
				if (!ns.empty())
				{
					double v(ns.front()); ns.pop_front();
					TreeBase* n(new Bst<KT,VT>(v));
					if (!root) 
					{
						root=n;  
						bubbles(*n,100,100);
					} else {
						TreeBase* tb(root->insert(root,n,root));
						if (tb) 
						{
							root=tb;	
							bubbles(*n,100,100);
						}
					}
				}
			}
			if (root) traverse(*root);
			if (!bubbles) throw string("Cannot run bubbles");
			return true;
		} 
		private:
		deque<double> ns;
		void traverse(TreeBase& n)
		{
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT k(nk);
			Bubble bu(grid,n);
			Bubbles::iterator bit(bubbles.find(bu));
			const Bubble& b(*bit);
			stringstream ss;
			if (!n.parent) 
			{
				ss<<"root->"<<setprecision(2)<<fixed<<k;
				b(w/2,30); 
			} else {
				Bst<KT,VT>& pnk(static_cast<Bst<KT,VT>&>(*n.parent));
				const KT pk(pnk);
				Bubble pbu(grid,pnk);
				Bubbles::iterator pbit(bubbles.find(pbu));
				const Bubble& pb(*pbit);
				const pair<int,int> pos(pb);
				ss<<dec<<pnk.depth<<", "<<setprecision(2)<<fixed<<k<<"("<<setprecision(2)<<fixed<<pk<<")";
				int d(nk.depth+1); d*=2;
				int m((w/cw)/d); m*=cw; m/=2;
				int Y(d*ch);
				//cout<<"d:"<<d<<", w/cw:"<<(w/cw)<<", m:"<<m<<" Y:"<<Y<<endl;
				if (k<pk)
				{
					b(pos.first-m,pos.second+Y);
				} else {
					b(pos.first+m,pos.second+Y);
				}
			}
			b(ss.str());
			if (n.left) traverse(*n.left);
			if (n.right) traverse(*n.right);
		}
		int tick;
		bool flip;
		KT node;
		X11Grid::GridBase& grid;
		Bubbles bubbles;
		TreeBase* root;
	};

	int PlotterBase::what(0);
	PlotterBase* PlotterBase::generate(X11Grid::GridBase& _grid,const int w,const int h,const int cw,const int ch)
	{ 
		switch (++what) 
		{ 
			default: what=0; return new Trees<double,string>(_grid,w,h,cw,ch); 
		} 
	}
} // namespace TreeDisplay




int main(int argc,char** argv)
{
	KeyMap keys;
	return X11Grid::x11main<TreeDisplay::TestStructure>(argc,argv,keys,0X333333);
}

