
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
	template <> pair<bool,int> TreeCanvas<int>::Next()
	{ 
		if (removing)
		{
			if (used.empty()) return make_pair<bool,int>(false,0);
			int w(rand()%used.size());
			set<int>::iterator it=used.begin();
			for (int j=0;j<w;j++) it++;
			used.erase(it);
			return make_pair<bool,int>(true,*it);
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

	template <> pair<bool,double> TreeCanvas<double>::Next()
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

	template <typename KT>
		void TreeCanvas<KT>::Deletions()
	{
			if (removal)
			{
				Bst<KT,VT>& nk(static_cast<Bst<KT,VT>&>(*removal));
				VT& valuenode(nk);
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
								cout<<"Integrity check failed:";
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
		void TreeCanvas<KT>::UpdateTree()
	{
			if (root) if (!((updateloop)%30)) if (root) traverse(*root);
			updateloop++;

			Deletions();


			if ((!waitfor) or (updateloop>waitfor) )
				if ((!movement) and (!stop))
			{
				movement=true;
				pair<bool,KT> next(Next());
				if (next.first)
				{
					if (!removing)
					{
						TreeNode<KT> tn(ScreenWidth,ScreenHeight);
						//cout<<"creating "<<next.second<<endl;
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
								cout<<"Integrity check failed:";
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



				} 
			}

			if (!root) {movement=false; removing=false; stop=false;}
			if (!flipcounter) flipcounter=((rand()%1000)+100);
			if (!(updateloop%flipcounter))
			{
				flipcounter=((rand()%1000)+100);
				if (root) removing=!removing;
			}


	}

} // TreeDisplay
