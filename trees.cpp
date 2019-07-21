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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MOST 500

namespace TreeDisplay
{
	void Trap::Load()
	{
		if (loaded) return;
		loaded=true;
		if (fname.empty()) return;
		struct stat sb;
		if (stat(fname.c_str(),&sb)) return;
		ifstream in(fname.c_str());
		if (in.fail()) return;
		while (!in.eof()) { string line; getline(in,line); if (!line.empty()) Read(line); }
	}

	template <>
		void Trapper<string>::Read(string line)
	{
		if (line.size()<2) return;
		if (line[0]=='i') { line.erase(0,2); InsertBreakPoints.insert(line); }
		if (line[0]=='e') { line.erase(0,2); EraseBreakPoints.insert(line); }
	}

	template <>
		void Trapper<string>::trap(const CurrentActions action,const string& key)
	{
		Load();
		if (state==Next) return;
		if (action==Inserting)	if (InsertBreakPoints.find(key)!=InsertBreakPoints.end()) state=Stepping;
		if (action==Erasing)		if (EraseBreakPoints.find(key)!=EraseBreakPoints.end()) state=Stepping;
	}

	void Trap::stepper(string msg)
	{
		if (state==Running) return;
		if (state==Continuing) return;
		if ((state==Next) and (msg!="Begin") and (!msg.empty()) ) return;
		if ((state==Next) and (stop)) 
		{
			if (!movement) { stop=false;}
			else {tout<<"-"; tout.flush(); return;}
		}
		if (msg.empty()) { return; }
		tout<<msg<<endl; 
		Describe(); tout<<">";
		string check;
		getline(cin,check); if (check=="") return;
		cmd=check;
		if ((cmd=="r") or (cmd=="run")) state=Running;
		if ((cmd=="n") or (cmd=="next")) {state=Next; stop=true;}
		if ((cmd=="s") or (cmd=="step")) state=Stepping;
		if ((cmd=="c") or (cmd=="continue")) 
		{
			Clear();
			state=Continuing;
		}
	}

	const string& TreeSource::operator()(int line)
	{
		if (empty()) Load();
		ndx=-1;
		sourcecode="";
		if (line>size()) return blank;
		txt=(*this)[line];
		size_t ds(txt.find("//"));
		if (ds==string::npos) {sourcecode=txt; return txt;}
		ds+=2;
		string comment; comment.assign(txt,ds,txt.size()-ds);
		ds-=2;
		txt.erase(ds,txt.size()-ds);
		size_t eol(txt.find_last_not_of(" \n"));
		if (eol!=string::npos) txt.erase(eol,txt.size()-eol);
		size_t son(comment.find_first_of("0123456789"));
		if ( (son!=string::npos) and (son!=0) ) comment.erase(0,son);
		size_t eon(comment.find_first_not_of("0123456789"));
		if (eon!=string::npos) comment.erase(eon,comment.size()-eon);
		ndx=atoi((char*)comment.c_str());
		sourcecode=txt;
		stringstream ssc; ssc<<"("<<comment<<") ";
		txt.insert(0,ssc.str().c_str());
		return txt;
	}

	string deblank(string lines)
	{
		stringstream ss;
		string ret;
		ret=lines;
		while (true)
		{
			size_t dbl(ret.find("\n\n"));
			if (dbl==string::npos) break;
			ret.erase(dbl,1);
		}
		return ret;
	}
} // TreeDisplay

#define BREAKPOINT asm ("int $0X03") ;
namespace TreeObjects
{
	template <>
		void Bst<double,TreeNode<double> >::Update(Trunk* node,Trunk* pnode,bool erasing)
	{
			typedef double KT;
			typedef TreeNode<KT> VT ;
			if (!node) return; if (!pnode) return;
			if (node->isnil()) return; //if (pnode->isnil()) return;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.Data()(nd.key,(*pnode),pnode->Parent(),erasing);
	}

	template <>
		void Bst<int,TreeNode<int> >::Update(Trunk* node,Trunk* pnode,bool erasing)
	{
			typedef int KT;
			typedef TreeNode<KT> VT ;
			if (!node) return; if (!pnode) return;
			if (node->isnil()) return; //if (pnode->isnil()) return;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.data(nd.key,(*pnode),pnode->Parent(),erasing);
	}

