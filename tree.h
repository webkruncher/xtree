#ifndef KRUNCH_RB_TREE_H
#define KRUNCH_RB_TREE_H

namespace TreeObjects
{
	struct TreeBase
	{
		virtual bool operator<(const TreeBase&) = 0;
		virtual bool operator==(const TreeBase&) = 0;
		virtual ~TreeBase() {if (left) delete left; if (right) delete right; left=NULL; right=NULL;}
		virtual TreeBase* insert(TreeBase* root,TreeBase*) = 0;
		TreeBase() : parent(NULL), left(NULL), right(NULL) {}
		TreeBase(const TreeBase& a) : parent(a.parent), left(a.left), right(a.right) {}

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

		TreeBase* sibling(TreeBase* node)
		{
			if (!node->parent) throw string("root nodes have no sibling");
			if (node==node->parent->left) return node->parent->right;
			else return node->parent->left;
		}

		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node)
		{
			TreeBase* other(node->right);
			node->right = other->left;
			if ( other->left != NULL ) other->left->parent = node;
			other->parent = node->parent;
			if ( node->parent == NULL ) root = other;
			else
        if ( node == node->parent->left ) 
					node->parent->left = other;
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
        if ( node == node->parent->right ) node->parent->right = other;
        else node->parent->left = other;
			other->right = node;
			node->parent = other;
			root->parent=NULL;
			return root;
		}

