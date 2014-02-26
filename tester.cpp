
#include <tree.h>
#include <treemap.h>
#include <treeset.h>
using namespace TreeObjects;

#include <iostream>
#include <string>
using namespace std;


struct Widget
{
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
	//RbMap<int,int> *root=new RbMap<int,int>(1,0);
	//cout<<"Tester"<<endl;
	return 0;
}

int Widget::test()
{
	bool ok(true);
	{
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
		cout<<endl<<"Done, testing"<<endl;cout.flush();
		ok=(widgets.isBST());
		cout<<"unwinding"<<endl;cout.flush();
	}
	cout<<"unwound"<<endl;cout.flush();
	if (ok) return 0; else return -1;
}
