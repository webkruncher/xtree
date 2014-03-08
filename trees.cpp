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
#include "x11grid.h"
using namespace X11Methods;
#include <treemap.h>
using namespace TreeObjects;
#include <trees.h>
using namespace TreeDisplay;
#include <math.h>

#define MOST 100

namespace TreeObjects
{
	template <>
		void Bst<double,TreeNode<double> >::Update(Trunk* node,Trunk* pnode,bool erasing)
	{
			typedef double KT;
			typedef TreeNode<KT> VT ;
			if (!node) return; if (!pnode) return;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.Data()(nd.key,(*pnode),pnode->Parent(),erasing);
	}

	template <>
		void Bst<int,TreeNode<int> >::Update(Trunk* node,Trunk* pnode,bool erasing)
	{
			typedef int KT;
			typedef TreeNode<KT> VT ;
			if (!node) return; if (!pnode) return;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.data(nd.key,(*pnode),pnode->Parent(),erasing);
	}

	template <>
		void Bst<string,TreeNode<string> >::Update(Trunk* node,Trunk* pnode,bool erasing)
	{
			typedef string KT;
			typedef TreeNode<KT> VT ;
			if (!node) return; if (!pnode) return;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.Data()(nd.key,(*pnode),pnode->Parent(),erasing);
	}


	template <> Trunk* RbMap<int,TreeNode<int> >::red(Trunk* n)
	{
		if (!n) return n; 
		RbMap<int,TreeNode<int> >& nd(static_cast<RbMap<int,TreeNode<int> >&>(*n)); 
		nd.clr=RED;
		nd.data(0X800000); 
		return n;
	}

	template <> Trunk* RbMap<int,TreeNode<int> >::black(Trunk* n)
	{
		if (!n) return n; 
		RbMap<int,TreeNode<int> >& nd(static_cast<RbMap<int,TreeNode<int> >&>(*n)); 
		nd.clr=BLACK;
		nd.data(0X00000); 
		return n;
	}

	template <> Trunk* RbMap<double,TreeNode<double> >::red(Trunk* n)
	{
		if (!n) return n; 
		RbMap<double,TreeNode<double> >& nd(static_cast<RbMap<double,TreeNode<double> >&>(*n)); 
		nd.clr=RED;
		nd.data(0X800000); 
		return n;
	}

	template <> Trunk* RbMap<double,TreeNode<double> >::black(Trunk* n)
	{
		if (!n) return n; 
		RbMap<double,TreeNode<double> >& nd(static_cast<RbMap<double,TreeNode<double> >&>(*n)); 
		nd.clr=BLACK;
		nd.data(0X00000); 
		return n;
	}

	template <> Trunk* RbMap<string,TreeNode<string> >::red(Trunk* n)
	{
		if (!n) return n; 
		RbMap<string,TreeNode<string> >& nd(static_cast<RbMap<string,TreeNode<string> >&>(*n)); 
		nd.clr=RED;
		nd.data(0X800000); 
		return n;
	}

	template <> Trunk* RbMap<string,TreeNode<string> >::black(Trunk* n)
	{
		if (!n) return n; 
		RbMap<string,TreeNode<string> >& nd(static_cast<RbMap<string,TreeNode<string> >&>(*n)); 
		nd.clr=BLACK;
		nd.data(0X00000); 
		return n;
	}
}

namespace TreeDisplay
{
	int GenerateNumber(int Max) { return ((rand()%Max)+(Max/2)); }
	double GenerateNumber(double Max) { double k=(rand()%((int)Max)); k/=(((double)(rand()%((int)Max))+1)); return k; }

	struct Strings : vector<string>
	{
		Strings()
		{
			Strings& m(*this);
			if (true)
			{
				m("Marco"); m("Jack"); m("Fred"); m("Joe"); m("Nat"); m("Jim"); m("Hop"); 
				m("Pop"); m("Sally"); m("Fox"); m("Sox"); m("Lorax"); m("Sam"); m("Joe");
				m("Sue"); m("Dave"); m("Charlie"); m("Diane"); m("Angie"); m("John"); m("Frank");
				m("Rick"); m("Dick"); m("Jane"); m("Jill"); m("Bruno"); m("Tina"); m("Ike");
				m("Michelle"); m("Vallerie"); m("Roxanne"); m("Jessie"); m("Pete"); m("Suzie"); m("Bill");
				m("Mary"); m("Joan"); m("Pat"); m("Karen"); m("Martin"); m("Ted"); m("Allie");
			} else {
				m("widget1");
				m("widget2");
				m("widget3");
			}
			//while (size()<MOST) {stringstream ss; ss<<"s"<<size(); m(ss.str()); }
		}
		private: void operator()(string s){push_back(s);}
	} GlobalStrings;

	pair<bool,string> NextString(set<string>& used,const int Max,const bool removing)
	{
		if (removing)
		{
			if (used.empty()) return make_pair<bool,string>(false,"");
			int w(rand()%used.size());
			set<string>::iterator it=used.begin();
			for (int j=0;j<w;j++) it++;
			used.erase(it);
			return make_pair<bool,string>(true,*it);
		}

		if (used.size()==(Max-1)) return make_pair<bool,string>(false,"");
		srand(time(0));
		string k; do 
		{ 
				int j(rand()%GlobalStrings.size());
				k=GlobalStrings[j];
		} while (used.find(k)!=used.end());
		used.insert(k);
		return make_pair<bool,string>(true,k);
	}	

