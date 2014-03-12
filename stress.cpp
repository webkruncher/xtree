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
#include <iostream.h>
using namespace TreeObjects;
using namespace std;

void Insert(TreeObjects::Set<long>& items,const long number) { items.insert(number);}
void Erase(TreeObjects::Set<long>& items,const long number) { items.erase(number);}

template <typename T>
	void Test(T& items,const long* numbers,const long size) 
{ 
	cerr<<"Inserts:"<<size<<endl; cerr.flush();
	for (long j=0;j<size;j++) Insert(items,numbers[j]);
	cerr<<"Deletes:"<<size<<endl; cerr.flush();
	for (long j=0;j<size/2;j++) Erase(items,numbers[j]);
	cerr<<"Inserts:"<<size<<endl; cerr.flush();
	for (long j=size/2;j>=0;j--) Insert(items,numbers[j]);
	cerr<<"Deletes:"<<size<<endl; cerr.flush();
	for (long j=0;j<size;j++) Erase(items,numbers[j]);
}



int main(int,char**)
{
	TreeObjects::Set<long> items;
	long size(1000000);
	while (true)
	{
		long numbers[size];
		for (long j=0;j<size;j++)
			numbers[j]=rand();
		Test<TreeObjects::Set<long> >(items,numbers,size);
	}
	
	return 0;
}

