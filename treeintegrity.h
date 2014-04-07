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
#ifndef TREE_INTEGRITY
#define TREE_INTEGRITY
#include <stack>

#define BREAKPOINT asm ("int $0X03") ;
namespace TreeIntegrity
{
	struct IntegrityAdvisor
	{
		virtual void clear(Trunk&,string) = 0;
		virtual void message(Trunk&,string,string) = 0;
	};


	template<typename KT>
		inline bool AssertRelations(ostream& tout,Trunk* node)
	{
		BstBase<KT>& M(static_cast<BstBase<KT>&>(*node));
		const KT& key(M);
		Trunk* p(node->Parent());
		if (node->isnul(p)) return true;
		Trunk* l(p->Left());
		Trunk* r(p->Right());
		BstBase<KT>& P(static_cast<BstBase<KT>&>(*p));
		const KT& pk(P);
		if (!l->isnil()) 
		{
			BstBase<KT>& L(static_cast<BstBase<KT>&>(*l));
			const KT& lk(L);
			if (lk==key)
			{
				//tout<<key<<" is on the left side of it's parent, "<<pk<<endl;	
				return true;
			}
		}
		if (!r->isnil()) 
		{
			BstBase<KT>& R(static_cast<BstBase<KT>&>(*r));
			const KT& rk(R);
			if (rk==key)
			{
				//tout<<key<<" is on the right side of it's parent, "<<pk<<endl;	
				return true;
			}
		}
		tout<<key<<" is on neither side of it's parent node, "<<pk<<endl;
		return false;
		
	}

	template<typename KT>
		inline bool PrintInOrder(ostream& tout,Trunk* node)
	{
		if (!node) return false;
		if (node->isnil()) return false;
		if (!node->isnul(node->Left())) 
				if (!PrintInOrder<KT>(tout,node->Left())) return false;
		BstBase<KT>& rk(static_cast<BstBase<KT>&>(*node));
		const KT& v(rk); tout<<v<<" ";
		if (!node->isnul(node->Right())) 
				if (!PrintInOrder<KT>(tout,node->Right())) return false;
		return true;
	}	

	template<typename KT>
		inline bool InOrderAssertions(ostream& tout,Trunk* node)
	{
		if (!node) return false;
		if (node->isnil()) return false;
		if (!AssertRelations<KT>(tout,node)) return false;
		if (!node->isnul(node->Left())) 
				if (!InOrderAssertions<KT>(tout,node->Left())) return false;
		BstBase<KT>& rk(static_cast<BstBase<KT>&>(*node));
		//const KT& v(rk); tout<<v<<" ";
		if (!node->isnul(node->Right())) 
				if (!InOrderAssertions<KT>(tout,node->Right())) return false;
		return true;
	}	

	

	struct HiLowBase
	{
		virtual ostream& operator<<(ostream& o) = 0;
	};
	inline ostream& operator<<(ostream& o,HiLowBase& h){return h.operator<<(o);}

	template<typename KT>
		struct HiLow : HiLowBase
	{
		HiLow() : trigger(false), deep(0),shallow(0X7FFF) {}
		operator const bool () const
		{
			if (!trigger) return true;
			if (deep>(shallow*2)) return false;
			return true;
		}
		virtual bool qualify(Trunk* rbnode)
		{
			if (rbnode->isleaf(rbnode)) return true;
			else return false;
		}

		virtual int DepthFinder(Trunk& tb,int d=0)
		{ 
			if (d>100) throw string("HiLow::DepthFinder error");
			if (tb.isnil()) return d;
			if (!tb.Parent()) return d;
			if (tb.Parent()->isnil()) return d;
			d=DepthFinder(*tb.Parent(),d+1); 
			return d; 
		}

		void operator()(ostream& tout,Trunk* node)
		{
			HiLow<KT>& me(*this);
			if (!node) return ;
			if (node->isnil()) return ;
			if (this->qualify(node))
			{
				trigger=true;
				int d(DepthFinder(*node));
				d++;
				if (d<shallow) shallow=d;
				if (d>deep) deep=d;
				BstBase<KT>& rk(static_cast<BstBase<KT>&>(*node));
				const KT& k(rk);
				//tout<<k<<" is "<<d<<" deep "<<endl;
			}
			if (!node->isnul(node->Left())) me(tout,node->Left());
			if (!node->isnul(node->Right())) me(tout,node->Right());
		}	
		virtual ostream& operator<<(ostream& o)
			{ o<<"Shortest: "<<shallow<<", Deepest:"<<deep<<endl; return o; } 
		private:
		bool trigger;
		int shallow,deep;
	};

	template<typename KT>
		struct RbHiLow : HiLow<KT>
	{
		int DepthFinder(Trunk& tb,int d=0)
		{ 
			if (d>100) throw string("RbHiLow::DepthFinder error");
			if (tb.isnil()) return d;
			if (!tb.Parent()) return d;
			if (tb.Parent()->isnil()) return d;
			RbBase* prbc(dynamic_cast<RbBase*>(&tb));
			if (!prbc) {cout<<"This is not an RbBase node"<<endl; return false;}
			RbBase& rbc(*prbc);
			if (rbc.color(&tb)==RbBase::BLACK) d=DepthFinder(*tb.Parent(),d+1); 
			else d=DepthFinder(*tb.Parent(),d); 
			return d; 
		}
	};

