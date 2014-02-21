
#ifndef TREE_DISPLAY_H
#define TREE_DISPLAY_H
#include <math.h>
namespace TreeDisplay
{
	struct Invalid : X11Methods::InvalidArea<Rect> { void insert(Rect r) {set<Rect>::insert(r); } };

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
			const double distx(x-tx);
			const double disty(y-ty);
			const double direction(atan2(disty, distx));
			const double distance(sqrt( (distx * distx) + (disty * disty) ) );
			if (distance<10)
			{
				if (empty()) return make_pair<double,double>(0,0);
				//cout<<tx<<"-"<<x<<", "<<ty<<"-"<<ty<<", d:"<<distance<<endl; cout.flush();
				{
					x=back().first; 
					y=back().second; 
					pop_back();
				}
			}
			const double force(distance/10);
			const double dx(force*cos(direction));
			const double dy(force*sin(direction));
			return make_pair<double,double>(dx,dy);
		}
		private: double x,y;
	};

	template<typename KT>
		struct TreeNode 
	{
		TreeNode() : SW(0),SH(0),x(0),y(0),moved(true) {}
		TreeNode(const int _SW,const int _SH) : SW(_SW),SH(_SH),x(_SW/4),y(10), motion(x,y), color(0X003333), moved(true){ BoxSize(); }
		TreeNode(const TreeNode& a) : text(a.text),SW(a.SW),SH(a.SH),CW(a.CW),CH(a.CH),x(a.x),y(a.y),moved(true),motion(a.x,a.y),color(a.color)  {}
		void operator()(TreeBase& node,TreeBase* parent) {}
		void operator()(unsigned long long _color)
		{
			color=_color;
		}
		void operator()(KT _k,TreeBase& node,TreeBase* parent)
		{
			k=_k;
			if (!parent) 
			{
				double x=(SW/2)-(CW/2); double y=(CH*3);
				motion(x,y);
				stringstream sstxt; sstxt<<node;
				Text(0,k,k,sstxt.str());
			} else {
				Bst<KT,TreeNode<KT> >& parentnode(static_cast<Bst<KT,TreeNode<KT> >&>(*parent));
				TreeNode<KT>& pn(parentnode);
				KT pk(parentnode);
				double px(pn.x);
				double py(pn.y);
				const double sw(SW); double sh(SH);
				double y;
				if (k<pk) y=py+(CH*3);
				else y=py+(CH*3)-CH;
				double x;
				if (parent->parent)
				{
					Bst<KT,TreeNode<KT> >& grandparentnode(static_cast<Bst<KT,TreeNode<KT> >&>(*parent->parent));
					TreeNode<KT>& gpn(grandparentnode);
					double gpx(gpn.x);
					double dx(gpn.x-pn.x);
					dx/=16; dx*=9; // 9/16s of the difference between parent and grand-parent
					if (dx<0) dx*=-1;
					if (k<pk) x=px-dx; else x=px+dx; 
					motion(x,y);
					stringstream sstxt; sstxt<<node;
					Text(node.depth,k,pk,sstxt.str());
				} else {
					if (k<pk) x=px/2; else x=(sw+px)/2; // half the difference between root and left or right edge
					motion(x,y);
					stringstream sstxt; sstxt<<node;
					Text(node.depth,k,pk,sstxt.str());
				}
			}
		}
		void operator()(Invalid& invalid,Display* display,GC& gc,Pixmap& bitmap)
		{
			moved=false;
			pair<double,double> p(motion.next(x,y));
			if ((p.first) or (p.second)) moved=true;
			{
				pair<double,double> ul(x-(CW/2),y-(CH/2));
				pair<double,double> lr(ul.first+CW,ul.second+CH);
				Rect iv(ul.first-1,ul.second-1,lr.first+2,lr.second+2);
				invalid.insert(iv);
				XSetForeground(display,gc,0X777777);
				XPoint& points(iv);
				XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			}
			x+=p.first; y+=p.second;
			{
				pair<double,double> ul(x-(CW/2),y-(CH/2));
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
		double x,y;
		Motion motion;
		unsigned long color;
		void Text(int depth,KT k,KT pk,string txt) {stringstream ss; ss<<depth<<")"<<k<<","<<pk<<" "<<txt; text=ss.str().c_str();}
		void BoxSize() {CW=50; CH=12;}
	};

	template <> void TreeNode<int>::Text(int depth,int k,int pk,string txt)
	{
		stringstream ss;
		//ss<<depth<<"["<<k<<","<<pk; 
		ss<<k<<" "<<txt;
		text=ss.str();
	}

	template <> void TreeNode<int>::BoxSize() { CW=50; CH=12; }

	template <> void TreeNode<double>::Text(int depth,double k,double pk,string txt)
	{
		stringstream ss;
		ss<<depth<<">"<<setprecision(2)<<k;
		ss<<k<<" "<<txt;
		text=ss.str();
	}

	template <> void TreeNode<double>::BoxSize() { CW=60; CH=12; }

	template<typename KT>
		struct TreeCanvas : Canvas
	{
		typedef TreeNode<KT> VT ;
		TreeCanvas(Display* _display,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: Canvas(_display,_gc,_ScreenWidth,_ScreenHeight),updateloop(0),root(NULL),movement(true) {}
		virtual ~TreeCanvas() {if (root) delete root;}
		virtual void operator()(Pixmap& bitmap) { movement=false; if (root) draw(invalid,*root,bitmap); }
		virtual TreeBase* generate(KT& key,TreeNode<KT>& treenode) { return new Bst<KT,VT>(key,treenode); }
		virtual void update() 
		{
			//if (updateloop>300) { if (root) delete root; root=NULL; return; }
			string tyid(typeid(KT).name());
			if (!movement)
			{
				pair<bool,KT> next(Next());
				if (next.first)
				{
						TreeNode<KT> tn(ScreenWidth,ScreenHeight);
						//TreeBase* n(new Bst<KT,VT>(next.second,tn));
						TreeBase* n(generate(next.second,tn));
						if (!root) 
						{
							root=n;  
							movement=true;
						} else {
							TreeBase* tb(root->insert(root,n));
							if (tb) root=tb; // for in case the root was rotated
						}
						KT maxvalue(root->maxValue(root));
						KT minvalue(root->minValue(root));
						bool isbst(root->isBST(root));
						cout<<"Min:"<<setprecision(2)<<fixed<<minvalue<<" ";
						cout<<"Max:"<<setprecision(2)<<fixed<<maxvalue<<" ";
						cout<<"isBST:"<<boolalpha<<isbst;
						cout.flush();
						//cout<<"Min:"<<*used.begin();
						//cout<<"Max:"<<*used.rbegin();
						if (minvalue!=(*used.begin())) throw string("Min check failed");
						if (maxvalue!=(*used.rbegin())) throw string("Max check failed");
						if (!isbst) throw string("isBST failed");
						long ttl(root->countnodes());
						if (ttl!=used.size()) throw string("Wrong number of nodes counted");
						cout<<" Total:"<<ttl<<" == "<<used.size()<<endl;
				} 
			}
		
			if (!((updateloop)%10)) if (root) traverse(*root);
			updateloop++;
		}
		virtual operator InvalidBase& () {return invalid;}
		private:
		bool movement;
		set<KT> used;
		unsigned long updateloop;
		TreeBase* root;
		Invalid invalid;
		pair<bool,KT> Next() { return make_pair<bool,KT>(true,rand()%10); }
		void traverse(TreeBase& n)
		{
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
		if (used.size()==30) return make_pair<bool,int>(false,0);
		if (false)
		{	
			static int j(-11);
			j++;
			used.insert(j);
			return make_pair<bool,int>(true,j);
		}

		srand(time(0));
		int k;
		do 
		{
			k=(rand()%100); 
			k+=50; 
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
	};
} // TreeDisplay

#endif  //TREE_DISPLAY_H


