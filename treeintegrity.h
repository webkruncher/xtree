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

namespace TreeIntegrity
{
	template<typename KT,typename VT> 
		inline void PrintInOrder(TreeBase* node)
	{
		if (node->left) PrintInOrder<KT,VT>(node->left);
		Bst<KT,VT>& rk(static_cast<Bst<KT,VT>&>(*node));
		const KT& v(rk); cout<<v<<" ";
		if (node->right) PrintInOrder<KT,VT>(node->right);
	}	

	template<typename KT,typename VT> 
		inline bool BstIntegrity(TreeBase* root,set<KT>& used)
	{
			if (!root) return true;
			Bst<KT,VT>& rk(static_cast<Bst<KT,VT>&>(*root));
			KT maxvalue(rk.maxValue(root));
			KT minvalue(rk.minValue(root));
			KT rootvalue(rk);
			bool isbst(root->isBST(root));
			cout<<"Root:"<<setprecision(2)<<fixed<<rootvalue<<" ";
			cout<<"Min:"<<setprecision(2)<<fixed<<minvalue<<" ";
			cout<<"Max:"<<setprecision(2)<<fixed<<maxvalue<<" ";
			cout<<"isBST:"<<boolalpha<<isbst;
			cout<<"; Used: Min:"<<*used.begin();
			cout<<", Max:"<<*used.rbegin()<<endl;
			cout.flush();
			if (minvalue!=(*used.begin())) {cout<<("Min check failed")<<endl; return false;}
			if (maxvalue!=(*used.rbegin())) {cout<<("Max check failed")<<endl; return false;}
			if (!isbst) {cout<<("isBST failed")<<endl; return false;}
			long ttl(root->countnodes());
			if (ttl!=used.size()) {cout<<("Wrong number of nodes counted")<<endl; return false;}
			//cout<<" Total:"<<ttl<<" == "<<used.size()<<endl;
			return true;
	}

	namespace RedBlackCheck
	{

		template<typename KT,typename VT>
			struct Visitor
		{
			Visitor(RbMap<KT,VT>& _rk) : rk(_rk),ok(true) {}
			operator bool () 
			{ 
				PostOrder(rk);
				return ok; 
			}
			private:
			RbMap<KT,VT>& rk;
			bool ok;
			int PostOrder(TreeBase& node)
			{
				RbMap<KT,VT>& rb(static_cast<RbMap<KT,VT>&>(node));
				int ld(0),rd(0);	
				if (node.parent)
					if (rb.color(&rb)==RbMap<KT,VT>::BLACK) 
						if (node.parent->left==&node) ld=1; else rd=1;
				if (node.left) ld+=PostOrder(*node.left);
				if (node.right) rd+=PostOrder(*node.right);
				Visit(node,ld,rd);
				return max(ld,rd);
			}
			void clear(VT& data)
			{
				map<string,string>::iterator it;
				it=data.find("lr"); if (it!=data.end()) data.erase(it);
				it=data.find("rb"); if (it!=data.end()) data.erase(it);
			}	
			void Visit(TreeBase& node,const int lc,const int rc)
			{
				RbMap<KT,VT>& rb(static_cast<RbMap<KT,VT>&>(node));
				const KT& key(rb);
				VT& data(rb.Data()); clear(data);
				#if 1
				int diff(abs(lc-rc));
				if (diff>2) 
				{
					stringstream ss;
					ss<<lc<<","<<rc;
					if (rb.color(&rb)==RbMap<KT,VT>::BLACK) data["lr"]=ss.str();
				}
				#endif
				if (rb.color(&node)==RbMap<KT,VT>::RED) 
				{
					bool leftisred(false),rightisred(false);
					if (rb.right) if (rb.color(rb.right)==RbMap<KT,VT>::RED) rightisred=true;
					if (rb.left) if (rb.color(rb.left)==RbMap<KT,VT>::RED) leftisred=true;
					stringstream ss;
					if (leftisred) ss<<"L";
					if (rightisred) ss<<"R";
					if (!ss.str().empty()) data["rb"]=ss.str();
				}
			}
		}; 
	} // RedBlackCheck

	template<typename KT,typename VT> 
		inline bool RedBlackIntegrity(TreeBase* root)
	{
		if (!root) return true;
		//cout<<"Checking Red Black"<<endl;
		RbMap<KT,VT>& rk(static_cast<RbMap<KT,VT>&>(*root));
		TreeBase* leftmost(root->LeftMost());
		TreeBase* rightmost(root->RightMost());
		RbMap<KT,VT>& lm(static_cast<RbMap<KT,VT>&>(*leftmost));
		RbMap<KT,VT>& rm(static_cast<RbMap<KT,VT>&>(*rightmost));
		const KT& L(lm);
		const KT& R(rm);
		RedBlackCheck::Visitor<KT,VT> visitor(rk);
		if (!visitor) {cout<<"Red Black check failed"<<endl; return false;}
		return true;
	}
} //TreeIntegrity
#endif // TREE_INTEGRITY

