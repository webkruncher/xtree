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
namespace TreeDisplay
{
	template<typename KT>
		struct TreeCanvas : Canvas
	{
		typedef TreeNode<KT> VT ;
		TreeCanvas(Display* _display,Window& _window,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: window(_window), 
				Canvas(_display,_gc,_ScreenWidth,_ScreenHeight),
				updateloop(0),root(NULL),movement(false),stop(false),waitfor(0),removing(false),removal(NULL),flipcounter(0) { }
		virtual ~TreeCanvas() {if (root) delete root;}
		virtual void operator()(Pixmap& bitmap) 
		{   
			XSetForeground(display,gc,0X777777);
			XFillRectangle(display,bitmap,gc,0,0,ScreenWidth,ScreenHeight);
			if (root) draw(invalid,*root,bitmap); 
		}
		virtual TreeBase* generate(KT& key,TreeNode<KT>& treenode) { return new Bst<KT,VT>(key,treenode); }
		virtual void update() { UpdateTree(); }
		virtual operator InvalidBase& () {return invalid;}
		protected:
		bool stop,removing;
		virtual bool CheckIntegrity(TreeBase* root) { return TreeIntegrity::BstIntegrity<KT>(root,used); }
		private:
		void UpdateTree();
		void Deletions();
		void Additions();
		Window& window;
		bool movement;
		int flipcounter;
		set<KT> used;
		unsigned long updateloop,waitfor;
		TreeBase* root,*removal;
		Invalid invalid;
		pair<bool,KT> Next(int Max) { return make_pair<bool,KT>(true,rand()%Max); }
		void traverse(TreeBase& n)
		{
			movement=false;
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT& key(nk);
			VT& data(nk.Data());
			data(key,nk,nk.parent);
			if (n.left) traverse(*n.left);
			if (n.right) traverse(*n.right);
		}
		void draw(Invalid& invalid,TreeBase& n,Pixmap& bitmap)
		{
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT& key(nk);
			VT& data(nk.Data());
			data(invalid,window,display,gc,bitmap);
			if (data.Moved()) movement=true;
			if (n.left) draw(invalid,*n.left,bitmap);
			if (n.right) draw(invalid,*n.right,bitmap);
		}

	};


	template<typename KT>
		struct RbMapCanvas : TreeCanvas<KT>, TreeIntegrity::IntegrityAdvisor
	{
		typedef TreeNode<KT> VT ;
		RbMapCanvas(Display* _display,Window& _window,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: TreeCanvas<KT>(_display,_window,_gc,_ScreenWidth,_ScreenHeight) {}
		virtual TreeBase* generate(KT& key,TreeNode<KT>& treenode) { return new RbMap<KT,VT>(key,treenode); }
		virtual bool CheckIntegrity(TreeBase* root)
		{
			if (!TreeCanvas<KT>::CheckIntegrity(root)) return false;
			return TreeIntegrity::RedBlackIntegrity<KT>(root,*this);
		}
		void clear(TreeBase& node,string name)
		{
			RbMapBase<KT,VT>& rb(static_cast<RbMapBase<KT,VT>&>(node));
			VT& data(rb.Data());
			map<string,string>::iterator it;
			it=data.find(name); if (it!=data.end()) data.erase(it);
		}
		void message(TreeBase& node,string name,string value) 
		{
			RbMapBase<KT,VT>& rb(static_cast<RbMapBase<KT,VT>&>(node));
			VT& data(rb.Data());
			data[name]=value;
		}
	};
} // TreeDisplay

#endif  //TREE_DISPLAY_H


