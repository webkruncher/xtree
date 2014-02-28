#include <treeset.h>

#include <cstdio>
#include <memory>
#include <set>
#include <sys/time.h>

void TestTreeSet(const long* numbers,const long size)
{
	TreeObjects::Set<long> items;
	for (long j=0;j<size;j++) items.insert(numbers[j]);
	for (long j=0;j<size;j++) items.erase(numbers[j]);
}

void TestStlSet(const long* numbers,const long size)
{
	std::set<long> items;
	for (long j=0;j<size;j++) items.insert(numbers[j]);
	for (long j=0;j<size;j++) items.erase(numbers[j]);
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

int main(int,char**)
{
	char hash(0);
	long N(0);
	fscanf(stdin,"%c%ld",&hash,&N);
	if (hash!='#') {printf("First line of input must be length preceeded by #\n"); return -1;}
	printf("Input size:%ld\n",N);
	long* numbers=(long*)malloc(sizeof(long)*N); if (!numbers) {printf("Cannot allocate numbers array\n"); return -1;}
	{for (long j=0;j<N;j++) fscanf(stdin,"%ld",&numbers[j]);}


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

	free(numbers);


	printf("STL:  %0.02f seconds\n",StlTime/1e9);
	printf("Tree: %0.02f seconds\n",TreeObjectsTime/1e9);
	return 0;
}

