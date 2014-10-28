/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"

void VarDecl::Check() {
	PrintDebug("vdecl", "entering var decl");
	initLevel(NULL, true);
	//lookup(id);
	printLevel();
	type->Check();
	PrintDebug("vdecl", "exiting var decl");
}
        
void ClassDecl::Check() {
	PrintDebug("cdecl", "entering class decl");
	initLevel(NULL, true);
	//lvl = new Level();
	members->addList(lvl);
	members->checkList();
	
	PrintDebug("cdecl", "leaving class decl");
};

void InterfaceDecl::Check() {
	PrintDebug("idecl", "entering interface decl");
	//lvl = new Level();
	members->addList(lvl);
	members->checkList();

	PrintDebug("idecl", "leaving interface decl");
};

void FnDecl::Check() {
	PrintDebug("fdecl", "entering fndecl");
	
	initLevel(NULL, true);
	//lvl = new Level();
	formals->addList(lvl);

	body->initLevel(lvl, true);
	//body->getLvl()->link(lvl);
	PrintDebug("fdecl", "entering bodycheck");
	body->Check();

	PrintDebug("fdecl", "leaving fndecl");
};

bool FnDecl::samePrototype(FnDecl * parent) {
	for(int i = 0; i < formals->NumElements(); ++i) {
		VarDecl *p, *c;
		p = parent->formals->Nth(i);
		c = formals->Nth(i);
		if(c->getType() != p->getType()) {
			return false;
		}
	}
	return true;
};


         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}


VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}
  

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
}


InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
}

	
FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}



