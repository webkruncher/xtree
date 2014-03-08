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
using namespace TreeObjects;
#include <iostream>
using namespace std;

int main(int,char**)
{
	Set<int> nums;
	for (int j=0;j<=100;j+=10) nums.insert(j);
	BstBase<int>* root(nums);
	if (!root) {cout<<"No root"<<endl; return -1;}
	BstBase<int>* item(root->find(30));
	Iterator<int> it(item);
	while (it++)
	{
		const int& n(it);
		cout<<n<<endl;
	}
	BstBase<int>& righty(it);
	const int& onehundred(righty);
	if (onehundred!=100) cout<<"Right Failed"<<endl;
	while (it--)
	{
		const int& n(it);
		cout<<n<<endl;
	}
	BstBase<int>& lefty(it);
	const int& zero(lefty);
	if (zero!=0) cout<<"Left Failed"<<endl;
	return 0;
}


