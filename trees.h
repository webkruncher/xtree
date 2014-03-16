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
namespace TreeDisplay
{
	template<typename KT>
		struct TreeCanvas : Canvas, Sentinel
	{
		typedef TreeNode<KT> VT ;
		TreeCanvas(TreeJournal::Journal& _journal,ostream& _out,bool _ignorestop,Display* _display,Window& _window,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: journal(_journal), out(_out), window(_window), 
				Canvas(_display,_gc,_ScreenWidth,_ScreenHeight),
				updateloop(0),root(NULL),movement(false),stop(false),waitfor(0),removing(false),removal(NULL),flipcounter(0) , ignorestop(_ignorestop), reported(false)
					{ if (journal==ios_base::in) journal>>entry; }
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
		ostream& out;
		TreeJournal::Entry<KT> entry;
		bool stop,removing;
		virtual bool CheckIntegrity(Trunk* root) 
			{ if (!TreeIntegrity::BstIntegrity<KT>(out,root,used)) return this->IntegrityReport(root); return true;}
		virtual bool IntegrityReport(Trunk* root) 
		{
			if (reported) return false;
			reported=true;
			out<<"Integrity check failed:"<<endl;
			if (used.size()<20)
			{
				out<<"Used:"; for (typename set<KT>::iterator it=used.begin();it!=used.end();it++) out<<(*it)<<" ";	
				out<<endl<<" Bst:"; 
				TreeIntegrity::PrintInOrder<KT,VT>(out,root);
				out<<endl;
			}
			out.flush();
			return false;
		}
		private:
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

	};


	template<typename KT>
		struct RbMapCanvas : TreeCanvas<KT>, TreeIntegrity::IntegrityAdvisor
	{
		typedef TreeNode<KT> VT ;
		RbMapCanvas(TreeJournal::Journal& _journal,ostream& _out,bool _ignorestop,Display* _display,Window& _window,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: TreeCanvas<KT>(_journal,_out,_ignorestop,_display,_window,_gc,_ScreenWidth,_ScreenHeight),out(_out) {}
		virtual Trunk* generate(KT& key,TreeNode<KT>& treenode) 
		{ 
			return new RbMap<KT,VT>(static_cast<Sentinel&>(*this),key,treenode); 
		}
		virtual bool CheckIntegrity(Trunk* root)
		{
			if (!TreeCanvas<KT>::CheckIntegrity(root)) return this->IntegrityReport(root);;
			if (!TreeIntegrity::RedBlackIntegrity<KT>(out,root,*this)) return this->IntegrityReport(root);
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
		private: ostream& out;
	};
} // TreeDisplay

#endif  //TREE_DISPLAY_H


