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

#ifndef TREE_DISPLAY_H
#define TREE_DISPLAY_H
#include <math.h>
#include <fstream>
struct Invalid : X11Methods::InvalidArea<Rect> { void insert(Rect r) {set<Rect>::insert(r); } };
#include "motivation.h"
#include "nodedisplay.h"
#include "treeintegrity.h"
#include "journal.h"
#include <sys/stat.h>
namespace TreeDisplay
{

	enum CurrentActions {NoAction=100,Inserting,Erasing} ;

	struct Trap
	{
		Trap(const string _fname,ostream& _tout,bool& _stop,bool& _movement) : fname(_fname),tout(_tout), state(Running),loaded(false),stop(_stop),movement(_movement) {}
		void stepper(string msg);
		void Load();
		private: 
		const string fname; bool loaded;
		bool& stop;
		bool& movement;
		ostream& tout;
		string cmd;
		virtual void Read(string) =0;
		protected:
		enum States {Running,Stepping,Next} state;
		virtual void Describe() = 0;
	};

	struct TreeSource : private vector<string>
	{
		TreeSource() : blank("----") {}	
		const string& operator()(int line)
		{
			if (empty()) Load();
			if (line>size()) return blank;
			return (*this)[line];
		}
		private:
		void Load()
		{
			ifstream in("tree.h");
			if (in.fail()) return;
			while (!in.eof()) {string line; getline(in,line); push_back(line);}
		}
		const string blank;
	};

	template<typename KT>
		struct Trapper : Trap
	{
		Trapper(const string _trapname,ostream& _tout,bool& _stop,bool& _movement) : Trap(_trapname,_tout,_stop,_movement) {}
		void trap(const CurrentActions,const KT&){}
		private:
		set<KT> InsertBreakPoints,EraseBreakPoints;
		virtual void Read(string){} 
	};

