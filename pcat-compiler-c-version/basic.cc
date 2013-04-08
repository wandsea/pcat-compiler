#include "basic.h"

int string::length () { return strlen(sp); };

string::string() { sp = NULL; };

//string::string( string& b ) { sp = b.sp; };

string::string( const char* s ) { sp = new (GC) char[strlen(s)+1]; strcpy(sp,s); };


string& string::operator = ( string y ) {
  sp = y.sp;
  return *this;
};

string& string::operator = ( const char* s ) {
  sp = new (GC) char[strlen(s)+1];
  strcpy(sp,s);
  return *this;
};

bool string::operator == ( string a ) { return strcmp(sp,a.sp)==0; };
bool string::operator != ( string a ) { return strcmp(sp,a.sp)!=0; };
bool string::operator >= ( string a ) { return strcmp(sp,a.sp)>=0; };
bool string::operator <= ( string a ) { return strcmp(sp,a.sp)<=0; };
bool string::operator > ( string a ) { return strcmp(sp,a.sp)>0; };
bool string::operator < ( string a ) { return strcmp(sp,a.sp)<0; };

ostream& operator << ( ostream& s, string a ) { return s << a.content(); };
istream& operator << ( istream& s, string a ) { return s >> a.content(); };

char string::nthchar ( int n ) { return sp[n]; };

string string::substring ( int n, int size ) {
  string r;
  if (n>length())
     return r;
  r.sp = new (GC) char[size];
  strncpy(r.sp,sp+n,size);
  return r;
};

int string::position ( string s ) {
  char* ptr;
  ptr = strstr(sp,s.sp);
  if (ptr==NULL)
     return -1;
  return (int) ptr - (int) sp;
};

string operator + ( string a, string b ) {
  string r;
  r.sp = new (GC) char[strlen(a.sp)+strlen(b.sp)+1];
  strcpy(r.sp,a.sp);
  strcat(r.sp,b.sp);
  return r;
};

string operator + (string a, const char* s) {
  string r;
  r.sp = new (GC) char[strlen(s)+strlen(a.sp)+1];
  strcpy(r.sp,a.sp);
  strcat(r.sp,s);
  return r;
};

string operator + (const char* a, string b) {
  string r;
  r.sp = new (GC) char[strlen(a)+strlen(b.sp)+1];
  strcpy(r.sp,a);
  strcat(r.sp,b.sp);
  return r;
};



/*--------------------------------------------------------------------------------*/


Ast variable (string s) {
  ast *e = new ast;
  e->tag = ast::VARIABLE;
  e->info.Variable = new string(s);
  return e;
};


Ast integer (int n) {
  ast *e = new ast;
  e->tag = ast::INTEGER;
  e->ast::info.Integer = n;
  return e;
};


Ast real (float n) {
  ast *e = new ast;
  e->tag = ast::REAL;
  e->ast::info.Real = n;
  return e;
};


Ast cstring (string s) {
  ast *e = new ast;
  e->tag = ast::STRING;
  e->ast::info.Cstring = new string(s);
  return e;
};


Ast node (string s, list<Ast>* r) {
   ast *e = new ast;
   e->tag = ast::NODE;
   e->ast::info.Node.Name = new string(s);
   e->ast::info.Node.Arguments = r;
   return e;
};


ostream& operator<<( ostream& s, Ast e ) {
  if (e->variablep()) s << *e->info.Variable;
  else if (e->integerp()) s << e->info.Integer;
  else if (e->realp()) s << e->info.Real;
  else if (e->stringp()) s << '\"' << *e->info.Cstring << '\"';
  else if (e->nodep())
       s << *e->info.Node.Name << e->info.Node.Arguments;
};


int ast::size () {
  if (variablep()) return strlen(info.Variable->content());
  else if (integerp()) return strlen(form("%i",info.Integer));
  else if (realp()) return strlen(form("%f",info.Real));
  else if (stringp()) return strlen(info.Cstring->content())+2;
  else if (nodep())
       { int n = info.Node.Name->length()+1;
	 for(list<Ast>* r = info.Node.Arguments; r->consp(); r=r->tl)
	    n += r->hd->size()+1;
	 return n;
	};
  return 0;
};

void ast::pretty_print (int pos) {
  if (variablep()) cout << *info.Variable;
  else if (integerp()) cout << info.Integer;
  else if (realp()) cout << info.Real;
  else if (stringp()) cout << '\"' << *info.Cstring << '\"';
  else if (nodep())
	  if (info.Node.Arguments->consp() && pos+size()>100)
	  { cout << *info.Node.Name << '(';
	    int new_pos = pos+info.Node.Name->length()+1;
	    info.Node.Arguments->hd->pretty_print(new_pos);
	    for(list<Ast>* r=info.Node.Arguments->tl; r->consp(); r=r->tl)
	    { cout << ",\n";
	      for(int i=0; i<new_pos; i++) cout << ' ';
	      r->hd->pretty_print(new_pos);
	    };
	    cout << ")";
	  }
	  else cout << *info.Node.Name ;//<< info.Node.Arguments;
};


bool ast::operator == ( Ast x ) {
  if (&(*this)==&(*x)) return true;
  else if (variablep() && x->variablep())
       return *info.Variable == *x->info.Variable;
  else if (integerp() && x->integerp())
       return info.Integer == x->info.Integer;
  else if (realp() && x->realp())
       return info.Real == x->info.Real;
  else if (stringp() && x->stringp())
       return *info.Cstring == *x->info.Cstring;
  else if (nodep() && x->nodep())
          if (*info.Node.Name == *x->info.Node.Name)
	     { bool b = true;
	       list<Ast>* s = x->info.Node.Arguments;
	       list<Ast>* r = info.Node.Arguments;
	       for(;b && r->consp() && s->consp(); r=r->tl, s=s->tl)
		  b = b && (r->hd == s->hd);
	       return (b && r->nullp() && s->nullp());
             }
          else return false;
  else return false;
};

