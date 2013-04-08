/********************************************************************************
*
* File: basic.h
* Some useful classes 
* Programmer: Leonidas Fegaras, UTA
* Date: 12/17/97
*
********************************************************************************/

#include <string.h>
#include <stream.h>
#include <fstream.h>

/* The garbage collector */
#include "/usr/local/include/gc/gc_cpp.h"


/* force the use of garbage collector */
#define malloc GC_malloc
#define realloc GC_realloc


/*--------------------------------------------------------------------------------
-  Strings
---------------------------------------------------------------------------------*/


class string: public gc {
private:

  char* sp;		      /* the string content */

public:

  /* string constructors */
  string();
  string( const char* );
//  string( string& );

  /* assignment to string */
  string& operator = ( string );
  string& operator = ( const char* );

  /* return the string content; use it with caution */
  inline char* content() { return sp; }

  /* string comparisons */
  bool operator == ( string );
  bool operator != ( string );
  bool operator <= ( string );
  bool operator >= ( string );
  bool operator < ( string );
  bool operator > ( string );

  /* string concatenation */
  friend string operator + ( string, string );
  friend string operator + ( string, const char* );
  friend string operator + ( const char*, string );

  /* string length */
  int length ();

  /* the nth character of string */
  char nthchar ( int );

  /* create a new string by copying the k characters of the string starting from n */
  string substring ( int n, int k );

  /* position of a string inside a string */
  int position ( string );

  /* print/read a string */
  friend ostream& operator << ( ostream&, string );
  friend istream& operator >> ( istream&, string );

};


typedef string* String;


/*--------------------------------------------------------------------------------
-  list<T> is a list of elements of type T
----------------------------------------------------------------------------------*/


template<class T>
class list: public gc {
private:

  enum { Nil, Cons } tag;

public:

  T	hd;	/* list head */
  list*	tl;	/* list tail */

  /* list constructor */
  list () { tag = Nil; };

  /* test whether the list is null or not */
  inline bool nullp () { return (tag==Nil); };
  inline bool consp () { return (tag==Cons); };

  /* construct a new list cell by inserting x on top of r */
  list* cons ( T );
  friend inline list* Cons ( T x, list* r ) { return r->cons(x); };

  /* list size */
  int length();

  /* append y at the end of x forming a new list */
  list* append ( list* );
  friend inline list* Append ( list* x, list* y ) { return x->append(y); };

  /* list reverse */
  list* reverse ();

  /* check for membership in a list */
  bool member ( T );

  /* the nth element of a list */
  T nth ( int );

  /* display a list */
  friend ostream& operator<< ( ostream&, list* );

};


typedef list<string>* Names;


/*--------------------------------------------------------------------------------
-  binding lists associate names with values of type T
--------------------------------------------------------------------------------*/


template<class T>
class binding: public gc {
private:

  typedef struct { string name; T info; } pair;

  list<pair>* env;

public:

  /* binding constructors */
  binding () { env = new list<pair>; };
  binding ( string, T );

  /* extend the binding list with a new binding */
  binding* extend ( string, T );

  /* append two binding lists */
  binding* extend ( binding* r );

  /* find a value T given its name */
  T find ( string );

  /* is the name bound in the binding list? */
  bool in ( string );

  /* return the list of all the names bound in the binding list */
  Names names ();

  inline int length () { return env->length(); };

  /* display a binding list */
  friend ostream& operator<< ( ostream&, binding* );

};



/*--------------------------------------------------------------------------------
-  This is the data structure to represent Abstract Syntax Trees (ASTs)
--------------------------------------------------------------------------------*/


class ast;

typedef ast* Ast;

class ast: public gc {
private:

  enum { VARIABLE, INTEGER, REAL, STRING, NODE } tag;

  union { String				Variable;
          int					Integer;
          float					Real;
          String				Cstring;
    struct { String		Name;
	     list<Ast>*		Arguments; }	Node;
        } info;

public:

  typedef ast* Ast;

  /* AST constructors */
  ast () { tag = INTEGER; info.Integer = 0; };
  friend Ast variable ( string ) ;
  friend Ast integer ( int );
  friend Ast real ( float );
  friend Ast cstring ( string );
  friend Ast node ( string, list<Ast>* );

  /* AST accessors */
  inline string variable () { return *info.Variable; };
  inline int integer () { return info.Integer; };
  inline float real () { return info.Real; };
  inline string cstring () { return *info.Cstring; };
  inline string name () { return *info.Node.Name; };
  inline list<Ast>* arguments () { return info.Node.Arguments; };

  /* testing the type of ast */
  inline bool variablep () { return (tag==VARIABLE); };
  inline bool integerp () { return (tag==INTEGER); };
  inline bool realp () { return (tag==REAL); };
  inline bool stringp () { return (tag==STRING); };
  inline bool nodep () { return (tag==NODE); };

  /* ast deep equality */
  bool operator == ( Ast );

  /* print with indentations to look nice */
  void pretty_print ( int );

  /* print an ast */
  friend ostream& operator<< ( ostream&, Ast );

private:

  int size (); /* used in pretty printing */

};


#define Nil (new list<Ast>)


/*--------------------------------------------------------------------------------
-  The template methods' code must be included here, since templates are macros
--------------------------------------------------------------------------------*/

#include "basic.templates"