	template<typename KT>
		struct TreeCanvas : Canvas, Sentinel, Trapper<KT>
	{
		typedef TreeNode<KT> VT ;
		TreeCanvas(TreeJournal::Journal& _journal,const string _trapname,ostream& _tout,bool _ignorestop,Display* _display,Window& _window,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: journal(_journal), tout(_tout), window(_window), 
				Canvas(_display,_gc,_ScreenWidth,_ScreenHeight),
				Trapper<KT>(_trapname,_tout,stop,movement),
				updateloop(0),root(NULL),movement(false),stop(false),waitfor(0),removing(false),removal(NULL),flipcounter(0) , ignorestop(_ignorestop), reported(false)
		{ 
			msgbuffer.reset(new stringstream);
			if (journal==ios_base::in) journal>>entry; 
		}
		virtual ~TreeCanvas() 
		{
			if ((root) and (root->isnil())) throw string("Root is NIL");
			if ((root) and (!root->isnil()) ) delete root;
		}
		virtual void operator()(Pixmap& bitmap) 
		{   
			XSetForeground(display,gc,0X777777);
			XFillRectangle(display,bitmap,gc,0,0,ScreenWidth,ScreenHeight);
			movement=false;
			if ((root) and (!root->isnil())) draw(invalid,*root,bitmap); 
		}
		virtual Trunk* generate(KT& key,TreeNode<KT>& treenode) 
		{ 
			return new Bst<KT,VT>(static_cast<Sentinel&>(*this),key,treenode); 
		}
		virtual void update() { UpdateTree(); }
		virtual operator InvalidBase& () {return invalid;}
		protected:
		bool reported;
		bool ignorestop;
		TreeJournal::Journal& journal;
		ostream& tout;
		TreeJournal::Entry<KT> entry;
		bool stop,removing;
		virtual bool CheckIntegrity(Trunk* root) 
			{ if (!TreeIntegrity::BstIntegrity<KT>(tout,root,used)) return this->IntegrityReport(root); return true;}
		virtual bool IntegrityReport(Trunk* root) 
		{
			if (reported) return false;
			reported=true;
			tout<<"Integrity check failed:"<<endl;
			if (used.size()<20)
			{
				tout<<"Used:"; for (typename set<KT>::iterator it=used.begin();it!=used.end();it++) tout<<(*it)<<" ";	
				tout<<endl<<" Bst:"; 
				TreeIntegrity::PrintInOrder<KT,VT>(tout,root);
				tout<<endl;
			}
			if (!(*msgbuffer.get()).str().empty()) tout<<(*msgbuffer.get()).str()<<endl;tout.flush();
			msgbuffer.reset(new stringstream);
			tout.flush();
			return false;
		}
		private:
		virtual void Describe() { TreeIntegrity::Describe<KT>(root,tout); }
		auto_ptr<stringstream> msgbuffer;
		void UpdateTree();
		void Deletions();
		void Additions();
		Window& window;
		bool movement;
		int flipcounter;
		set<KT> used;
		unsigned long updateloop,waitfor;
		Trunk* root,*removal;
		Invalid invalid;
		pair<bool,KT> Next(int Max) { return make_pair<bool,KT>(true,rand()%Max); }
		CurrentActions CurrentAction;
		TreeSource treesource;
		void traverse(Trunk& n)
		{
			if (n.isnil()) return;
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT& key(nk);
			VT& data(nk.Data());
			data(key,nk,nk.Parent());
			if (!isnul(n.Left())) traverse(*n.Left());
			if (!isnul(n.Right())) traverse(*n.Right());
		}
		void draw(Invalid& invalid,Trunk& n,Pixmap& bitmap)
		{
			if (n.isnil()) return;
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT& key(nk);
			VT& data(nk.Data());
			data(invalid,window,display,gc,bitmap);
			if (data.Moved()) movement=true;
			if (!n.isnul(n.Left())) draw(invalid,*n.Left(),bitmap);
			if (!n.isnul(n.Right())) draw(invalid,*n.Right(),bitmap);
		}

		virtual Trunk& operator<<(const Trunk& a) 
		{
			stringstream ssmsg; ssmsg;
			Trunk& n(const_cast<Trunk&>(a));
			if (n.isnil()) 
			{
				(*msgbuffer.get())<<"NIL "; 
				ssmsg<<"NIL ";
			} else {
				Bst<KT,VT>& bst(static_cast<Bst<KT,VT>&>(n));
				const KT& key(bst);
				(*msgbuffer.get())<<key<<" ";
				ssmsg<<key<<" ";
				trap(CurrentAction,key);
			}
			this->stepper(ssmsg.str().c_str());
			return *this;
		}

		virtual Trunk& operator<<(const string& a)
		{
			(*msgbuffer.get())<<a<<" ";
			this->stepper("Message");
			return *this;
		}

		virtual Trunk& operator<<(const int a)
		{
			(*msgbuffer.get())<<"["<<a<<"] ";
			this->stepper("Number");
			return *this;
		}

		virtual Trunk& Insrt()
		{
			(*msgbuffer.get())<<"Insert:";
			CurrentAction=Inserting;
			this->stepper("Inserting");
			return *this;
		}

		virtual Trunk& Erse()
		{
			(*msgbuffer.get())<<"Erase:";
			CurrentAction=Erasing;
			this->stepper("Erasing");
			return *this;
		}

		virtual Trunk& Rotlft()
		{
			(*msgbuffer.get())<<"<< ";
			this->stepper("RotateLeft");
			return *this;
		}

		virtual Trunk& Rotrgt()
		{
			(*msgbuffer.get())<<">> ";
			this->stepper("RotateRight");
			return *this;
		}

		virtual Trunk& DblRedFix()
		{
			(*msgbuffer.get())<<"* ";
			this->stepper("Kludge");
			return *this;
		}

		virtual Trunk& Trnsp()
		{
			(*msgbuffer.get())<<"<> ";
			this->stepper("Transplant");
			return *this;
		}

		virtual Trunk& operator()(char* _file,int line)
		{
			if (!_file) return *this;
			struct stat sb;
			if (!stat("tree.h",&sb)==0) return *this;
			string file(_file);
			if (file=="./tree.h") (*msgbuffer.get())<<endl<<line<<treesource(line)<<endl;
			else (*msgbuffer.get())<<file<<line<<"; ";
			return *this;
		}

		virtual Trunk& Begin()
		{
			if (!(*msgbuffer.get()).str().empty()) tout<<"Last actions:"<<endl<<(*msgbuffer.get()).str()<<endl;tout.flush();
			this->stepper("Begin");
			msgbuffer.reset(new stringstream);
			return *this;
		}
	};


	template<typename KT>
		struct RbMapCanvas : TreeCanvas<KT>, TreeIntegrity::IntegrityAdvisor
	{
		typedef TreeNode<KT> VT ;
		RbMapCanvas(TreeJournal::Journal& _journal,const string _trapname,ostream& _tout,bool _ignorestop,Display* _display,Window& _window,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: TreeCanvas<KT>(_journal,_trapname,_tout,_ignorestop,_display,_window,_gc,_ScreenWidth,_ScreenHeight),tout(_tout) {}
		virtual Trunk* generate(KT& key,TreeNode<KT>& treenode) 
		{ 
			return new RbMap<KT,VT>(static_cast<Sentinel&>(*this),key,treenode); 
		}
		virtual bool CheckIntegrity(Trunk* root)
		{
			if (!TreeCanvas<KT>::CheckIntegrity(root)) return this->IntegrityReport(root);;
			if (!TreeIntegrity::RedBlackIntegrity<KT>(tout,root,*this)) return this->IntegrityReport(root);
			return true;
		}
		void clear(Trunk& node,string name)
		{
			RbMapBase<KT,VT>& rb(static_cast<RbMapBase<KT,VT>&>(node));
			VT& data(rb.Data());
			map<string,string>::iterator it(data.find(name));
			if (it!=data.end()) data.erase(it);
		}
		void message(Trunk& node,string name,string value) 
		{
			RbMapBase<KT,VT>& rb(static_cast<RbMapBase<KT,VT>&>(node));
			VT& data(rb.Data());
			data[name]=value;
		}
		private: ostream& tout;
	};
} // TreeDisplay

#endif  //TREE_DISPLAY_H


