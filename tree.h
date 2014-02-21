#ifndef KRUNCH_RB_TREE_H
#define KRUNCH_RB_TREE_H

namespace TreeObjects
{
	struct TreeBase
	{
		virtual bool operator<(const TreeBase&) = 0;
		virtual bool operator==(const TreeBase&) = 0;
		virtual ~TreeBase() {}
		virtual TreeBase* insert(TreeBase*,TreeBase*,int depth=0) = 0;
		TreeBase() : parent(NULL), left(NULL), right(NULL),depth(0) {}
		public:
		TreeBase *parent,*left,*right;
		int depth;
		virtual ostream& operator<<(ostream&) const =0;
	};
	inline ostream& operator<<(ostream& o,const TreeBase& b) {return b.operator<<(o);}

	template <typename KT,typename VT>
		struct Bst : public TreeBase
	{
		Bst(const KT _key) : key(_key) {}
		Bst(const KT _key,const VT _data) : key(_key),data(_data) {}
		virtual ~Bst() {if (left) delete left; if (right) delete right; cout<<"~"<<key<<" ";}
		virtual TreeBase* insert(TreeBase* root,TreeBase* n,int _depth=0)
		{
			depth=_depth+1;
			if ((*n)==(*this)) {delete n; return NULL;}
			n->parent=this;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*n));
			nd.data(key,*this,parent);
			if (((int)floor(nd.key))%2) nd.data(0XFF00); else nd.data(0XFF);
			if ((*n)<(*this))
			{
				if (left) return left->insert(root,n,depth); else left=n;
			} else {
				if (right) return right->insert(root,n,depth); else right=n;
			}
			return root; // Return this if this needs to become the new root
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
		virtual ostream& operator<<(ostream& o) const {o<<key;return o;}
		const KT key;
		VT data;	
	};

	template <typename KT,typename VT>
		struct RbTree : public Bst<KT,VT>
	{
		#define RED 1
		#define BLACK 2 
		RbTree(const KT _key) : Bst<KT,VT>(_key) {}
		RbTree(const KT _key,const VT _data) : Bst<KT,VT>(_key,_data) {}
		virtual TreeBase* insert(TreeBase* root,TreeBase* node,int _depth=0)
		{
			this->depth=_depth+1;
			if ((*node)==(*this)) {delete node; return NULL;}
			node->parent=this;
			RbTree<KT,VT>& nd(static_cast<RbTree<KT,VT>&>(*node));
			nd.data(this->key,*this,this->parent);
			if ((*node)<(*this))
			{
				if (this->left) 
				{
					TreeBase* insrt(this->left->insert(root,node,this->depth)); 
					return color(insrt);
				} else { 
					this->left=node;
					color(node);
				}
			} else {
				if (this->right) 
				{
					TreeBase* insrt(this->right->insert(root,node,this->depth)); 
					return color(insrt);
				} else { 
					this->right=node;
					color(node);
				}
			}
			color(this);
			RbTree<KT,VT>& rd(static_cast<RbTree<KT,VT>&>(*root)); 
			rd.data(0XFF00); 
			return root; // Return this if this needs to become the new root
		}
		TreeBase* rotateleft(TreeBase* rnode)
		{
			return rnode;
		}
		private:
		char clr;
		TreeBase* color(TreeBase* n)
		{
			if ( ((!n->left) and (!n->right)) or (!n->parent)) 
			{
				return black(n); 
			} else {
				if ( (!n->left) or (!n->right) ) 
				{
					return black(n); 
				} else {
					return red(n);
				}
			}
		}
		TreeBase* red(TreeBase* n) 
		{ 
			if (!n) return n; 
			RbTree<KT,VT>& nd(static_cast<RbTree<KT,VT>&>(*n)); 
			nd.clr=RED;
			nd.data(0XFF0000); 
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
	};
} //namespace TreeObjects
#endif // KRUNCH_RB_TREE_H