	template<typename KT>
		inline void TestSuccessor(ostream& tout,Trunk* root,Trunk* node,set<KT>& used,bool& ok)
	{
		BstBase<KT>& rk(static_cast<BstBase<KT>&>(*node));
		const KT& v(rk); //tout<<v<<" ";
		typename set<KT>::iterator fit(used.find(v));
		typename set<KT>::reverse_iterator it(used.find(v));
		if (fit==used.end()) throw string("Your key is not in the used set");
		Trunk* maximum(root->RightMost());
		if (node==maximum) 
		{
			// tbd...
			//if (fit==used.end()) return; 
			//tout<<"RightMost node is not used.rbegin()"<<endl;
			//ok=false;
			return;
		}
		fit++;
		Trunk* successor(node->Successor());
		BstBase<KT>& prk(static_cast<BstBase<KT>&>(*successor));
		const KT& pv(prk); 
		const KT& upv(*fit);
		if (pv!=upv)
		{
			tout<<"Successor test failed for "<<v<<":"<<pv<<"!="<<upv<<endl;
			ok=false;
		}
	}

	template<typename KT>
		inline void TestPredecessor(ostream& tout,Trunk* root,Trunk* node,set<KT>& used,bool& ok)
	{
		BstBase<KT>& rk(static_cast<BstBase<KT>&>(*node));
		const KT& v(rk); //tout<<v<<" ";
		typename set<KT>::iterator it(used.find(v));
		if (it==used.end()) throw string("Your key is not in the used set");
		Trunk* minimum(root->LeftMost());
		if (node==minimum) 
		{
			if (it==used.begin()) return; 
			tout<<"Leftmost node is not used.begin()"<<endl;
			ok=false;
			return;
		}
		it--;
		Trunk* predecessor(node->Predecessor());
		BstBase<KT>& prk(static_cast<BstBase<KT>&>(*predecessor));
		const KT& pv(prk); 
		const KT& upv(*it);
		if (pv!=upv)
		{
			tout<<"Predecessor test failed for "<<v<<":"<<pv<<"!="<<upv<<endl;
			ok=false;
		}
		
	}

	template<typename KT>
		inline void TestPredecessorsAndSuccessors(ostream& tout,Trunk* root,Trunk* node,set<KT>& used,bool& ok)
	{
		if (!node) return;
		if (node->isnul(node->Left())) TestPredecessorsAndSuccessors<KT>(tout,root,node->Left(),used,ok);
		if (!node->isnil()) 
		{
			TestPredecessor<KT>(tout,root,node,used,ok);
			TestSuccessor<KT>(tout,root,node,used,ok);
		}
		if (node->isnul(node->Right())) TestPredecessorsAndSuccessors<KT>(tout,root,node->Right(),used,ok);
	}	

	template<typename KT>
		inline bool BstIntegrity(ostream& tout,Trunk* root,set<KT>& used)
	{
			bool ok(true);
			if (!root) return true;
			if (root->isnil()) return true;
			if (used.empty()) 
			{
				tout<<"This tree should be empty!"<<endl;
				return false;
			}
			BstBase<KT>& rk(static_cast<BstBase<KT>&>(*root));
			KT maxvalue(rk.maxValue(root));
			KT minvalue(rk.minValue(root));
			KT rootvalue(rk);
			bool isbst(root->isBST(root));
			if (minvalue!=(*used.begin())) {tout<<("Min check failed")<<endl; ok= false;}
			if (maxvalue!=(*used.rbegin())) {tout<<("Max check failed")<<endl; ok= false;}
			if (!isbst) {tout<<("isBST failed")<<endl; ok= false;}
			long ttl(root->countnodes());
			if (ttl!=used.size()) {tout<<"Tree is not the same size as the used set"<<endl; ok=false;}
			if (ok) 
			{
				TestPredecessorsAndSuccessors<KT>(tout,root,root,used,ok);	
				if (!ok) tout<<"Predecessor and successor check failed"<<endl;
			}
			
			stringstream sout;
			if (!InOrderAssertions<KT>(sout,root)) 
			{
				tout<<"In order assertions failed"<<endl;
				ok=false;
			}

			//if (sout.str().empty()) ok=false;
			//else tout<<sout.str().c_str()<<endl;


			if (!ok)
			{
				tout<<"Ok:"<<boolalpha<<ok<<", Total:"<<ttl<<" ?= "<<used.size()<<", ";
				tout<<"Root:"<<setprecision(2)<<fixed<<rootvalue<<" ";
				tout<<"Min:"<<setprecision(2)<<fixed<<minvalue<<" ";
				tout<<"Max:"<<setprecision(2)<<fixed<<maxvalue<<" ";
				tout<<"isBST:"<<boolalpha<<isbst;
				tout<<"; Used: Min:"<<*used.begin();
				tout<<", Max:"<<*used.rbegin()<<endl;
				tout.flush();
			}
			return ok;
	}

