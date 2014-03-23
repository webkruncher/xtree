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

	struct Trunk
	{
		virtual const bool isnil()  = 0;
		const bool isnul(Trunk* node)  
		{
			if (!node) return true;
			if (node->isnil()) return true;
			return false;	
		}
		virtual bool operator<(const Trunk&) =0;//{return false;}
		virtual bool operator==(const Trunk&) =0;//{return false;}
		virtual Trunk* insert(Trunk* root,Trunk*,char d=0) {return NULL;}
		virtual bool isBST(Trunk* node) {return false;}
		virtual void Update(Trunk* node,Trunk* pnode,bool erasing=false) {}
		virtual Trunk* LeftMost(){return NULL;}
		virtual Trunk* RightMost(){return NULL;}
		virtual Trunk* Predecessor(){return NULL;}
		virtual Trunk* Successor(){return NULL;}
		virtual const bool isleaf(Trunk* node) const {return false;}
		virtual long countnodes() {return 0;}
		virtual Trunk* RotateLeft(Trunk* root, Trunk* node){return root;}
		virtual Trunk* RotateRight(Trunk* root, Trunk* node){return root;}
		virtual Trunk* remove(Trunk* root,Trunk* pfound) {return root;}
		virtual Trunk* transplant(Trunk* root,Trunk* u,Trunk* v){return root;}
		virtual Trunk* Parent() = 0;
		virtual void SetParent(Trunk* p) = 0;
		virtual Trunk* Left() = 0;
		virtual void SetLeft(Trunk* l) = 0;
		virtual Trunk* Right() = 0;
		virtual void SetRight(Trunk* r) = 0;
		virtual Trunk* GetNil() = 0;
		virtual Trunk& operator<<(const Trunk& a) {return *this;}
		virtual Trunk& operator<<(const char* a){return *this;}
		virtual Trunk& operator<<(const int a){return *this;}
		virtual Trunk& Begin(){return *this;}
		virtual Trunk& Finish(){return *this;}
		virtual Trunk& Insrt(){return *this;}
		virtual Trunk& Erse(){return *this;}
		virtual Trunk& Rotlft(){return *this;}
		virtual Trunk& Rotrgt(){return *this;}
		virtual Trunk& Trnsp(){return *this;}
		virtual Trunk& DblRedFix(){return *this;}
		virtual Trunk& operator()(char*,int){return *this;}
	};

	inline Trunk& operator<<(Trunk& t,Trunk& a)					{return t.operator<<(a);}
	inline Trunk& operator<<(Trunk& t,const char* a)		{return t.operator<<(a);}
	inline Trunk& operator<<(Trunk& t,int a)						{return t.operator<<(a);}
	inline Trunk& begin(Trunk& t)												{return t.Begin();}
	inline Trunk& finish(Trunk& t)											{return t.Finish();}
	inline Trunk& insrt(Trunk& t)												{return t.Insrt();}
	inline Trunk& erse(Trunk& t)												{return t.Erse();}
	inline Trunk& rotlft(Trunk& t)											{return t.Rotlft();}
	inline Trunk& rotrgt(Trunk& t)											{return t.Rotrgt();}
	inline Trunk& trnsp(Trunk& t)												{return t.Trnsp();}
	inline Trunk& dblredfix(Trunk& t)										{return t.DblRedFix();}
	inline Trunk& operator<<(Trunk& t,Trunk& (*pf)(Trunk&) ){return (*pf)(t);}
	#define Msg (*this->GetNil())
	#define Trace Msg(__FILE__,__LINE__);

	struct TreeBase : Trunk
	{
		TreeBase(Trunk& _sentinel) : sentinel(_sentinel),left(NULL),right(NULL),parent(NULL) {}
		~TreeBase() {if (left) if (left!=&sentinel) delete left; if (right) if (right!=&sentinel) delete right; left=NULL; right=NULL;}
		bool operator<(const Trunk&) = 0;
		bool operator==(const Trunk&) = 0;
		Trunk* insert(Trunk* root,Trunk*,char d=0) = 0;
		bool isBST(Trunk* node) = 0;
		void Update(Trunk* node,Trunk* pnode,bool erasing=false) = 0;
		Trunk* LeftMost();
		Trunk* RightMost();
		Trunk* Predecessor();
		Trunk* Successor();
		const bool isleaf(Trunk* node) const;
		virtual const bool isnil() {return false;}
		long countnodes() ; 
		Trunk* RotateLeft(Trunk* root, Trunk* node);
		Trunk* RotateRight(Trunk* root, Trunk* node);
		Trunk* remove(Trunk* root,Trunk* pfound) = 0;
		Trunk* transplant(Trunk* root,Trunk* u,Trunk* v);
		Trunk* Parent(){return parent;}
		void SetParent(Trunk* p) { parent=p; }
		Trunk* Left(){return left;}
		void SetLeft(Trunk* l){left=l;}
		Trunk* Right(){return right;}
		void SetRight(Trunk* r){right=r;}
		virtual Trunk* GetNil() {return &sentinel;}
		private: Trunk& sentinel; Trunk *parent,*left,*right;
		TreeBase& operator=(const TreeBase&){parent=NULL;left=NULL;right=NULL;} 
		TreeBase(const TreeBase& a) : sentinel(a.sentinel), parent(a.parent), left(a.left), right(a.right) {}
	};

	struct Sentinel : Trunk
	{
		virtual bool operator<(const Trunk&) {return false;}
		virtual bool operator==(const Trunk&){return false;}
		virtual const bool isnil() { return true; }
		Trunk* Parent(){return NULL;}
		void SetParent(Trunk*){}
		Trunk* Left(){return NULL;}
		void SetLeft(Trunk*){}
		Trunk* Right(){return NULL;}
		void SetRight(Trunk*){}
		virtual Trunk* GetNil() {return this;}
	};


	inline Trunk* TreeBase::LeftMost()
	{
		Trunk* current = this;
		while (!isnul(current->Left())) current = current->Left();
		return current;
	}

	inline Trunk* TreeBase::RightMost()
	{
		Trunk* current = this;
		while (!isnul(current->Right())) current = current->Right();
		return current;
	}

	inline Trunk* TreeBase::Predecessor()
	{
		if (!isnul(Left())) return Left()->RightMost();
		Trunk* y(Parent());
		Trunk* x(this);
		while ((!isnul(y->Parent())) and (!y->Left()->isnil()) and x==y->Left()) { x=y; y=y->Parent(); }
		return y;
	}

	inline Trunk* TreeBase::Successor()
	{
		if (!isnul(Right())) return Right()->LeftMost();
		Trunk* y(Parent());
		Trunk* x(this);
		while ((!isnul(y->Parent())) and (!y->Left()->isnil())  and x==y->Right()) { x=y; y=y->Parent(); }
		return y;
	}

	inline const bool TreeBase::isleaf(Trunk* node) const
	{
		if 
		(
			((!node->Left()) or (node->Left()->isnil()) )
				and 
			((!node->Right()) or (node->Right()->isnil()) )
		) 
			return true; else return false;	
	}

	inline long TreeBase::countnodes()  
	{
		long leftnodes(0); if (!isnul(Left())) leftnodes=Left()->countnodes();
		long rightnodes(0); if (!isnul(Right())) rightnodes=Right()->countnodes();
		return (leftnodes+rightnodes+1);
	}

	inline Trunk* TreeBase::RotateLeft(Trunk* root, Trunk* node)
	{
		if (this->isnul(node)) return root;
		Trunk* other(node->Right());
		if (this->isnul(other)) return root;
		Msg<<(*node)<<rotlft<<(*other);
		node->SetRight(other->Left());
		if ( !isnul(other->Left()) ) other->Left()->SetParent(node);
		other->SetParent(node->Parent());
		if ( isnul(node->Parent()) ) 
		{
			Trace
			root = other;
		} else {
			Trace
			if ( node == node->Parent()->Left() ) node->Parent()->SetLeft(other);
			else node->Parent()->SetRight(other);
		}
		other->SetLeft(node);
		node->SetParent(other);
		root->SetParent(GetNil());
		Update(node,node->Parent());
		Update(other,other->Parent());
		return root;
	}

	inline Trunk* TreeBase::RotateRight(Trunk* root, Trunk* node)
	{
		if (this->isnul(node)) return root;
		Trunk* other(node->Left());
		if (this->isnul(other)) return root;
		Msg<<(*node)<<rotrgt<<(*other);
		node->SetLeft(other->Right());
		if ( !isnul(other->Right()) ) other->Right()->SetParent(node);
		other->SetParent(node->Parent());
		if ( isnul(node->Parent()) ) 
		{
			Trace
			root = other;
		} else  {
			Trace
			if ( node == node->Parent()->Right() ) node->Parent()->SetRight(other);
			else node->Parent()->SetLeft(other);
		}
		other->SetRight(node);
		node->SetParent(other);
		root->SetParent(GetNil());
		Update(node,node->Parent());
		Update(other,other->Parent());
		return root;
	}

	template <typename KT>
		struct BstBase : public TreeBase
	{
		BstBase(Trunk& _sentinel,const KT _key) : TreeBase(_sentinel),key(_key) {}
		KT minValue(Trunk* node) 
		{
			Trunk* current(node);
			while (!isnul(current->Left())) current=current->Left();
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*current));
			return(nd.key);
		}
		KT maxValue(Trunk* node) 
		{
			Trunk* current = node;
			while (!isnul(current->Right())) current = current->Right();
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*current));
			return(nd.key);
		}

		virtual bool isBST(Trunk* node) 
		{
			if (isnul(node)) return(true);
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*node));
			if ((!isnul(node->Left())) && maxValue(node->Left()) > nd.key) return(false);
			if ((!isnul(node->Right())) && minValue(node->Right()) <= nd.key) return(false);
			if (!isBST(node->Left()) || !isBST(node->Right())) return(false);
			return(true);
		}

		virtual BstBase<KT>* find(const KT& what)
		{	
			BstBase<KT>& nd(static_cast<BstBase<KT>&>(*this));
			if (nd==what) return this;
			if (nd<what)
			{
				if (!isnul(Right()))
				{
					BstBase<KT>& ld(static_cast<BstBase<KT>&>(*Right()));
					return ld.find(what);
				}
			} else {
				if (!isnul(Left()))
				{
					BstBase<KT>& rd(static_cast<BstBase<KT>&>(*Left()));
					return rd.find(what);
				}
			}
			return NULL;
		}

		virtual void Update(Trunk* node,Trunk* pnode,bool erasing=false) { }

		virtual Trunk* erase(Trunk* root,Trunk* found)
		{	
			Msg<<begin;
			if (root->isnul(found)) return root;
			if (found->isnil()) return root;
			Trunk *p(found->Parent()),*l(found->Left()),*r(found->Right());
			Trunk* newroot(remove(root,found));
			found->SetLeft(NULL); found->SetRight(NULL);
			if (newroot)
			{
				if (!newroot->isnul(p)) Update(found,p); 
				if (!newroot->isnul(l)) Update(l,found); 
				if (!newroot->isnul(r)) Update(r,found); 
			}
			delete found;
			Msg<<finish;
			return newroot;
		}

		Trunk* insert(Trunk* root,Trunk* node,char d=0)
		{
			if ((*node)==(*this)) {delete node; return NULL;}
			node->SetParent(this);
			if (!d)
			{
				node->SetLeft(this->GetNil());
				node->SetRight(this->GetNil());
			}
			Update(node,this);
			if ((*node)<(*this))
			{
				if (!this->isnul(this->Left())) 
				{
					return this->Left()->insert(root,node,d+1);
				} else { 
					this->SetLeft(node);
				}
			} else {
				if (!this->isnul(this->Right()))
				{
					return this->Right()->insert(root,node,d+1);
				} else { 
					this->SetRight(node);
				}
			}
			return root;
		}

		virtual bool operator<(const Trunk& that) 
		{ 
			const BstBase<KT>& a(static_cast<BstBase<KT>&>(*this));
			Trunk& other(const_cast<Trunk&>(that));
			const BstBase<KT>& b(static_cast<BstBase<KT>&>(other));
			return a.key<b.key; 
		}

		virtual bool operator==(const Trunk& that) 
		{ 
			const BstBase<KT>& a(static_cast<BstBase<KT>&>(*this));
			Trunk& other(const_cast<Trunk&>(that));
			const BstBase<KT>& b(static_cast<BstBase<KT>&>(other));
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

		virtual const bool isleaf(Trunk* node) const {return TreeBase::isleaf(node);}

		virtual Trunk* RotateLeft(Trunk* root, Trunk* node)
		{
			Trunk* newroot(TreeBase::RotateLeft(root,node));
			this->Update(node,this->Parent()); 
			return newroot;
		}

		virtual Trunk* RotateRight(Trunk* root, Trunk* node)
		{
			Trunk* newroot(TreeBase::RotateRight(root,node));
			this->Update(node,this->Parent()); 
			return newroot;
		}
		protected:
		const KT key;
	};

	template <typename KT>
		struct Iterator
	{
		Iterator(Trunk* _node) : node(_node) {Invalidate();}
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
			if (!root) return;
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
		Trunk *root,*begin,*end;
		Trunk* Root()
		{
			if (!node) NULL;
			Trunk* current(node);
			while (current and current->Parent() and (!current->Parent()->isnil()) ) current=current->Parent();
			return current;
		}
		Trunk* node;
	};

	inline Trunk* TreeBase::transplant(Trunk* root,Trunk* u,Trunk* v)
	{
		if (u and v) Msg<<(*u)<<trnsp<<(*v);
		Trunk* ret(root);
		if (root->isnul(u->Parent()))
		{
			Trace
			ret=v;
		} else {
			if (u==u->Parent()->Left())
			{
				Trace
				u->Parent()->SetLeft(v);
			} else {
				Trace
				u->Parent()->SetRight(v);
			}
		}
		if (v) v->SetParent(u->Parent());
		Update(u,u->Parent());
		if (v) Update(v,v->Parent());
		return ret;
	}

	inline Trunk* TreeBase::remove(Trunk* root,Trunk* pfound)
	{
		if (isnul(pfound->Left()))
		{
			root=transplant(root,pfound,pfound->Right());
		} else {
			if (isnul(pfound->Right()))
			{
				root=transplant(root,pfound,pfound->Left());
			} else {
				Trunk* y(pfound->Right()->LeftMost());
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
		if (isnul(root)) return NULL;
		return root;
	}


	template <typename KT,typename VT>
		struct Bst : public BstBase<KT>
	{
		Bst(Trunk& _sentinel,const KT _key) : BstBase<KT>(_sentinel,_key) {}
		Bst(Trunk& _sentinel,const KT _key,const VT _data) : BstBase<KT>(_sentinel,_key), data(_data) {}
		VT& Data(){return data;}
		protected:
		virtual void Update(Trunk* node,Trunk* pnode,bool erasing=false) { }
		VT data;	
		virtual Trunk* remove(Trunk* root,Trunk* pfound) { return TreeBase::remove(root,pfound); }
	};


	struct RbBase 
	{
		enum COLOR {RED=10,BLACK=20} ;
		const COLOR Red() const {return RED;}
		const COLOR Black() const {return BLACK;}
		void CopyColor(Trunk* that,Trunk* target) { if (color(that)==BLACK) black(target); else red(target);}
		virtual Trunk* red(Trunk* n) = 0;
		virtual Trunk* black(Trunk* n) = 0;
		virtual const bool isleaf(Trunk* node) const = 0;
		virtual Trunk* GetNil() = 0;
		virtual Trunk* RotateLeft(Trunk* root, Trunk* node) = 0;
		virtual Trunk* RotateRight(Trunk* root, Trunk* node) = 0;
		virtual const COLOR color(Trunk* n) const = 0;
		virtual void Update(Trunk* node,Trunk* pnode,bool erasing=false) = 0;
		virtual operator Trunk& () = 0;
		virtual Trunk* remove(Trunk* root,Trunk* pfound) ;

		void Rotator(Trunk* node) {Update(node,node->Parent());}

		Trunk* RedAndBlackDelete(Trunk* root, Trunk* node)
		{
			Trace
			while ( (node!=root) and (color(node) == BLACK) )																	// 1
			{
				Trace
				if (!node) return black(root);
				if (node->isnil()) {node=node->Parent(); continue;}
				if ( node == node->Parent()->Left() )																						// 2
				{
					Trace
					Trunk* other(node->Parent()->Right());																				// 3
					if ( color(other) == RED )																										// 4
					{
						Trace
						black(other);																																// 5
						red(node->Parent());																												// 6
						root=this->RotateLeft(root,node->Parent());																	// 7
						other=node->Parent()->Right();																							// 8
					}
					if ( (color(other->Left())==BLACK) and (color(other->Right())==BLACK) )				// 9
					{
						Trace
						red(other);																																	// 10
						node=node->Parent();																												// 11
					} else {
						Trace
						if (color(other->Right()) == BLACK)																					// 12
						{
							Trace
							black(other->Left());																											// 13
							red(other);																																// 14
							root=this->RotateRight(root,other);																				// 15
							other=node->Parent()->Right();																						// 16
						}
						Trace
						CopyColor(node->Parent(),other);																						// 17
						black(node->Parent());																											// 18
						black(other->Right());																											// 19
						root=this->RotateLeft(root,node->Parent());																	// 20
						node=root;																																	// 21
					}
				} 
				if ( node == node->Parent()->Right() )																					// 2
				{
					Trace
					Trunk* other(node->Parent()->Left());																					// 3
					if ( color(other) == RED )																										// 4
					{
						Trace
						black(other);																																// 5
						red(node->Parent());																												// 6
						root=this->RotateRight(root,node->Parent());																// 7
						other=node->Parent()->Left();																								// 8
					}
					if ( (color(other->Left())==BLACK) and (color(other->Right())==BLACK) )				// 9
					{
						Trace
						red(other);																																	// 10
						node=node->Parent();																												// 11
					} else {
						Trace
						if (color(other->Left()) == BLACK)																					// 12
						{
							Trace
							black(other->Right());																										// 13
							red(other);																																// 14
							root=this->RotateLeft(root,other);																				// 15
							other=node->Parent()->Left();																							// 16
						}
						Trace
						CopyColor(node->Parent(),other);																						// 17
						black(node->Parent());																											// 18
						black(other->Left());																												// 19
						root=this->RotateRight(root,node->Parent());																// 20
						node=root;																																	// 21
					}
				} 
			}
			Trace
			black(node);																																			// 23
			return (root);
		}

		Trunk* RedAndBlackInsert(Trunk* root, Trunk* node)
		{
			black(root); 
			red(node);
			while ( color(node->Parent()) == RED ) 
			{
				if ( node->Parent() == node->Parent()->Parent()->Left() ) 
				{
					Trunk* other(node->Parent()->Parent()->Right());
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
					Trunk* other(node->Parent()->Parent()->Left());
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

		virtual Trunk* erase(Trunk* root,Trunk* found)
		{
			Msg<<begin;
			Trace
			Trunk *p(found->Parent()),*l(found->Left()),*r(found->Right());
			Trunk* newroot(remove(root,found));
			if (newroot)
			{
				if (!root->isnul(found)) Update(found,p,true); 
				if (!root->isnul(newroot)) Update(newroot,newroot); 
				if (!root->isnul(l)) Update(l,p); if (!root->isnul(r)) Update(r,p);
			}
			found->SetLeft(NULL); found->SetRight(NULL);
			delete found;
			Msg<<finish;
			return newroot;
		}
		COLOR clr;
	};

	inline Trunk* RbBase::remove(Trunk* root,Trunk* pfound)
	{
		if (pfound) Msg<<erse<<(*pfound); 
		Trace
		Trunk& me(static_cast<Trunk&>(*this));
		Trunk* Y(pfound);																		// 1
		Trunk* X(NULL);
		char Ycolor(this->color(Y));												// 2
		if (root->isnul(pfound->Left()))										// 3
		{
			Trace
			X=pfound->Right();																// 4
			root=me.transplant(root,pfound,pfound->Right());	// 5
		} else {
			if (root->isnul(pfound->Right()))									// 6
			{
				Trace
				X=pfound->Left();																// 7
				root=me.transplant(root,pfound,pfound->Left());	// 8
			} else {
				Trace
				Y=(pfound->Right()->LeftMost());								// 9
				Ycolor=(this->color(Y));												// 10
				X=Y->Right();																		// 11
				if (Y->Parent()==pfound)												// 12
				{
					Trace
					X->SetParent(Y);															// 13
				} else {
					root=me.transplant(root,Y,Y->Right());				// 14
					Y->SetRight(pfound->Right());									// 15
					Y->Right()->SetParent(Y);											// 16
				}
				root=me.transplant(root,pfound,Y);							// 17
				Y->SetLeft(pfound->Left());											// 18
				Y->Left()->SetParent(Y);												// 19
				if (color(pfound)==BLACK) black(Y); else red(Y);// 20
			}
		}
		if (Ycolor==BLACK)																	// 21
			if ((root) and (X)) return RedAndBlackDelete(root,X);//22
		Trace
		return root;
	}


	template <typename KT,typename VT>
		struct RbMapBase : public Bst<KT,VT>, RbBase
	{
		RbMapBase(Trunk& _sentinel,const KT _key) : Bst<KT,VT>(_sentinel,_key) {}
		RbMapBase(Trunk& _sentinel,const KT _key,const VT _data) : Bst<KT,VT>(_sentinel,_key,_data) {}
		virtual void DisplayColor(const unsigned long ) {}

		Trunk* insert(Trunk* root,Trunk* node,char d=0)
		{
			Msg<<begin;
			//if (node) {Msg<<insrt<<(*node); Msg.operator<<((int)d);}
			//Trace
			root=Bst<KT,VT>::insert(root,node,d);
			if (!root) return NULL; // attempted to add a duplicate, new node was deleted
			if (d) return black(root);
			RbMapBase<KT,VT>& nd(static_cast<RbMapBase<KT,VT>&>(*node));
			nd.SetLeft(root->GetNil());
			nd.SetRight(root->GetNil());
			Trunk* ret(this->RedAndBlackInsert(root,node));
			Msg<<finish;
			return ret;
		}
		
		virtual Trunk* red(Trunk* n) = 0;
		virtual Trunk* black(Trunk* n) = 0;
		virtual const COLOR color(Trunk* n) const = 0;

		virtual const bool isleaf(Trunk* node) const {return BstBase<KT>::isleaf(node);}
		virtual Trunk* RotateLeft(Trunk* root, Trunk* node) 
			{ root=BstBase<KT>::RotateLeft(root,node); RbBase::Rotator(node); return root;}
		virtual Trunk* RotateRight(Trunk* root, Trunk* node) 
			{ root=BstBase<KT>::RotateRight(root,node); RbBase::Rotator(node); return root;}
		virtual void Update(Trunk* node,Trunk* pnode,bool erasing=false) { Bst<KT,VT>::Update(node,pnode,erasing); }
		virtual Trunk* remove(Trunk* root,Trunk* pfound)  
		{
			Bst<KT,VT>& b(static_cast<Bst<KT,VT>&>(*pfound));
			return RbBase::remove(root,pfound);
		}
		virtual Trunk* erase(Trunk* root,Trunk* found){return RbBase::erase(root,found);}
		virtual operator Trunk& () {return *this;}
		virtual Trunk* GetNil() {return BstBase<KT>::GetNil();}
	};


	template <typename KT>
		struct RbSetBase : public BstBase<KT>, RbBase
	{
		RbSetBase(Trunk& _sentinel,const KT _key) : BstBase<KT>(_sentinel,_key) {}
		virtual void DisplayColor(const unsigned long ) {}

		Trunk* insert(Trunk* root,Trunk* node,char d=0)
		{
			root=BstBase<KT>::insert(root,node,d);
			if (!root) return NULL; // attempted to add a duplicate, new node was deleted
			if (d) return black(root);
			return this->RedAndBlackInsert(root,node);
		}
		
		virtual Trunk* red(Trunk* n) = 0;
		virtual Trunk* black(Trunk* n) = 0;
		virtual const COLOR color(Trunk* n) const = 0;

		virtual const bool isleaf(Trunk* node) const {return BstBase<KT>::isleaf(node);}
		virtual Trunk* RotateLeft(Trunk* root, Trunk* node) 
			{ root=BstBase<KT>::RotateLeft(root,node); RbBase::Rotator(node); return root;}
		virtual Trunk* RotateRight(Trunk* root, Trunk* node) 
			{ root=BstBase<KT>::RotateRight(root,node); RbBase::Rotator(node); return root;}
		virtual void Update(Trunk* node,Trunk* pnode,bool erasing=false) { }
		virtual Trunk* remove(Trunk* root,Trunk* pfound)
		{
			BstBase<KT>& b(static_cast<BstBase<KT>&>(*pfound));
			return BstBase<KT>::remove(root,pfound);
			//return Trunk::remove(root,pfound);
		}
		virtual Trunk* erase(Trunk* root,Trunk* found){return RbBase::erase(root,found);}
		virtual operator Trunk& () {return *this;}
		virtual Trunk* GetNil() {return BstBase<KT>::GetNil();}
	};

	template <typename KT,typename VT>
		struct RbMap : public RbMapBase<KT,VT>
	{
		typedef RbMapBase<KT,VT> TB;

		RbMap(Trunk& _sentinel,const KT _key) : TB(_sentinel,_key) {}
		RbMap(Trunk& _sentinel,const KT _key,const VT _data) : TB(_sentinel,_key,_data) {}
		void DisplayColor(const unsigned long ) {}

		virtual Trunk* red(Trunk* n) 
		{ 
			if (!n) return n; 
			if (n->isnil()) return n;
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Red();
			if (!n->Left()) n->SetLeft(this->GetNil());
			if (!n->Right()) n->SetRight(this->GetNil());
			if (n->isnil()) return n;
			nd.DisplayColor(0X800000);
			this->Update(n,n->Parent());
			return n;
		}

		virtual Trunk* black(Trunk* n) 
		{ 
			if (!n) return n; 
			if (n->isnil()) return n;
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Black();
			if (!n->Left()) n->SetLeft(this->GetNil());
			if (!n->Right()) n->SetRight(this->GetNil());
			if (n->isnil()) return n;
			nd.DisplayColor(0X000000);
			this->Update(n,n->Parent());
			return n;
		}

		virtual const RbBase::COLOR color(Trunk* n) const
		{
			if (!n) return RbBase::BLACK; 
			if (n->isnil()) return RbBase::BLACK;
			TB& nd(static_cast<TB&>(*n)); 
			return nd.clr;
		}
	};

	template <typename KT>
		struct RbSet : public RbSetBase<KT>
	{
		typedef RbSetBase<KT> TB;
		RbSet(Trunk& _sentinel,const KT _key) : TB(_sentinel,_key) {}
		virtual Trunk* red(Trunk* n) 
		{ 
			if (!n) return n; 
			if (n->isnil()) return n;
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Red();
			if (!n->Left()) n->SetLeft(this->GetNil());
			if (!n->Right()) n->SetRight(this->GetNil());
			if (n->isnil()) return n;
			this->Update(n,n->Parent());
			return n;
		}
		virtual Trunk* black(Trunk* n) 
		{ 
			if (!n) return n; 
			if (n->isnil()) return n;
			TB& nd(static_cast<TB&>(*n)); 
			nd.clr=this->Black();
			if (!n->Left()) n->SetLeft(this->GetNil());
			if (!n->Right()) n->SetRight(this->GetNil());
			if (n->isnil()) return n;
			this->Update(n,n->Parent());
			return n;
		}
		virtual const RbBase::COLOR color(Trunk* n) const
		{
			if (!n) return RbBase::BLACK; 
			if (n->isnil()) return RbBase::BLACK;
			TB& nd(static_cast<TB&>(*n)); 
			return nd.clr;
		}
	};
} //namespace TreeObjects
#endif // DIGITAL_ARBORIST_H


