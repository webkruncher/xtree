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

namespace TreeObjects
{
	#ifndef NULL
	#define NULL 0
	#endif

	struct TreeBase;
	struct SentinelBase
	{
		virtual bool isnil() { return false;}
		virtual bool operator<(const SentinelBase&) {return false;}
		virtual bool operator==(const SentinelBase&) {return false;}
		virtual SentinelBase* insert(SentinelBase* root,SentinelBase*,char d=0) {return NULL;}
		virtual bool isBST(SentinelBase* node) {return false;}
		virtual void Update(SentinelBase* node,SentinelBase* pnode,bool erasing=false) {}
		virtual SentinelBase* LeftMost(){return NULL;}
		virtual SentinelBase* RightMost(){return NULL;}
		virtual SentinelBase* Predecessor(){return NULL;}
		virtual SentinelBase* Successor(){return NULL;}
		virtual const bool isleaf(SentinelBase* node) const {return false;}
		virtual const bool isnul(SentinelBase* node) const {return true;}
		virtual long countnodes() {return 0;}
		virtual SentinelBase* RotateLeft(SentinelBase* root, SentinelBase* node){return NULL;}
		virtual SentinelBase* RotateRight(SentinelBase* root, SentinelBase* node){return NULL;}
		virtual SentinelBase* remove(SentinelBase* root,SentinelBase* pfound) {return NULL;}
		virtual SentinelBase* transplant(SentinelBase* root,SentinelBase* u,SentinelBase* v){return NULL;}
		virtual SentinelBase* Parent() = 0;
		virtual void SetParent(SentinelBase* p) = 0;
		virtual SentinelBase* Left() = 0;
		virtual void SetLeft(SentinelBase* l) = 0;
		virtual SentinelBase* Right() = 0;
		virtual void SetRight(SentinelBase* r) = 0;
	};


	struct TreeBase : SentinelBase
	{
		TreeBase() : left(NULL),right(NULL),parent(NULL) {}
		TreeBase(const TreeBase& a) : parent(a.parent), left(a.left), right(a.right) {}
		virtual ~TreeBase() {if (left) delete left; if (right) delete right; left=NULL; right=NULL;}
		virtual bool operator<(const SentinelBase&) = 0;
		virtual bool operator==(const SentinelBase&) = 0;
		virtual SentinelBase* insert(SentinelBase* root,SentinelBase*,char d=0) = 0;
		virtual bool isBST(SentinelBase* node) = 0;
		virtual void Update(SentinelBase* node,SentinelBase* pnode,bool erasing=false) = 0;
		SentinelBase* LeftMost();
		SentinelBase* RightMost();
		SentinelBase* Predecessor();
		SentinelBase* Successor();
		const bool isleaf(SentinelBase* node) const;
		const bool isnul(SentinelBase* node) const;
		long countnodes() ; 
		SentinelBase* RotateLeft(SentinelBase* root, SentinelBase* node);
		SentinelBase* RotateRight(SentinelBase* root, SentinelBase* node);
		SentinelBase* remove(SentinelBase* root,SentinelBase* pfound) = 0;
		SentinelBase* transplant(SentinelBase* root,SentinelBase* u,SentinelBase* v);
		virtual SentinelBase* Parent(){return parent;}
		virtual void SetParent(SentinelBase* p){parent=p;}
		virtual SentinelBase* Left(){return left;}
		virtual void SetLeft(SentinelBase* l){left=l;}
		virtual SentinelBase* Right(){return right;}
		virtual void SetRight(SentinelBase* r){right=r;}
		private: SentinelBase *parent,*left,*right;
	};

	struct Sentinel : SentinelBase
	{
		virtual bool isnil() {return true;}
		virtual void SetParent(SentinelBase*){}
		virtual SentinelBase* Left(){return NULL;}
		virtual void SetLeft(SentinelBase*){}
		virtual SentinelBase* Right(){return NULL;}
		virtual void SetRight(SentinelBase*){}
	};

