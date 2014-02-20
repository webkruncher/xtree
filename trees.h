
#ifndef TREE_DISPLAY_H
#define TREE_DISPLAY_H


namespace TreeDisplay
{
	template<typename KT>
		struct TreeNode
	{
		void operator()(KT _k,TreeBase* parent)
		{
			k=_k;
			if (!parent) {x=1024/2; y=100;}
				else
			{
				Bst<KT,TreeNode<KT> >& p(static_cast<Bst<KT,TreeNode<KT> >&>(*parent));
				TreeNode<KT>& pn(p);
				double px(pn.x);
				double py(pn.y);
				double pk(p);
				if (k<pk) x=px-50; else x=px+50;
				y=py+20;
			}
		}
		void operator()(Display* display,GC& gc,Pixmap& bitmap)
		{
			XSetForeground(display,gc,0XFF);
			XFillRectangle(display,bitmap,gc, x-30,y-20,60,40);
			XSetForeground(display,gc,0XFFFFFF);
			stringstream ss;ss<<setprecision(2)<<fixed<<k;
			XDrawString(display,bitmap,gc,x-20,y+14,ss.str().c_str(),ss.str().size());
		}
		private:
		KT k;
		double x,y;
	};



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
			if (!((updateloop++)%100)) 
			{
				{
					srand(time(0));
					KT k(rand()%10000); k/=((KT)(rand()%100));
					TreeBase* n(new Bst<KT,VT>(k));
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


