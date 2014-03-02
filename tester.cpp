/*
* Copyright (c) Jack M. Thompson WebKruncher.com, exexml.com
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the WebKruncher nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jack M. Thompson ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jack M. Thompson BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <treeset.h>

#include <cstdio>
#include <memory>
#include <set>
#include <sys/time.h>
#include <utility>
#include <vector>
#include <iostream>
#include <iomanip>
#include <locale> 

#define TREEOBJECTS first
#define STLOBJECTS second



struct Punk: public std::numpunct<char>
{
	protected:
	virtual char do_thousands_sep() const {return ',';}
	virtual std::string do_grouping() const {return "\03";}
};

void Insert(std::set<long>& items,const long number) { items.insert(number); }
void Erase(std::set<long>& items,const long number) { items.erase(number);}
void Insert(TreeObjects::Set<long>& items,const long number) { items.insert(number);}
void Erase(TreeObjects::Set<long>& items,const long number) { items.erase(number);}

template <typename T>
	void Test(T& items,const long* numbers,const long size) 
{ 
	for (long j=0;j<size;j++) Insert(items,numbers[j]);
	for (long j=0;j<size/2;j++) Erase(items,numbers[j]);
	for (long j=size/2;j>=0;j--) Insert(items,numbers[j]);
	for (long j=0;j<size;j++) Erase(items,numbers[j]);
}

void TestTreeSet(const long* numbers,const long size)
{
	TreeObjects::Set<long> items;
	Test<TreeObjects::Set<long> >(items,numbers,size);
}

void TestStlSet(const long* numbers,const long size)
{
	std::set<long> items;
	Test<std::set<long> >(items,numbers,size);
}


inline double timedifference(timespec& x, timespec& y)
{	//warning: not fully tested
	timespec result; const double scale(1E+9); 
	if (x.tv_nsec < y.tv_nsec) { double nsec = (y.tv_nsec - x.tv_nsec) / scale + 1; y.tv_nsec -= scale * nsec; y.tv_sec += nsec; }
	if (x.tv_nsec - y.tv_nsec > scale) { double nsec = (x.tv_nsec - y.tv_nsec) / scale; y.tv_nsec += scale * nsec; y.tv_sec -= nsec; }
	result.tv_sec = x.tv_sec - y.tv_sec;
	result.tv_nsec = x.tv_nsec - y.tv_nsec;
	return ((result.tv_sec*scale) + result.tv_nsec);
}

std::pair<double,double> TestBoth(const long* numbers,const long N)
{
	timespec t1,t2;
	double StlTime,TreeObjectsTime;

	clock_gettime(CLOCK_MONOTONIC,&t1);
	TestTreeSet(numbers,N);
	clock_gettime(CLOCK_MONOTONIC,&t2);
	TreeObjectsTime=timedifference(t2,t1);

	clock_gettime(CLOCK_MONOTONIC,&t1);
	TestStlSet(numbers,N);
	clock_gettime(CLOCK_MONOTONIC,&t2);
	StlTime=timedifference(t2,t1);
	std::pair<double,double> ret;
	ret.TREEOBJECTS=TreeObjectsTime;
	ret.STLOBJECTS=StlTime;
	return ret;
}

struct Times : std::vector<std::pair<double,double> >
{
	operator std::pair<double,double> ()
	{
		using namespace std;
		pair<double,double> avg;
		for (iterator it=begin();it!=end();it++)
		{
			avg.first+=it->first;
			avg.second+=it->second;
		}
		avg.first/=size();
		avg.second/=size();
		return avg;
	}
};

int main(int,char**)
{
	using namespace std;
	char hash(0);
	long N(0),T(20);
	fscanf(stdin,"%c%ld",&hash,&N);
	if (hash!='#') {printf("First line of input must be length preceeded by #\n"); return -1;}
	long* numbers=(long*)malloc(sizeof(long)*N); if (!numbers) {printf("Cannot allocate numbers array\n"); return -1;}
	{for (long j=0;j<N;j++) fscanf(stdin,"%ld",&numbers[j]);}
	Times alltimes;
	for (int t=0;t<T;t++) alltimes.push_back(TestBoth(numbers,N));	
	free(numbers);



	Punk punk; cout.imbue(locale(cout.getloc(), &punk));
	std::pair<double,double> Averages(alltimes);


	double much;

	if (Averages.TREEOBJECTS>Averages.STLOBJECTS) 
			much=(100-((Averages.STLOBJECTS/Averages.TREEOBJECTS)*100));
	else
			much=(100-((Averages.TREEOBJECTS/Averages.STLOBJECTS)*100));

	Averages.first/=1e9; Averages.second/=1e9;
	
	cout<<"Testing with "<<N<<" random generated numbers, average times after "<<T<<" iterations:"<<endl;
	cout<<setw(14)<<right<<"STL:"<<setprecision(3)<<setw(10)<<fixed<<Averages.STLOBJECTS<<" seconds"<<endl;
	cout<<setw(14)<<right<<"TreeObjects:"<<setprecision(3)<<setw(10)<<fixed<<Averages.TREEOBJECTS<<" seconds"<<endl;
	string resulttext;
	if (Averages.STLOBJECTS>Averages.TREEOBJECTS) resulttext="The Tree Objects set was ";
	else resulttext="The std::set was ";
	cout<<resulttext<<setprecision(3)<<fixed<<much<<"% faster"<<endl<<endl;
	
	return 0;
}

