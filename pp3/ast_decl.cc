/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "errors.h"

void VarDecl::Check() {
	PrintDebug("entry", "entering var decl");
	initLevel(NULL, true);
	//lookup(id);
	printLevel();
	PrintDebug("entry", "entering type check");
	type->Check();
	PrintDebug("entry", "exiting var decl");
}
        
void ClassDecl::Check() {
	PrintDebug("entry", "entering class decl");
	initLevel(NULL, true);
	if (implements != NULL) {
		implements->checkList();
	}
	if (extends != NULL) {
		extends->Check();
		Decl * d = lookup(extends->getId());
		if (d!=NULL) {
			ClassDecl * ext = (ClassDecl *)d;
			for(int i = 0; ext->members!=NULL && i < ext->members->NumElements(); ++i) {
				Decl* d2 = ext->members->Nth(i);
				if (d2->sGetT() != s_FDecl) lvl->add(d2);
			}
		}
	}
	//lvl = new Level();
	members->addList(lvl);
	members->checkList();


	/*
	if (extends != NULL) {
		Decl * d = lookup(extends->getId());
		if (d!=NULL) {

			ClassDecl * ext = (ClassDecl *)d;
			

	*/

	bool listMade = false;

	if(extends != NULL && implements == NULL) {
		listMade = true;
		implements = new List<NamedType *>();
	}
	PrintDebug("cdecl", "classmade = %d, implements is %ld\n", listMade, implements);
	if(extends!=NULL && implements != NULL) implements->Append(extends);

	
	for(int i = 0; implements != NULL && i < implements->NumElements(); ++i) {

		Decl * d = lookup(implements->Nth(i)->getId());
		if(d == NULL) {
			//ReportError::IdentifierNotDeclared(implements->Nth(i)->getId(), LookingForInterface);
			continue;
		}
		if(d->sGetT() == s_IDecl) {
			//interface not found
			PrintDebug("cdecl", "in interface");
			InterfaceDecl * imp = (InterfaceDecl *) d;
			List<Decl *> * mem = imp->getMembers();
			if (mem == NULL) continue;
			for(int j = 0; j < mem->NumElements(); ++j) {
				if(mem->Nth(j)->sGetT() != s_FDecl) continue;
				FnDecl * toDo = (FnDecl *) mem->Nth(j);
				FnDecl * toComp = (FnDecl *)lookup(mem->Nth(j)->getId(), false);
				if (toComp != NULL && !(toDo->samePrototype(toComp))) {
						ReportError::OverrideMismatch(toComp);
						}
			}
		} else if (d->sGetT() == s_CDecl) {
			ClassDecl * ext = (ClassDecl *)d;
			PrintDebug("cdecl", "found extended class %s", ext->id->getName());
			List<Decl *> * mem = ext->members;
			if (mem == NULL) continue;
			for(int j = 0; j < mem->NumElements(); ++j) {
				if(mem->Nth(j)->sGetT() != s_FDecl) continue;
				FnDecl * toDo = (FnDecl *) mem->Nth(j);
				PrintDebug("cdecl", "found function %s", toDo->getId()->getName());
				FnDecl * toComp = (FnDecl *)lookup(mem->Nth(j)->getId(), false);
				if(toComp!=NULL) 
				PrintDebug("cdecl", "found function in current class %s", toComp->getId()->getName());
				else PrintDebug("cdecl", "%s not founds", toDo->getId()->getName());
				if (toComp != NULL && !(toDo->samePrototype(toComp))) {
					ReportError::OverrideMismatch(toComp);
				}
				PrintDebug("cdecl", "end of iteration");
			}
		}
	}
	if(extends!=NULL && implements != NULL) implements->RemoveAt(implements->NumElements()-1);
	if(listMade) {
		delete implements;
		implements = NULL;
	}
	
	
	PrintDebug("entry", "leaving class decl");
};

void InterfaceDecl::Check() {
	PrintDebug("entry", "entering interface decl");
	initLevel(NULL, true);
	//lvl = new Level();
	members->addList(lvl);
	members->checkList();

	PrintDebug("entry", "leaving interface decl");
};

void FnDecl::Check() {
	PrintDebug("entry", "entering fndecl");
	
	initLevel(NULL, true);
	//lvl = new Level();
	PrintDebug("fdecl", "adding formals");
	formals->addList(lvl);
	formals->checkList();

	PrintDebug("fdecl", "initializing body");
	if(body == NULL) PrintDebug("fdecl", "body is null");
	if (body != NULL) {
		body->initLevel(lvl, true);
		//body->getLvl()->link(lvl);
		PrintDebug("fdecl", "entering bodycheck");
		body->Check();
	}

	PrintDebug("entry", "leaving fndecl");
};

bool FnDecl::samePrototype(FnDecl * parent) {
	if(parent != NULL && parent->formals != NULL && formals != NULL && parent->formals->NumElements() != formals->NumElements()) return false;
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



