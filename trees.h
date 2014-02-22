
#ifndef TREE_DISPLAY_H
#define TREE_DISPLAY_H
#include <math.h>
struct Invalid : X11Methods::InvalidArea<Rect> { void insert(Rect r) {set<Rect>::insert(r); } };
#include "Motivation.h"
#include "NodeDisplay.h"
namespace TreeDisplay
{
	template<typename KT>
		struct TreeCanvas : Canvas
	{
		typedef TreeNode<KT> VT ;
		TreeCanvas(Display* _display,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: Canvas(_display,_gc,_ScreenWidth,_ScreenHeight),updateloop(0),root(NULL),movement(false),stop(false),waitfor(0) {}
		virtual ~TreeCanvas() {if (root) delete root;}
		virtual void operator()(Pixmap& bitmap) {  if (root) draw(invalid,*root,bitmap); }
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
						}

						if (!CheckIntegrity(root)) stop=true;
				} 
			}
		
			if (!((updateloop)%10)) if (root) traverse(*root);
			updateloop++;
		}
		virtual operator InvalidBase& () {return invalid;}
		protected:
		bool stop;
		virtual bool CheckIntegrity(TreeBase* root)
		{
			if (!root) return true;
			Bst<KT,VT>& rk(static_cast<Bst<KT,VT>&>(*root));
			KT maxvalue(rk.maxValue(root));
			KT minvalue(rk.minValue(root));
			bool isbst(root->isBST(root));
			//cout<<"Min:"<<setprecision(2)<<fixed<<minvalue<<" ";
			//cout<<"Max:"<<setprecision(2)<<fixed<<maxvalue<<" ";
			//cout<<"isBST:"<<boolalpha<<isbst;
			//cout.flush();
			//cout<<"Min:"<<*used.begin();
			//cout<<"Max:"<<*used.rbegin();
			if (minvalue!=(*used.begin())) {cout<<("Min check failed")<<endl; return false;}
			if (maxvalue!=(*used.rbegin())) {stop=true; cout<<("Max check failed")<<endl; return false;}
			if (!isbst) {cout<<("isBST failed")<<endl; return false;}
			long ttl(root->countnodes());
			if (ttl!=used.size()) {cout<<("Wrong number of nodes counted")<<endl; return false;}
			//cout<<" Total:"<<ttl<<" == "<<used.size()<<endl;
			return true;
		}
		private:
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
			data(invalid,display,gc,bitmap);
			if (data.Moved()) movement=true;
			if (n.left) draw(invalid,*n.left,bitmap);
			if (n.right) draw(invalid,*n.right,bitmap);
		}

	};

	template <> inline pair<bool,int> TreeCanvas<int>::Next()
	{ 
		static int dbler(0);
		dbler++;
		if (!(dbler%10))  // return a random value that was already inserted
		{
			int w(rand()%used.size());
			set<int>::iterator it=used.begin();
			for (int j=0;j<w;j++) it++;
			return make_pair<bool,int>(true,*it);
		}

		int Max(999);
		if (used.size()==(Max-1)) return make_pair<bool,int>(false,0);
		if (false)
		{	
			static int j(11);
			j--;
			used.insert(j);
			return make_pair<bool,int>(true,j);
		}

		srand(time(0));
		int k;
		do 
		{
			k=(rand()%Max); 
			k+=(Max/2); 
		} while (used.find(k)!=used.end());
		used.insert(k);
		return make_pair<bool,int>(true,k);
	}

	template <> inline pair<bool,double> TreeCanvas<double>::Next()
	{ 
		if (used.size()==1000) return make_pair<bool,double>(false,0);
		srand(time(0));
		double k;
		do 
		{
			k=(rand()%100); 
			k/=(((double)(rand()%100)+1));
		} while (used.find(k)!=used.end());
		used.insert(k);
		return make_pair<bool,double>(true,k);
	}

	template<typename KT>
		struct RbTreeCanvas : TreeCanvas<KT>
	{
		typedef TreeNode<KT> VT ;
		RbTreeCanvas(Display* _display,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: TreeCanvas<KT>(_display,_gc,_ScreenWidth,_ScreenHeight) {}
		virtual TreeBase* generate(KT& key,TreeNode<KT>& treenode) { return new RbTree<KT,VT>(key,treenode); }
		virtual bool CheckIntegrity(TreeBase* root)
		{
			if (!root) return true;
			//cout<<"Checking Red Black"<<endl;
			RbTree<KT,VT>& rk(static_cast<RbTree<KT,VT>&>(*root));
			if (!TreeCanvas<KT>::CheckIntegrity(root)) return false;
			TreeBase* leftmost(root->LeftMost());
			TreeBase* rightmost(root->RightMost());
			RbTree<KT,VT>& lm(static_cast<RbTree<KT,VT>&>(*leftmost));
			RbTree<KT,VT>& rm(static_cast<RbTree<KT,VT>&>(*rightmost));
			const KT& L(lm);
			const KT& R(rm);
			//cout<<"Leftmost:"<<L<<", Rightmost:"<<R<<endl;

			struct Visitor
			{
				Visitor(RbTree<KT,VT>& _rk) : rk(_rk),ok(true) {}
				operator bool () { InOrder(rk); return ok; }
				private:
				RbTree<KT,VT>& rk;
				bool ok;
				void InOrder(TreeBase& node)
				{
					if (node.left) InOrder(*node.left);
					Visit(node);
					if (node.right) InOrder(*node.right);
				}
				void Visit(TreeBase& node)
				{
					RbTree<KT,VT>& rb(static_cast<RbTree<KT,VT>&>(node));
					const KT& key(rb);
					int ld(0),rd(0);
					if (node.left) BlackNodeCounter(node.left,ld);
					if (node.right) BlackNodeCounter(node.right,rd);
					int diff(abs(ld-rd));
					if (diff>2) 
					{
						cout<<"Imbalanced:"<<key<<" l:"<<ld<<" r:"<<rd<<endl; cout.flush();
						ok=false;
					}
					if (rb.color(&node)==RbTree<KT,VT>::RED) 
					{
						if (rb.left) if (rb.color(rb.left)==RbTree<KT,VT>::RED)   {cout<<key<<" is a red node with a red child"<<endl; ok=false;}
						if (rb.right) if (rb.color(rb.right)==RbTree<KT,VT>::RED) {cout<<key<<" is a red node with a red child"<<endl; ok=false;}
					}
				}
				void BlackNodeCounter(TreeBase* n,int& d)
				{
					if (!n) return;
					RbTree<KT,VT>& rb(static_cast<RbTree<KT,VT>&>(*n));
					if (rb.color(n)==RbTree<KT,VT>::BLACK) d++;
					if (n->right) BlackNodeCounter(n->right,d);
					if (n->left) BlackNodeCounter(n->left,d);
					if (!n->left) d++; if (!n->right) d++;
				}
			} visitor(rk);

			if (!visitor) {cout<<"Red Black check failed"<<endl; return false;}
			return true;
			
		}
	};
} // TreeDisplay

#endif  //TREE_DISPLAY_H


