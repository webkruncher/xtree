
#ifndef TREE_DISPLAY_H
#define TREE_DISPLAY_H


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



#if 0
#include <math.h>
namespace TreeDisplay
{

	struct Invalid : X11Grid::InvalidGrid { };

	struct TreeGrid;
	struct TreeRow;
	struct TreeColumn;
	struct TreeCell;

	struct TestStructure : X11Grid::DefaultStructure
	{
		typedef TreeGrid GridType;
		typedef TreeRow RowType;
		typedef TreeColumn ColumnType;
		typedef TreeCell CellType;
	};


	struct TreeCell : X11Grid::Cell
	{
			TreeCell(X11Grid::GridBase& _grid,const int _x,const int _y,bool _dead=true)
				: X11Grid::Cell(_grid,_x,_y,0X333333),X(_x), Y(_y), 
					remove(false),lastcolor(0),age(0) { }
			virtual bool update(const unsigned long updateloop,const unsigned long updaterate);
			virtual void operator()(Pixmap& bitmap);
			private:
			const int X,Y;
			bool remove;
			unsigned long lastcolor;
			int age;
	};

	struct TreeColumn : X11Grid::Column<TestStructure>
	{
			TreeColumn(X11Grid::GridBase& _grid,const int _position) : X11Grid::Column<TestStructure>(_grid,_position) {}
			void Birth(const int x,const int y);
			virtual bool update(const unsigned long updateloop,const unsigned long updaterate)
			{
				cerr<<size()<<" columns"<<endl;
				if (empty()) return true;
				for (iterator it=begin();it!=end();it++) 
					if (it->second.update(updateloop,updaterate)) erase(it);
				if (empty()) return true;
				return false;
			}
			virtual void operator()(Pixmap& bitmap)
				{ for (iterator it=begin();it!=end();it++) it->second(bitmap); }
	};

	struct TreeRow : X11Grid::Row<TestStructure>
	{
			TreeRow(X11Grid::GridBase& _grid) : X11Grid::Row<TestStructure>(_grid) {}
			virtual void update(const unsigned long updateloop,const unsigned long updaterate) ;
			virtual void operator()(Pixmap& bitmap)
			{ 
				for (TreeRow::iterator it=this->begin();it!=this->end();it++) it->second(bitmap);
			}
	};


	struct PlotterBase
	{
		static PlotterBase* generate(X11Grid::GridBase& _grid,const int _w,const int _h,const int _cw,const int _ch);	
		virtual operator const bool () = 0;
		protected: 
		PlotterBase(const int _w,const int _h,const int _cw,const int _ch) : w(_w),h(_h),cw(_cw),ch(_ch) {}
		const int w,h,cw,ch;
		private: static int what;
	};

	struct Plot
	{
		Plot(X11Grid::GridBase& _grid,const int _w,const int _h,const int _cw,const int _ch) : grid(_grid), plotter(NULL) , w(_w),h(_h),cw(_cw),ch(_ch) {reset();}
		virtual ~Plot(){if (plotter) delete plotter;}
		operator const bool (){return *plotter;} 
		void reset() { if (plotter) delete plotter; plotter=PlotterBase::generate(grid,w,h,cw,ch); }
		private:
		X11Grid::GridBase& grid;
		PlotterBase* plotter;
		const int w,h,cw,ch;
	};


	struct TreeGrid : X11Grid::Grid<TestStructure>
	{
		TreeGrid(Display* _display,GC& _gc,const int _ScreenWidth, const int _ScreenHeight,const unsigned long _bkcolor)
			: X11Grid::Grid<TestStructure>(_display,_gc,_ScreenWidth,_ScreenHeight,_bkcolor),
					CW(8),CH(8), updaterate(1),updateloop(0),plot(*this,_ScreenWidth,_ScreenHeight,8,8)  {}
		virtual operator InvalidBase& () {return invalid;}
		private:
		Plot plot;
		Invalid invalid;
		const int CW,CH;
		const int updaterate;
		unsigned long updateloop;
		void operator()(Pixmap& bitmap) 
		{ 
			if (updateloop<2)
			{
				XSetForeground(display,gc,bkcolor);
				InvalidBase& _invalidbase(*this);
				Invalid& _invalid(static_cast<Invalid&>(_invalidbase));
				X11Grid::ProximityRectangle r(-10,-10,0,0,ScreenWidth,ScreenHeight);
				_invalid.insert(-10,-10,r);
				XFillRectangle(display,bitmap,gc,0,00,ScreenWidth,ScreenHeight);
			}
			X11Grid::Grid<TestStructure>::operator()(bitmap);
		}
		virtual void update()
		{
			TreeGrid& grid(*this);
			++updateloop;	
			//if (updateloop%updaterate) return;
			if (!plot) plot.reset();
			TreeRow::update(updateloop,updaterate);
			//grid[plot]=0;
		}
		private:
		void operator()(const unsigned long color,Pixmap&  bitmap,const int _x,const int _y)
		{
			InvalidBase& _invalidbase(*this);
			Invalid& _invalid(static_cast<Invalid&>(_invalidbase));
			const int x(_x*CW);
			const int y(_y*CW);
			const int border(2);
			X11Grid::ProximityRectangle r(x,y,(x-(CW/2))+border,(y-(CH/2))+border,(x+(CW/2))-border,(y+(CH/2))-border);	
			XPoint& points(r);
			XSetForeground(display,gc,color);
			XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			_invalid.insert(_x,_y,r);
		}
	};


		bool TreeCell::update(const unsigned long updateloop,const unsigned long updaterate)
		{
			if (updateloop%updaterate) return false;
			map<string,int>& metrics(static_cast<map<string,int>&>(grid));	
			X11Grid::GridBase& gridbase(static_cast<X11Grid::GridBase&>(grid));	
			TreeGrid& lifegrid(static_cast<TreeGrid&>(grid));	
			if (color==0X333333) remove=true;
			return remove;
		}

		void TreeCell::operator()(Pixmap& bitmap)
		{ 
				if (cards.empty()) color=0X333333;
				if (deactivate) {color=background; active=false;}
				if (!cards.empty()) 
				{
					int offset(0);
					for (map<unsigned long,X11Grid::Card*>::iterator cit=cards.begin();cit!=cards.end();cit++) 
					{
						offset+=10;
						grid(*cit->second,bitmap,X+offset,Y+offset);
					}
				} else grid(color,bitmap,X,Y);
				//X11Grid::Cell::operator()(bitmap); 
		}
#if 0
		void TreeCell::operator-=(Card* c)
		{
			if (!c) return;
			const unsigned long id(*c);
			map<unsigned long,Card*>::iterator it(cards.find(id));
			if (it==cards.end()) return;
			cards.erase(it);
			if (cards.empty()) active=false;
			grid.cover(c,background,X,Y);
		}
#endif

		void TreeRow::update(const unsigned long updateloop,const unsigned long updaterate) 
		{
			cerr<<size()<<" rows"<<endl;
			for (iterator it=begin();it!=end();it++) 
				if (it->second.update(updateloop,updaterate)) erase(it);
		}
} // TreeDisplay
#else
#endif
#endif  //TREE_DISPLAY_H


