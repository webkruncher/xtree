#ifndef KRUNCH_RB_TREE_H
#define KRUNCH_RB_TREE_H

namespace TreeObjects
{
	struct TreeBase
	{
		virtual bool operator<(const TreeBase&) = 0;
		virtual bool operator==(const TreeBase&) = 0;
		virtual ~TreeBase() {}
		virtual TreeBase* insert(TreeBase* root,TreeBase*) = 0;
		TreeBase() : parent(NULL), left(NULL), right(NULL) {}

		virtual int isBST(TreeBase* node) = 0;
		virtual long countnodes()  = 0;

		TreeBase* LeftMost()
		{
			TreeBase* current = this;
			while (current->left != NULL) current = current->left;
			return current;
		}
		TreeBase* RightMost()
		{
			TreeBase* current = this;
			while (current->right != NULL) current = current->right;
			return current;
		}
		TreeBase* grandparent(TreeBase* node)
		{
			if ((node != NULL) && (node->parent != NULL))
			return node->parent->parent;
			else return NULL;
		}
		 
		TreeBase* uncle(TreeBase* node)
		{
			TreeBase* g(grandparent(node));
			if (g == NULL) return NULL; 
			if (node->parent == g->left) return g->right;
			else return g->left;
		}

		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node)
		{
			TreeBase* other(node->right);
			node->right = other->left;
			if ( other->left != NULL ) other->left->parent = node;
			other->parent = node->parent;
			if ( node->parent == NULL ) root = other;
			else
        if ( (*node) == (*node->parent->left) ) node->parent->left = other;
        else node->parent->right = other;
			other->left = node;
			node->parent = other;
			root->parent=NULL;
			return root;
		}

		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node)
		{
			TreeBase* other(node->left);
			node->left = other->right;
			if ( other->right != NULL ) other->right->parent = node;
			other->parent = node->parent;
			if ( node->parent == NULL ) root = other;
			else
        if ( (*node) == (*node->parent->right) ) node->parent->right = other;
        else node->parent->left = other;
			other->right = node;
			node->parent = other;
			root->parent=NULL;
			return root;
		}

		TreeBase *parent,*left,*right;
		virtual ostream& operator<<(ostream&) const =0;
	};
	inline ostream& operator<<(ostream& o,const TreeBase& b) {return b.operator<<(o);}

	template <typename KT,typename VT>
		struct Bst : public TreeBase
	{
		Bst(const KT _key) : key(_key) {}
		Bst(const KT _key,const VT _data) : key(_key), data(_data) {}
		virtual ~Bst() {if (left) delete left; if (right) delete right; }
		virtual long countnodes()  
		{
			long leftnodes(0); if (left) leftnodes=left->countnodes();
			long rightnodes(0); if (right) rightnodes=right->countnodes();
			return (leftnodes+rightnodes+1);
		}
		KT minValue(TreeBase* node) 
		{
			TreeBase* current = node;
			while (current->left != NULL) current = current->left;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*current));
			return(nd.key);
		}
		KT maxValue(TreeBase* node) 
		{
			TreeBase* current = node;
			while (current->right != NULL) current = current->right;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*current));
			return(nd.key);
		}

		virtual int isBST(TreeBase* node) 
		{
			if (!node) return(true);
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			if (node->left!=NULL && maxValue(node->left) > nd.key) return(false);
			if (node->right!=NULL && minValue(node->right) <= nd.key) return(false);
			if (!isBST(node->left) || !isBST(node->right)) return(false);
			return(true);
		}
		virtual TreeBase* insert(TreeBase* root,TreeBase* node)
		{
			if ((*node)==(*this)) {delete node; return NULL;}
			node->parent=this;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.data(this->key,*this,this->parent);
			if ((*node)<(*this))
			{
				if (this->left) 
				{
					return this->left->insert(root,node);
				} else { 
					this->left=node;
				}
			} else {
				if (this->right) 
				{
					return this->right->insert(root,node);
				} else { 
					this->right=node;
				}
			}
			return root;
		}
		virtual bool operator<(const TreeBase& _b) 
		{ 
			const Bst<KT,VT>& a(static_cast<Bst<KT,VT>&>(*this));
			TreeBase& __b(const_cast<TreeBase&>(_b));
			const Bst<KT,VT>& b(static_cast<Bst<KT,VT>&>(__b));
			return a.key<b.key; 
		}
		virtual bool operator==(const TreeBase& _b) 
		{ 
			const Bst<KT,VT>& a(static_cast<Bst<KT,VT>&>(*this));
			TreeBase& __b(const_cast<TreeBase&>(_b));
			const Bst<KT,VT>& b(static_cast<Bst<KT,VT>&>(__b));
			return a.key==b.key; 
		}
		operator const KT& (){return key;}
		operator VT& (){return data;}
		virtual ostream& operator<<(ostream& o) const {return o;}
		const KT key;
		VT data;	
	};

	template <typename KT,typename VT>
		struct RbTree : public Bst<KT,VT>
	{
		enum {RED=1,BLACK};
		RbTree(const KT _key) : Bst<KT,VT>(_key) {}
		RbTree(const KT _key,const VT _data) : Bst<KT,VT>(_key,_data) {}

		virtual TreeBase* insert(TreeBase* root,TreeBase* node)
		{
			root=Bst<KT,VT>::insert(root,node);
			if (!root) return NULL; // attempted to add a duplicate, new node was deleted
			return RedAndBlack(root,node);
		}
		const char color(TreeBase* n) const
		{
			if (!n) return 0; 
			RbTree<KT,VT>& nd(static_cast<RbTree<KT,VT>&>(*n)); 
			return nd.clr;
		}
		private:
		char clr;

		TreeBase* red(TreeBase* n) 
		{ 
			if (!n) return n; 
			RbTree<KT,VT>& nd(static_cast<RbTree<KT,VT>&>(*n)); 
			nd.clr=RED;
			nd.data(0X800000); 
			return n;
		}
		TreeBase* black(TreeBase* n) 
		{ 
			if (!n) return n; 
			RbTree<KT,VT>& nd(static_cast<RbTree<KT,VT>&>(*n)); 
			nd.clr=BLACK;
			nd.data(0); 
			return n;
		}

		virtual ostream& operator<<(ostream& o) const { return o; }

		TreeBase* RedAndBlack(TreeBase* root, TreeBase* node)
		{
			if (!node) return root;
			if (!node->parent) return root;
			red(node);
			while ( (node->parent) && (color(node->parent) == RED) ) 
			{
				if ( node->parent == node->parent->parent->left ) 
				{
					TreeBase* other(node->parent->parent->right);
					if ( color(other) == RED ) 
					{
						black(node->parent);
						black(other);
						red(node->parent->parent);
						node=node->parent->parent;
					} else {
						if (node==node->parent->right ) 
						{
							node=node->parent;
							root=this->RotateLeft(root,node);
						}
						black(node->parent);
						red(node->parent->parent);
						root=this->RotateRight( root, node->parent->parent );
					}
				} else {
					TreeBase* other(node->parent->parent->left);
					if ( color(other) == RED ) 
					{
						black(node->parent);
						black(other);
						red(node->parent->parent);
						node=node->parent->parent;
					} else {
						if (node==node->parent->left ) 
						{
							node=node->parent;
							root=this->RotateRight(root,node);
						} else {
							black(node->parent);
							red(node->parent->parent);
							root=this->RotateLeft(root,node->parent->parent);
						}
					}
				}
			}
			return black(root);
		}

		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node)
		{
			TreeBase* newroot(Bst<KT,VT>::RotateLeft(root,node));
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.data(this->key,*this,this->parent);
			Bst<KT,VT>& nr(static_cast<Bst<KT,VT>&>(*newroot));
			nr.data(nr.key,*newroot,NULL);
			return newroot;
		}

		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node)
		{
			TreeBase* newroot(Bst<KT,VT>::RotateRight(root,node));
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*node));
			nd.data(this->key,*this,this->parent);
			Bst<KT,VT>& nr(static_cast<Bst<KT,VT>&>(*newroot));
			nr.data(nr.key,*newroot,NULL);
			return newroot;
		}
	};
} //namespace TreeObjects
#endif // KRUNCH_RB_TREE_H


