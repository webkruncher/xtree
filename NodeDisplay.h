#ifndef NODE_DISPLAY_H
#define NODE_DISPLAY_H

namespace TreeDisplay
{
	inline int DepthFinder(TreeBase& tb,int d=0)
		{ if (tb.parent) d=DepthFinder(*tb.parent,d+1); return d; }

	struct NodeBase
	{
		NodeBase() : SW(0),SH(0),X(0),Y(0),moved(true),motion(100,100) {}
		NodeBase(const int _SW,const int _SH) : SW(_SW),SH(_SH),X(_SW/4),Y(10),moved(true), motion(X,Y), color(0X003333) { }
		NodeBase(const NodeBase& a) : text(a.text),SW(a.SW),SH(a.SH),CW(a.CW),CH(a.CH),X(a.X),Y(a.Y),moved(a.moved), motion(a.X,a.Y),color(a.color)  {}
		public:
		bool moved;
		protected: 
		void operator()(Invalid& invalid,Display* display,GC& gc,Pixmap& bitmap);
		Motion motion;
		string text;
		const int SW,SH;
		double X,Y;
		unsigned long color;
		int CW,CH;
	};

	template<typename KT>
		struct TreeNode : NodeBase
	{
		TreeNode() {}
		TreeNode(const int _SW,const int _SH) : NodeBase(_SW,_SH) { BoxSize(); }
		TreeNode(const TreeNode& a) : NodeBase(a)  {}
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
			{ NodeBase::operator()(invalid,display,gc,bitmap); }
		private:
		//const string tyid;
		KT k;
		void Text(KT k,KT pk,string txt="") {stringstream ss; ss<<k<<","<<pk<<" "<<txt; text=ss.str().c_str();}
		void BoxSize() {CW=50; CH=12;}
	};

	template <> inline void TreeNode<int>::Text(int k,int pk,string txt)
	{
		stringstream ss;
		ss<<k; if (!txt.empty()) ss<<" "<<txt;
		text=ss.str();
	}

	template <> inline void TreeNode<int>::BoxSize() { CW=30; CH=12; }

	template <> inline void TreeNode<double>::Text(double k,double pk,string txt)
	{
		stringstream ss;
		ss<<k; if (!txt.empty()) ss<<" "<<txt;
		text=ss.str();
	}

	template <> inline void TreeNode<double>::BoxSize() { CW=60; CH=12; }
} //namespace TreeDisplay

#endif // NODE_DISPLAY_H

