/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "scope.h"
#include "errors.h"
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}

bool Decl::ConflictsWithPrevious(Decl *prev) {
    ReportError::DeclConflict(this, prev);
    return true;
}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}
  
void VarDecl::Check() { type->Check(); }

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
    cType = new NamedType(n);
    cType->SetParent(this);
    convImp = NULL;
}

void ClassDecl::Check() {
	PrintDebug("cdecl", "entering cdecl\n");

    if (extends && !extends->IsClass()) {
        ReportError::IdentifierNotDeclared(extends->GetId(), LookingForClass);
        extends = NULL;
    }
    for (int i = 0; i < implements->NumElements(); i++) {
        NamedType *in = implements->Nth(i);
        if (!in->IsInterface()) {
            ReportError::IdentifierNotDeclared(in->GetId(), LookingForInterface);
            implements->RemoveAt(i--);
        }
    }
    PrepareScope();
    members->CheckAll();
    List<FnDecl*> funs = List<FnDecl*>();
    for (int i = 0; i < members->NumElements(); ++i) {
	    FnDecl * f = dynamic_cast<FnDecl *>(members->Nth(i));
	    if (f) {
		PrintDebug("cdecl", "adding %s\n", f->GetName());
		    funs.Append(f);
	   }
    }
    // for all the implemented classes
    for (int i = 0; i < convImp->NumElements(); ++i) {
	    InterfaceDecl * dd = convImp->Nth(i);
	    List<Decl *> *mems = dd->getMembers();
	    // for all functions in the implemented class
	    for (int j = 0; j < mems->NumElements(); ++j) {
		    FnDecl * fd = dynamic_cast<FnDecl*>(mems->Nth(j));
		    if (fd) {
			    // find in current class
			    for (int k = 0; k < funs.NumElements(); ++k) {
				    FnDecl * cd = funs.Nth(k);
				    if (cd->MatchesPrototype(fd)) {
					goto continue_inner;
				    }
			    }
			    // fd Not found
			    ReportError::InterfaceNotImplemented(this, implements->Nth(i));
			    goto continue_outer;
		    }
continue_inner:
	    ;
	    }
continue_outer:
	    ;
    }
}

// This is not done very cleanly. I should sit down and sort this out. Right now
// I was using the copy-in strategy from the old compiler, but I think the link to
// parent may be the better way now.
Scope *ClassDecl::PrepareScope()
{
    if (nodeScope) return nodeScope;
    nodeScope = new Scope();  
    if (extends) {
        ClassDecl *ext = dynamic_cast<ClassDecl*>(parent->FindDecl(extends->GetId())); 
        if (ext) nodeScope->CopyFromScope(ext->PrepareScope(), this);
    }
    convImp = new List<InterfaceDecl*>;
    for (int i = 0; i < implements->NumElements(); i++) {
        NamedType *in = implements->Nth(i);
        InterfaceDecl *id = dynamic_cast<InterfaceDecl*>(in->FindDecl(in->GetId()));
        if (id) {
		nodeScope->CopyFromScope(id->PrepareScope(), NULL);
            convImp->Append(id);
	  }
    }
    members->DeclareAll(nodeScope);
    return nodeScope;
}



InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
}

void InterfaceDecl::Check() {
    PrepareScope();
    members->CheckAll();
}
  
Scope *InterfaceDecl::PrepareScope() {
    if (nodeScope) return nodeScope;
    nodeScope = new Scope();  
    members->DeclareAll(nodeScope);
    return nodeScope;
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

void FnDecl::Check() {
	PrintDebug("fdecl", "entering fdecl\n");
	PrintDebug("fdecl", "here 1\n");
	if (returnType) {
		PrintDebug("fdecl", "here 2\n");
		returnType->Check();
		PrintDebug("fdecl", "here 3\n");
	}
	PrintDebug("fdecl", "here 4\n");
    if (body) {
	    PrintDebug("fdecl", "here 5\n");
        nodeScope = new Scope();
	PrintDebug("fdecl", "here 6\n");
        formals->DeclareAll(nodeScope);
	PrintDebug("fdecl", "here 7\n");
        formals->CheckAll();
	PrintDebug("fdecl", "here 8\n");
	body->Check();
	PrintDebug("fdecl", "here 9\n");
    }
    PrintDebug("fdecl", "here 10\n");
	PrintDebug("fdecl", "leaving fdecl\n");
}

bool FnDecl::ConflictsWithPrevious(Decl *prev) {
 // special case error for method override
    if (IsMethodDecl() && prev->IsMethodDecl() && parent != prev->GetParent()) { 
        if (!MatchesPrototype(dynamic_cast<FnDecl*>(prev))) {
            ReportError::OverrideMismatch(this);
            return true;
        }
        return false;
    }
    ReportError::DeclConflict(this, prev);
    return true;
}

bool FnDecl::IsMethodDecl() 
  { return dynamic_cast<ClassDecl*>(parent) != NULL || dynamic_cast<InterfaceDecl*>(parent) != NULL; }

bool FnDecl::MatchesPrototype(FnDecl *other) {
    if (!returnType->IsEquivalentTo(other->returnType)) return false;
    if (formals->NumElements() != other->formals->NumElements())
        return false;
    for (int i = 0; i < formals->NumElements(); i++)
        if (!formals->Nth(i)->GetDeclaredType()->IsEquivalentTo(other->formals->Nth(i)->GetDeclaredType()))
            return false;
    return true;
}



