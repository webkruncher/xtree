
#ifndef XRBTREE_H
#define XRBTREE_H
#include <math.h>
namespace XRbTree
{

	struct Invalid : X11Grid::InvalidGrid { };

	struct XRbTreeGrid;
	struct XRbTreeRow;
	struct XRbTreeColumn;
	struct XRbTreeCell;

	struct TestStructure : X11Grid::DefaultStructure
	{
		typedef XRbTreeGrid GridType;
		typedef XRbTreeRow RowType;
		typedef XRbTreeColumn ColumnType;
		typedef XRbTreeCell CellType;
	};


	struct XRbTreeCell : X11Grid::Cell
	{
			XRbTreeCell(X11Grid::GridBase& _grid,const int _x,const int _y,bool _dead=true)
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

	struct XRbTreeColumn : X11Grid::Column<TestStructure>
	{
			XRbTreeColumn(X11Grid::GridBase& _grid,const int _position) : X11Grid::Column<TestStructure>(_grid,_position) {}
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

	struct XRbTreeRow : X11Grid::Row<TestStructure>
	{
			XRbTreeRow(X11Grid::GridBase& _grid) : X11Grid::Row<TestStructure>(_grid) {}
			virtual void update(const unsigned long updateloop,const unsigned long updaterate) ;
			virtual void operator()(Pixmap& bitmap)
			{ 
				for (XRbTreeRow::iterator it=this->begin();it!=this->end();it++) it->second(bitmap);
			}
	};


	struct PlotterBase
	{
		static PlotterBase* generate(X11Grid::GridBase& _grid,const int _w,const int _h);	
		//virtual operator Point& () = 0;
		virtual operator const bool () = 0;
		protected: 
		PlotterBase(const int _w,const int _h) : w(_w),h(_h) {}
		int w,h;
		private: static int what;
	};

	struct Plot
	{
		Plot(X11Grid::GridBase& _grid,const int _w,const int _h) : grid(_grid), plotter(NULL) , w(_w),h(_h) {reset();}
		virtual ~Plot(){if (plotter) delete plotter;}
		operator const bool (){return *plotter;} 
		void reset() { if (plotter) delete plotter; plotter=PlotterBase::generate(grid,w,h); }
		//operator Point& () { return (*plotter); } 
		private:
		X11Grid::GridBase& grid;
		PlotterBase* plotter;
		const int w,h;
	};


	struct XRbTreeGrid : X11Grid::Grid<TestStructure>
	{
		XRbTreeGrid(Display* _display,GC& _gc,const int _ScreenWidth, const int _ScreenHeight,const unsigned long _bkcolor)
			: X11Grid::Grid<TestStructure>(_display,_gc,_ScreenWidth,_ScreenHeight,_bkcolor),
					CW(8),CH(8), updaterate(1),updateloop(0),plot(*this,_ScreenWidth/8,_ScreenHeight/8)  {}
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
			XRbTreeGrid& grid(*this);
			++updateloop;	
			//if (updateloop%updaterate) return;
			if (!plot) plot.reset();
			XRbTreeRow::update(updateloop,updaterate);
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
			X11Grid::ProximityRectangle r(_x,_y,(x-(CW/2))+border,(y-(CH/2))+border,(x+(CW/2))-border,(y+(CH/2))-border);	
			XPoint& points(r);
			XSetForeground(display,gc,color);
			XFillPolygon(display,bitmap,  gc,&points, 4, Complex, CoordModeOrigin);
			_invalid.insert(_x,_y,r);
		}
	};


		bool XRbTreeCell::update(const unsigned long updateloop,const unsigned long updaterate)
		{
			if (updateloop%updaterate) return false;
			map<string,int>& metrics(static_cast<map<string,int>&>(grid));	
			X11Grid::GridBase& gridbase(static_cast<X11Grid::GridBase&>(grid));	
			XRbTreeGrid& lifegrid(static_cast<XRbTreeGrid&>(grid));	
			if (color==0X333333) remove=true;
			return remove;
		}

		void XRbTreeCell::operator()(Pixmap& bitmap)
		{ 
				if (cards.empty()) color=0X333333;
				X11Grid::Cell::operator()(bitmap); 
		}

		void XRbTreeRow::update(const unsigned long updateloop,const unsigned long updaterate) 
		{
			cerr<<size()<<" rows"<<endl;
			for (iterator it=begin();it!=end();it++) 
				if (it->second.update(updateloop,updaterate)) erase(it);
		}
} // XRbTree
#endif  //XRBTREE_H


