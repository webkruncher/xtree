
#ifndef TREE_INTEGRITY
#define TREE_INTEGRITY

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
		//cout<<"Leftmost:"<<L<<", Rightmost:"<<R<<endl;

		struct Visitor
		{
			Visitor(RbTree<KT,VT>& _rk) : rk(_rk),ok(true) {}
			operator bool () { InOrder(rk); return ok; }
			private:
			RbTree<KT,VT>& rk;
			bool ok;
			void InOrder(TreeBase& node)
			{
				if (node.left) InOrder(*node.left);
				Visit(node);
				if (node.right) InOrder(*node.right);
			}
			void Visit(TreeBase& node)
			{
				RbTree<KT,VT>& rb(static_cast<RbTree<KT,VT>&>(node));
				const KT& key(rb);
				VT& data(rb);
				data.clear();
				int ld(0),rd(0);
				if (node.left) BlackNodeCounter(node.left,ld);
				if (node.right) BlackNodeCounter(node.right,rd);
				int diff(abs(ld-rd));
				if (diff>2) 
				{
					cout<<"l:"<<ld<<" r:"<<rd<<endl; cout.flush();
					stringstream ss;
					ss<<ld<<","<<rd;
					data["Balance"]=ss.str();
				}
				if (rb.color(&node)==RbTree<KT,VT>::RED) 
				{
					if (rb.right) if (rb.color(rb.right)==RbTree<KT,VT>::RED) data["Right"]="Red";
					if (rb.left) if (rb.color(rb.left)==RbTree<KT,VT>::RED) data["Left"]="Red";
				}
			}
			void BlackNodeCounter(TreeBase* n,int& d)
			{
				if (!n) return;
				RbTree<KT,VT>& rb(static_cast<RbTree<KT,VT>&>(*n));
				if (rb.color(n)==RbTree<KT,VT>::BLACK) d++;
				if (n->right) BlackNodeCounter(n->right,d);
				if (n->left) BlackNodeCounter(n->left,d);
				if (!n->left) d++; if (!n->right) d++;
			}
		} visitor(rk);

		if (!visitor) {cout<<"Red Black check failed"<<endl; return false;}
		return true;
	}

#endif // TREE_INTEGRITY

