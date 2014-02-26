#ifndef DIGITAL_ARBORIST_MAPS_H
#define DIGITAL_ARBORIST_MAPS_H
#include <tree.h>

namespace TreeObjects
{
	// Wip - designed to resemble an stl map
	template <typename KT,typename VT>
		struct Map
	{
		Map() : root(NULL) {}
		virtual ~Map() {if (root) delete root;}
		void erase(const KT key)
		{
			if (!root) return;
			TreeBase* found(root->find(key));
			if (found) root=static_cast<RbTree<KT,VT>*>(root->erase(root,found));
		}
		VT& operator[](const KT key)
		{
			if (!root) {root=new RbTree<KT,VT>(key); return root->Data();}
			TreeBase* found(root->find(key));
			if (found) return static_cast<RbTree<KT,VT>*>(found)->Data();
			RbTree<KT,VT>* node(new RbTree<KT,VT>(key)); 
			root->insert(root,node);
			return node->Data();
			throw string("Cannot insert");
		}
		void inorder(TreeBase* node=NULL)
		{
			if (node==NULL) node=root;
			if (node->left) inorder(node->left);	
			VT& data(static_cast<RbTree<KT,VT>*>(node)->Data());
			const bool t(data);
			if (node->right) inorder(node->right);	
		}
		bool isBST() { if (!root) return true; return root->isBST(root); }
		private:
		RbTree<KT,VT>* root;
	};

} // namespace TreeObjects

#endif // DIGITAL_ARBORIST_MAPS_H

