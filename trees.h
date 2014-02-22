
#ifndef TREE_DISPLAY_H
#define TREE_DISPLAY_H
#include <math.h>
namespace TreeDisplay
{
	struct Invalid : X11Methods::InvalidArea<Rect> { void insert(Rect r) {set<Rect>::insert(r); } };

	template <typename KT,typename VT>
		inline void BlackNodeCounter(RbTree<KT,VT>* n,int& d)
		{
			if (!n) return;
			RbTree<KT,VT>& nd(*n); 
			if (color(n)==RbTree<KT,VT>::BLACK) d++;
			if (n->right) BlackNodeCounter<KT,VT>(n->right,d);
			if (n->left) BlackNodeCounter<KT,VT>(n->left,d);
		}

	inline int DepthFinder(TreeBase& tb,int d=0)
		{ if (tb.parent) d=DepthFinder(*tb.parent,d+1); return d; }

	struct Motion : private deque<pair<double,double> >
	{
		Motion(const double _x,const double _y) : x(_x),y(_y) { }
		void operator()(double x,double y)
		{
			pair<double,double> now(x,y);;
			if (now!=front()) push_front(now);
		}
		pair<double,double> next(double tx,double ty)
		{
			double distx(x-tx);
			double disty(y-ty);
			double direction(atan2(disty, distx));
			double distance(sqrt( (distx * distx) + (disty * disty) ) );

			if (distance<4)
			{
				if (empty()) return make_pair<double,double>(0,0);
				{ x=back().first; y=back().second; pop_back(); }
			}

			if (distance<4) return make_pair<double,double>(0,0);

			double force(distance/6);
			const double dx(force*cos(direction));
			const double dy(force*sin(direction));
			return make_pair<double,double>(dx,dy);
		}
		private: double x,y;
	};

	template<typename KT>
		struct TreeNode 
	{
		TreeNode() : SW(0),SH(0),X(0),Y(0),moved(true) {}
		TreeNode(const int _SW,const int _SH) : SW(_SW),SH(_SH),X(_SW/4),Y(10), motion(X,Y), color(0X003333), moved(true){ BoxSize(); }
		TreeNode(const TreeNode& a) : text(a.text),SW(a.SW),SH(a.SH),CW(a.CW),CH(a.CH),X(a.X),Y(a.Y),moved(true),motion(a.X,a.Y),color(a.color)  {}
		void operator()(TreeBase& node,TreeBase* parent) {}
		void operator()(unsigned long long _color) { color=_color; }
		void operator()(KT _k,TreeBase& node,TreeBase* parent)
		{
			k=_k;
			if (!parent) 
			{
				double x=(SW/2)-(CW/2); double y=(CH*3);
				motion(x,y);
				Text(k,k);
			} else {
				Bst<KT,TreeNode<KT> >& parentnode(static_cast<Bst<KT,TreeNode<KT> >&>(*parent));
				TreeNode<KT>& pn(parentnode);
				if (pn.moved) return;
				KT pk(parentnode);
				double px(pn.X);
				double py(pn.Y);
				const double sw(SW); double sh(SH);
				double y;
				if (k<pk) y=py+(CH*3);
				else y=py+(CH*3)-CH;
				double x;
				if (parent->parent)
				{
					Bst<KT,TreeNode<KT> >& grandparentnode(static_cast<Bst<KT,TreeNode<KT> >&>(*parent->parent));
					TreeNode<KT>& gpn(grandparentnode);
					if (gpn.moved) return;
					double gpx(gpn.X);
					double dx(gpn.X-pn.X);
					dx/=16; dx*=7; // 7/16s of the difference between parent and grand-parent
					if (dx<0) dx*=-1;
					if (k<pk) x=px-dx; else x=px+dx; 

					int depth(DepthFinder(node)+3);
					y=CH*((depth*depth)/2);
					if (k<pk) y+=CH;

					motion(x,y);
					Text(k,pk);
				} else {
					if (k<pk) x=px/2; else x=(sw+px)/2; // half the difference between root and left or right edge
					motion(x,y);
					Text(k,pk);
				}
			}
		}
		void operator()(Invalid& invalid,Display* display,GC& gc,Pixmap& bitmap)
		{
			moved=false;
			pair<double,double> D(motion.next(X,Y));
			if ((D.first) or (D.second)) moved=true;
			{
				pair<double,double> ul(X-(CW/2),Y-(CH/2));
				pair<double,double> lr(ul.first+CW,ul.second+CH);
				Rect iv(ul.first-1,ul.second-1,lr.first+2,lr.second+2);
				invalid.insert(iv);
				XSetForeground(display,gc,0X777777);
				XPoint& points(iv);
				XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			}
			X+=D.first; Y+=D.second;
			{
				pair<double,double> ul(X-(CW/2),Y-(CH/2));
				pair<double,double> lr(ul.first+CW,ul.second+CH);
				Rect iv(ul.first,ul.second,lr.first,lr.second);
				invalid.insert(iv);
				XPoint& points(iv);
				XSetForeground(display,gc,color);
				XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
				XSetForeground(display,gc,0XFFFFFF);
				XDrawString(display,bitmap,gc,ul.first,ul.second+(CH),text.c_str(),text.size());
			}
		}
		bool moved;
		private:
		string text;
		const int SW,SH;
		const string tyid;
		int CW,CH;
		KT k;
		double X,Y;
		Motion motion;
		unsigned long color;
		void Text(KT k,KT pk,string txt="") {stringstream ss; ss<<k<<","<<pk<<" "<<txt; text=ss.str().c_str();}
		void BoxSize() {CW=50; CH=12;}
	};