	inline SentinelBase* TreeBase::LeftMost()
	{
		SentinelBase* current = this;
		while (!isnul(current->Left())) current = current->Left();
		return current;
	}

	inline SentinelBase* TreeBase::RightMost()
	{
		SentinelBase* current = this;
		while (!isnul(current->Right())) current = current->Right();
		return current;
	}

	inline SentinelBase* TreeBase::Predecessor()
	{
		if (!isnul(Left())) return Left()->RightMost();
		SentinelBase* y(Parent());
		SentinelBase* x(this);
		while ((!isnul(y->Parent())) and x==y->Left()) { x=y; y=y->Parent(); }
		return y;
	}

	inline SentinelBase* TreeBase::Successor()
	{
		if (!isnul(Right())) return Right()->LeftMost();
		SentinelBase* y(Parent());
		SentinelBase* x(this);
		while ((!isnul(y->Parent())) and x==y->Right()) { x=y; y=y->Parent(); }
		return y;
	}


	inline const bool TreeBase::isleaf(SentinelBase* node) const
	{
		if 
		(
			((!node->Left()) or (node->Left()->isnil()) )
				and 
			((!node->Right()) or (node->Right()->isnil()) )
		) 
			return true; else return false;	
	}

	inline const bool TreeBase::isnul(SentinelBase* node) const
	{
		if (!node) return true;
		if (node->isnil()) return true;
		return false;	
	}


	inline long TreeBase::countnodes()  
	{
		long leftnodes(0); if (Left()) leftnodes=Left()->countnodes();
		long rightnodes(0); if (Right()) rightnodes=Right()->countnodes();
		return (leftnodes+rightnodes+1);
	}

	inline SentinelBase* TreeBase::RotateLeft(SentinelBase* root, SentinelBase* node)
	{
		if (!node) return root;
		SentinelBase* other(node->Right());
		if (!other) return root;
		node->SetRight(other->Left());
		if ( !isnul(other->Left()) ) other->Left()->SetParent(node);
		other->SetParent(node->Parent());
		if ( isnul(node->Parent()) ) root = other;
		else 
			if ( node == node->Parent()->Left() ) node->Parent()->SetLeft(other);
			else node->Parent()->SetRight(other);
		other->SetLeft(node);
		node->SetParent(other);
		root->SetParent(NULL);
		return root;
	}

	inline SentinelBase* TreeBase::RotateRight(SentinelBase* root, SentinelBase* node)
	{
		if (!node) return root;
		SentinelBase* other(node->Left());
		if (!other) return root;
		node->SetLeft(other->Right());
		if ( !isnul(other->Right()) ) other->Right()->SetParent(node);
		other->SetParent(node->Parent());
		if ( isnul(node->Parent()) ) root = other;
		else 
			if ( node == node->Parent()->Right() ) node->Parent()->SetRight(other);
			else node->Parent()->SetLeft(other);
		other->SetRight(node);
		node->SetParent(other);
		root->SetParent(NULL);
		return root;
	}




