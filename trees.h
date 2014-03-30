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
#include "treexml.h"
namespace TreeDisplay
{
	string deblank(string);
	enum CurrentActions {NoAction=100,Inserting,Erasing} ;

	struct Trap
	{
		Trap(const string _fname,ostream& _tout,bool& _stop,bool& _movement) : fname(_fname),tout(_tout), state(Running),loaded(false),stop(_stop),movement(_movement) {}
		void stepper(string msg);
		void Load();
		private: 
		virtual void Clear() = 0;
		const string fname; bool loaded;
		bool& stop;
		bool& movement;
		ostream& tout;
		string cmd;
		virtual void Read(string) =0;
		protected:
		enum States {Running,Stepping,Next,Continuing} state;
		virtual void Describe() = 0;
	};

	template<typename KT>
		struct Trapper : Trap
	{
		Trapper(const string _trapname,ostream& _tout,bool& _stop,bool& _movement) : Trap(_trapname,_tout,_stop,_movement) {}
		void trap(const CurrentActions,const KT&){}
		private:
		virtual void Clear()
		{
			InsertBreakPoints.clear();EraseBreakPoints.clear();
		}
		set<KT> InsertBreakPoints,EraseBreakPoints;
		virtual void Read(string){} 
	};

	struct TreeSource : private vector<string>
	{
		TreeSource() : blank("----"),ndx(-1) {}	
		const string& operator()(int line);
		const int Ndx(){return ndx;}
		const string SourceCode(){return sourcecode;}
		private:
		void Load()
		{
			ifstream in("tree.h");
			if (in.fail()) return;
			while (!in.eof()) {string line; getline(in,line); push_back(line);}
		}
		const string blank;
		string txt,sourcecode;
		int ndx;
	};

	template <typename KT>
		struct ajaxface : XmlTree::TreeXml 
	{
		private:
		const KT tokt(const string) const;
		const string tostr(const KT) const;
		const bool SIsRoot(const string what) const
		{
			const KT kwhat(this->tokt(what));
			return this->IsRoot(kwhat);
		}
		const bool SHasLeft(const string what) const
		{
			const KT kwhat(this->tokt(what));
			return this->HasLeft(kwhat);
		}
		const bool SHasRight(const string what) const
		{
			const KT kwhat(this->tokt(what));
			return this->HasRight(kwhat);
		}
		const string SParentOf(const string what) const
		{
			const KT kwhat(this->tokt(what));
			return this->tostr(this->ParentOf(kwhat));
		}
		const int SDepthOf(const string what) const
		{
			const KT kwhat(this->tokt(what));
			return this->DepthOf(kwhat);
		}
		const string SColorOf(const string what) const
		{
			const KT kwhat(this->tokt(what));
			return this->ColorOf(kwhat);
		}
		const string SLeftOf(const string what) const
		{
			const KT kwhat(this->tokt(what));
			return this->tostr(this->LeftOf(kwhat));
		}
		const string SRightOf(const string what) const
		{
			const KT kwhat(this->tokt(what));
			return this->tostr(this->RightOf(kwhat));
		}

		virtual const bool IsRoot(const KT what) const = 0;
		virtual const bool HasLeft(const KT what) const = 0;
		virtual const bool HasRight(const KT what) const = 0;
		virtual const KT ParentOf(const KT what) const = 0;
		virtual const int DepthOf(const KT what) const = 0;
		virtual const string ColorOf(const KT what) const = 0;
		virtual const KT LeftOf(const KT what) const = 0;
		virtual const KT RightOf(const KT what) const = 0;
	};