	template <>
		void Bst<string,TreeNode<string> >::Update(Trunk* node,Trunk* pnode,bool erasing)
	{
			typedef string KT;
			typedef TreeNode<KT> VT ;
			if (!node) return; if (!pnode) return;
			if (node->isnil()) return; //if (pnode->isnil()) return;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.Data()(nd.key,(*pnode),pnode->Parent(),erasing);
	}

	template <> void RbMap<int,TreeNode<int> >::DisplayColor(const unsigned long color) { Data()(color); }
	template <> void RbMap<double,TreeNode<double> >::DisplayColor(const unsigned long color) { Data()(color); }
	template <> void RbMap<string,TreeNode<string> >::DisplayColor(const unsigned long color) { Data()(color); }

}

namespace TreeDisplay
{
	int last(0);
	int GenerateNumber(int Max)
	{
		//return (last++);
		return ((rand()%Max)+(Max/2)); 
	}
	double GenerateNumber(double Max) { double k=(rand()%((int)Max)); k/=(((double)(rand()%((int)Max))+1)); return k; }

	struct Strings : vector<string>
	{
		Strings()
		{
			Strings& m(*this);
			ifstream names("names.txt");
			if (names.fail()) return;
			while (!names.eof())
			{
				string name; getline(names,name);
				if (name.empty()) continue;
				transform(name.begin(), name.end(), name.begin(), ::tolower);
				name[0]=::toupper(name[0]);	
				m(name);
			}
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

	template <> pair<bool,int> TreeCanvas<int>::Next(int Max) { return NextItem<int>(used,Max,removing); }
	template <> pair<bool,double> TreeCanvas<double>::Next(int Max) { return NextItem<double>(used,Max,removing); }
	template <> pair<bool,string> TreeCanvas<string>::Next(int Max) { return NextString(used,Max,removing); }

	template <typename KT>
		void TreeCanvas<KT>::Deletions()
	{
			if (removal)
			{
				Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*removal));
				const KT& key(nk);
				VT& valuenode(nk.Data());
				if (!valuenode.undisplay()) return;
				if (journal==ios_base::out) entry-=key;
				Bst<KT,VT>& rr(static_cast<Bst<KT,VT>&>(*root));
				Trunk* newroot(rr.erase(root,removal));
				removal=NULL;
				if (journal==ios_base::in)  removing=false;
				if (newroot!=root) 
				{
					if (newroot and !newroot->isnil())
					{
						root=newroot;
						if (root) 
						{
							Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*newroot));
							const KT& newkey(nk);
							root->SetParent(this);
						}
					} else {
						root=NULL;
						tout<<"The tree is now empty, clearing journal entries."<<endl;
						if (journal==ios_base::out) entry.clear();
					}
				}
				if ((!root)	or (root->isnil())) { movement=false; removing=false; used.clear();stop=false; }
			} 
	}

	template <typename KT>
		void TreeCanvas<KT>::Additions()
	{
			ajaxface<KT>& treexml(*this);;
			if (removal) return;
			if ( (journal==ios_base::in)  and (removing) ) return;
			if ((!waitfor) or (updateloop>waitfor) )
				if ((!movement) and (!stop))
			{
				waitfor=0;
				movement=true;

				bool ReadingJournal(false);

				if (this->GetState()!=Trap::Stepping)
					if (journal==ios_base::in)  
				{
					ReadingJournal=entry;
					if (!ReadingJournal) 
					{
						tout<<"Journal is finished"<<endl;
						CheckIntegrity(root);
						if (this->GetState()==Trap::Continuing) 
						{
							stop=false; 
							journal.close(); 
							xmloutname.clear(); 
							return;
						}
						this->SetState(Trap::Stepping);
						if (!xmloutname.empty())
						{
							ofstream xmlout((char*)xmloutname.c_str());
							xmlout<<treexml;
						}
						return; // Journal is finished
					}
				}

				pair<bool,KT> next((ReadingJournal)?entry:Next(MOST));
				if (ReadingJournal)
				{
					if (next.first) tout<<"Add:"<<next.second<<endl; else tout<<"Delete:"<<next.second<<endl;
					if (!next.first) {removing=true; next.first=true;}
					if (removing) 
					{
						typename set<KT>::iterator it(used.find(next.second));
						if (it!=used.end()) used.erase(it);
					} else used.insert(next.second);
				} else if (!next.first) removing=true;

				if (next.first)
				{
					if (!removing)
					{
						TreeNode<KT> tn(ScreenWidth,ScreenHeight);
						if (journal==ios_base::out) entry+=next.second;
						Trunk* n(generate(next.second,tn));
						if ((!root) or (root->isnil()) ) waitfor=updateloop+10;
						else waitfor=updateloop+20;


						waitfor=updateloop+1;

						if ((!root)or (root->isnil()) )  
						{
							root=n;  
							if (root) root->SetParent(this);
						}	else  {
							Trunk* nr(root->insert(root,n));
							if (nr) 
							{
								if (nr!=root)
								{
									Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*nr));
									const KT& newkey(nk);
									root=nr;
									if (root) root->SetParent(this);
								}
							}
						}
					} else {
						if ((root) and (!root->isnil()))
						{
							Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*root));
							removal=nk.find(next.second);
						}
					}
				}
			} 
	}

	template <typename KT>
		void TreeCanvas<KT>::UpdateTree()
	{
			ajaxface<KT>& treexml(*this);;
			this->stepper("");
			if ( false ) 
			{
				if (!(updateloop%20))if ((root) and (!root->isnil())) traverse(*root);
			} else {
				if ((root) and (!root->isnil())) traverse(*root);
			}
			updateloop++;
			if ( (entrylimit) and (journal==ios_base::out) )
			{
					if (entry.NItems()>entrylimit) clearing=true;
					if (clearing)
					{
						if (used.empty()) {entry.clear(); clearing=false;}
							else
						{
							movement=false;
							removing=true;
						}
					}
			}

			if (!ignorestop) 
			{
				if (stop) return;
						if (!removal) 
							if (journal!=ios_base::in)  
								if (!CheckIntegrity(root)) 
						{
							stop=true; 
							if (journal==ios_base::out) {journal<<entry; journal.close();}
							if (!xmloutname.empty())
							{
								ofstream xmlout((char*)xmloutname.c_str());
								xmlout<<treexml;
							}
							return;
						}
			}

			Deletions();
			Additions();



			//if (removal) return;
			if (journal==ios_base::in) return;
			if (clearing) return;

			if ( (!root) or (root->isnil()) ){movement=false; removing=false; stop=false;}
			if (!flipcounter) flipcounter=((rand()%1000)+100);
#if 0
			if (!(updateloop%flipcounter))
			{
				flipcounter=((rand()%1000)+100);
				if ( (root) or (!root->isnil()) ) removing=!removing;
			}
#endif
	}

	template<> const string ajaxface<string>::tokt(const string what) const { return what; }
	template<> const int ajaxface<int>::tokt(const string what) const { if (what.empty()) return 0; return atoi(what.c_str()); }
	template<> const double ajaxface<double>::tokt(const string what) const { if (what.empty()) return 0; return atof(what.c_str()); }

	template<> const string ajaxface<string>::tostr(const string what) const { return what; }
	template<> const string ajaxface<int>::tostr(const int what) const { stringstream ss; ss<<what; string ret; ret=ss.str().c_str(); return ret;}
	template<> const string ajaxface<double>::tostr(const double what) const { stringstream ss; ss<<what; string ret; ret=ss.str().c_str(); return ret;}

	template <typename KT>
		const bool TreeCanvas<KT>::IsRoot(const KT what) const 
	{
		if (!root) return true;
		if (root->isnil()) throw string("The root node is nil");
		BstBase<KT>* rk(dynamic_cast<BstBase<KT>*>(root));
		if (!rk) throw string("This root is not the right type");
		BstBase<KT>& nk(*rk);
		const KT nkey(nk);
		cout<<"Looking in "<<nkey<<" for "<<what<<endl;
		if (nkey==what) return nk.isnil();
		BstBase<KT>* found(nk.find(what));
		if (!found) cout<<"Error: Could not find "<<what<<";"<<endl;
		if (!found) return true;
		BstBase<KT>& fnk(static_cast<BstBase<KT>&>(*found));
		const KT fkey(fnk);
		const bool isr(nkey==fkey);
		//cout<<what<<" ?isroot:"<<boolalpha<<isr<<endl;
		return isr;
	}

	template <typename KT>
		const KT TreeCanvas<KT>::GetRoot() const 
	{
		if (!root) throw string("No root");
		if (root->isnil()) throw string("The root node is nil");
		BstBase<KT>* rk(dynamic_cast<BstBase<KT>*>(root));
		if (!rk) throw string("This root is not the right type");
		BstBase<KT>& nk(*rk);
		const KT nkey(nk);
		return nkey;
	}

	template <typename KT>
		const bool TreeCanvas<KT>::HasLeft(const KT what) const 
	{
		if (!root) return false;
		BstBase<KT>& nk(static_cast<BstBase<KT>&>(*root));
		BstBase<KT>* found(nk.find(what));
		if (!found) return false;
		BstBase<KT>* l(static_cast<BstBase<KT>*>(found->Left()));
		if (root->isnul(l)) return false;
		return true;
	}

	template <typename KT>
		const bool TreeCanvas<KT>::HasRight(const KT what) const 
	{
		if (!root) return false;
		BstBase<KT>& nk(static_cast<BstBase<KT>&>(*root));
		BstBase<KT>* found(nk.find(what));
		if (!found) return false;
		BstBase<KT>* r(static_cast<BstBase<KT>*>(found->Right()));
		if (root->isnul(r)) return false;
		return true;
	}

	template <typename KT>
		const KT TreeCanvas<KT>::ParentOf(const KT what) const
		{
			if (!root) throw string("No root");
			BstBase<KT>& nk(static_cast<BstBase<KT>&>(*root));
			BstBase<KT>* found(nk.find(what));
			if (!found) throw string("Key not found");
			BstBase<KT>* parent(static_cast<BstBase<KT>*>(found->Parent()));
			if (root->isnul(parent)) return what; //throw string("Parent is null");
			const KT pkey(*parent);
			return pkey;
		}

	template <typename KT>
		const int TreeCanvas<KT>::DepthOf(const KT what) const
		{
			if (!root) throw string("No root");
			BstBase<KT>& nk(static_cast<BstBase<KT>&>(*root));
			Trunk* found(nk.find(what));
			if (root->isnul(found)) return -1;
			int ret(0);
			while (!(root->isnul(found)) and (root!=found) )
			{
				if (ret>20) return -99999;
				if (root->isnul(found)) return ret;
				found=found->Parent();
				if (!found) return ret;
				ret++;
			}
			return ret;
		}

	template <typename KT>
		const string TreeCanvas<KT>::ColorOf(const KT what) const
		{
			if (!root) throw string("No root");
			// USING RTTI
			BstBase<KT>* rk(dynamic_cast<BstBase<KT>*>(root));
			if (!rk) throw string("This root is not the right type");
			BstBase<KT>& nk(*rk);
			BstBase<KT>* found(nk.find(what));
			if (!found) return "white";
			// USING RTTI
			Bst<KT,VT>* bst(dynamic_cast<Bst<KT,VT>*>(found));
			if (!bst) throw string("This is not a map node");
			const unsigned long c(bst->Data().GetColor());
			if (c&0XFF0000) return "red"; else return "black";
		}

	template <typename KT>
		const KT TreeCanvas<KT>::LeftOf(const KT what)  const
		{
			if (!root) throw string("No root");
			BstBase<KT>& nk(static_cast<BstBase<KT>&>(*root));
			BstBase<KT>* found(nk.find(what));
			if (!found) throw string("Node not found");
			BstBase<KT>* l(static_cast<BstBase<KT>*>(found->Left()));
			if (root->isnul(l)) throw string("Node is nul");
			return (*l);
		}
		
	template <typename KT>
		const KT TreeCanvas<KT>::RightOf(const KT what)  const
		{
			if (!root) throw string("No root");
			BstBase<KT>& nk(static_cast<BstBase<KT>&>(*root));
			BstBase<KT>* found(nk.find(what));
			if (!found) throw string("Node not found");
			BstBase<KT>* r(static_cast<BstBase<KT>*>(found->Right()));
			if (root->isnul(r)) throw string("Node is nul");
			return (*r);
		}

} // TreeDisplay


