
#ifndef TREE_INTEGRITY
#define TREE_INTEGRITY

namespace TreeIntegrity
{

	template<typename KT,typename VT> 
		inline bool BstIntegrity(TreeBase* root,set<KT>& used)
	{
			if (!root) return true;
			Bst<KT,VT>& rk(static_cast<Bst<KT,VT>&>(*root));
			KT maxvalue(rk.maxValue(root));
			KT minvalue(rk.minValue(root));
			bool isbst(root->isBST(root));
			//cout<<"Min:"<<setprecision(2)<<fixed<<minvalue<<" ";
			//cout<<"Max:"<<setprecision(2)<<fixed<<maxvalue<<" ";
			//cout<<"isBST:"<<boolalpha<<isbst;
			//cout.flush();
			//cout<<"Min:"<<*used.begin();
			//cout<<"Max:"<<*used.rbegin();
			if (minvalue!=(*used.begin())) {cout<<("Min check failed")<<endl; return false;}
			if (maxvalue!=(*used.rbegin())) {cout<<("Max check failed")<<endl; return false;}
			if (!isbst) {cout<<("isBST failed")<<endl; return false;}
			long ttl(root->countnodes());
			if (ttl!=used.size()) {cout<<("Wrong number of nodes counted")<<endl; return false;}
			//cout<<" Total:"<<ttl<<" == "<<used.size()<<endl;
			return true;
	}

	namespace RedBlackCheck
	{
		template<typename KT,typename VT>
			struct Visitor
		{
			Visitor(RbTree<KT,VT>& _rk) : rk(_rk),ok(true) {}
			operator bool () 
			{ 
				PostOrder(rk);
				return ok; 
			}
			private:
			RbTree<KT,VT>& rk;
			bool ok;
			void PostOrder(TreeBase& node)
			{
				RbTree<KT,VT>& rb(static_cast<RbTree<KT,VT>&>(node));
				if (node.left) PostOrder(*node.left);
				if (node.right) PostOrder(*node.right);
				Visit(node);
			}
			void clear(VT& data)
			{
				map<string,string>::iterator it;
				it=data.find("lr"); if (it!=data.end()) data.erase(it);
				it=data.find("rb"); if (it!=data.end()) data.erase(it);
			}
			void Visit(TreeBase& node)
			{
				RbTree<KT,VT>& rb(static_cast<RbTree<KT,VT>&>(node));
				const KT& key(rb);
				VT& data(rb); clear(data);
				#if 0
				int diff(abs(lc-rc));
				if (diff>2) 
				{
					stringstream ss;
					ss<<lc<<","<<rc;
					if (rb.color(&rb)==RbTree<KT,VT>::BLACK) data["lr"]=ss.str();
				}
				#endif
				if (rb.color(&node)==RbTree<KT,VT>::RED) 
				{
					bool leftisred(false),rightisred(false);
					if (rb.right) if (rb.color(rb.right)==RbTree<KT,VT>::RED) rightisred=true;
					if (rb.left) if (rb.color(rb.left)==RbTree<KT,VT>::RED) leftisred=true;
					stringstream ss;
					if (leftisred) ss<<"L";
					if (rightisred) ss<<"R";
					if (!ss.str().empty()) data["rb"]=ss.str();
				}
			}
		}; 
	} // RedBlackCheck

	template<typename KT,typename VT> 
		inline bool RedBlackIntegrity(TreeBase* root)
	{
		if (!root) return true;
		//cout<<"Checking Red Black"<<endl;
		RbTree<KT,VT>& rk(static_cast<RbTree<KT,VT>&>(*root));
		TreeBase* leftmost(root->LeftMost());
		TreeBase* rightmost(root->RightMost());
		RbTree<KT,VT>& lm(static_cast<RbTree<KT,VT>&>(*leftmost));
		RbTree<KT,VT>& rm(static_cast<RbTree<KT,VT>&>(*rightmost));
		const KT& L(lm);
		const KT& R(rm);
		RedBlackCheck::Visitor<KT,VT> visitor(rk);
		if (!visitor) {cout<<"Red Black check failed"<<endl; return false;}
		return true;
	}
} //TreeIntegrity
#endif // TREE_INTEGRITY

