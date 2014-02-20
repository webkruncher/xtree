
#ifndef TREE_DISPLAY_H
#define TREE_DISPLAY_H


namespace TreeDisplay
{
	template<typename KT>
		struct TreeNode
	{
		TreeNode() : text("error"),SW(0),SH(0) {}
		TreeNode(const string _text,const int _SW,const int _SH) : text(_text),SW(_SW),SH(_SH)
			{ BoxSize(); }
		TreeNode(const TreeNode& a) : text(a.text),SW(a.SW),SH(a.SH),CW(a.CW),CH(a.CH) {}
		void operator()(KT _k,TreeBase* parent)
		{
			k=_k;
			if (!parent) 
			{
				x=(SW/2)-(CW/2); y=(CH*3);
				Text(0,k,k);
			} else {
				Bst<KT,TreeNode<KT> >& node(static_cast<Bst<KT,TreeNode<KT> >&>(*parent));
				TreeNode<KT>& pn(node);
				double px(pn.x);
				double py(pn.y);
				KT pk(node);
				double D(SW/(node.depth*7));
				if (k<pk) D*=-1;
				double mx(px+D);
				x=mx;
				y=py+(CH*2);
				Text(node.depth,k,pk);
			}
		}
		void operator()(Display* display,GC& gc,Pixmap& bitmap)
		{
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
		void Text(int depth,KT k,KT pk) {}
		void BoxSize() {}
	};


	template <> void TreeNode<int>::Text(int depth,int k,int pk)
	{
		stringstream ss;
		ss<<depth<<">"<<k<<","<<pk; 
		text=ss.str();
	}

	template <> void TreeNode<int>::BoxSize()
	{
		CW=60; CH=10;
	}


	template <> void TreeNode<double>::Text(int depth,double k,double pk)
	{
		stringstream ss;
		ss<<depth<<">"<<setprecision(2)<<k;
		text=ss.str();
	}

	template <> void TreeNode<double>::BoxSize()
	{
		CW=60; CH=10;
	}

	template<typename KT>
		struct TreeCanvas : Canvas
	{
		typedef TreeNode<KT> VT ;
		TreeCanvas(Display* _display,GC& _gc,const int _ScreenWidth, const int _ScreenHeight)
			: Canvas(_display,_gc,_ScreenWidth,_ScreenHeight),tick(0), updateloop(0),root(NULL) {}
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
				bool go=true;
				if (tyid=="i") if (used.size()==100) go=false; else if (used.size()==1000) go=false;
				if (go) 
				{
					srand(time(0));
					KT k;
					do 
					{
						k=(rand()%100); 
						if (tyid=="i") k+=50; else k/=(((KT)(rand()%100)+1));
					} while (used.find(k)!=used.end());
					used.insert(k);
					stringstream ss; ss<<(tick++)<<") "<<setprecision(2)<<k;
					TreeNode<KT> tn(ss.str().c_str(),ScreenWidth,ScreenHeight);
					TreeBase* n(new Bst<KT,VT>(k,tn));
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
		int tick;
		unsigned long updateloop;
		TreeBase* root;
		InvalidArea<Rect> invalid;
		void traverse(TreeBase& n)
		{
			Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(n));
			const KT& key(nk);
			VT& data(nk);
			data(key,nk.parent);
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

} // TreeDisplay

#endif  //TREE_DISPLAY_H


