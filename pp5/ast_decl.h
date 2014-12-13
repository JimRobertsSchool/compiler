/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 *
 * pp5: You will need to extend the Decl classes to implement 
 * code generation for declarations.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "list.h"
#include "ast_type.h"

class Type;
class NamedType;
class Identifier;
class Stmt;
class ClassDecl;
class FnDecl;

typedef enum {UNKNOWN=0, varDecl, functionDecl, classDecl, interfaceDecl} declType;

class Decl : public Node 
{
  protected:
    Identifier *id;
  
  public:
    Decl(Identifier *name);
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
    virtual declType getDeclType() { return UNKNOWN; }; 
    Identifier * getId() { return id; };
    char * getName() { return id->getName(); };
    //ClassDecl * inClass();
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
    
  public:
    Location * loc;
    VarDecl(Identifier *name, Type *type);
    declType getDeclType() { return varDecl; };
    Type * getType() { return type; };
    void Emit();
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;

  public:
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
    declType getDeclType() { return classDecl; };
    void Emit();
    void makeMembers();
    void setMembers(ClassDecl * p);
    Hashtable<Location *> * locs;
    List<char *> * methodNames;
    List<char *> * methodParent;
    List<const char *> * table;
    List<FnDecl *> * fns;
    List<VarDecl*> * vdl;
    Hashtable<FnDecl *> * fdecs;
    Hashtable<Type *> * vdecs;
    int size;
    char * getExtends() { return extends ? extends->getName() : NULL; };
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    InterfaceDecl(Identifier *name, List<Decl*> *members);
    declType getDeclType() { return interfaceDecl; };
};

class FnDecl : public Decl 
{
  protected:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body;
    
  public:
    Location * loc;
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
    declType getDeclType() { return functionDecl; };
    void Emit();
    Type * getRet() { return returnType; }
    char * genName();
};

#endif
