
#ifndef TREE_DISPLAY_H
#define TREE_DISPLAY_H
#include <math.h>
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
			: window(_window), Canvas(_display,_gc,_ScreenWidth,_ScreenHeight),updateloop(0),root(NULL),movement(false),stop(false),waitfor(0),removing(false) {}
		virtual ~TreeCanvas() {if (root) delete root;}
		virtual void operator()(Pixmap& bitmap) 
		{   
			XSetForeground(display,gc,0X777777);
			//invalid.Fill(display,bitmap,gc);
			XFillRectangle(display,bitmap,gc,0,0,ScreenWidth,ScreenHeight);
			if (root) draw(invalid,*root,bitmap); 
		}
		virtual TreeBase* generate(KT& key,TreeNode<KT>& treenode) { return new Bst<KT,VT>(key,treenode); }
		virtual void update() 
		{
			//if (updateloop>300) { if (root) delete root; root=NULL; return; }
			string tyid(typeid(KT).name());
			if ((!waitfor) or (updateloop>waitfor) )
				if ((!movement) and (!stop))
			{
				movement=true;
				pair<bool,KT> next(Next());
				if (next.first)
				{
					if (!removing)
					{
						TreeNode<KT> tn(ScreenWidth,ScreenHeight);
						TreeBase* n(generate(next.second,tn));
						if (!root) waitfor=updateloop+10;
						if (!root) root=n;  
							else 
						{
							TreeBase* nr(root->insert(root,n));
							if (nr) 
							{
								if (root!=nr) cout<<"The root node rotated"<<endl;
								root=nr;
							} else cout<<next.second<<" is a duplicate and was deleted"<<endl;
						}
					} else {
						if (root) 
						{
							Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*root));
							cout<<"Erase:"<<next.second<<endl;
							root=nk.erase(root,next.second);
						}
					}

					if (!CheckIntegrity(root)) stop=true;
				} else {
					removing=!removing;
					cout<<"Removing:"<<boolalpha<<removing<<endl;
				} 
			}
		
			if (!((updateloop)%30)) if (root) traverse(*root);
			updateloop++;
		}
		virtual operator InvalidBase& () {return invalid;}
		protected:
		bool stop,removing;
		virtual bool CheckIntegrity(TreeBase* root) { return TreeIntegrity::BstIntegrity<KT,VT>(root,used); }
		private:
		Window& window;
		bool movement;
		set<KT> used;
		unsigned long updateloop,waitfor;
		TreeBase* root;
		Invalid invalid;
		pair<bool,KT> Next() { return make_pair<bool,KT>(true,rand()%10); }
		void traverse(TreeBase& n)
		{
			movement=false;
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT& key(nk);
			VT& data(nk);
			data(key,nk,nk.parent);
			if (n.left) traverse(*n.left);
			if (n.right) traverse(*n.right);
		}
		void draw(Invalid& invalid,TreeBase& n,Pixmap& bitmap)
		{
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT& key(nk);
			VT& data(nk);
			data(invalid,window,display,gc,bitmap);
			if (data.Moved()) movement=true;
			if (n.left) draw(invalid,*n.left,bitmap);
			if (n.right) draw(invalid,*n.right,bitmap);
		}

	};


	template<typename KT>
		struct RbTreeCanvas : TreeCanvas<KT>
	{
		typedef TreeNode<KT> VT ;
		RbTreeCanvas(Display* _display,Window& _window,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: TreeCanvas<KT>(_display,_window,_gc,_ScreenWidth,_ScreenHeight) {}
		virtual TreeBase* generate(KT& key,TreeNode<KT>& treenode) { return new RbTree<KT,VT>(key,treenode); }
		virtual bool CheckIntegrity(TreeBase* root)
		{
			if (!TreeCanvas<KT>::CheckIntegrity(root)) return false;
			return TreeIntegrity::RedBlackIntegrity<KT,VT>(root);
		}
	};
} // TreeDisplay

#endif  //TREE_DISPLAY_H


