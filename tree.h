#ifndef DIGITAL_ARBORIST_H
#define DIGITAL_ARBORIST_H

namespace TreeObjects
{
	#ifndef NULL
	#define NULL 0
	#endif
	struct TreeBase
	{
		virtual bool operator<(const TreeBase&) = 0;
		virtual bool operator==(const TreeBase&) = 0;
		virtual ~TreeBase() {if (left) delete left; if (right) delete right; left=NULL; right=NULL;}
		virtual TreeBase* insert(TreeBase* root,TreeBase*) = 0;
		TreeBase() : parent(NULL), left(NULL), right(NULL) {}
		TreeBase(const TreeBase& a) : parent(a.parent), left(a.left), right(a.right) {}

		virtual bool isBST(TreeBase* node) = 0;
		virtual void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) = 0;

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
			if (!node->parent) return NULL;
			if (node==node->parent->left) return node->parent->right;
			else return node->parent->left;
		}

		virtual long countnodes()  
		{
			long leftnodes(0); if (left) leftnodes=left->countnodes();
			long rightnodes(0); if (right) rightnodes=right->countnodes();
			return (leftnodes+rightnodes+1);
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
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound); 
	};

	template <typename KT>
		struct BstBase : public TreeBase
	{
		BstBase(const KT _key) : key(_key) {}
		KT minValue(TreeBase* node) 
		{
			TreeBase* current(node);
			while (current->left!=NULL) current=current->left;
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*current));
			return(nd.key);
		}
		KT maxValue(TreeBase* node) 
		{
			TreeBase* current = node;
			while (current->right != NULL) current = current->right;
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*current));
			return(nd.key);
		}

		virtual bool isBST(TreeBase* node) 
		{
			if (!node) return(true);
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*node));
			if (node->left!=NULL && maxValue(node->left) > nd.key) return(false);
			if (node->right!=NULL && minValue(node->right) <= nd.key) return(false);
			if (!isBST(node->left) || !isBST(node->right)) return(false);
			return(true);
		}

		virtual TreeBase* find(const KT& what)
		{
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*this));
			if (nd==what) return this;
			if (nd<what)
			{
				if (right)
				{
					BstBase<KT>& ld(static_cast<BstBase<KT>&>(*right));
					return ld.find(what);
				}
			} else {
				if (left)
				{
					BstBase<KT>& rd(static_cast<BstBase<KT>&>(*left));
					return rd.find(what);
				}
			}
			return NULL;
		}

		virtual void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) { }

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
			const BstBase<KT>& a(static_cast<BstBase<KT>&>(*this));
			TreeBase& __b(const_cast<TreeBase&>(_b));
			const BstBase<KT>& b(static_cast<BstBase<KT>&>(__b));
			return a.key<b.key; 
		}
		virtual bool operator==(const TreeBase& _b) 
		{ 
			const BstBase<KT>& a(static_cast<BstBase<KT>&>(*this));
			TreeBase& __b(const_cast<TreeBase&>(_b));
			const BstBase<KT>& b(static_cast<BstBase<KT>&>(__b));
			return a.key==b.key; 
		}
		virtual bool operator<(const KT& b) 
		{ 
			const BstBase<KT>& a(static_cast<BstBase<KT>&>(*this));
			return a.key<b; 
		}
		virtual bool operator==(const KT& b) 
		{ 
			const BstBase<KT>& a(static_cast<BstBase<KT>&>(*this));
			return a.key==b;
		}
		operator const KT& (){return key;}
		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node)
		{
			TreeBase* newroot(TreeBase::RotateLeft(root,node));
			this->Update(node,this->parent); 
			return newroot;
		}

		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node)
		{
			TreeBase* newroot(TreeBase::RotateRight(root,node));
			this->Update(node,this->parent); 
			return newroot;
		}
		protected:
		const KT key;
	};


	inline TreeBase* TreeBase::remove(TreeBase* root,TreeBase* pfound)
	{
		if (!pfound) return root;
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


	template <typename KT,typename VT>
		struct Bst : public BstBase<KT>
	{
		Bst(const KT _key) : BstBase<KT>(_key) {}
		Bst(const KT _key,const VT _data) : BstBase<KT>(_key), data(_data) {}
		VT& Data(){return data;}
		protected:
		virtual void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) { }
		VT data;	
	};


	struct RbBase 
	{
		enum COLOR {NONE=0,RED,BLACK} ;
		const COLOR Red() const {return RED;}
		const COLOR Black() const {return BLACK;}
		virtual TreeBase* red(TreeBase* n) = 0;
		virtual TreeBase* black(TreeBase* n) = 0;
		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node) = 0;
		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node) = 0;
		virtual const char color(TreeBase* n) const = 0;
		virtual void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) = 0;
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound) = 0;

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
						} else {
							black(node->parent);
							red(node->parent->parent);
							root=this->RotateRight( root, node->parent->parent );
						}
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

		// This violates both red black invariants.  
		// A rb-transplant needs to be created, and the fixup 
		// will need to use a doubly black and red black node
		// with a method to walk up the tree, repainting until
		// both rb invariants are preserved.
		virtual TreeBase* erase(TreeBase* root,TreeBase* found)
		{
			if (!found) return root;
			TreeBase *p(found->parent),*l(found->left),*r(found->right);
			TreeBase* newroot(remove(root,found));
			Update(found,p,true); 
			if ( (p) and (newroot) )
			{
				if (p->right) newroot=this->RedAndBlack(newroot,p->right);
				if (p->left) newroot=this->RedAndBlack(newroot,p->left);
			}
			found->left=NULL; found->right=NULL;
			if (newroot) Update(newroot,newroot); 
			if (l) Update(l,p); if (r) Update(r,p);
			delete found;
			return newroot;
		}
		COLOR clr; // color enum
	};


	template <typename KT,typename VT>
		struct RbMapMapBase : public Bst<KT,VT>, RbBase
	{
		RbMapMapBase(const KT _key) : Bst<KT,VT>(_key) {}
		RbMapMapBase(const KT _key,const VT _data) : Bst<KT,VT>(_key,_data) {}

		virtual TreeBase* insert(TreeBase* root,TreeBase* node)
		{
			root=Bst<KT,VT>::insert(root,node);
			if (!root) return NULL; // attempted to add a duplicate, new node was deleted
			return this->RedAndBlack(root,node);
		}
		
		virtual TreeBase* red(TreeBase* n) = 0;
		virtual TreeBase* black(TreeBase* n) = 0;
		virtual const char color(TreeBase* n) const = 0;

		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node) { return BstBase<KT>::RotateLeft(root,node); }
		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node) { return BstBase<KT>::RotateRight(root,node); }
		virtual void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) { Bst<KT,VT>::Update(node,pnode,erasing); }
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound){return TreeBase::remove(root,pfound);}
		virtual TreeBase* erase(TreeBase* root,TreeBase* found){return RbBase::erase(root,found);}
	};


	template <typename KT>
		struct RbMapSetBase : public BstBase<KT>, RbBase
	{
		RbMapSetBase(const KT _key) : BstBase<KT>(_key) {}

		virtual TreeBase* insert(TreeBase* root,TreeBase* node)
		{
			root=BstBase<KT>::insert(root,node);
			if (!root) return NULL; // attempted to add a duplicate, new node was deleted
			return this->RedAndBlack(root,node);
		}
		
		virtual TreeBase* red(TreeBase* n) = 0;
		virtual TreeBase* black(TreeBase* n) = 0;
		virtual const char color(TreeBase* n) const = 0;

		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node) { return BstBase<KT>::RotateLeft(root,node); }
		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node) { return BstBase<KT>::RotateRight(root,node); }
		virtual void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) { }
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound){return TreeBase::remove(root,pfound);}
		virtual TreeBase* erase(TreeBase* root,TreeBase* found){return RbBase::erase(root,found);}
	};

	template <typename KT,typename VT>
		struct RbMap : public RbMapMapBase<KT,VT>
	{
		typedef RbMapMapBase<KT,VT> TB;

		RbMap(const KT _key) : TB(_key) {}
		RbMap(const KT _key,const VT _data) : TB(_key,_data) {}

		virtual TreeBase* red(TreeBase* n) 
		{ 
			if (!n) return n; 
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Red();
			return n;
		}
		virtual TreeBase* black(TreeBase* n) 
		{ 
			if (!n) return n; 
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Black();
			return n;
		}
		virtual const char color(TreeBase* n) const
		{
			if (!n) return 0; 
			TB& nd(static_cast<TB&>(*n)); 
			return nd.clr;
		}
	};

	template <typename KT>
		struct RbSet : public RbMapSetBase<KT>
	{
		typedef RbMapSetBase<KT> TB;
		RbSet(const KT _key) : TB(_key) {}
		virtual TreeBase* red(TreeBase* n) 
		{ 
			if (!n) return n; 
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Red();
			return n;
		}
		virtual TreeBase* black(TreeBase* n) 
		{ 
			if (!n) return n; 
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Black();
			return n;
		}
		virtual const char color(TreeBase* n) const
		{
			if (!n) return 0; 
			TB& nd(static_cast<TB&>(*n)); 
			return nd.clr;
		}
	};
} //namespace TreeObjects
#endif // DIGITAL_ARBORIST_H


