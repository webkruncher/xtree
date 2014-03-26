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

#ifndef TREE_XML_H
#define TREE_XML_H

/*	This is the interface class that will be
		instanciated in the user object.  It 
		contains a void*.  This technique keeps the
		user code agnostic of the exexml tools
		and isolates the exexml tools from the user.
		The treexml translation unit will cast this
		pointer to an XmlFamily::Xml object, and
		use it internally.
*/

namespace XmlTree
{
	struct Payload;
	struct TreeXml 
	{
		TreeXml() : xml(NULL){}
		~TreeXml();
		void Begin();
		void Done();
		void Finish();
		void SourceTrace(const string fname,const int linendx,const int CLRSndx,const string sourcecode);
		void Insrt();
		void Erse();
		void Key(const string keyname);
		void Trnsp();
		void Rotlft();
		void Rotrgt();
		void Number(const int a);
		virtual const bool SIsRoot(const string what) const = 0; 
		virtual const bool SHasLeft(const string what) const = 0;
		virtual const bool SHasRight(const string what) const = 0;
		virtual const string SParentOf(const string what) const = 0; 
		virtual const int SDepthOf(const string what) const = 0; 
		virtual const string SColorOf(const string what) const = 0; 
		virtual const string SLeftOf(const string what) const = 0; 
		virtual const string SRightOf(const string what) const = 0; 
		private:
		Payload& payload();
		void* xml;
		friend ostream& operator<<(ostream&,TreeXml&);
		ostream& operator<<(ostream& o);
	};
	inline ostream& operator<<(ostream& o,TreeXml& t){return t.operator<<(o);}

	#ifdef TREEXML
	// This area is for treexml objects only
	// and not seen by the user code.

	struct SubTreePrinter
	{
		SubTreePrinter(Payload& _payload) : current(NULL), mainmode(None),submode(NoSub),payload(_payload) {}
		void clear();
		void Insrt();
		void Erse();
		void Trnsp();
		void Done();
		void Finish();
		void Rotlft();
		void Rotrgt();
		void Key(const string keyname,XmlNode*);
		private:
		enum MainMode { None, Inserting, Removing } mainmode;
		enum SubMode { NoSub, LeftRotating, RightRotating,Transplanting,FixupInsert,FixupDelete } submode;
		Payload& payload;
		XmlNode* current;
		deque<string> recent;
		void Print();
	};

	struct Item : XmlNode
	{
		friend struct Payload;
		virtual XmlNodeBase* NewNode(Xml& _doc,XmlNodeBase* parent,stringtype name) 
		{ 
			XmlNodeBase* ret(NULL);
			ret=new Item(_doc,parent,name); 
			return ret;
		}
		virtual bool operator()(ostream& o) { return XmlNode::operator()(o); }
		Item(Xml& _doc,const XmlNodeBase* _parent,stringtype _name) : XmlNode(_doc,_parent,_name),LastAction(NULL) {}
		Item* Begin(Xml& _doc,XmlNode* parent);
		void Done();
		void Finish();
		void SourceTrace(const string fname,const int linendx,const int CLRSndx,const string sourcecode);
		void Insrt();
		void Erse();
		void Key(const string keyname);
		void Trnsp();
		void Rotlft();
		void Rotrgt();
		void Number(const int a);
		virtual ostream& operator<<(ostream&);
		Item& SubItem(string name);
		void operator=(string what) { textsegments[textsegments.size()]=XmlFamily::TextElement(GetDoc(),this,what); }
		private:
		Item* LastAction;
		operator const TreeXml& () ;
		operator Payload& () ;
		operator SubTreePrinter& () ;
	};
	inline ostream& operator<<(ostream& o,Item& xmlnode){return xmlnode.operator<<(o);}

	struct Payload : Xml
	{
		Payload(TreeXml& _treexml) : current(NULL),treexml(_treexml),subtreeprinter(*this) {}
		virtual XmlNode* NewNode(Xml& _doc,stringtype name) { return new Item(_doc,NULL,name); }
		ostream& operator<<(ostream& o) { Xml::operator<<(o); return o;}
		operator Item& () { if (!Root) throw string("No root node"); return static_cast<Item&>(*Root); }
		void Begin();
		void Done();
		void Finish();
		void SourceTrace(const string fname,const int linendx,const int CLRSndx,const string sourcecode);
		void Insrt();
		void Erse();
		void Key(const string keyname);
		void Trnsp();
		void Rotlft();
		void Rotrgt();
		void Number(const int a);
		operator TreeXml& (){return treexml;}
		operator SubTreePrinter& (){return subtreeprinter;}
		private:
		Item* current;
		Item& item();
		TreeXml& treexml;
		SubTreePrinter subtreeprinter;
	};
	inline ostream& operator<<(ostream& o,Payload& xml){return xml.operator<<(o);}

	
	#endif //TREEXML

} // XmlTree

#endif // TREE_XML_H

