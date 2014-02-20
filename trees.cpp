
#include "x11grid.h"
using namespace X11Methods;
#include <tree.h>
using namespace TreeeObjects;
#include <trees.h>
using namespace TreeDisplay;
#include <math.h>

#define KEYTYPE int

namespace Utilities
{
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
} // Utilities


int main(int argc,char** argv)
{
		//for (double j=1;j<100;j++)
		//	cout<<setprecision(2)<<setw(5)<<"log("<<j<<")="<<setprecision(2)<<setw(5)<<log(j)<<endl;
		KeyMap keys;
		XSizeHints displayarea;
		Display *display;//(XOpenDisplay(""));
		display = XOpenDisplay (getenv ("DISPLAY"));
		int screen(DefaultScreen(display));
		const unsigned long background(0X777777);
		const unsigned long foreground(0X777777);

		displayarea.x = 000;
		displayarea.y = 000;
		Utilities::GetScreenSize(display,displayarea.width,displayarea.height);
		displayarea.flags = PPosition | PSize;

		Utilities::CmdLine cmdline(argc,argv,"trees");

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
				else window=Utilities::ScreenRoot(DefaultScreenOfDisplay(display));
				gc=XCreateGC(display,window,0,0);
				if (wid) XMapRaised(display,window);
		}


		XSetForeground(display,gc,foreground);
		XFillRectangle(display,window,gc, displayarea.x,displayarea.y, displayarea.width,displayarea.height);

		stringstream except;
		try
		{
			TreeCanvas<KEYTYPE> canvas(display,gc,displayarea.width, displayarea.height);
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

