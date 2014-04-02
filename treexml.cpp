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

/*
	This unit will take input that was generated from the Msg manipulators
	in the tree sources and generate xml output.  This output will describe
	the exact steps applied to the tree, and annotate them with meaningful
	descriptions, and where applicable, line numbers of pseudo code from
	'Introduction To Algorithms' (CLRS).  The xml may then be transformed
	against the provided xslt sheet to produce an ajax page that helps 
	the user understand the algorithms.

	This features will implement WebKruncher's custom XML tool, exexml.
*/


#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <fstream>
#include <deque>
using namespace std;
typedef string stringtype;
typedef char chartype;
typedef stringstream stringstreamtype;
#include <exexml>
using namespace XmlFamily;
#define TREEXML
#include "treexml.h"
namespace XmlTree
{
	TreeXml::~TreeXml()
	{
		if (xml)
		{
			Xml* xmltrunk(static_cast<Xml*>(xml));
			delete xmltrunk;
		}
	}

	ostream& TreeXml::operator<<(ostream& o)
	{
		if (xml)
		{
			Xml* xmltrunk(static_cast<Xml*>(xml));
			o<<(*xmltrunk);
		}
		return o;
	}

	Payload& TreeXml::payload()
	{
		if (!xml) {xml=new Payload(*this); payload().Begin();}
		Xml* xmltrunk(static_cast<Xml*>(xml));
		Payload& payload(static_cast<Payload&>(*xmltrunk));
		return payload;
	}

	void TreeXml::Begin()
	{
		if (!xml) xml=new Payload(*this);
		payload().Begin();
	}

	void TreeXml::Key(const string keyname) { payload().Key(keyname); }
	void TreeXml::Number(const int a) { payload().Number(a); }
	void TreeXml::Done() { payload().Done(); }
	void TreeXml::Finish() { payload().Finish(); }
	void TreeXml::SourceTrace(const string fname,const int linendx,const int CLRSndx,const string sourcecode)
		{ payload().SourceTrace(fname,linendx,CLRSndx,sourcecode); }

	void TreeXml::Insrt() { payload().Insrt(); }
	void TreeXml::Erse() { payload().Erse(); }
	void TreeXml::Trnsp() { payload().Trnsp(); }
	void TreeXml::Rotlft() { payload().Rotlft(); }
	void TreeXml::Rotrgt() { payload().Rotrgt(); }

	Item::operator Payload& () { Xml& doc(GetDoc()); return static_cast<Payload&>(doc); }
	Item::operator const TreeXml& () { Payload& payload(*this); return payload; }
	Item::operator SubTreePrinter& () { Payload& payload(*this); return payload;	}

	Item* Item::Begin(Xml& _doc,XmlNode* parent)
	{
		Item* n(static_cast<Item*>(NewNode(_doc,parent,"Item")));
		appendChild(n);
		SubTreePrinter& stp(*this);
		stp.clear();
		return n;
	}

	ostream& Item::operator<<(ostream& o)
	{
		if (Name()=="Tree")
		{
			o<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl<<endl;
			o<<"<?xml-stylesheet type=\"text/xsl\" href=\"./trees.xslt\"?>"<<endl<<endl;
		}
		if ( (children.empty()) and (textsegments.empty()) and (attributes.empty()) ) return o;
		return XmlNode::operator<<(o)<<endl;
	}


	void Item::Insrt()
	{
		attributes["insrt"]=XmlFamily::TextElement(Document,this,"true");
		SubTreePrinter& stp(*this);
		stp.Insrt();
	}


	void Item::Erse()
	{
		attributes["erse"]=XmlFamily::TextElement(Document,this,"true");
		SubTreePrinter& stp(*this);
		stp.Erse();
	}

	void Item::Trnsp()
	{
		Item* n(static_cast<Item*>(NewNode(GetDoc(),this,"Transplant")));
		appendChild(n);
		LastAction=n;
		SubTreePrinter& stp(*this);
		stp.Trnsp();
	}

	void Item::Done()
	{
		Item* n(static_cast<Item*>(NewNode(GetDoc(),this,"Item")));
		appendChild(n);
		LastAction=n;
		n->attributes["done"]=XmlFamily::TextElement(Document,n,"true");
		SubTreePrinter& stp(*n);
		stp.Done();
	}
	void Item::Finish()
	{
		SubTreePrinter& stp(*this);
		stp.Finish();
		LastAction=NULL;
	}

	void Item::Rotlft()
	{
		Item* n(static_cast<Item*>(NewNode(GetDoc(),this,"RotateLeft")));
		appendChild(n);
		LastAction=n;
		SubTreePrinter& stp(*this);
		stp.Rotlft();
	}

	void Item::Rotrgt()
	{
		Item* n(static_cast<Item*>(NewNode(GetDoc(),this,"RotateRight")));
		appendChild(n);
		LastAction=n;
		SubTreePrinter& stp(*this);
		stp.Rotrgt();
	}

	Item& Item::SubItem(string subname)
	{
		Item* n(static_cast<Item*>(NewNode(GetDoc(),this,subname)));
		appendChild(n);
		return *n;
	}

	void Item::Key(const string keyname)
	{
		if (LastAction) { LastAction->Key(keyname); return; }
		const TreeXml& treexml(*this);
		Item& subitem(SubItem("key"));

		subitem.textsegments[subitem.textsegments.size()]=
			XmlFamily::TextElement(GetDoc(),&subitem,keyname);

		SubTreePrinter& stp(*this);
		stp.Key(keyname,this);
	}

