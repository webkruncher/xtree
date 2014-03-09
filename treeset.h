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
#ifndef DIGITAL_ARBORIST_SETS_H
#define DIGITAL_ARBORIST_SETS_H
#include <tree.h>

namespace TreeObjects
{
	template <typename KT>
		struct Set : Sentinel
	{
		Set() : root(NULL) {}
		virtual ~Set() 
		{
			if ((root) and (!root->isnil()) ) delete root;
		}
		void erase(const KT key)
		{
			if (!root) return;
			if (root->isnil()) return;
			TreeBase* found(root->find(key));
			if (found) if (!root->isnul(found)) root=static_cast<RbSet<KT>*>(root->erase(root,found));
			if (root) root->SetParent(this);
		}
		void insert(const KT key)
		{
			if (!root) 
			{
				root=new RbSet<KT>(static_cast<Sentinel&>(*this),key); 
				if (root) root->SetParent(this);
				return ;
			}
			TreeBase* found(root->find(key));
			if (found) return ;
			RbSet<KT>* node(new RbSet<KT>(static_cast<Sentinel&>(*this),key)); 
			root=static_cast<RbSet<KT>*>(root->insert(root,node));
			if (root) root->SetParent(this);
		}
		void inorder(TreeBase* node=NULL)
		{
			if (isnul(node)) node=root;
			if (!isnul(node->left)) inorder(node->left);	
			RbSet<KT>& item(static_cast<RbSet<KT>& >(*node)); 
			const KT& key(item);
			if (!isnul(node->right)) inorder(node->right);	
		}
		operator BstBase<KT>* () const {return root;}
		bool isBST() { if (isnul(root)) return true; return root->isBST(root); }
		private:
		RbSet<KT>* root;
	};

} // namespace TreeObjects

#endif // DIGITAL_ARBORIST_SETS_H


