#ifndef TREE_JOURNAL_H
#define TREE_JOURNAL_H

#include <iostream>
using namespace std;

namespace TreeJournal
{
	struct Journal;
	struct journalbuf : streambuf
	{
		#define BUFFER_SIZE 4096
	public:
		journalbuf() : mode(static_cast<ios_base::openmode>(NULL)), obuffer(NULL),ibuffer(NULL) {}
		journalbuf(const string _name,const ios_base::openmode _mode) : name(_name), mode(_mode), obuffer(NULL),ibuffer(NULL)
		{
			if (mode&ios_base::in) in.open(name.c_str());
			if (mode&ios_base::out) out.open(name.c_str());
			if (!name.empty())
			{
				ibuffer=(char*)malloc(BUFFER_SIZE+6);setg(ibuffer+4,ibuffer+4,ibuffer+4);
				obuffer=(char*)malloc(BUFFER_SIZE+6);setp(obuffer,obuffer+(BUFFER_SIZE-1));
			}
		}
		virtual ~journalbuf() {sync(); if (obuffer) free(obuffer); if (ibuffer) free(ibuffer);if (out.is_open()) out.close(); if (in.is_open()) in.close();}
		operator const ios_base::openmode () const 
		{
			if (mode&ios_base::in) return ios_base::in;
			if (mode&ios_base::out) return ios_base::out;
			return mode;
		}
	protected:
		friend struct Journal;
		virtual int sync(){if (flushout()==EOF) return -1;			return 0;}
		virtual void close() const
		{
			if (mode|ios_base::out) out.close();
			if (mode|ios_base::in) in.close();
			mode=static_cast<ios_base::openmode>(NULL);
		}
		virtual int underflow()
		{
			if (gptr() < egptr()) return *gptr();
			int numPutback = gptr()-eback(); if (numPutback>4) numPutback=4;
			memcpy(ibuffer+(BUFFER_SIZE-numPutback),gptr()-numPutback,numPutback);
			int num=read((char*)ibuffer+4,(size_t)BUFFER_SIZE-4);
			if (num<=0) return EOF;
			setg(ibuffer+(4-numPutback),ibuffer+4,ibuffer+4+num);
			return *gptr();
		}
		virtual int overflow(int c)
		{
			if (c!=EOF) {*pptr()=c; pbump(1);}
			if (flushout()==EOF) {return EOF;}
			return c;
		}
		virtual int read(char* dest,size_t size)
		{
			memset(dest,0,size);
			if (in.eof()) return EOF;
			return in.readsome((char*)dest,size);
		}
	private:
		virtual int flushout()
		{
			int num = pptr()-pbase();
			if (!num) return 0;
			if (write(obuffer,num)!=num) return EOF;
			pbump(-num);
			return num;
		}		
		virtual int write(const char* source,size_t size)
		{
			out.write((char*)source,size);
			return size;
		}
		const string name;
		mutable ios_base::openmode mode;
		char* ibuffer,*obuffer;
		mutable ofstream out;
		mutable ifstream in;
	};


	struct Journal : iostream
	{
		Journal() {}
		Journal(const string _name,const ios_base::openmode _mode) 
			: sbuf(_name,_mode), iostream(&sbuf) {}
		operator const ios_base::openmode () const {return sbuf;}
		virtual void close()
		{
			sbuf.sync();
			sbuf.close();
		}
	private:
		journalbuf sbuf;		
	};

	template <typename KT>
		struct Paragraph : pair<bool,deque<KT> >
			{ Paragraph(const bool _first) { pair<bool,deque<KT> >& me(*this); me.first=_first; } };

