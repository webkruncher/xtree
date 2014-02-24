
#include "x11grid.h"
using namespace X11Methods;
#include <tree.h>
using namespace TreeObjects;
#include <trees.h>
using namespace TreeDisplay;
#include <math.h>

int buggin(1);
namespace TreeDisplay
{
	template <> pair<bool,int> TreeCanvas<int>::Next(ifstream* input)
	{ 
		if (removing)
		{
			if (input)
			{
				string line;
				ifstream& in(*input);
				if (in.eof()) return make_pair<bool,int>(false,0);
				getline(in,line);
				if (line.empty()) return make_pair<bool,int>(false,0);
				int i(atoi(line.c_str()));
				used.erase(i);
				return make_pair<bool,int>(true,i);
			}
			if (used.empty()) return make_pair<bool,int>(false,0);
			int w(rand()%used.size());
			set<int>::iterator it=used.begin();
			for (int j=0;j<w;j++) it++;
			used.erase(it);
			return make_pair<bool,int>(true,*it);
		}
		#if 0
			static int dbler(0);
			dbler++;
			if (!(dbler%10))  // return a random value that was already inserted
			{
				int w(rand()%used.size());
				set<int>::iterator it=used.begin();
				for (int j=0;j<w;j++) it++;
				return make_pair<bool,int>(true,*it);
			}
		#endif

		if (input)
		{
			string line;
			ifstream& in(*input);
			if (in.eof()) return make_pair<bool,int>(false,0);
			getline(in,line);
			if (line.empty()) return make_pair<bool,int>(false,0);
			int i(atoi(line.c_str()));
			used.insert(i);
			return make_pair<bool,int>(true,i);
		}

		int Max(30);
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

	template <> pair<bool,double> TreeCanvas<double>::Next(ifstream* input)
	{ 
		if (removing)
		{
				int w(rand()%used.size());
				set<double>::iterator it=used.begin();
				for (int j=0;j<w;j++) it++;
				used.erase(it);
				return make_pair<bool,int>(true,*it);
		}
		if (used.size()==10) return make_pair<bool,double>(false,0);
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
