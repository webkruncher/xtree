#ifndef KRUNCH_RB_TREE_H
#define KRUNCH_RB_TREE_H

namespace KrunchRbTree
{

	struct RbTreeBase
	{
		virtual bool operator<(const RbTreeBase&) = 0;
		virtual ~RbTreeBase(); 
		virtual RbTreeBase* insert(RbTreeBase*) = 0;
		RbTreeBase() : parent(NULL), left(NULL), right(NULL) {}
		public:
		RbTreeBase *parent,*left,*right;
		virtual ostream& operator<<(ostream&) const =0;
	};
	inline ostream& operator<<(ostream& o,const RbTreeBase& b) {return b.operator<<(o);}

	template <typename KT,typename VT>
		struct RbTree : public RbTreeBase
	{
		RbTree(const KT _key) : key(_key) {}
		RbTree(const KT _key,const VT _data) : key(_key),data(_data) {}
		virtual ~RbTree() {cout<<"~"<<key<<" ";}
		virtual RbTreeBase* insert(RbTreeBase* n)
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
		virtual bool operator<(const RbTreeBase& _b) 
		{ 
			const RbTree<KT,VT>& a(static_cast<RbTree<KT,VT>&>(*this));
			RbTreeBase& __b(const_cast<RbTreeBase&>(_b));
			const RbTree<KT,VT>& b(static_cast<RbTree<KT,VT>&>(__b));
			return a.key<b.key; 
		}
		operator const KT (){return key;}
		virtual ostream& operator<<(ostream& o) const {o<<key;return o;}
		private:
		const KT key;
		char color;
		VT data;	
	};

	RbTreeBase::~RbTreeBase() { }
} //namespace KrunchRbTree
#endif // KRUNCH_RB_TREE_H


