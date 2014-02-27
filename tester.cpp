
#include <iostream>
#include <string>
#include <sstream>

#include <tree.h>
#include <treemap.h>
#include <treeset.h>
using namespace TreeObjects;

using namespace std;


struct Widget
{
	Widget() {}
	Widget(int j)
	{
		stringstream ss; ss<<j; name=ss.str();
	}
	virtual ~Widget(){}
	void operator=(string what){name=what;}
	ostream& operator<<(ostream& o) const {o<<name; return o;}
	operator const bool()
	{
		cout<<"T:"<<name<<",";
	}
	static int test();
	private: string name;
};
inline ostream& operator<<(ostream& o,const Widget& w){return w.operator<<(o);}


int main(int,char**)
{
	return Widget::test();
	return 0;
}

int Widget::test()
{
	bool ok(true);
	{
		cout<<"Testing map"<<endl;
		Map<string,Widget> widgets;
		{
			Widget& first(widgets["widget1"]);
			first="Test1";
			cout<<"First:"<<widgets["widget1"]<<endl;
		}

		{
			Widget& second(widgets["widget2"]);
			second="Test2";
			cout<<"Second:"<<widgets["widget2"]<<endl;
		}

		{
			Widget& third(widgets["widget3"]);
			third="Test3";
			cout<<"Third:"<<widgets["widget3"]<<endl;
		}

		widgets.inorder();
		widgets.erase("widget1");
		ok=(widgets.isBST());
	}
	cout<<" Map test complete, success:"<<boolalpha<<ok<<endl;
	if (!ok)  return -1;
	{
		cout<<"Testing set"<<endl;
		Set<string> items;
		items.insert("Jack");
		items.insert("Fred");
		items.insert("Jack");
		items.insert("Joe");
		items.insert("Nat");
		items.erase("Jack");
		items.erase("Jim");
		items.inorder();
		ok=(items.isBST());
	}
	cout<<" Set test complete, success:"<<boolalpha<<ok<<endl;
	if (!ok)  return -1;
	return 0;
}

