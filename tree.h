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
#ifndef DIGITAL_ARBORIST_H
#define DIGITAL_ARBORIST_H

		#include <iostream>		 // temporary
		using namespace std;

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
		virtual TreeBase* insert(TreeBase* root,TreeBase*,char d=0) = 0;
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

		virtual const bool isleaf(TreeBase* node) const
		{
			if ((!node->left) and (!node->right)) return true; else return false;	
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
			if (!node) return root;
			TreeBase* other(node->right);
			if (!other) return root;
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
			if (!node) return root;
			TreeBase* other(node->left);
			if (!other) return root;
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
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound) = 0;
		TreeBase* transplant(TreeBase* root,TreeBase* u,TreeBase* v);
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


		TreeBase* insert(TreeBase* root,TreeBase* node,char d=0)
		{
			if ((*node)==(*this)) {delete node; return NULL;}
			node->parent=this;
			Update(node,this);
			if ((*node)<(*this))
			{
				if (this->left) 
				{
					return this->left->insert(root,node,d+1);
				} else { 
					this->left=node;
				}
			} else {
				if (this->right) 
				{
					return this->right->insert(root,node,d+1);
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
		virtual const bool isleaf(TreeBase* node) const {return TreeBase::isleaf(node);}
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


	inline TreeBase* TreeBase::transplant(TreeBase* root,TreeBase* u,TreeBase* v)
	{
		TreeBase* ret(root);
		if (u->parent==NULL)
		{
			ret=v;
		} else {
			if (u==u->parent->left)
			{
				u->parent->left=v;
			} else {
				u->parent->right=v;
			}
		}
		if (v!=NULL) v->parent=u->parent;
		return ret;
	}

	inline TreeBase* TreeBase::remove(TreeBase* root,TreeBase* pfound)
	{
		if (pfound->left==NULL)
		{
			root=transplant(root,pfound,pfound->right);
		} else {
			if (pfound->right==NULL)
			{
				root=transplant(root,pfound,pfound->left);
			} else {
				TreeBase* y(pfound->right->LeftMost());
				if (y->parent!=pfound)
				{
					root=transplant(root,y,y->right);
					y->right=pfound->right;
					y->right->parent=y;
				}
				root=transplant(root,pfound,y);
				y->left=pfound->left;
				y->left->parent=y;
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
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound)
		{
			TreeBase& me(static_cast<TreeBase&>(*this));
			return me.remove(root,pfound);
		}
	};


	struct RbBase 
	{
		enum COLOR {NONE=0,RED,BLACK} ;
		const COLOR Red() const {return RED;}
		const COLOR Black() const {return BLACK;}
		virtual TreeBase* red(TreeBase* n) = 0;
		virtual TreeBase* black(TreeBase* n) = 0;
		virtual const bool isleaf(TreeBase* node) const = 0;
		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node) = 0;
		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node) = 0;
		virtual const char color(TreeBase* n) const = 0;
		virtual void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) = 0;
		virtual operator TreeBase& () = 0;
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound); 

		void Rotator(TreeBase* node) {}

		TreeBase* RedAndBlackDelete(TreeBase* root, TreeBase* node)
		{
			if (!node) return root;
			while ( (node!=root) and (color(node) == BLACK) ) 
			{
				if (!node->parent) continue;
				if ( node == node->parent->left ) 
				{
					TreeBase* other(node->parent->right);
					if ( color(other) == RED ) 
					{
						black(other);
						red(other->parent);
						root=this->RotateLeft(root,node->parent);
						other=node->parent->right;
					}
					if (!other) {node=node->parent;continue;}
					if ( (color(other->left)==BLACK) and (color(other->right)==BLACK) )
					{
						red(other);
						node=node->parent;
					} else {
						if (color(other->right) == BLACK) 
						{
							black(other->left);
							red(other);
							root=this->RotateRight(root,other);
							other=node->parent->right;
						}
						if (color(node->parent)==BLACK) black(other); else red(other);
						black(node->parent);
						black(other->right);
						root=this->RotateLeft(root,node->parent);
						node=root;
					}
				} 
				if (!node->parent) continue;
				if ( node == node->parent->right ) 
				{
					TreeBase* other(node->parent->left);
					if ( color(other) == RED ) 
					{
						black(other);
						red(other->parent);
						root=this->RotateRight(root,node->parent);
						other=node->parent->left;
					}
					if (!other) {node=node->parent;continue;}
					if ( (color(other->left)==BLACK) and (color(other->right)==BLACK) )
					{
						red(other);
						node=node->parent;
					} else {
						if (color(other->left) == BLACK) 
						{
							black(other->right);
							red(other);
							root=this->RotateLeft(root,other);
							other=node->parent->left;
						}
						if (color(node->parent)==BLACK) black(other); else red(other);
						black(node->parent);
						black(other->left);
						root=this->RotateRight(root,node->parent);
						node=root;
					}
				} 
			}
			black(node);
			return black(root);
		}

		TreeBase* RedAndBlackInsert(TreeBase* root, TreeBase* node)
		{
			black(root); red(node);
			while ( color(node->parent) == RED ) 
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
						}
						black(node->parent);
						red(node->parent->parent);
						root=this->RotateLeft(root,node->parent->parent);
					}
				}
			}
			return black(root);
		}




		virtual TreeBase* erase(TreeBase* root,TreeBase* found)
		{
			if (!found) return root;
			TreeBase *p(found->parent),*l(found->left),*r(found->right);
			TreeBase* newroot(RbBase::remove(root,found));
			Update(found,p,true); 
			found->left=NULL; found->right=NULL;
			if (newroot) Update(newroot,newroot); 
			if (l) Update(l,p); if (r) Update(r,p);
			delete found;
			return newroot;
		}
		COLOR clr; // color enum
	};

	inline TreeBase* RbBase::remove(TreeBase* root,TreeBase* pfound)
	{
		TreeBase& me(static_cast<TreeBase&>(*this));
		TreeBase* Y(pfound);
		TreeBase* X(NULL);
		char Ycolor(this->color(Y));
		if (pfound->left==NULL)
		{
			X=pfound->right;
			root=me.transplant(root,pfound,pfound->right);
		} else {
			if (pfound->right==NULL)
			{
				X=pfound->left;
				root=me.transplant(root,pfound,pfound->left);
			} else {
				Y=(pfound->right->LeftMost());
				Ycolor=(this->color(Y));
				X=Y->right;
				if (Y->parent!=pfound)
				{
					root=me.transplant(root,Y,Y->right);
					Y->right=pfound->right;
					Y->right->parent=Y;
				}
				root=me.transplant(root,pfound,Y);
				Y->left=pfound->left;
				Y->left->parent=Y;
				if (color(pfound)==BLACK) black(Y); else red(Y);
			}
		}
		if (Ycolor==BLACK) return RedAndBlackDelete(root,X);
		return root;
	}


	template <typename KT,typename VT>
		struct RbMapBase : public Bst<KT,VT>, RbBase
	{
		RbMapBase(const KT _key) : Bst<KT,VT>(_key) {}
		RbMapBase(const KT _key,const VT _data) : Bst<KT,VT>(_key,_data) {}

		TreeBase* insert(TreeBase* root,TreeBase* node,char d=0)
		{
			root=Bst<KT,VT>::insert(root,node,d+1);
			if (!root) return NULL; // attempted to add a duplicate, new node was deleted
			if (d) return black(root);
			return this->RedAndBlackInsert(root,node);
		}
		
		virtual TreeBase* red(TreeBase* n) = 0;
		virtual TreeBase* black(TreeBase* n) = 0;
		virtual const char color(TreeBase* n) const = 0;

		virtual const bool isleaf(TreeBase* node) const {return BstBase<KT>::isleaf(node);}
		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node) 
			{ root=BstBase<KT>::RotateLeft(root,node); RbBase::Rotator(node); return root;}
		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node) 
			{ root=BstBase<KT>::RotateRight(root,node); RbBase::Rotator(node); return root;}
		virtual void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) { Bst<KT,VT>::Update(node,pnode,erasing); }
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound){return TreeBase::remove(root,pfound);}
		virtual TreeBase* erase(TreeBase* root,TreeBase* found){return RbBase::erase(root,found);}
		virtual operator TreeBase& () {return *this;}
	};


	template <typename KT>
		struct RbSetBase : public BstBase<KT>, RbBase
	{
		RbSetBase(const KT _key) : BstBase<KT>(_key) {}

		TreeBase* insert(TreeBase* root,TreeBase* node,char d=0)
		{
			root=BstBase<KT>::insert(root,node,d+1);
			if (!root) return NULL; // attempted to add a duplicate, new node was deleted
			if (d) return black(root);
			return this->RedAndBlackInsert(root,node);
		}
		
		virtual TreeBase* red(TreeBase* n) = 0;
		virtual TreeBase* black(TreeBase* n) = 0;
		virtual const char color(TreeBase* n) const = 0;

		virtual const bool isleaf(TreeBase* node) const {return BstBase<KT>::isleaf(node);}
		virtual TreeBase* RotateLeft(TreeBase* root, TreeBase* node) 
			{ root=BstBase<KT>::RotateLeft(root,node); RbBase::Rotator(node); return root;}
		virtual TreeBase* RotateRight(TreeBase* root, TreeBase* node) 
			{ root=BstBase<KT>::RotateRight(root,node); RbBase::Rotator(node); return root;}
		virtual void Update(TreeBase* node,TreeBase* pnode,bool erasing=false) { }
		virtual TreeBase* remove(TreeBase* root,TreeBase* pfound){return TreeBase::remove(root,pfound);}
		virtual TreeBase* erase(TreeBase* root,TreeBase* found){return RbBase::erase(root,found);}
		virtual operator TreeBase& () {return *this;}
	};

	template <typename KT,typename VT>
		struct RbMap : public RbMapBase<KT,VT>
	{
		typedef RbMapBase<KT,VT> TB;

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
		struct RbSet : public RbSetBase<KT>
	{
		typedef RbSetBase<KT> TB;
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


