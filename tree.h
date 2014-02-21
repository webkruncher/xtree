#ifndef KRUNCH_RB_TREE_H
#define KRUNCH_RB_TREE_H

namespace TreeObjects
{
	struct TreeBase
	{
		virtual bool operator<(const TreeBase&) = 0;
		virtual bool operator==(const TreeBase&) = 0;
		virtual ~TreeBase() {}
		virtual void insert(TreeBase*,int depth=0) = 0;
		TreeBase() : parent(NULL), left(NULL), right(NULL),depth(0) {}

		virtual int minValue(TreeBase* node)  = 0;
		virtual int maxValue(TreeBase* node)  = 0;
		virtual int isBST(TreeBase* node) = 0;
		virtual long countnodes()  = 0;

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
		virtual long countnodes()  
		{
			long leftnodes(0); if (left) leftnodes=left->countnodes();
			long rightnodes(0); if (right) rightnodes=right->countnodes();
			return (leftnodes+rightnodes+1);
		}
		virtual int minValue(TreeBase* node) 
		{
			TreeBase* current = node;
			while (current->left != NULL) current = current->left;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*current));
			return(nd.key);
		}
		virtual int maxValue(TreeBase* node) 
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
		virtual void insert(TreeBase* n,int _depth=0)
		{
			depth=_depth+1;
			if ((*n)==(*this)) {delete n; return;}
			n->parent=this;
			Bst<KT,VT>& nd(static_cast<Bst<KT,VT>&>(*n));
			nd.data(key,*this,parent);
			if (((int)floor(nd.key))%2) nd.data(0XFF00); else nd.data(0XFF);
			if ((*n)<(*this))
			{
				if (left) {left->insert(n,depth); return ;}else left=n;
			} else {
				if (right) {right->insert(n,depth); return; } else right=n;
			}
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
		#define RED 1
		#define BLACK 2 
		RbTree(const KT _key) : Bst<KT,VT>(_key),ldepth(0),rdepth(0) {}
		RbTree(const KT _key,const VT _data) : Bst<KT,VT>(_key,_data),ldepth(0),rdepth(0) {}
		virtual void insert(TreeBase* node,int _depth=0)
		{
			this->depth=_depth+1;
			if ((*node)==(*this)) {delete node; }
			node->parent=this;
			RbTree<KT,VT>& nd(static_cast<RbTree<KT,VT>&>(*node));
			nd.data(this->key,*this,this->parent);
			if ((*node)<(*this))
			{
				if (this->left) 
				{
					this->left->insert(node,this->depth); 
					RbTree<KT,VT>& id(static_cast<RbTree<KT,VT>&>(*node)); 
					this->ldepth=0; DepthCounter(this->left,this->ldepth);
					int diff(id.ldepth-id.rdepth);
					if (abs(diff)>1)
					{
						if (diff<0) rotateleft(color(node));
						else rotateright(color(node));
					}
					color(node); return;
				} else { 
					this->left=node;
					this->ldepth=1;
					color(node);
				}
			} else {
				if (this->right) 
				{
					this->right->insert(node,this->depth); 
					RbTree<KT,VT>& id(static_cast<RbTree<KT,VT>&>(*node)); 
					this->rdepth=0; DepthCounter(this->right,this->rdepth);
					int diff(id.ldepth-id.rdepth);
					if (abs(diff)>1)
					{
						if (diff<0) rotateleft(color(node));
						else rotateright(color(node));
					}
					color(node); return;
				} else { 
					this->right=node;
					this->rdepth=1;
					color(node);
				}
			}
			color(this);
			RbTree<KT,VT>& rd(static_cast<RbTree<KT,VT>&>(*this)); 
			//rd.data(0X8000); 
			int diff(rd.ldepth-rd.rdepth);
		}
		void rotateleft(TreeBase* rnode)
		{
			//TreeBase* ret(rnode->right);
			//if (rnode->left)  ret=rnode->left;
			//ret->parent=rnode->right=NULL;
		}
		void rotateright(TreeBase* rnode)
		{
		}
		private:
		char clr;
		int ldepth,rdepth;

		void DepthCounter(TreeBase* n,int& d)
		{
			if (!n) return;
			RbTree<KT,VT>& nd(static_cast<RbTree<KT,VT>&>(*n)); 
			if (nd.clr==BLACK) d++;
			if (n->right) DepthCounter(n->right,d);
			if (n->left) DepthCounter(n->left,d);
		}

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
		virtual ostream& operator<<(ostream& o) const 
		{
			o<<ldepth<<","<<rdepth;
			return o;
		}
	};
} //namespace TreeObjects
#endif // KRUNCH_RB_TREE_H