	namespace RedBlackCheck
	{
		template<typename KT>
			struct Visitor
		{
			Visitor(ostream& _tout,Trunk& _rk,IntegrityAdvisor& _advisor) : tout(_tout), rk(_rk),advisor(_advisor),ok(true) {}
			operator bool () { PostOrder(rk); return ok; }
			private:
			ostream& tout;
			Trunk& rk;
			IntegrityAdvisor& advisor;
			bool ok;
			int PostOrder(Trunk& node)
			{
				if (node.isnil()) return 0;
				RbBase* prbc(dynamic_cast<RbBase*>(&node));
				if (!prbc) {tout<<"Error in dynamic cast"<<endl;return 0;}
				RbBase& rbc(*prbc);
				int ld(0),rd(0);	
				if (node.Parent())
				{
					if (rbc.color(&node)==RbBase::BLACK) 
						if (node.Parent()->Left()==&node) ld=1; else rd=1;
				}
				if (node.Left()) ld+=PostOrder(*node.Left());
				if (node.Right()) rd+=PostOrder(*node.Right());
				Visit(node,ld,rd);
				return max(ld,rd);
			}
			void Visit(Trunk& node,const int lc,const int rc)
			{
				BstBase<KT>& rb(static_cast<BstBase<KT>&>(node));
				RbBase* prbc(dynamic_cast<RbBase*>(&node));
				if (!prbc) {tout<<"This is not an RbBase node"<<endl; return;}
				RbBase& rbc(*prbc);
				const KT& key(rb);
				advisor.clear(node,"d");
				advisor.clear(node,"c");
				int m((max(abs(lc),abs(rc)))*2);
				int diff(abs(lc-rc));
				if (diff>2)  
				{
					//ok=false;
					//stringstream ss;
					//ss<<lc<<","<<rc;
					//if (rbc.color(&node)==RbBase::BLACK) 
					//	advisor.message(node,"d",ss.str());
				}
				if (!node.isnil() and rbc.color(&node)==RbBase::RED) 
				{
					bool leftisred(false),rightisred(false);
					if (!node.isnul(rb.Right())) if (rbc.color(rb.Right())==RbBase::RED) rightisred=true;
					if (!node.isnul(rb.Left())) if (rbc.color(rb.Left())==RbBase::RED) leftisred=true;
					stringstream ss;
					if (leftisred) ss<<"L";
					if (rightisred) ss<<"R";
					if (!ss.str().empty()) 
						advisor.message(node,"c",ss.str());
					//if (leftisred or rightisred) ok=false;
				}
			}
		}; 
	} // RedBlackCheck

	template<typename KT>
		inline bool RedBlackIntegrity(ostream& tout,Trunk* root,IntegrityAdvisor& advisor)
	{
		if (!root) return true;
		if (root->isnil()) return true;
		Trunk& rk(static_cast<Trunk&>(*root));
		Trunk* leftmost(root->LeftMost());
		Trunk* rightmost(root->RightMost());
		RedBlackCheck::Visitor<KT> visitor(tout,rk,advisor);
		//if (!visitor) {tout<<"Red Black check failed"<<endl; return false;}
		if (!visitor) {return false;}
		long ttl(root->countnodes());
		if (ttl>3)
		{
				RbBase* prbc(dynamic_cast<RbBase*>(root));
				if (!prbc) {tout<<"This is not an RbBase node"<<endl; return false;}
				RbBase& rbc(*prbc);
				RbHiLow<KT> hilow;
				hilow(cerr,root); 
				if (!hilow)
				{
					tout<<"Imbalanced:"<<hilow<<endl;
					return false;
				} //else tout<<"In balance:"<<hilow<<endl;
		}
		return true;
		//return visitor;
	}


	template<typename KT>
		inline void Describe(Trunk* root,ostream& tout)
	{
			if (!root) {cout<<"The root is null"<<endl; return;}
			if (root->isnil()) {cout<<"The root is nil"<<endl; return;}
			BstBase<KT>& rk(static_cast<BstBase<KT>&>(*root));
			KT maxvalue(rk.maxValue(root));
			KT minvalue(rk.minValue(root));
			KT rootvalue(rk);
			bool isbst(root->isBST(root));
			if (!isbst) {tout<<("isBST failed")<<endl; return;}
			long ttl(root->countnodes());
			//if (ok) TestPredecessorsAndSuccessors<KT>(tout,root,root,used,ok);
			cerr<<"Total Nodes:"<<ttl<<endl;
			cerr<<"Root:"<<setprecision(2)<<fixed<<rootvalue<<" ";
			cerr<<"Min:"<<setprecision(2)<<fixed<<minvalue<<" ";
			cerr<<"Max:"<<setprecision(2)<<fixed<<maxvalue<<" ";
			cerr<<"isBST:"<<boolalpha<<isbst;
	}



} //TreeIntegrity
#endif // TREE_INTEGRITY

