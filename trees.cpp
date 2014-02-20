
#include "x11grid.h"
using namespace X11Methods;
#include <tree.h>
using namespace TreeeObjects;
#include <trees.h>
#include <math.h>

#if 0
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
			TestRect r(x-22,y-12,x+28,y+12);	
			XPoint& points(r);
			XSetForeground(display,gc,color);
			XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			InvalidArea<TestRect>& invalid(static_cast<InvalidArea<TestRect>&>(_invalid));
			invalid.insert(r);
		}

		virtual void operator()(Pixmap& bitmap,const int x,const int y,Display* display,GC& gc,X11Methods::InvalidBase& _invalid)
		{
			TestRect r(x-22,y-12,x+28,y+12);	
			XPoint& points(r);
			XSetForeground(display,gc,0XFFFFFF);
			stringstream ss; ss<<text;
			XDrawString(display,bitmap,gc,x-24,y,ss.str().c_str(),ss.str().size());
			InvalidArea<TestRect>& invalid(static_cast<InvalidArea<TestRect>&>(_invalid));
			invalid.insert(r);
		}
		void operator()(int x,int y) const 
		{
			cout<<"push:"<<text<<", "<<x<<","<<y<<endl;
			next.push_front(make_pair<int,int>(x,y)); 
		} //{ tx=x; ty=y; }
		void operator()(string _text) const { text=_text;}
		operator const pair<int,int> () const {return make_pair<int,int>(tx,ty);}
		const bool update() const
		{
			X11Grid::GridBase& gb(const_cast<X11Grid::GridBase&>(grid));
if (next.size()) cout<<text<<" points:"<<next.size()<<", "<<tx<<"?="<<X<<", "<<ty<<"?="<<Y<<endl;
			if ( (tx==X) && (ty==Y) ) 
			{
				if (!next.empty())
				{
cout<<"!"<<text<<" points:"<<next.size()<<", "<<tx<<"?="<<X<<", "<<ty<<"?="<<Y<<endl;
					pair<int,int> p(next.back()); next.pop_back();
					tx=p.first; ty=p.second;
				} else return true; 
			}
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
		mutable deque<pair<int,int> > next;

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
			Bubble b(grid,node,x,y,x,y);
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
				srand(time(0));
				while (ns.size()<6)
				{
					int v((rand()%40)-20);
					if (find(ns.begin(),ns.end(),v)==ns.end()) ns.push_back(v);
				}
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
						bubbles(*n,w/2,30);
					} else {
						TreeBase* tb(root->insert(root,n));
						if (tb) 
						{
							root=tb;	
							bubbles(*n,w/2,30);
							Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*n));
							const KT k(nk);
							Bubble bu(grid,*n);
							Bubbles::iterator bit(bubbles.find(bu));
							const Bubble& b(*bit);
							touchparents(b,n->parent);
						}
					}
				}
			}
			if (root) traverse(*root);
			if (!bubbles) throw string("Cannot run bubbles");
			return true;
		} 
		private:
		void touchparents(const Bubble& b,TreeBase* p)
		{
			if (p)
			{
				Bst<KT,VT>& pnk(static_cast<Bst<KT,VT>&>(*p));
				const KT pk(pnk);
				Bubble pbu(grid,pnk);
				Bubbles::iterator pbit(bubbles.find(pbu));
				const Bubble& pb(*pbit);
				const pair<int,int> pos(pb);
				cout<<" touch "<<pk<<endl;
				if (p->parent) touchparents(b,p->parent);
				b(pos.first,pos.second);
			}
		}
		deque<double> ns;
		void traverse(TreeBase& n)
		{
			Bst<KT,VT>& rk(static_cast<Bst<KT,VT>&>(*root));
			const KT rv(rk);
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT k(nk);
			Bubble bu(grid,n);
			Bubbles::iterator bit(bubbles.find(bu));
			const Bubble& b(*bit);
			stringstream ss;
			if (!n.parent) 
			{
				ss<<setprecision(0)<<fixed<<k;
				if (oldparents.find(k)==oldparents.end())
				{
					oldparents[k]=n.parent;
					b(w/2,30); 
				}
			} else {
				if (oldparents[k]!=n.parent)
				{
					oldparents[k]=n.parent;
					Bst<KT,VT>& pnk(static_cast<Bst<KT,VT>&>(*n.parent));
					const KT pk(pnk);
					Bubble pbu(grid,pnk);
					Bubbles::iterator pbit(bubbles.find(pbu));
					const Bubble& pb(*pbit);
					const pair<int,int> pos(pb);
					ss<<setprecision(0)<<fixed<<k<<","<<setprecision(0)<<fixed<<pk;
					int d(nk.depth+1); d*=10;
					int m((w/cw)/d); m*=(cw); 
					int Y(d*(ch/2));
					//cout<<"d:"<<d<<", w/cw:"<<(w/cw)<<", m:"<<m<<" Y:"<<Y<<endl;
					if (k<pk)
					{
						if (rv<k) {m/=4;Y-=ch;}
						b(pos.first-m,pos.second+Y);
					} else {
						if (k<rv) m/=4;
						b(pos.first+m,pos.second+Y);
					}
				}
			}
			if (!ss.str().empty()) b(ss.str());
			if (n.left) traverse(*n.left);
			if (n.right) traverse(*n.right);
		}
		int tick;
		bool flip;
		KT node;
		X11Grid::GridBase& grid;
		Bubbles bubbles;
		TreeBase* root;
		map<double,TreeBase*> oldparents;
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