	template <typename KT>
		class Entry : deque<Paragraph<KT> >
	{
		friend Journal& operator<<(Journal&,Entry&);
		inline Journal& operator<<(Journal& j)
		{
			const unsigned long L(this->size());
			j.write((char*)&L,sizeof(L));
			while (!this->empty())
			{
				Paragraph<KT>& p(this->front()); 
				const unsigned long l(p.second.size());
				j.write((char*)&l,sizeof(l));
				j<<((p.first)?"+":"-");
				deque<KT>& d(p.second);
				while (!p.second.empty())
				{
					KT& k(p.second.front()); 
					if (T=='S') j<<k<<endl;
					else j.write((char*)&k,sizeof(k));
					p.second.pop_front();
				}
				this->pop_front();
			}	
			return j;
		}
		friend Journal& operator>>(Journal&,Entry&);
		Journal& operator>>(Journal& j)
		{
			Entry<KT>& me(*this);
			j.read((char*)&T,sizeof(char));
			//cout<<"Type:"<<T<<endl;
			unsigned long L; j.read((char*)&L,sizeof(L));
			//cout<<L<<" paragraphs"<<endl;
			while (L--)
			{
				unsigned long l; j.read((char*)&l,sizeof(l));
				//cout<<"l:"<<l<<endl;
				char addordelete; j.read((char*)&addordelete,sizeof(char));
				//cout<<addordelete<<endl;
				while (l--)
				{
					KT k; 
					if (T=='S') getline(j,reinterpret_cast<string&>(k));
					else j.read((char*)&k,sizeof(k));
					if (addordelete=='+') me+=k; else me-=k;
					//cout<<k<<endl;
				}
			}
			return j;
		}
		char T;
		public:
		void clear() { deque<Paragraph<KT> >::clear(); }
		operator const bool () const { return !this->empty(); }
		operator pair<bool,KT> ()
		{
			if (this->empty()) throw string("Empty journal");
			Paragraph<KT>& current(this->front());
			const bool addordelete(current.first);
			const KT key(current.second.front());
			current.second.pop_front();
			if (current.second.empty()) this->pop_front();
			return make_pair<bool,KT>(addordelete,key);
		}

		void operator+=(int);
		void operator-=(int);
		void operator+=(double);
		void operator-=(double);
		void operator+=(string);
		void operator-=(string);
	};

	inline Journal& operator<<(Journal& j,Entry<int>& e)
	{
		e.T='I'; j<<e.T;
		return e.operator<<(j);
	}
	inline Journal& operator>>(Journal& j,Entry<int>& e){return e.operator>>(j);}

	template <> inline void Entry<int>::operator+=(int j)
	{
		typedef int KT;
		if (empty()) {Paragraph<KT> e(true); push_back(e);}
		else if (!back().first) {Paragraph<KT> e(true); push_back(e);}
		back().second.push_back(j);
	}	

	template <> inline void Entry<int>::operator-=(int j)
	{
		typedef int KT;
		if (empty()) {Paragraph<KT> e(false); push_back(e);}
		else if (back().first) {Paragraph<KT> e(false); push_back(e);}
		back().second.push_back(j);
	}	

	inline Journal& operator<<(Journal& j,Entry<double>& e)
	{
		e.T='D'; j<<e.T;
		return e.operator<<(j);
	}
	inline Journal& operator>>(Journal& j,Entry<double>& e){return e.operator>>(j);}

	template <> inline void Entry<double>::operator+=(double j)
	{
		typedef double KT;
		if (empty()) {Paragraph<KT> e(true); push_back(e);}
		else if (!back().first) {Paragraph<KT> e(true); push_back(e);}
		back().second.push_back(j);
	}	

	template <> inline void Entry<double>::operator-=(double j)
	{
		typedef double KT;
		if (empty()) {Paragraph<KT> e(false); push_back(e);}
		else if (back().first) {Paragraph<KT> e(false); push_back(e);}
		back().second.push_back(j);
	}	

	inline Journal& operator<<(Journal& j,Entry<string>& e)
	{
		e.T='S'; j<<e.T;
		return e.operator<<(j);
	}
	inline Journal& operator>>(Journal& j,Entry<string>& e){return e.operator>>(j);}

	template <> inline void Entry<string>::operator+=(string j)
	{
		typedef string KT;
		if (empty()) {Paragraph<KT> e(true); push_back(e);}
		else if (!back().first) {Paragraph<KT> e(true); push_back(e);}
		back().second.push_back(j);
	}	

	template <> inline void Entry<string>::operator-=(string j)
	{
		typedef string KT;
		if (empty()) {Paragraph<KT> e(false); push_back(e);}
		else if (back().first) {Paragraph<KT> e(false); push_back(e);}
		back().second.push_back(j);
	}	


} // TreeJournal

#endif // TREE_JOURNAL_H

