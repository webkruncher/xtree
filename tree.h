#ifndef KRUNCH_RB_TREE_H
#define KRUNCH_RB_TREE_H

namespace TreeeObjects
{

	struct TreeBase
	{
		virtual bool operator<(const TreeBase&) = 0;
		virtual bool operator==(const TreeBase&) = 0;
		virtual ~TreeBase(); 
		//virtual TreeBase* insert(TreeBase*,TreeBase*,TreeBase* p=NULL,int depth=0) = 0;
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
		virtual ~Bst() {cout<<"~"<<key<<" ";}
		//virtual TreeBase* insert(TreeBase* root,TreeBase* n,TreeBase* p=NULL,int _depth=0)
		virtual TreeBase* insert(TreeBase* root,TreeBase* n,int _depth=0)
		{
			depth=_depth+1;
			if ((*n)==(*this)) {delete n; return NULL;}
			n->parent=this;
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
		operator const KT (){return key;}
		virtual ostream& operator<<(ostream& o) const {o<<key;return o;}
		private:
		const KT key;
		char color;
		VT data;	
	};

	TreeBase::~TreeBase() { }
} //namespace TreeeObjects
#endif // KRUNCH_RB_TREE_H