	template <typename KT>
		struct BstBase : public TreeBase
	{
		BstBase(const KT _key) : key(_key) {}
		KT minValue(SentinelBase* node) 
		{
			SentinelBase* current(node);
			while (current->Left()!=NULL) current=current->Left();
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*current));
			return(nd.key);
		}
		KT maxValue(SentinelBase* node) 
		{
			SentinelBase* current = node;
			while (current->Right() != NULL) current = current->Right();
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*current));
			return(nd.key);
		}

		virtual bool isBST(SentinelBase* node) 
		{
			if (!node) return(true);
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*node));
			if (node->Left()!=NULL && maxValue(node->Left()) > nd.key) return(false);
			if (node->Right()!=NULL && minValue(node->Right()) <= nd.key) return(false);
			if (!isBST(node->Left()) || !isBST(node->Right())) return(false);
			return(true);
		}

		virtual BstBase<KT>* find(const KT what)
		{
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*this));
			if (nd==what) return this;
			if (nd<what)
			{
				if (Right())
				{
					BstBase<KT>& ld(static_cast<BstBase<KT>&>(*Right()));
					return ld.find(what);
				}
			} else {
				if (Left())
				{
					BstBase<KT>& rd(static_cast<BstBase<KT>&>(*Left()));
					return rd.find(what);
				}
			}
			return NULL;
		}

		virtual void Update(SentinelBase* node,SentinelBase* pnode,bool erasing=false) { }

		virtual SentinelBase* erase(SentinelBase* root,SentinelBase* found)
		{
			if (!found) return root;
			SentinelBase *p(found->Parent()),*l(found->Left()),*r(found->Right());
			SentinelBase* newroot(TreeBase::remove(root,found));
			found->SetLeft(NULL); found->SetRight(NULL);
			Update(found,found->Parent(),true); 
			Update(p,found); Update(l,found); Update(r,found);
			delete found;
			return newroot;
		}


		SentinelBase* insert(SentinelBase* root,SentinelBase* node,char d=0)
		{
			if ((*node)==(*this)) {delete node; return NULL;}
			node->SetParent(this);
			Update(node,this);
			if ((*node)<(*this))
			{
				if (this->Left()) 
				{
					return this->Left()->insert(root,node,d+1);
				} else { 
					this->SetLeft(node);
				}
			} else {
				if (this->Right()) 
				{
					return this->Right()->insert(root,node,d+1);
				} else { 
					this->SetRight(node);
				}
			}
			return root;
		}
		virtual bool operator<(const SentinelBase& _b) 
		{ 
			const BstBase<KT>& a(static_cast<BstBase<KT>&>(*this));
			SentinelBase& __b(const_cast<SentinelBase&>(_b));
			const BstBase<KT>& b(static_cast<BstBase<KT>&>(__b));
			return a.key<b.key; 
		}
		virtual bool operator==(const SentinelBase& _b) 
		{ 
			const BstBase<KT>& a(static_cast<BstBase<KT>&>(*this));
			SentinelBase& __b(const_cast<SentinelBase&>(_b));
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
		virtual const bool isleaf(SentinelBase* node) const {return TreeBase::isleaf(node);}
		virtual SentinelBase* RotateLeft(SentinelBase* root, SentinelBase* node)
		{
			SentinelBase* newroot(TreeBase::RotateLeft(root,node));
			this->Update(node,this->Parent()); 
			return newroot;
		}

		virtual SentinelBase* RotateRight(SentinelBase* root, SentinelBase* node)
		{
			SentinelBase* newroot(TreeBase::RotateRight(root,node));
			this->Update(node,this->Parent()); 
			return newroot;
		}
		protected:
		const KT key;
	};

	template <typename KT>
		struct Iterator
	{
		Iterator(SentinelBase* _node) : node(_node) {Invalidate();}
		bool operator--(int)
		{
			if (begin==node) return false;
			node=node->Predecessor(); return true;
		}
		bool operator++(int)
		{
			if (end==node) return false;
			node=node->Successor(); return true;
		}
		void Invalidate()
		{
			root=Root();
			begin=root->LeftMost();
			end=root->RightMost();
		}
		operator const KT& ()
		{
			BstBase<KT>& b(static_cast<BstBase<KT>&>(*node));
			return b;
		}
		operator BstBase<KT>& ()
		{
			BstBase<KT>& b(static_cast<BstBase<KT>&>(*node));
			return b;
		}
		private:
		SentinelBase *root,*begin,*end;
		SentinelBase* Root()
		{
			SentinelBase* current(node);
			while (current->Parent()) current=current->Parent();
			return current;
		}
		SentinelBase* node;
	};

	inline SentinelBase* TreeBase::transplant(SentinelBase* root,SentinelBase* u,SentinelBase* v)
	{
		SentinelBase* ret(root);
		if (u->Parent()==NULL)
		{
			ret=v;
		} else {
			if (u==u->Parent()->Left())
			{
				u->Parent()->SetLeft(v);
			} else {
				u->Parent()->SetRight(v);
			}
		}
		if (v!=NULL) v->SetParent(u->Parent());
		return ret;
	}

	inline SentinelBase* TreeBase::remove(SentinelBase* root,SentinelBase* pfound)
	{
		if (pfound->Left()==NULL)
		{
			root=transplant(root,pfound,pfound->Right());
		} else {
			if (pfound->Right()==NULL)
			{
				root=transplant(root,pfound,pfound->Left());
			} else {
				SentinelBase* y(pfound->Right()->LeftMost());
				if (y->Parent()!=pfound)
				{
					root=transplant(root,y,y->Right());
					y->SetRight(pfound->Right());
					y->Right()->SetParent(y);
				}
				root=transplant(root,pfound,y);
				y->SetLeft(pfound->Left());
				y->Left()->SetParent(y);
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
		virtual void Update(SentinelBase* node,SentinelBase* pnode,bool erasing=false) { }
		VT data;	
		virtual SentinelBase* remove(SentinelBase* root,SentinelBase* pfound)
		{
			SentinelBase& me(static_cast<SentinelBase&>(*this));
			return me.remove(root,pfound);
		}
	};


	struct RbBase 
	{
		enum COLOR {NONE=0,RED=10,BLACK=20} ;
		const COLOR Red() const {return RED;}
		const COLOR Black() const {return BLACK;}
		virtual SentinelBase* red(SentinelBase* n) = 0;
		virtual SentinelBase* black(SentinelBase* n) = 0;
		virtual const bool isleaf(SentinelBase* node) const = 0;
		virtual SentinelBase* RotateLeft(SentinelBase* root, SentinelBase* node) = 0;
		virtual SentinelBase* RotateRight(SentinelBase* root, SentinelBase* node) = 0;
		virtual const COLOR color(SentinelBase* n) const = 0;
		virtual void Update(SentinelBase* node,SentinelBase* pnode,bool erasing=false) = 0;
		virtual operator SentinelBase& () = 0;
		virtual SentinelBase* remove(SentinelBase* root,SentinelBase* pfound); 

		void Rotator(SentinelBase* node) {Update(node,node->Parent());}

		SentinelBase* RedAndBlackDelete(SentinelBase* root, SentinelBase* node)
		{
			if (!node) return root;
			while ( (node!=root) and (color(node) == BLACK) ) 
			{
				if (!node->Parent()) continue;
				if ( node == node->Parent()->Left() ) 
				{
					SentinelBase* other(node->Parent()->Right());
					if ( color(other) == RED ) 
					{
						black(other);
						red(other->Parent());
						root=this->RotateLeft(root,node->Parent());
						other=node->Parent()->Right();
					}
					if (!other) {node=node->Parent();continue;}
					if ( (color(other->Left())==BLACK) and (color(other->Right())==BLACK) )
					{
						red(other);
						node=node->Parent();
					} else {
						if (color(other->Right()) == BLACK) 
						{
							black(other->Left());
							red(other);
							root=this->RotateRight(root,other);
							other=node->Parent()->Right();
						}
						if (color(node->Parent())==BLACK) black(other); else red(other);
						black(node->Parent());
						black(other->Right());
						root=this->RotateLeft(root,node->Parent());
						node=root;
					}
				} 
				if (!node->Parent()) continue;
				if ( node == node->Parent()->Right() ) 
				{
					SentinelBase* other(node->Parent()->Left());
					if ( color(other) == RED ) 
					{
						black(other);
						red(other->Parent());
						root=this->RotateRight(root,node->Parent());
						other=node->Parent()->Left();
					}
					if (!other) {node=node->Parent();continue;}
					if ( (color(other->Left())==BLACK) and (color(other->Right())==BLACK) )
					{
						red(other);
						node=node->Parent();
					} else {
						if (color(other->Left()) == BLACK) 
						{
							black(other->Right());
							red(other);
							root=this->RotateLeft(root,other);
							other=node->Parent()->Left();
						}
						if (color(node->Parent())==BLACK) black(other); else red(other);
						black(node->Parent());
						black(other->Left());
						root=this->RotateRight(root,node->Parent());
						node=root;
					}
				} 
			}
			black(node);
			return (root);
		}

		SentinelBase* RedAndBlackInsert(SentinelBase* root, SentinelBase* node)
		{
			black(root); red(node);
			while ( color(node->Parent()) == RED ) 
			{
				if ( node->Parent() == node->Parent()->Parent()->Left() ) 
				{
					SentinelBase* other(node->Parent()->Parent()->Right());
					if ( color(other) == RED ) 
					{
						black(node->Parent());
						black(other);
						red(node->Parent()->Parent());
						node=node->Parent()->Parent();
					} else {
						if (node==node->Parent()->Right() ) 
						{
							node=node->Parent();
							root=this->RotateLeft(root,node);
						} 
						black(node->Parent());
						red(node->Parent()->Parent());
						root=this->RotateRight( root, node->Parent()->Parent() );
					}
				} else {
					SentinelBase* other(node->Parent()->Parent()->Left());
					if ( color(other) == RED ) 
					{
						black(node->Parent());
						black(other);
						red(node->Parent()->Parent());
						node=node->Parent()->Parent();
					} else {
						if (node==node->Parent()->Left() ) 
						{
							node=node->Parent();
							root=this->RotateRight(root,node);
						}
						black(node->Parent());
						red(node->Parent()->Parent());
						root=this->RotateLeft(root,node->Parent()->Parent());
					}
				}
			}
			return black(root);
		}




		virtual SentinelBase* erase(SentinelBase* root,SentinelBase* found)
		{
			if (!found) return root;
			SentinelBase *p(found->Parent()),*l(found->Left()),*r(found->Right());
			SentinelBase* newroot(RbBase::remove(root,found));
			Update(found,p,true); 
			found->SetLeft(NULL); found->SetRight(NULL); 
			if (newroot) Update(newroot,newroot); 
			if (l) Update(l,p); if (r) Update(r,p);
			delete found;
			return newroot;
		}
		COLOR clr; // color enum
	};

	inline SentinelBase* RbBase::remove(SentinelBase* root,SentinelBase* pfound)
	{
		SentinelBase& me(static_cast<SentinelBase&>(*this));
		SentinelBase* Y(pfound);
		SentinelBase* X(NULL);
		char Ycolor(this->color(Y));
		if (pfound->Left()==NULL)
		{
			if (pfound->Right())	
			{
				Y=(pfound->Right()->LeftMost());
				Ycolor=(this->color(Y));
			}
			X=pfound->Right();
			root=me.transplant(root,pfound,pfound->Right());
		} else {
			if (pfound->Right()==NULL)
			{
				if (pfound->Left()) Y=(pfound->Left()->RightMost());
				else Y=pfound->RightMost();
				Ycolor=(this->color(Y));
				X=pfound->Left();
				root=me.transplant(root,pfound,pfound->Left());
			} else {
				Y=(pfound->Right()->LeftMost());
				Ycolor=(this->color(Y));
				X=Y->Right();
				if (Y->Parent()!=pfound)
				{
					root=me.transplant(root,Y,Y->Right());
					Y->SetRight(pfound->Right());
					Y->Right()->SetParent(Y);
				}
				root=me.transplant(root,pfound,Y);
				Y->SetLeft(pfound->Left());
				Y->Left()->SetParent(Y);
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

		SentinelBase* insert(SentinelBase* root,SentinelBase* node,char d=0)
		{
			root=Bst<KT,VT>::insert(root,node,d+1);
			if (!root) return NULL; // attempted to add a duplicate, new node was deleted
			if (d) return black(root);
			return this->RedAndBlackInsert(root,node);
		}
		
		virtual SentinelBase* red(SentinelBase* n) = 0;
		virtual SentinelBase* black(SentinelBase* n) = 0;
		virtual const COLOR color(SentinelBase* n) const = 0;

		virtual const bool isleaf(SentinelBase* node) const {return BstBase<KT>::isleaf(node);}
		virtual SentinelBase* RotateLeft(SentinelBase* root, SentinelBase* node) 
			{ root=BstBase<KT>::RotateLeft(root,node); RbBase::Rotator(node); return root;}
		virtual SentinelBase* RotateRight(SentinelBase* root, SentinelBase* node) 
			{ root=BstBase<KT>::RotateRight(root,node); RbBase::Rotator(node); return root;}
		virtual void Update(SentinelBase* node,SentinelBase* pnode,bool erasing=false) { Bst<KT,VT>::Update(node,pnode,erasing); }
		virtual SentinelBase* remove(SentinelBase* root,SentinelBase* pfound){return TreeBase::remove(root,pfound);}
		virtual SentinelBase* erase(SentinelBase* root,SentinelBase* found){return RbBase::erase(root,found);}
		virtual operator SentinelBase& () {return *this;}
	};


	template <typename KT>
		struct RbSetBase : public BstBase<KT>, RbBase
	{
		RbSetBase(const KT _key) : BstBase<KT>(_key) {}

		SentinelBase* insert(SentinelBase* root,SentinelBase* node,char d=0)
		{
			root=BstBase<KT>::insert(root,node,d+1);
			if (!root) return NULL; // attempted to add a duplicate, new node was deleted
			if (d) return black(root);
			return this->RedAndBlackInsert(root,node);
		}
		
		virtual SentinelBase* red(SentinelBase* n) = 0;
		virtual SentinelBase* black(SentinelBase* n) = 0;
		virtual const COLOR color(SentinelBase* n) const = 0;

		virtual const bool isleaf(SentinelBase* node) const {return BstBase<KT>::isleaf(node);}
		virtual SentinelBase* RotateLeft(SentinelBase* root, SentinelBase* node) 
			{ root=BstBase<KT>::RotateLeft(root,node); RbBase::Rotator(node); return root;}
		virtual SentinelBase* RotateRight(SentinelBase* root, SentinelBase* node) 
			{ root=BstBase<KT>::RotateRight(root,node); RbBase::Rotator(node); return root;}
		virtual void Update(SentinelBase* node,SentinelBase* pnode,bool erasing=false) { }
		virtual SentinelBase* remove(SentinelBase* root,SentinelBase* pfound){return SentinelBase::remove(root,pfound);}
		virtual SentinelBase* erase(SentinelBase* root,SentinelBase* found){return RbBase::erase(root,found);}
		virtual operator SentinelBase& () {return *this;}
	};

	template <typename KT,typename VT>
		struct RbMap : public RbMapBase<KT,VT>
	{
		typedef RbMapBase<KT,VT> TB;

		RbMap(const KT _key) : TB(_key) {}
		RbMap(const KT _key,const VT _data) : TB(_key,_data) {}

		virtual SentinelBase* red(SentinelBase* n) 
		{ 
			if (!n) return n; 
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Red();
			return n;
		}
		virtual SentinelBase* black(SentinelBase* n) 
		{ 
			if (!n) return n; 
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Black();
			return n;
		}
		virtual const RbBase::COLOR color(SentinelBase* n) const
		{
			if (!n) return RbBase::NONE; 
			TB& nd(static_cast<TB&>(*n)); 
			return nd.clr;
		}
	};

	template <typename KT>
		struct RbSet : public RbSetBase<KT>
	{
		typedef RbSetBase<KT> TB;
		RbSet(const KT _key) : TB(_key) {}
		virtual SentinelBase* red(SentinelBase* n) 
		{ 
			if (!n) return n; 
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Red();
			return n;
		}
		virtual SentinelBase* black(SentinelBase* n) 
		{ 
			if (!n) return n; 
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Black();
			return n;
		}
		virtual const RbBase::COLOR color(SentinelBase* n) const
		{
			if (!n) return RbBase::NONE; 
			TB& nd(static_cast<TB&>(*n)); 
			return nd.clr;
		}
	};
} //namespace TreeObjects
#endif // DIGITAL_ARBORIST_H


