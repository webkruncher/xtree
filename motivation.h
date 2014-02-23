#ifndef MOTIVATION_H
#define MOTIVATION_H
namespace TreeDisplay
{
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
} //namespace TreeDisplay
#endif MOTIVATION_H

