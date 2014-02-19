#ifndef KRUNCH_RB_TREE_H
#define KRUNCH_RB_TREE_H

namespace TreeeObjects
{

	struct TreeBase
	{
		virtual bool operator<(const TreeBase&) = 0;
		virtual ~TreeBase(); 
		virtual TreeBase* insert(TreeBase*) = 0;
		TreeBase() : parent(NULL), left(NULL), right(NULL) {}
		public:
		TreeBase *parent,*left,*right;
		virtual ostream& operator<<(ostream&) const =0;
	};
	inline ostream& operator<<(ostream& o,const TreeBase& b) {return b.operator<<(o);}

	template <typename KT,typename VT>
		struct Bst : public TreeBase
	{
		Bst(const KT _key) : key(_key) {}
		Bst(const KT _key,const VT _data) : key(_key),data(_data) {}
		virtual ~Bst() {cout<<"~"<<key<<" ";}
		virtual TreeBase* insert(TreeBase* n)
		{
			if ((*n)<(*this))
			{
				if (left) left->insert(n); else left=n;
				left->parent=this;
			} else {
				if (right) right->insert(n); else right=n;
				right->parent=this;
			}
			return this;
		}
		virtual bool operator<(const TreeBase& _b) 
		{ 
			const Bst<KT,VT>& a(static_cast<Bst<KT,VT>&>(*this));
			TreeBase& __b(const_cast<TreeBase&>(_b));
			const Bst<KT,VT>& b(static_cast<Bst<KT,VT>&>(__b));
			return a.key<b.key; 
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


