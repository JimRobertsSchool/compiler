/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"

using namespace std;

void VarDecl::Emit() {
};

void ClassDecl::Emit() {
	for (int i = 0; i < fns->NumElements(); ++i) {
		FnDecl * fd = fns->Nth(i);
		fd->Emit();
	}
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
		Location * loc = new Location(fpRelative, varSize * i + cg->OffsetToFirstParam + (4*(inClass()?1:0)), vd->getName());
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

	for (int i = 0; i < members->NumElements(); ++i) {
		Decl * d = members->Nth(i);

		if (d->getDeclType() == varDecl) {
			vars.Append(dynamic_cast<VarDecl *>(d));
			vdl->Append(dynamic_cast<VarDecl *>(d));
		} else {
			funs.Append(dynamic_cast<FnDecl *>(d));
			fns->Append(dynamic_cast<FnDecl *>(d));
			fdecs->Enter(d->getName(), dynamic_cast<FnDecl *>(d));
		}
	}

	for (int i = 0; i < funs.NumElements(); ++i) {
		FnDecl * f = funs.Nth(i);
		Location * l = locs->Lookup(f->getName());
		if (l) {
			for (int j = 0; j < methodNames->NumElements(); ++j) {
				if (!strcmp(methodNames->Nth(j), f->getName())) {
					methodParent->ReplaceAt(getName(), j);
					j += 100;
				}
			}
		} else {
			locs->Enter(f->getName(), new Location(gpRelative, 4*methodNames->NumElements(), f->getName()));
			methodNames->Append(f->getName());
			methodParent->Append(getName());
		}
	}

	for (int i = 0; i < vdl->NumElements(); ++i) {
		//vars.Append(vdl->Nth(i));
	}
	
	for (int i = 0; i < vars.NumElements(); ++i) {
		VarDecl * v = vars.Nth(i);
		locs->Enter(v->getName(), new Location(fpRelative, size + 4 * (i+1), v->getName()));
		vdecs->Enter(v->getName(), v->getType());
	}
	size += 4 * vars.NumElements();


	for (int i = 0; i < methodNames->NumElements(); ++ i) {
		string s = "_";
		s.append(methodParent->Nth(i));
		s.append(".");
		s.append(methodNames->Nth(i));
		table->Append(strdup(s.c_str()));
	}

}

void ClassDecl::setMembers(ClassDecl * p) {
List<char *> * names = p->methodNames; List<char *> * parents = p->methodParent; Hashtable<Location *> * l = p->locs; int s = p->size;


	//size = s;

	for (int i = 0; i < names->NumElements(); ++i) {
		methodNames->Append(names->Nth(i));
		methodParent->Append(parents->Nth(i));
	}

	for (int i = 0; i < p->vdl->NumElements(); ++i) {
		VarDecl * vd = p->vdl->Nth(i);
		vdl->Append(p->vdl->Nth(i));
		vdecs->Enter(vd->getName(), vd->getType());
	}

	for (int i = 0; i < p->fns->NumElements(); ++i) {
		FnDecl * fd = p->fns->Nth(i);
		//fns->Append(p->fns->Nth(i));
		fdecs->Enter(fd->getName(), fd);
	}

	size = p->size;

	Iterator<Location*> iter = l->GetIterator();
	Location *decl;
	while ((decl = iter.GetNextValue()) != NULL) {
		locs->Enter(decl->GetName(), decl);
	}

}

char * FnDecl::genName() {
	ClassDecl * m = this->inClass();

	char temp[100];
	if (m) {
		sprintf(temp, "%s%s%s%s", "_", m->getName(), ".", strdup(getName()));
	} else {
		sprintf(temp, "%s%s%s%s", m ? "_" : "", m ? m->getName():"", m || strcmp("main", getName()) ? "_":"", strdup(getName()));
	}
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
    fdecs = new Hashtable<FnDecl *>();
    vdecs = new Hashtable<Type*>();
    methodNames = new List<char *>();
    methodParent = new List<char *>();
    table = new List<const char *>();
    fns = new List<FnDecl *>();
    vdl = new List<VarDecl *>();
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