		TreeBase *parent,*left,*right;
		virtual ostream& operator<<(ostream&) const =0;
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound); 
	};
	inline ostream& operator<<(ostream& o,const TreeBase& b) {return b.operator<<(o);}

	template <typename KT,typename VT>
		struct Bst : public TreeBase
	{
		Bst(const KT _key) : key(_key) {}
		Bst(const KT _key,const VT _data) : key(_key), data(_data) {}
		virtual long countnodes()  
		{
			long leftnodes(0); if (left) leftnodes=left->countnodes();
			long rightnodes(0); if (right) rightnodes=right->countnodes();
			return (leftnodes+rightnodes+1);
		}
		KT minValue(TreeBase* node) 
		{
			TreeBase* current(node);
			while (current->left!=NULL) current=current->left;
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

		virtual TreeBase* find(const KT& what)
		{
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*this));
			if (nd==what) return this;
			if (nd<what)
			{
				if (right)
				{
					Bst<KT,VT>& ld(static_cast<Bst<KT,VT>&>(*right));
					return ld.find(what);
				}
			} else {
				if (left)
				{
					Bst<KT,VT>& rd(static_cast<Bst<KT,VT>&>(*left));
					return rd.find(what);
				}
			}
			return NULL;
		}

		void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) { }

		virtual TreeBase* erase(TreeBase* root,TreeBase* found)
		{
			if (!found) return root;
			TreeBase *p(found->parent),*l(found->left),*r(found->right);
			TreeBase* newroot(TreeBase::remove(root,found));
			found->left=NULL; found->right=NULL;
			Update(found,found->parent,true); 
			Update(p,found); Update(l,found); Update(r,found);
			delete found;
			return newroot;
		}


		virtual TreeBase* insert(TreeBase* root,TreeBase* node)
		{
			if ((*node)==(*this)) {delete node; return NULL;}
			node->parent=this;
			Update(node,this);
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
		virtual bool operator<(const KT& b) 
		{ 
			const Bst<KT,VT>& a(static_cast<Bst<KT,VT>&>(*this));
			return a.key<b; 
		}
		virtual bool operator==(const KT& b) 
		{ 
			const Bst<KT,VT>& a(static_cast<Bst<KT,VT>&>(*this));
			return a.key==b;
		}
		operator const KT& (){return key;}
		VT& Data(){return data;}
		virtual ostream& operator<<(ostream& o) const {return o;}
		protected:
		const KT key;
		VT data;	
	};

	template <typename KT,typename VT>
		struct RbTree : public Bst<KT,VT>
	{
		enum COLOR {NONE=0,RED,BLACK} ;
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

		virtual TreeBase* erase(TreeBase* root,TreeBase* found)
		{
			if (!found) return root;
			TreeBase *p(found->parent),*l(found->left),*r(found->right);
			TreeBase* newroot(TreeBase::remove(root,found));
			this->Update(found,p,true); 
			if ( (p) and (newroot) )
			{
				if (p->right) newroot=RedAndBlack(newroot,p->right);
				if (p->left) newroot=RedAndBlack(newroot,p->left);
			}
			found->left=NULL; found->right=NULL;
			if (newroot) this->Update(newroot,newroot); 
			if (l) this->Update(l,p); if (r) this->Update(r,p);
			delete found;
			return newroot;
		}

		private:
		COLOR clr; // color enum

		TreeBase* red(TreeBase* n) 
		{ 
			if (!n) return n; 
			RbTree<KT,VT>& nd(static_cast<RbTree<KT,VT>&>(*n)); 
			nd.clr=RED;
			return n;
		}
		TreeBase* black(TreeBase* n) 
		{ 
			if (!n) return n; 
			RbTree<KT,VT>& nd(static_cast<RbTree<KT,VT>&>(*n)); 
			nd.clr=BLACK;
			return n;
		}

		virtual ostream& operator<<(ostream& o) const { return o; }

		TreeBase* RedAndBlack(TreeBase* root, TreeBase* node)
		{
			if (!node) return root;
			if (!node->parent) return root;
			red(node);
			while ( (node) && (node->parent) && (node->parent->parent) && (color(node->parent) == RED) ) 
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
			this->Update(node,this->parent); 
			return newroot;
		}

		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node)
		{
			TreeBase* newroot(Bst<KT,VT>::RotateRight(root,node));
			this->Update(node,this->parent); 
			return newroot;
		}
	};

	inline TreeBase* TreeBase::remove(TreeBase* root,TreeBase* pfound)
	{
		if (!pfound) throw string("Cannot delete null node");
		TreeBase& found(*pfound);
		if (!found.parent)
		{	
			if ((!found.left) and (!found.right))  return NULL;	
			if ((!found.left) or (!found.right))
			{
				if (found.left) {found.left->parent=NULL; return found.left; }
				if (found.right) {found.right->parent=NULL; return found.right; }
			} else {
				TreeBase* L(found.left);
				TreeBase* R(found.right);
				TreeBase* LeftsRightMost(L->RightMost());
				LeftsRightMost->right=R;
				R->parent=LeftsRightMost;
				L->parent=NULL;
				return L;
			}
		} else {
			TreeBase& parent(*found.parent);
			if ((!found.left) and (!found.right)) 
			{
				if (parent.left==pfound)  parent.left=NULL;
				if (parent.right==pfound) parent.right=NULL;
				return root;
			} 
			if ((!found.left) or (!found.right))
			{
				if (found.left)
				{
					TreeBase* C(found.left);
					if (parent.left==pfound)
					{
						parent.left=C;
						C->parent=&parent;
					} else {
						parent.right=C;
						C->parent=&parent;
					}
				} else {
					TreeBase* C(found.right);
					if (parent.left==pfound)
					{
						parent.left=C;
						C->parent=&parent;
					} else {
						parent.right=C;
						C->parent=&parent;
					}
				}
			} else {
				if (parent.left==pfound) 
				{
					TreeBase* L(found.left);
					TreeBase* R(found.right);
					parent.left=R; R->parent=&parent;
					TreeBase* NewSide(R->LeftMost());
					L->parent=NewSide;
					NewSide->left=L;
				} else {
					TreeBase* L(found.left);
					TreeBase* R(found.right);
					parent.right=L; L->parent=&parent;
					TreeBase* NewSide(L->RightMost());
					R->parent=NewSide;
					NewSide->right=R;
				}
			}
		}
		return root;
	}


	// Wip - designed to resemble an stl map
	template <typename KT,typename VT>
		struct Map
	{
		Map() : root(NULL) {}
		virtual ~Map() {if (root) delete root;}
		void erase(const KT& key)
		{
			if (!root) return;
			TreeBase* found(root->find(key));
			if (found) root=static_cast<RbTree<KT,VT>*>(root)->erase(root,found);
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
		private:
		RbTree<KT,VT>* root;
	};

} //namespace TreeObjects
#endif // KRUNCH_RB_TREE_H