	template <typename T>
		pair<bool,T> NextItem(set<T>& used,const int Max,const bool removing)
	{
		if (removing)
		{
			if (used.empty()) return make_pair<bool,T>(false,0);
			int w(rand()%used.size());
			typename set<T>::iterator it=used.begin();
			for (int j=0;j<w;j++) it++;
			used.erase(it);
			return make_pair<bool,T>(true,*it);
		}

		if (used.size()==(Max-1)) return make_pair<bool,T>(false,0);
		srand(time(0));
		T k; do { k=GenerateNumber((T)Max); } while (used.find(k)!=used.end());
		used.insert(k);
		return make_pair<bool,T>(true,k);
	}

#if 0
	template <> pair<bool,int> TreeCanvas<int>::Next(int Max) 
	{
		static int n(0);
		int next(n);
		n++;
		used.insert(next);
		return make_pair<bool,int>(true,next);
	}
#else
	template <> pair<bool,int> TreeCanvas<int>::Next(int Max) { return NextItem<int>(used,Max,removing); }
#endif
	template <> pair<bool,double> TreeCanvas<double>::Next(int Max) { return NextItem<double>(used,Max,removing); }
	template <> pair<bool,string> TreeCanvas<string>::Next(int Max) { return NextString(used,Max,removing); }

	template <typename KT>
		void TreeCanvas<KT>::Deletions()
	{
			if (removal)
			{
				Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*removal));
				VT& valuenode(nk.Data());
				if (!valuenode.undisplay()) return;
				Trunk* newroot(nk.erase(root,removal));
				removal=NULL;

				//if (newroot==root) cout<<"Root node is unchanged"<<endl; cout.flush();
				if (!newroot) ;//cout<<"The tree is now empty, the root is NULL"<<endl; 
					else if (newroot!=root)
					{
						//cout<<"newroot"<<endl; cout.flush();
						Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*newroot));
						const KT& rootkey(nk);
						//cout<<"The new root node is "<<rootkey<<endl;
					}

				if (newroot!=root) 
				{
					//cout<<"Root node is different"<<endl;
					root=newroot;
				}
				if (!root)	
				{
					//cout<<"clear"<<endl; cout.flush();
					movement=false; removing=false; used.clear();stop=false;
				}
				//cout<<"counting nodes"<<endl; cout.flush();
				//if (root) cout<<"The tree now has "<<root->countnodes()<<" nodes"<<endl; cout.flush();

				//if (root) cout<<"Integrity check..."<<endl; cout.flush();
				if (root) 
					if (!CheckIntegrity(root)) 
				{
					if (used.size()<40)
					{
						cout<<"Integrity check failed:"<<endl;
						cout<<"Used: ";
						for (typename set<KT>::iterator it=used.begin();it!=used.end();it++) 
							cout<<" "<<(*it);	
						cout<<endl<<" Bst:";
						TreeIntegrity::PrintInOrder<KT,VT>(root);
						cout<<endl;
						cout.flush();
					}
					stop=true;
				}

#if 0
				if (root)
					if (used.size()<40)
					{
						cout<<"Removed."<<endl;
						cout<<"Used:";
						for (typename set<KT>::iterator it=used.begin();it!=used.end();it++) 
							cout<<(*it)<<" ";	
						cout<<endl<<" Bst:";
						TreeIntegrity::PrintInOrder<KT,VT>(root);
						cout<<endl;
						cout.flush();
					}
				//waitfor=updateloop+10;
#endif
			}
	}

	template <typename KT>
		void TreeCanvas<KT>::Additions()
	{
			if ((!waitfor) or (updateloop>waitfor) )
				if ((!movement) and (!stop))
			{
				waitfor=0;
				movement=true;
				pair<bool,KT> next(Next(MOST));
				if (next.first)
				{
					if (!removing)
					{
						TreeNode<KT> tn(ScreenWidth,ScreenHeight);
						Trunk* n(generate(next.second,tn));
						if (!root) waitfor=updateloop+10;
						if (!root) root=n;  
							else 
						{
							Trunk* nr(root->insert(root,n));
							if (nr) 
							{
								//if (root!=nr) cout<<"The root node rotated"<<endl;
								root=nr;
							} else cout<<next.second<<" is a duplicate and was deleted"<<endl;
						}
					} else {
						if (root) 
						{
							Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*root));
							//cout<<"Erase:"<<next.second<<endl;
							removal=nk.find(next.second);
						}
					}
						if (!removal)
							if (!CheckIntegrity(root)) 
						{
							if (used.size()<20)
							{
								cout<<"Integrity check failed:"<<endl;
								cout<<"Used:";
								for (typename set<KT>::iterator it=used.begin();it!=used.end();it++) 
									cout<<(*it)<<" ";	
								cout<<endl<<" Bst:";
								TreeIntegrity::PrintInOrder<KT,VT>(root);
								cout<<endl;
								cout.flush();
							}
							stop=true;
						}
				} 
			}
	}


	template <> void TreeCanvas<string>::UpdateTree()
	{
			//if (root) if (!((updateloop)%MOST)) 
			//if (root) traverse(*root);
			if (!(updateloop%20))if (root) traverse(*root);
			updateloop++;

			Deletions();
			Additions();

			if (!root) {movement=false; removing=false; stop=false;}
			if (!flipcounter) flipcounter=((rand()%1000)+100);
			if (!(updateloop%flipcounter))
			{
				flipcounter=((rand()%100)+100);
				if (root) removing=!removing;
			}
	}

	template <typename KT>
		void TreeCanvas<KT>::UpdateTree()
	{
			//if (root) if (!((updateloop)%MOST)) 
			if (!(updateloop%20))if (root) traverse(*root);
			updateloop++;

			Deletions();
			Additions();

			if (!root) {movement=false; removing=false; stop=false;}
			if (!flipcounter) flipcounter=((rand()%1000)+100);
			if (!(updateloop%flipcounter))
			{
				flipcounter=((rand()%1000)+100);
				if (root) removing=!removing;
			}
	}

} // TreeDisplay

