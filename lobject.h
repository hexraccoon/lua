/*
** $Id: lobject.h,v 1.32 1999/10/11 16:13:11 roberto Exp roberto $
** Type definitions for Lua objects
** See Copyright Notice in lua.h
*/

#ifndef lobject_h
#define lobject_h


#include <limits.h>

#include "lua.h"


#ifdef DEBUG
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#define LUA_INTERNALERROR(s)	assert(0)
#define LUA_ASSERT(c,s)		assert(c)
#else
#define LUA_INTERNALERROR(s)	/* empty */
#define LUA_ASSERT(c,s)		/* empty */
#endif


/*
** "real" is the type "number" of Lua
** GREP LUA_NUMBER to change that
*/
#ifndef LUA_NUM_TYPE
#define LUA_NUM_TYPE double
#endif


typedef LUA_NUM_TYPE real;

#define Byte lua_Byte	/* some systems have Byte as a predefined type */
typedef unsigned char  Byte;  /* unsigned 8 bits */


#define MAX_INT   (INT_MAX-2)  /* maximum value of an int (-2 for safety) */

typedef unsigned int IntPoint; /* unsigned with same size as a pointer (for hashing) */


/*
** Lua TYPES
** WARNING: if you change the order of this enumeration,
** grep "ORDER LUA_T"
*/
typedef enum {
  LUA_T_USERDATA =  0,  /* tag default for userdata */
  LUA_T_NUMBER   = -1,  /* fixed tag for numbers */
  LUA_T_STRING   = -2,  /* fixed tag for strings */
  LUA_T_ARRAY    = -3,  /* tag default for tables (or arrays) */
  LUA_T_PROTO    = -4,  /* fixed tag for functions */
  LUA_T_CPROTO   = -5,  /* fixed tag for Cfunctions */
  LUA_T_NIL      = -6,  /* last "pre-defined" tag */
  LUA_T_CLOSURE  = -7,
  LUA_T_CLMARK   = -8,  /* mark for closures */
  LUA_T_PMARK    = -9,  /* mark for Lua prototypes */
  LUA_T_CMARK    = -10, /* mark for C prototypes */
  LUA_T_LINE     = -11
} lua_Type;

#define NUM_TAGS  7


typedef union {
  lua_CFunction f;  /* LUA_T_CPROTO, LUA_T_CMARK */
  real n;  /* LUA_T_NUMBER */
  struct TaggedString *ts;  /* LUA_T_STRING, LUA_T_USERDATA */
  struct TProtoFunc *tf;  /* LUA_T_PROTO, LUA_T_PMARK */
  struct Closure *cl;  /* LUA_T_CLOSURE, LUA_T_CLMARK */
  struct Hash *a;  /* LUA_T_ARRAY */
  int i;  /* LUA_T_LINE */
} Value;


typedef struct TObject {
  lua_Type ttype;
  Value value;
} TObject;



/*
** String headers for string table
*/

typedef struct TaggedString {
  struct TaggedString *nexthash;  /* chain hash table */
  struct TaggedString *nextglobal;  /* chain global variables */
  unsigned long hash;
  int constindex;  /* hint to reuse constants (= -1 if this is a userdata) */
  union {
    struct {
      TObject globalval;
      long len;  /* if this is a string, here is its length */
    } s;
    struct {
      int tag;
      void *v;  /* if this is a userdata, here is its value */
    } d;
  } u;
  unsigned char marked;
  char str[1];   /* \0 byte already reserved */
} TaggedString;




/*
** Function Prototypes
*/
typedef struct TProtoFunc {
  struct TProtoFunc *next;
  int marked;
  struct TObject *consts;
  int nconsts;
  Byte *code;  /* ends with opcode ENDCODE */
  int lineDefined;
  TaggedString  *source;
  struct LocVar *locvars;  /* ends with line = -1 */
} TProtoFunc;

typedef struct LocVar {
  TaggedString *varname;           /* NULL signals end of scope */
  int line;
} LocVar;





/* Macros to access structure members */
#define ttype(o)        ((o)->ttype)
#define nvalue(o)       ((o)->value.n)
#define svalue(o)       ((o)->value.ts->str)
#define tsvalue(o)      ((o)->value.ts)
#define clvalue(o)      ((o)->value.cl)
#define avalue(o)       ((o)->value.a)
#define fvalue(o)       ((o)->value.f)
#define tfvalue(o)	((o)->value.tf)

#define protovalue(o)	((o)->value.cl->consts)


/*
** Closures
*/
typedef struct Closure {
  struct Closure *next;
  int marked;
  int nelems;  /* not included the first one (always the prototype) */
  TObject consts[1];  /* at least one for prototype */
} Closure;



typedef struct node {
  TObject key;
  TObject val;
  struct node *next;  /* for chaining */
} Node;

typedef struct Hash {
  int htag;
  Node *node;
  unsigned int size;
  Node *firstfree;  /* this position is free; all positions after it are full */
  struct Hash *next;
  int marked;
} Hash;


extern const char *const luaO_typenames[];


#define luaO_typename(o)        luaO_typenames[-ttype(o)]


extern const TObject luaO_nilobject;


#define luaO_equalObj(t1,t2)	((ttype(t1) != ttype(t2)) ? 0 \
                                      : luaO_equalval(t1,t2))
int luaO_equalval (const TObject *t1, const TObject *t2);
int luaO_redimension (int oldsize);
int luaO_str2d (const char *s, real *result);

#ifdef OLD_ANSI
void luaO_memup (void *dest, void *src, int size);
void luaO_memdown (void *dest, void *src, int size);
#else
#include <string.h>
#define luaO_memup(d,s,n)	memmove(d,s,n)
#define luaO_memdown(d,s,n)	memmove(d,s,n)
#endif

#endif