	template<typename KT>
		struct TreeCanvas : Canvas, Sentinel, Trapper<KT>, ajaxface<KT>
	{
		stringstream& messg()
		{ 
			if (!msgbuffer.get()) msgbuffer.reset(new stringstream); 
			return (*msgbuffer.get());
		}
		#define Msg messg()
		typedef TreeNode<KT> VT ;
		TreeCanvas(TreeJournal::Journal& _journal,const string _trapname,const string _xmloutname,const int _entrylimit,ostream& _tout,bool _ignorestop,Display* _display,Window& _window,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: journal(_journal), tout(_tout), window(_window), 
				Canvas(_display,_gc,_ScreenWidth,_ScreenHeight),
				Trapper<KT>(_trapname,_tout,stop,movement),
				xmloutname(_xmloutname),
				entrylimit(_entrylimit),
				updateloop(0),root(NULL),movement(false),stop(false),waitfor(0),removing(false),removal(NULL),flipcounter(0) , ignorestop(_ignorestop), reported(false), clearing(false)
		{ 
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
		bool clearing;
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
			tout<<Msg.str()<<endl; tout.flush(); msgbuffer.reset(NULL);
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
		int flipcounter,entrylimit;
		set<KT> used;
		unsigned long updateloop,waitfor;
		Trunk* root,*removal;
		Invalid invalid;
		pair<bool,KT> Next(int Max) { return make_pair<bool,KT>(true,rand()%Max); }
		CurrentActions CurrentAction;
		TreeSource treesource;
		//XmlTree::TreeXml treexml;
		const string xmloutname;
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
			ajaxface<KT>& treexml(*this);;
			stringstream ssmsg; ssmsg;
			Trunk& n(const_cast<Trunk&>(a));
			if (n.isnil()) 
			{
				Msg<<"NIL ";
				ssmsg<<"NIL ";
				treexml.Key("NIL");
			} else {
				Bst<KT,VT>& bst(static_cast<Bst<KT,VT>&>(n));
				const KT& key(bst);
				Msg<<key<<" ";
				ssmsg<<key<<" ";
				trap(CurrentAction,key);
				stringstream keyname; keyname<<key;
				treexml.Key(keyname.str());
			}
			this->stepper(ssmsg.str().c_str());
			return *this;
		}

		virtual Trunk& operator<<(const string& a)
		{
			Msg<<a<<" ";
			this->stepper("Message");
			return *this;
		}

		virtual Trunk& operator<<(const int a)
		{
			ajaxface<KT>& treexml(*this);;
			Msg<<"["<<a<<"]";
			this->stepper("Number");
			treexml.Number(a);
			return *this;
		}

		virtual Trunk& Insrt()
		{
			ajaxface<KT>& treexml(*this);;
			Msg<<"Insert:";
			CurrentAction=Inserting;
			this->stepper("Inserting");
			treexml.Insrt();
			return *this;
		}

		virtual Trunk& Erse()
		{
			ajaxface<KT>& treexml(*this);;
			Msg<<"Erase:";
			CurrentAction=Erasing;
			this->stepper("Erasing");
			treexml.Erse();
			return *this;
		}

		virtual Trunk& Rotlft()
		{
			ajaxface<KT>& treexml(*this);;
			Msg<<"<< ";
			this->stepper("RotateLeft");
			treexml.Rotlft();
			return *this;
		}

		virtual Trunk& Rotrgt()
		{
			ajaxface<KT>& treexml(*this);;
			Msg<<">> ";
			this->stepper("RotateRight");
			treexml.Rotrgt();
			return *this;
		}

		virtual Trunk& Trnsp()
		{
			ajaxface<KT>& treexml(*this);;
			Msg<<"<> ";
			this->stepper("Transplant");
			treexml.Trnsp();
			return *this;
		}

		virtual Trunk& operator()(char* _file,int line)
		{
			ajaxface<KT>& treexml(*this);;
			if (!_file) return *this;
			struct stat sb;
			if (!stat("tree.h",&sb)==0) return *this;
			string file(_file);
			string treetxt;
			if (file=="./tree.h") treetxt=treesource(line);
			if (!treetxt.empty()) Msg<<endl<<line<<treetxt<<endl;
			else Msg<<file<<line<<"; ";
			treexml.SourceTrace(file,line,treesource.Ndx(),treesource.SourceCode());
			return *this;
		}

		virtual Trunk& Begin()
		{
			ajaxface<KT>& treexml(*this);;
			this->stepper("Begin");
			msgbuffer.reset(NULL);
			treexml.Begin();
			return *this;
		}

		virtual Trunk& Done()
		{
			ajaxface<KT>& treexml(*this);;
			Msg<<". ";
			this->stepper("Done");
			treexml.Done();
			return *this;
		}

		virtual Trunk& Finish()
		{
			ajaxface<KT>& treexml(*this);;
			if (!Msg.str().empty()) 
			{
				stringstream sso; sso<<endl<<Msg.str()<<"done."<<endl;
				tout<<deblank(sso.str());
				tout.flush();
			}
			this->stepper("Finish");
			msgbuffer.reset(NULL);
			treexml.Finish();
			return *this;
		}
		const bool IsRoot(const KT what) const;
		const bool HasLeft(const KT what) const;
		const bool HasRight(const KT what) const;
		const KT ParentOf(const KT what) const;
		const int DepthOf(const KT what) const;
		const string ColorOf(const KT what) const;
		const KT LeftOf(const KT what)  const;
		const KT RightOf(const KT what)  const;
	};


	template<typename KT>
		struct RbMapCanvas : TreeCanvas<KT>, TreeIntegrity::IntegrityAdvisor
	{
		typedef TreeNode<KT> VT ;
		RbMapCanvas(TreeJournal::Journal& _journal,const string _trapname,const string _xmloutname,const int _entrylimit,ostream& _tout,bool _ignorestop,Display* _display,Window& _window,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: TreeCanvas<KT>(_journal,_trapname,_xmloutname,_entrylimit,_tout,_ignorestop,_display,_window,_gc,_ScreenWidth,_ScreenHeight),tout(_tout) {}
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