#endif

	inline void GetScreenSize(Display* display,int& width, int& height)
	{
		 Screen* pscr(DefaultScreenOfDisplay(display));
		 if (!pscr) return;
		 width=pscr->width;
		 height=pscr->height;
	}

	struct stringmap : public map<string,string> { };
	class CmdLine : public stringmap
	{
	public:
		int Argc()
		{
			if (!argc) Build();
			return argc;
		}
	
		char** Argv()
		{
			if (!argv) Build();
			return argv;
		}

		CmdLine() : programname("utility"),argv(NULL),argc(0),ownerofargs(false) {}
		CmdLine(int _argc,char** _argv,string _progname="utility") 
			: programname(_progname), argv(_argv),argc(_argc),ownerofargs(false)
		{
			for (int i=1;i<argc;i++)
			{
				string arg=argv[i];
				bool trip=false;
				int nexti = i+1;
				if (arg.find("-")==0)
				{
					if (nexti<argc)
					{
						string next = argv[nexti];
						if (next.find("-")!=0)
						{
							(*this)[arg]=next;
							trip=true;
						}
					}
				}
				if (trip) i=nexti;
				else (*this)[arg]="";
			}
		}

		virtual ~CmdLine() { if (ownerofargs) if (argv) free(argv); }
		ostream& operator<<(ostream& o)
		{
			for(iterator it=begin();it!=end();it++)
				o<<it->first<<":"<<it->second<<endl;
			return o;
		}

		bool exists(string name)
		{
			for(iterator it=begin();it!=end();it++) if (it->first==name) return true;
			return false;
		}

		string programname; // user settable
		private: int argc; char **argv; string argbuffer; bool ownerofargs;
		void Build()
		{
			if (argc) return; if (argv) return;
			ownerofargs=true;
			size_t bytes(0);
			int items(0);
			for (iterator it=begin();it!=end();it++) 
			{
				bytes+=it->first.size(); 
				items++; 
				if (it->second.size()) {bytes+=it->second.size()+1; items++;} 
			}
			argbuffer.resize(bytes);
			argv=(char**)malloc(sizeof(char*)*items);
			if (!argv) throw string("Cannot allocate argv double pointer");
			int i(0);
			for (iterator it=begin();it!=end();it++)
			{
				argv[i++]=&argbuffer[argbuffer.size()];
				if (it->first.size()) argbuffer+=it->first;
				if (it->second.size()) 
				{
					argbuffer+="\0"; 
					argv[i++]=&argbuffer[argbuffer.size()];
					argbuffer+=it->second;
				}
			}
			argc=i;
		}
	};

	inline ostream& operator<<(ostream& o,CmdLine& l){return l.operator<<(o);}

	inline Window ScreenRoot(Screen* screen,Screen* tmp=0,Window root=0)
	{
		if (screen!=tmp) 
		{
			Display *display(DisplayOfScreen(screen));
			int i;
			Window ret,parent,*children(NULL);
			unsigned int nchildren;
			root = RootWindowOfScreen(screen);
			Atom swm(XInternAtom(display,"__SWM_VROOT",False));
			if (XQueryTree(display, root, &ret, &parent, &children, &nchildren)) 
			{
				for (i = 0;i<nchildren;i++) 
				{
					Atom type; int format; unsigned long num, after; Window *subroot(NULL);
					if (XGetWindowProperty(display,children[i],swm,0,1,False,XA_WINDOW,&type,&format,&num,&after,(unsigned char **)&subroot)==Success&&subroot) 
						{root=*subroot; break;}
				}
				if (children) XFree((char *)children);
			}
			tmp=screen;
		}
		return root;
	}