	void Item::Number(const int a)
	{
		if (LastAction) { LastAction->Number(a); return; }
		stringstream cd;
		cd<<"<Number>"<<a<<"</Number>";
			textsegments[textsegments.size()]=
				XmlFamily::TextElement(GetDoc(),this,cd.str());
	}

	void Item::SourceTrace(const string fname,const int linendx,const int CLRSndx,const string sourcecode)
	{
		if (LastAction) { LastAction->SourceTrace(fname,linendx,CLRSndx,sourcecode); return; }
		stringstream cd;
		cd<<"<Source ";
		if (CLRSndx!=-1) cd<<"clrs=\""<<CLRSndx<<"\" ";
		if (linendx!=-1) cd<<"line=\""<<linendx<<"\" ";
		cd<<"file=\""<<fname<<"\" ";
		cd<<"><![CDATA[";
		cd<<sourcecode; 
		cd<<"]]></Source>";
			textsegments[textsegments.size()]=
				XmlFamily::TextElement(GetDoc(),this,cd.str());
	}

	Item& Payload::item()
	{
		if (!current) throw string("Payload did not begin");
		return static_cast<Item&>(*current);
	}

	void Payload::Begin()
	{
		if (!Root) Root=NewNode(*this,"Tree");
		Item* rootitem(static_cast<Item*>(Root));
		current=rootitem->Begin(*this,rootitem);
	}

	void Payload::Finish() { item().Finish(); }
	void Payload::Done() { item().Done(); }
	void Payload::SourceTrace(const string fname,const int linendx,const int CLRSndx,const string sourcecode)
		{ item().SourceTrace(fname,linendx,CLRSndx,sourcecode); }
	void Payload::Key(const string keyname) { item().Key(keyname); }
	void Payload::Number(const int a) { item().Number(a); }
	void Payload::Insrt() { item().Insrt(); }
	void Payload::Erse() { item().Erse(); }
	void Payload::Trnsp() { item().Trnsp(); }
	void Payload::Rotlft() { item().Rotlft(); }
	void Payload::Rotrgt() { item().Rotrgt(); }

	void SubTreePrinter::clear() 
	{ 
		current=NULL; recent.clear(); 
		mainmode=None; submode=NoSub;
	}

	void SubTreePrinter::Insrt()
	{
		mainmode=Inserting;
	}

	void SubTreePrinter::Erse()
	{
		mainmode=Removing;
	}

	void SubTreePrinter::Trnsp()
	{
		submode=Transplanting;
		Print();
	}

	void SubTreePrinter::Done()
	{
		Print();
	}

	void SubTreePrinter::Finish()
	{
		Print();
	}

	void SubTreePrinter::Rotlft()
	{
		submode=LeftRotating;
		Print();
	}

	void SubTreePrinter::Rotrgt()
	{
		submode=RightRotating;
		Print();
	}

	void SubTreePrinter::Key(const string keyname,XmlNode* node)
	{
		current=node;
		recent.push_front(keyname);
		while (recent.size()>2) recent.pop_front();
		Print();
	}

	bool AppendSubTree(XmlNode& node,const string disposition,const string keyname,TreeXml& xmltree,const int depth,const string parentname="")
	{
		Item& I(static_cast<Item&>(node));
		Item& subitems(I.SubItem(disposition));
		subitems=keyname;
		const string color(xmltree.SColorOf(keyname));
		subitems.attributes["color"]=XmlFamily::TextElement(node.GetDoc(),&subitems,color);
		subitems.attributes["parent"]=XmlFamily::TextElement(node.GetDoc(),&subitems,parentname);
		const int keydepth(xmltree.SDepthOf(keyname));
		stringstream sd; sd<<keydepth; string sdepth; sdepth=sd.str().c_str();
		subitems.attributes["depth"]=XmlFamily::TextElement(node.GetDoc(),&subitems,sdepth);
		bool ret(false);
		if (xmltree.SHasLeft(keyname))
		{
			ret=true;
			const string sub(xmltree.SLeftOf(keyname));
			subitems.attributes["left"]=XmlFamily::TextElement(node.GetDoc(),&subitems,sub);
			AppendSubTree(subitems,"LeftTree",sub,xmltree,depth-1,keyname);
		}
		if (xmltree.SHasRight(keyname))
		{
			ret=true;
			const string sub(xmltree.SRightOf(keyname));
			subitems.attributes["right"]=XmlFamily::TextElement(node.GetDoc(),&subitems,sub);
			AppendSubTree(subitems,"RightTree",sub,xmltree,depth-1,keyname);
		}
		return ret;
	}

	void SubTreePrinter::Print()
	{
		TreeXml& xmltree(payload);
		try
		{
			if (!current) return;
			if (true)
			{
				const string node(xmltree.SGetRoot());
				AppendSubTree(*current,"subtree",node,xmltree,12); 
				return;
			}

			// may be useful for displaying small branches of larger trees

			if (recent.empty()) throw string("No recent nodes");
			int N(0);
			while (N<recent.size())
			{
				string node(recent[N]);
				while (true)
				{
					if (xmltree.SIsRoot(node))
					{
						if (AppendSubTree(*current,"subtree",node,xmltree,12)) return;
						N++; break;
					} else {
						string pnode=xmltree.SParentOf(node);
						if (pnode==node) { AppendSubTree(*current,"subtree",node,xmltree,12); return;}
						else node=pnode;
					}
				}
			}
		} catch (string& e) {
			cout<<"\t\tException:"<<e<<endl;
			xmltree.StopAndPrint();
		}
	}

} // XmlTree