	template <> void TreeNode<int>::Text(int k,int pk,string txt)
	{
		stringstream ss;
		ss<<k; if (!txt.empty()) ss<<" "<<txt;
		text=ss.str();
	}

	template <> void TreeNode<int>::BoxSize() { CW=30; CH=12; }

	template <> void TreeNode<double>::Text(double k,double pk,string txt)
	{
		stringstream ss;
		ss<<k; if (!txt.empty()) ss<<" "<<txt;
		text=ss.str();
	}

	template <> void TreeNode<double>::BoxSize() { CW=60; CH=12; }

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

						CheckIntegrity(root);
				} 
			}
		
			if (!((updateloop)%10)) if (root) traverse(*root);
			updateloop++;
		}
		virtual operator InvalidBase& () {return invalid;}
		protected:
		bool stop;
		virtual void CheckIntegrity(TreeBase* root)
		{
			if (!root) return;
			Bst<KT,VT>& rk(static_cast<Bst<KT,VT>&>(*root));
			KT maxvalue(rk.maxValue(root));
			KT minvalue(rk.minValue(root));
			bool isbst(root->isBST(root));
			cout<<"Min:"<<setprecision(2)<<fixed<<minvalue<<" ";
			cout<<"Max:"<<setprecision(2)<<fixed<<maxvalue<<" ";
			cout<<"isBST:"<<boolalpha<<isbst;
			cout.flush();
			//cout<<"Min:"<<*used.begin();
			//cout<<"Max:"<<*used.rbegin();
			if (minvalue!=(*used.begin())) {stop=true; cout<<("Min check failed")<<endl;}
			if (maxvalue!=(*used.rbegin())) {stop=true; cout<<("Max check failed")<<endl;}
			if (!isbst) {stop=true; cout<<("isBST failed")<<endl;}
			long ttl(root->countnodes());
			if (ttl!=used.size()) {stop=true; cout<<("Wrong number of nodes counted")<<endl;}
			cout<<" Total:"<<ttl<<" == "<<used.size()<<endl;
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
			if (data.moved) movement=true;
			if (n.left) draw(invalid,*n.left,bitmap);
			if (n.right) draw(invalid,*n.right,bitmap);
		}

	};

	template <> pair<bool,int> TreeCanvas<int>::Next()
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

	template <> pair<bool,double> TreeCanvas<double>::Next()
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
		virtual void CheckIntegrity(TreeBase* root)
		{
			if (!root) return;
			cout<<"Checking Red Black"<<endl;
			RbTree<KT,VT>& rk(static_cast<RbTree<KT,VT>&>(*root));
			TreeCanvas<KT>::CheckIntegrity(root);
		}
	};
} // TreeDisplay

#endif  //TREE_DISPLAY_H


