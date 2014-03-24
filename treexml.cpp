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

	void TreeXml::Begin()
	{
		if (!xml) xml=new Payload;
		Xml* xmltrunk(static_cast<Xml*>(xml));
		Payload& payload(static_cast<Payload&>(*xmltrunk));
		payload.Begin();	
	}

	void TreeXml::Insrt()
	{
		if (!xml) throw string("No xml");
		Xml* xmltrunk(static_cast<Xml*>(xml));
		Payload& payload(static_cast<Payload&>(*xmltrunk));
		payload.Insrt();	
	}

	void TreeXml::Erse()
	{
		if (!xml) throw string("No xml");
		Xml* xmltrunk(static_cast<Xml*>(xml));
		Payload& payload(static_cast<Payload&>(*xmltrunk));
		payload.Erse();	
	}

	void TreeXml::Trnsp()
	{
		if (!xml) throw string("No xml");
		Xml* xmltrunk(static_cast<Xml*>(xml));
		Payload& payload(static_cast<Payload&>(*xmltrunk));
		payload.Trnsp();	
	}

	void TreeXml::Key(const string keyname)
	{
		if (!xml) throw string("No xml");
		Xml* xmltrunk(static_cast<Xml*>(xml));
		Payload& payload(static_cast<Payload&>(*xmltrunk));
		payload.Key(keyname);	
	}

	void TreeXml::Finish()
	{
		if (!xml) throw string("No payload");
		Xml* xmltrunk(static_cast<Xml*>(xml));
		Payload& payload(static_cast<Payload&>(*xmltrunk));
		payload.Finish();	
	}

	void Payload::Begin()
	{
		if (!Root) Root=NewNode(*this,"Tree");
		Item* rootitem(static_cast<Item*>(Root));
		current=rootitem->Begin(*this,rootitem);
	}

	Item* Item::Begin(Xml& _doc,XmlNode* parent)
	{
		Item* n(static_cast<Item*>(NewNode(_doc,parent,"Item")));
		appendChild(n);
		return n;
	}

	void Payload::Finish()
	{
		if (!current) throw string("Payload did not begin");
		Item& item(static_cast<Item&>(*current));
		item.Finish();
	}

	void Payload::Insrt()
	{
		if (!current) throw string("Payload did not begin");
		Item& item(static_cast<Item&>(*current));
		item.Insrt();
	}

	void Item::Insrt()
	{
		attributes["insrt"]=XmlFamily::TextElement(Document,this,"true");
	}

	void Payload::Erse()
	{
		if (!current) throw string("Payload did not begin");
		Item& item(static_cast<Item&>(*current));
		item.Erse();
	}

	void Payload::Trnsp()
	{
		if (!current) throw string("Payload did not begin");
		Item& item(static_cast<Item&>(*current));
		item.Trnsp();
	}

	void Item::Erse()
	{
		attributes["erse"]=XmlFamily::TextElement(Document,this,"true");
	}

	void Item::Trnsp()
	{
		Item* n(static_cast<Item*>(NewNode(GetDoc(),this,"Transplant")));
		appendChild(n);
		LastAction=n;
	}

	void Payload::Key(const string keyname)
	{
		if (!current) throw string("Payload did not begin");
		Item& item(static_cast<Item&>(*current));
		item.Key(keyname);
	}

	void Item::Key(const string keyname)
	{
		if (LastAction)
			{ LastAction->Key(keyname); return; }
		string cd("<key><![CDATA[");
		cd+=keyname; cd+="]]></key>";
			textsegments[textsegments.size()]=
				XmlFamily::TextElement(GetDoc(),this,cd);
	}
} // XmlTree