int main(int argc,char** argv)
{
		KeyMap keys;
		XSizeHints displayarea;
		Display *display;//(XOpenDisplay(""));
		display = XOpenDisplay (getenv ("DISPLAY"));
		int screen(DefaultScreen(display));
		const unsigned long background(0X0);
		const unsigned long foreground(0X0);

		displayarea.x = 000;
		displayarea.y = 000;
		GetScreenSize(display,displayarea.width,displayarea.height);
		displayarea.flags = PPosition | PSize;

		CmdLine cmdline(argc,argv,"trees");

		XSetWindowAttributes attributes;
		Window window,parent(0);
		GC gc;
		unsigned int Class(CopyFromParent);
		int depth(CopyFromParent);
		unsigned int border(0);
		unsigned long valuemask(CopyFromParent);
		if (!cmdline.exists("-root"))
		{
			window=XCreateSimpleWindow(display,DefaultRootWindow(display), 
				displayarea.x,displayarea.y,displayarea.width,displayarea.height,5,foreground,background);
			gc=(XCreateGC(display,window,0,0));
			XSetBackground(display,gc,background);
			XSetForeground(display,gc,foreground);
			XSelectInput(display,window,ButtonPressMask|KeyPressMask|ExposureMask);
			XMapRaised(display,window);
		} else {

			long long int wid(0);
			if (cmdline.exists("-window-id"))
			{
				string sswid(cmdline["-window-id"]); sswid.erase(0,2);
				char* pEnd; wid = strtoll ((char*)sswid.c_str(), &pEnd, 16);
			}
				parent=wid; 
				if (parent)
					window=XCreateWindow(display, parent,
						displayarea.x,displayarea.y, displayarea.width,displayarea.height, 
						border, depth, Class, CopyFromParent, valuemask, &attributes);
				else window=ScreenRoot(DefaultScreenOfDisplay(display));
				gc=XCreateGC(display,window,0,0);
				if (wid) XMapRaised(display,window);
		}


		XSetForeground(display,gc,foreground);
		XFillRectangle(display,window,gc, displayarea.x,displayarea.y, displayarea.width,displayarea.height);

		stringstream except;
		try
		{
			TreeCanvas<double> canvas(display,gc,displayarea.width, displayarea.height);
			Program program(screen,display,window,gc,NULL,canvas,keys,displayarea.width,displayarea.height);
			program(argc,argv);
		}
		catch(runtime_error& e){except<<"runtime error:"<<e.what();}
		catch(...){except<<"unknown error";}
		if (!except.str().empty()) cout<<except.str()<<endl;

		XFreeGC(display,gc);
		XDestroyWindow(display,window);
		XCloseDisplay(display);
		return 0;
	}

