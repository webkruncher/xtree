
#include "x11grid.h"
using namespace X11Methods;
#include <trees.h>
#include <tree.h>
#include <math.h>

namespace XRbTree
{
	using namespace KrunchRbTree;
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
		Bubble(X11Grid::GridBase& _grid,RbTreeBase& _val,int _x,int _y,int _rx,int _ry) 
			: grid(_grid), Card(_grid), val(_val),X(_x),Y(_y),tx(_rx),ty(_ry),init(true) {}
		Bubble(X11Grid::GridBase& _grid,RbTreeBase& _val)
			: grid(_grid), Card(0), val(_val) {}
		virtual ~Bubble() {}
		const bool operator<(const Bubble& _a) const 
			{ return const_cast<RbTreeBase&>(val)<const_cast<RbTreeBase&>(_a.val); }
		virtual void cover(Display* display,GC& gc,Pixmap& bitmap,unsigned long color,X11Methods::InvalidBase& _invalid,const int X,const int Y) 
		{
			TestRect r(X-50,Y-10,X+70,Y+10);	
			XPoint& points(r);
			XSetForeground(display,gc,color);
			XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			InvalidArea<TestRect>& invalid(static_cast<InvalidArea<TestRect>&>(_invalid));
			invalid.insert(r);
		}

		virtual void operator()(Pixmap& bitmap,const int x,const int y,Display* display,GC& gc,X11Methods::InvalidBase& _invalid)
		{
			TestRect r(X-50,Y-10,X+70,Y+10);	
			XPoint& points(r);
			XSetForeground(display,gc,0X0080FF);
			XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			XSetForeground(display,gc,0X8800FF);
			//stringstream ss; ss<<id<<") "<<setprecision(3)<<fixed<<const_cast<RbTreeBase&>(val);
			stringstream ss; ss<<id<<") "<<text;
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
		const RbTreeBase& val;
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
		void operator()(RbTreeBase& node,int x,int y)
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
		Trees(X11Grid::GridBase& _grid,const int _w,const int _h) 
				: grid(_grid), PlotterBase(_w,_h), tick(0),node(0),bubbles(_grid),root(NULL),flip(true) {cout<<"T:"<<typeid(*this).name()<<" ";}
		operator const bool ()
		{
			if (!((tick++)%50)) 
			{
				srand(time(0));
				KT v(rand()%100);
				flip=!flip;
				if (flip) v*=-1;
				RbTreeBase* n(new RbTree<KT,VT>(v));
				if (!root) root=n; else root=root->insert(n);
				bubbles(*n,100,100);
			}
			if (root) traverse(*root);
			if (!bubbles) throw string("Cannot run bubbles");
			return true;
		} 
		private:
		void traverse(RbTreeBase& n)
		{
			RbTree<KT,VT>& nk(static_cast<RbTree<KT,VT>&>(n));
			const KT k(nk);
			Bubble bu(grid,n);
			Bubbles::iterator bit(bubbles.find(bu));
			const Bubble& b(*bit);
			stringstream ss;
			if (!n.parent) 
			{
				ss<<"root->"<<setprecision(2)<<fixed<<k;
				b((w*8)/2,30); 
			} else {
				RbTree<KT,VT>& pnk(static_cast<RbTree<KT,VT>&>(*n.parent));
				const KT pk(pnk);
				Bubble pbu(grid,pnk);
				Bubbles::iterator pbit(bubbles.find(pbu));
				const Bubble& pb(*pbit);
				const pair<int,int> pos(pb);
				ss<<setprecision(2)<<fixed<<k<<"("<<setprecision(2)<<fixed<<pk<<")";
				if (k<pk)
				{
					b(pos.first-60,pos.second+50);
				} else {
					b(pos.first+60,pos.second+50);
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
		RbTreeBase* root;
	};

	int PlotterBase::what(0);
	PlotterBase* PlotterBase::generate(X11Grid::GridBase& _grid,const int w,const int h)
	{ 
		switch (++what) 
		{ 
			default: what=0; return new Trees<double,string>(_grid,w,h); 
		} 
	}
} // namespace XRbTree




int main(int argc,char** argv)
{
	KeyMap keys;
	return X11Grid::x11main<XRbTree::TestStructure>(argc,argv,keys,0X333333);
}

