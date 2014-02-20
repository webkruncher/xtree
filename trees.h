
#ifndef TREE_DISPLAY_H
#define TREE_DISPLAY_H

namespace TreeDisplay
{
	struct Motion : private deque<pair<double,double> >
	{
		void operator()(double x,double y)
		{
			pair<double,double> now(x,y);;
			if (now!=current) push_back(now);
		}
		operator pair<double,double>& ()	
		{
			if (size()) { cout<<"!"; cout.flush(); current=back(); pop_back(); }
			return current;
		}
		private:
		pair<double,double> current;
	};

	template<typename KT>
		struct TreeNode
	{
		TreeNode() : SW(0),SH(0) {}
		TreeNode(const int _SW,const int _SH) : SW(_SW),SH(_SH) { BoxSize(); }
		TreeNode(const TreeNode& a) : text(a.text),SW(a.SW),SH(a.SH),CW(a.CW),CH(a.CH) {}
		void operator()(KT _k,TreeBase& node,TreeBase* parent)
		{
			k=_k;
			if (!parent) 
			{
				x=(SW/2)-(CW/2); y=(CH*3);
				motion(x,y);
				Text(0,k,k);
			} else {
				Bst<KT,TreeNode<KT> >& parentnode(static_cast<Bst<KT,TreeNode<KT> >&>(*parent));
				TreeNode<KT>& pn(parentnode);
				double px(pn.x);
				double py(pn.y);
				KT pk(parentnode);
				double D(SW/(parentnode.depth*7));
				if (k<pk) D*=-1;
				double mx(px+D);
				x=mx;
				y=py+(CH*2);
				motion(x,y);
				Text(node.depth,k,pk);
			}
		}
		void operator()(Display* display,GC& gc,Pixmap& bitmap)
		{
			pair<double,double> p(motion);
			x=(p.first); y=(p.second);
			XSetForeground(display,gc,0XFF);
			XFillRectangle(display,bitmap,gc, x-(CW/2),y-(CH/2),CW,CH);
			XSetForeground(display,gc,0XFFFFFF);
			XDrawString(display,bitmap,gc,x-((CW/2)-2),y+2,text.c_str(),text.size());
		}
		private:
		string text;
		const int SW,SH;
		const string tyid;
		int CW,CH;
		KT k;
		double x,y;
		Motion motion;
		void Text(int depth,KT k,KT pk) {stringstream ss; ss<<depth<<")"<<k<<","<<pk; text=ss.str().c_str();}
		void BoxSize() {CW=50; CH=10;}
	};

	template <> void TreeNode<int>::Text(int depth,int k,int pk)
	{
		stringstream ss;
		ss<<depth<<">"<<k<<","<<pk; 
		text=ss.str();
	}

	template <> void TreeNode<int>::BoxSize() { CW=60; CH=10; }

	template <> void TreeNode<double>::Text(int depth,double k,double pk)
	{
		stringstream ss;
		ss<<depth<<">"<<setprecision(2)<<k;
		text=ss.str();
	}

	template <> void TreeNode<double>::BoxSize() { CW=60; CH=10; }

	template<typename KT>
		struct TreeCanvas : Canvas
	{
		typedef TreeNode<KT> VT ;
		TreeCanvas(Display* _display,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: Canvas(_display,_gc,_ScreenWidth,_ScreenHeight),updateloop(0),root(NULL) {}
		virtual void operator()(Pixmap& bitmap) 
		{
			XSetForeground(display,gc,0X777777);
			InvalidBase& _invalidbase(*this);
			InvalidArea<Rect>& _invalid(static_cast<InvalidArea<Rect>&>(_invalidbase));
			X11Grid::Rect r(0,0,1024,768);
			if (root) render(*root,bitmap);
			_invalid.insert(r);
		}
		virtual void update() 
		{
			string tyid(typeid(KT).name());
			if (!((updateloop++)%10)) 
			{
				pair<bool,KT> next(Next());
				if (next.first)
				{
						TreeNode<KT> tn(ScreenWidth,ScreenHeight);
						TreeBase* n(new Bst<KT,VT>(next.second,tn));
						if (!root) 
						{
							root=n;  
						} else {
							TreeBase* tb(root->insert(root,n));
							if (tb) root=tb; // for in case the root was rotated
						}
				} 
			}
			if (root) traverse(*root);
		}
		virtual operator InvalidBase& () {return invalid;}
		private:
		set<KT> used;
		unsigned long updateloop;
		TreeBase* root;
		InvalidArea<Rect> invalid;
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
		void render(TreeBase& n,Pixmap& bitmap)
		{
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT& key(nk);
			VT& data(nk);
			data(display,gc,bitmap);
			if (n.left) render(*n.left,bitmap);
			if (n.right) render(*n.right,bitmap);
		}
	};

	template <> pair<bool,int> TreeCanvas<int>::Next()
	{ 
		if (used.size()==10) return make_pair<bool,int>(false,0);
		srand(time(0));
		int k;
		do 
		{
			k=(rand()%100); 
			k+=50; //k/=(((KT)(rand()%100)+1));
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

} // TreeDisplay

#endif  //TREE_DISPLAY_H


