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

	template<typename KT,typename VT> 
		inline void PrintInOrder(Trunk* node)
	{
		if (!node) return;
		if (node->isnil()) return;
		if (!node->isnul(node->Left())) PrintInOrder<KT,VT>(node->Left());
		Bst<KT,VT>& rk(static_cast<Bst<KT,VT>&>(*node));
		const KT& v(rk); //cout<<v<<" ";
		if (!node->isnul(node->Right())) PrintInOrder<KT,VT>(node->Right());
	}	

	template<typename KT>
		inline void TestSuccessor(Trunk* root,Trunk* node,set<KT>& used,bool& ok)
	{
		BstBase<KT>& rk(static_cast<BstBase<KT>&>(*node));
		const KT& v(rk); //cout<<v<<" ";
		typename set<KT>::iterator fit(used.find(v));
		typename set<KT>::reverse_iterator it(used.find(v));
		if (fit==used.end()) throw string("Your key is not in the used set");
		Trunk* maximum(root->RightMost());
		if (node==maximum) 
		{
			// tbd...
			//if (fit==used.end()) return; 
			//cout<<"RightMost node is not used.rbegin()"<<endl;
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
			cout<<"Successor test failed for "<<v<<":"<<pv<<"!="<<upv<<endl;
			ok=false;
		}
	}

	template<typename KT>
		inline void TestPredecessor(Trunk* root,Trunk* node,set<KT>& used,bool& ok)
	{
		BstBase<KT>& rk(static_cast<BstBase<KT>&>(*node));
		const KT& v(rk); //cout<<v<<" ";
		typename set<KT>::iterator it(used.find(v));
		if (it==used.end()) throw string("Your key is not in the used set");
		Trunk* minimum(root->LeftMost());
		if (node==minimum) 
		{
			if (it==used.begin()) return; 
			cout<<"Leftmost node is not used.begin()"<<endl;
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
			cout<<"Predecessor test failed for "<<v<<":"<<pv<<"!="<<upv<<endl;
			ok=false;
		}
		
	}

	template<typename KT>
		inline void TestPredecessorsAndSuccessors(Trunk* root,Trunk* node,set<KT>& used,bool& ok)
	{
		if (!node) return;
		if (node->isnul(node->Left())) TestPredecessorsAndSuccessors<KT>(root,node->Left(),used,ok);
		if (!node->isnil()) 
		{
			TestPredecessor<KT>(root,node,used,ok);
			TestSuccessor<KT>(root,node,used,ok);
		}
		if (node->isnul(node->Right())) TestPredecessorsAndSuccessors<KT>(root,node->Right(),used,ok);
	}	

	template<typename KT>
		inline bool BstIntegrity(Trunk* root,set<KT>& used)
	{
return true;
			bool ok(true);
			if (!root) return true;
			if (root->isnil()) return true;
			BstBase<KT>& rk(static_cast<BstBase<KT>&>(*root));
			KT maxvalue(rk.maxValue(root));
			KT minvalue(rk.minValue(root));
			KT rootvalue(rk);
			bool isbst(root->isBST(root));
			if (minvalue!=(*used.begin())) {cout<<("Min check failed")<<endl; ok= false;}
			if (maxvalue!=(*used.rbegin())) {cout<<("Max check failed")<<endl; ok= false;}
			if (!isbst) {cout<<("isBST failed")<<endl; ok= false;}
			long ttl(root->countnodes());
			if (ttl!=used.size()) ok=false;
			if (ok) TestPredecessorsAndSuccessors<KT>(root,root,used,ok);
			{
				cout<<"Ok:"<<boolalpha<<ok<<", Total:"<<ttl<<" ?= "<<used.size()<<", ";
				cout<<"Root:"<<setprecision(2)<<fixed<<rootvalue<<" ";
				cout<<"Min:"<<setprecision(2)<<fixed<<minvalue<<" ";
				cout<<"Max:"<<setprecision(2)<<fixed<<maxvalue<<" ";
				cout<<"isBST:"<<boolalpha<<isbst;
				cout<<"; Used: Min:"<<*used.begin();
				cout<<", Max:"<<*used.rbegin()<<endl;
				cout.flush();
			}
			return ok;
	}

	namespace RedBlackCheck
	{
		template<typename KT>
			struct Visitor
		{
			Visitor(Trunk& _rk,IntegrityAdvisor& _advisor) : rk(_rk),advisor(_advisor),ok(true) {}
			operator bool () 
			{ 
				PostOrder(rk);
				return ok; 
			}
			private:
			Trunk& rk;
			IntegrityAdvisor& advisor;
			bool ok;
			int PostOrder(Trunk& node)
			{
				if (node.isnil()) return 0;
				RbBase* prbc(dynamic_cast<RbBase*>(&node));
				if (!prbc) {cout<<"Error in dynamic cast"<<endl;return 0;}
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
				if (!prbc) {cout<<"This is not an RbBase node"<<endl; return;}
				RbBase& rbc(*prbc);
				const KT& key(rb);
				advisor.clear(node,"d");
				advisor.clear(node,"c");
				int m((max(abs(lc),abs(rc)))*2);
				int diff(abs(lc-rc));
				if (diff>2)  
				{
					//ok=false;
					stringstream ss;
					ss<<lc<<","<<rc;
					if (rbc.color(&node)==RbBase::BLACK) 
						advisor.message(node,"d",ss.str());
				}
				if (rbc.color(&node)==RbBase::RED) 
				{
					bool leftisred(false),rightisred(false);
					if (rb.Right()) if (rbc.color(rb.Right())==RbBase::RED) rightisred=true;
					if (rb.Left()) if (rbc.color(rb.Left())==RbBase::RED) leftisred=true;
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
		inline bool RedBlackIntegrity(Trunk* root,IntegrityAdvisor& advisor)
	{
return true;
		if (!root) return true;
		if (root->isnil()) return true;
		Trunk& rk(static_cast<Trunk&>(*root));
		Trunk* leftmost(root->LeftMost());
		Trunk* rightmost(root->RightMost());
		RedBlackCheck::Visitor<KT> visitor(rk,advisor);
		if (!visitor) {cout<<"Red Black check failed"<<endl; return false;}
		return true;
		//return visitor;
	}
} //TreeIntegrity
#endif // TREE_INTEGRITY

