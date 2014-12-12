/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"

void VarDecl::Emit() {
};

void ClassDecl::Emit() {
};

void FnDecl::Emit() {
	ClassDecl * m = this->inClass();

	/*
	char temp[100];
	sprintf(temp, "%s%s%s%s", m ? "_" : "", m ? m->getName():"", m || strcmp("main", getName()) ? "_":"", strdup(getName()));
	*/
	char * temp = genName();
	cg->GenLabel(temp);
	BeginFunc * fun = cg->GenBeginFunc();


	//int start = tempNumb == NULL ? 0 : *tempNumb;
	int start = 0;
	cg -> resetStack();

	List<Location *> parameters = List<Location *>();
	for (int i = 0; i < formals->NumElements(); ++i) {
		VarDecl * vd = formals -> Nth(i);
		Location * loc = new Location(fpRelative, varSize * i, vd->getName());
		parameters.Append(loc);
		locations->Enter(vd->getName(), loc, false);
		types->Enter(vd->getName(), vd->getType(), false);
	}

	body -> Emit();

	for (int i = 0; i < formals->NumElements(); ++i) {
		VarDecl * vd = formals -> Nth(i);
		Location * loc = parameters.Nth(i);
		locations->Remove(vd->getName(), loc);
		types->Remove(vd->getName(), vd->getType());
	}

	fun->SetFrameSize(varSize * (cg->getStackNum() - start));
	cg->GenEndFunc();

};

void ClassDecl::makeMembers() {
	List<FnDecl *> funs = List<FnDecl *>();
	List<VarDecl *> vars = List<VarDecl *>();
}

void ClassDecl::setMembers(List<const char *> * names, List<const char *> parents, Hashtable<Location *> l) {

}

ClassDecl * Decl::inClass() {

	Node * current = parent;

	while (current != NULL) {

		ClassDecl * cd = dynamic_cast<ClassDecl*>(current);
		if (cd != NULL) cd;
		current = current->GetParent();
		
	}

	return NULL;
}

char * FnDecl::genName() {
	ClassDecl * m = this->inClass();

	char temp[100];
	sprintf(temp, "%s%s%s%s", m ? "_" : "", m ? m->getName():"", m || strcmp("main", getName()) ? "_":"", strdup(getName()));
	return strdup(temp);
}
        
/* all original below */
         
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
    locs = new Hashtable<Location *>();
    methodNames = new List<const char *>();
    methodParent = new List<const char *>();
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



