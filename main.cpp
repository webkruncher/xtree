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
#include <treemap.h>
using namespace TreeObjects;
#include <trees.h>
using namespace TreeDisplay;
#include <math.h>
#include <journal.h>

namespace Utilities
{
	inline string runpipe(string what)
	{
		stringstream ssret;
		string ret; 
		char * line = NULL;	
		size_t len = 0;	ssize_t bread;
		FILE* fp = popen((char*)what.c_str(),"r");
		if (fp == NULL) {return "cannot open pipe";}
		while (1)
		{
			char buf[514];
			memset(buf,0,513);
			int l = fread(buf,1,512,fp);
			if (l > 0) ret+=buf;
			else break;
		}
		pclose(fp);
		return ret;
	}

	void* ssup(void*) 
	{
		while (true)
		{
			runpipe("xset dpms force on");
			runpipe("xset s off");
			runpipe("xset s noblank");
			sleep(60);
		}
	}
	inline void NoDpms() { pthread_t t; pthread_create(&t,0,ssup,NULL); }

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


TreeJournal::Journal* OpenJournal(Utilities::CmdLine& cmdline)
{
	TreeJournal::Journal* J(NULL);
	if (cmdline.exists("-i")) J=new TreeJournal::Journal(cmdline["-i"],ios_base::in);
	if (cmdline.exists("-o")) J=new TreeJournal::Journal(cmdline["-o"],ios_base::out);
	if (!J) J=new TreeJournal::Journal;
	TreeJournal::Journal& journal(*J);
	if (journal==ios_base::out)
	{
		if (cmdline.find("-bst")!=cmdline.end()) {journal<<"bst"<<endl;}
		if (cmdline.find("-redblack")!=cmdline.end()) {journal<<"redblack"<<endl;}
	}
	if (journal==ios_base::in)
	{
		cmdline.clear();
		string treetype; getline(journal,treetype);
		char keytype; journal.read(&keytype,1); journal.putback(keytype);
		cout<<"TreeType:"<<treetype<<endl;
		cout<<"KeyType:"<<keytype<<endl;
		treetype.insert(0,"-");
		cmdline[treetype]="";
		if (keytype=='I') cmdline["-int"]="";
		if (keytype=='D') cmdline["-double"]="";
		if (keytype=='S') cmdline["-string"]="";
		cout<<"Reading journal:"<<endl<<cmdline<<endl;
	}
	return J;
}

Canvas* CreateCanvas(Utilities::CmdLine& cmdline,const string trapname,const string xmloutname,TreeJournal::Journal& journal,ostream& out,const bool IgnoreStop,Display* display,Window& window,GC& gc,const int width, const int height)
{
	if (cmdline.find("-int")!=cmdline.end())
	{
		if (cmdline.find("-redblack")!=cmdline.end()) return new RbMapCanvas<int>(journal,trapname,xmloutname,out,IgnoreStop,display,window,gc,width, height);
		if (cmdline.find("-bst")!=cmdline.end()) return new TreeCanvas<int>(journal,trapname,xmloutname,out,IgnoreStop,display,window,gc,width, height);
	}
	if (cmdline.find("-double")!=cmdline.end())
	{
		if (cmdline.find("-redblack")!=cmdline.end()) return new RbMapCanvas<double>(journal,trapname,xmloutname,out,IgnoreStop,display,window,gc,width, height);
		if (cmdline.find("-bst")!=cmdline.end()) return new TreeCanvas<double>(journal,trapname,xmloutname,out,IgnoreStop,display,window,gc,width, height);
	}
	if (cmdline.find("-string")!=cmdline.end())
	{
		if (cmdline.find("-redblack")!=cmdline.end()) return new RbMapCanvas<string>(journal,trapname,xmloutname,out,IgnoreStop,display,window,gc,width, height);
		if (cmdline.find("-bst")!=cmdline.end()) return new TreeCanvas<string>(journal,trapname,xmloutname,out,IgnoreStop,display,window,gc,width, height);
	}
	throw string("What type of tree do you want to run?  Options are currently -bst and -redblack, and specify a key type as -int, -double or -string");
}

int main(int argc,char** argv)
{
	Display *display(XOpenDisplay(getenv("DISPLAY")));
	Window window,parent(0);
	GC gc;
	stringstream except;
	try
	{
		Utilities::CmdLine cmdline(argc,argv,"trees");

		string trapname;
		if (cmdline.exists("-trap")) trapname=cmdline["-trap"];
		string xmloutname;
		if (cmdline.exists("-xml")) xmloutname=cmdline["-xml"];
		
		KeyMap keys;
		XSizeHints displayarea;
		int screen(DefaultScreen(display));
		const unsigned long background(0X777777);
		const unsigned long foreground(0X777777);

		displayarea.x = 000;
		displayarea.y = 000;
		Utilities::GetScreenSize(display,displayarea.width,displayarea.height);
		displayarea.flags = PPosition | PSize;

		XSetWindowAttributes attributes;
		unsigned int Class(CopyFromParent);
		int depth(CopyFromParent);
		unsigned int border(0);
		unsigned long valuemask(CopyFromParent);

		ofstream dump;
		bool IgnoreStop=false;
		ostream* pout(NULL);
		if (cmdline.exists("-root"))
		{
			cout.rdbuf(0);
			dump.open("/dev/null");
			pout=&dump;
			IgnoreStop=true;
		} else pout=&cout;


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

		if (cmdline.exists("-dont-stop")) IgnoreStop=true;
		if (cmdline.exists("-no-dpms"))		Utilities::NoDpms();
		ostream& out(*pout);

		if (cmdline.exists("-root")) cmdline.erase("-root");
		if (cmdline.exists("-window-id")) cmdline.erase("-window-id");

		auto_ptr<TreeJournal::Journal> journal;
		if (cmdline.empty())
		{
			cmdline["-int"]="";
			cmdline["-redblack"]="";
		}
		journal.reset(OpenJournal(cmdline));
		auto_ptr<Canvas> pcanvas;
		pcanvas.reset(CreateCanvas(cmdline,trapname,xmloutname,*journal.get(),out,IgnoreStop,display,window,gc,displayarea.width, displayarea.height));
		Canvas& canvas(*pcanvas.get());
		Program program(screen,display,window,gc,NULL,canvas,keys,displayarea.width,displayarea.height);
		program(argc,argv);
	}
	catch(runtime_error& e){except<<"runtime error:"<<e.what();}
	catch(exception& e){except<<"runtime error:"<<e.what();}
	catch(string& e){except<<"exception: "<<e;}
	catch(char* e){except<<"exception: "<<e;}
	catch(...){except<<"unknown error";}
	if (!except.str().empty()) cout<<except.str()<<endl;

	XFreeGC(display,gc);
	XDestroyWindow(display,window);
	XCloseDisplay(display);
	return 0;
}

