#ifndef DIGITAL_ARBORIST_SETS_H
#define DIGITAL_ARBORIST_SETS_H
#include <tree.h>

namespace TreeObjects
{
	// Wip - designed to resemble an stl map
	template <typename KT>
		struct Set
	{
		Set() : root(NULL) {}
		virtual ~Set() {if (root) delete root;}
		void erase(const KT key)
		{
			if (!root) return;
			TreeBase* found(root->find(key));
			if (found) root=static_cast<RbSet<KT>*>(root->erase(root,found));
		}
		void insert(const KT key)
		{
			if (!root) {root=new RbSet<KT>(key); return ;}
			TreeBase* found(root->find(key));
			if (found) return ;
			RbSet<KT>* node(new RbSet<KT>(key)); 
			root->insert(root,node);
		}
		bool isBST() { if (!root) return true; return root->isBST(root); }
		private:
		RbSet<KT>* root;
	};

} // namespace TreeObjects

#endif // DIGITAL_ARBORIST_SETS_H


