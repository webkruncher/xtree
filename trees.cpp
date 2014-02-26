
#include "x11grid.h"
using namespace X11Methods;
#include <tree.h>
using namespace TreeObjects;
#include <trees.h>
using namespace TreeDisplay;
#include <math.h>

namespace TreeObjects
{
	template <>
		void Bst<double,TreeNode<double> >::Update(TreeBase* node,TreeBase* pnode,bool erasing)
	{
			typedef double KT;
			typedef TreeNode<KT> VT ;
			if (!node) return; if (!pnode) return;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.Data()(nd.key,(*pnode),pnode->parent,erasing);
	}

	template <>
		void Bst<int,TreeNode<int> >::Update(TreeBase* node,TreeBase* pnode,bool erasing)
	{
			typedef int KT;
			typedef TreeNode<KT> VT ;
			if (!node) return; if (!pnode) return;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.data(nd.key,(*pnode),pnode->parent,erasing);
	}

	template <>
		void Bst<string,TreeNode<string> >::Update(TreeBase* node,TreeBase* pnode,bool erasing)
	{
			typedef string KT;
			typedef TreeNode<KT> VT ;
			if (!node) return; if (!pnode) return;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.Data()(nd.key,(*pnode),pnode->parent,erasing);
	}


	template <> TreeBase* RbTree<int,TreeNode<int> >::red(TreeBase* n)
	{
		if (!n) return n; 
		RbTree<int,TreeNode<int> >& nd(static_cast<RbTree<int,TreeNode<int> >&>(*n)); 
		nd.clr=RED;
		nd.data(0X800000); 
		return n;
	}

	template <> TreeBase* RbTree<int,TreeNode<int> >::black(TreeBase* n)
	{
		if (!n) return n; 
		RbTree<int,TreeNode<int> >& nd(static_cast<RbTree<int,TreeNode<int> >&>(*n)); 
		nd.clr=BLACK;
		nd.data(0X00000); 
		return n;
	}

	template <> TreeBase* RbTree<double,TreeNode<double> >::red(TreeBase* n)
	{
		if (!n) return n; 
		RbTree<double,TreeNode<double> >& nd(static_cast<RbTree<double,TreeNode<double> >&>(*n)); 
		nd.clr=RED;
		nd.data(0X800000); 
		return n;
	}

	template <> TreeBase* RbTree<double,TreeNode<double> >::black(TreeBase* n)
	{
		if (!n) return n; 
		RbTree<double,TreeNode<double> >& nd(static_cast<RbTree<double,TreeNode<double> >&>(*n)); 
		nd.clr=BLACK;
		nd.data(0X00000); 
		return n;
	}

	template <> TreeBase* RbTree<string,TreeNode<string> >::red(TreeBase* n)
	{
		if (!n) return n; 
		RbTree<string,TreeNode<string> >& nd(static_cast<RbTree<string,TreeNode<string> >&>(*n)); 
		nd.clr=RED;
		nd.data(0X800000); 
		return n;
	}

	template <> TreeBase* RbTree<string,TreeNode<string> >::black(TreeBase* n)
	{
		if (!n) return n; 
		RbTree<string,TreeNode<string> >& nd(static_cast<RbTree<string,TreeNode<string> >&>(*n)); 
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
			m("Marco"); m("Jack"); m("Fred"); m("Joe"); m("Nat"); m("Jim"); m("Hop"); 
			m("Pop"); m("Sally"); m("Fox"); m("Sox"); m("Lorax"); m("Sam"); m("Joe");
			m("Sue"); m("Dave"); m("Charlie"); m("Diane"); m("Angie"); m("John"); m("Frank");
			m("Rick"); m("Dick"); m("Jane"); m("Jill"); m("Bruno"); m("Tina"); m("Ike");
			m("Michelle"); m("Vallerie"); m("Roxanne"); m("Jessie"); m("Pete"); m("Suzie"); m("Bill");
			m("Mary"); m("Joan"); m("Pat"); m("Karen"); m("Martin"); m("Ted"); m("Allie");
			//while (size()<50) {stringstream ss; ss<<"s"<<size(); m(ss.str()); }
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
				VT& valuenode(nk.Data());
				if (!valuenode.undisplay()) return;
				TreeBase* newroot(nk.erase(root,removal));
				removal=NULL;

				if (newroot==root) cout<<"Root node is unchanged"<<endl; cout.flush();
				if (!newroot) cout<<"The tree is now empty, the root is NULL"<<endl; 
					else if (newroot!=root)
					{
						cout<<"newroot"<<endl; cout.flush();
						Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*newroot));
						const KT& rootkey(nk);
						cout<<"The new root node is "<<rootkey<<endl;
					}

				if (newroot!=root) 
				{
					cout<<"Root node is different"<<endl;
					root=newroot;
				}
				if (!root)	
				{
					cout<<"clear"<<endl; cout.flush();
					movement=false; removing=false; used.clear();stop=false;
				}
				cout<<"counting nodes"<<endl; cout.flush();
				if (root) cout<<"The tree now has "<<root->countnodes()<<" nodes"<<endl; cout.flush();


				if (root) cout<<"Integrity check..."<<endl; cout.flush();
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
				waitfor=updateloop+10;
			}
	}

	template <typename KT>
		void TreeCanvas<KT>::Additions()
	{
			if ((!waitfor) or (updateloop>waitfor) )
				if ((!movement) and (!stop))
			{
				movement=true;
				pair<bool,KT> next(Next(40));
				if (next.first)
				{
					if (!removing)
					{
						TreeNode<KT> tn(ScreenWidth,ScreenHeight);
						TreeBase* n(generate(next.second,tn));
						if (!root) waitfor=updateloop+10;
						if (!root) root=n;  
							else 
						{
							TreeBase* nr(root->insert(root,n));
							if (nr) 
							{
								if (root!=nr) cout<<"The root node rotated"<<endl;
								root=nr;
							} else cout<<next.second<<" is a duplicate and was deleted"<<endl;
						}
					} else {
						if (root) 
						{
							Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*root));
							cout<<"Erase:"<<next.second<<endl;
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
			if (root) if (!((updateloop)%30)) if (root) traverse(*root);
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
			if (root) if (!((updateloop)%30)) if (root) traverse(*root);
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

