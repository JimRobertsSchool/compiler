/* File: ast_type.cc
 * -----------------
 * Implementation of type node classes.
 */
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>

#include "errors.h"
 
/* Class constants
 * ---------------
 * These are public constants for the built-in base types (int, double, etc.)
 * They can be accessed with the syntax Type::intType. This allows you to
 * directly access them and share the built-in types where needed rather that
 * creates lots of copies.
 */

Type *Type::intType    = new Type("int");
Type *Type::doubleType = new Type("double");
Type *Type::voidType   = new Type("void");
Type *Type::boolType   = new Type("bool");
Type *Type::nullType   = new Type("null");
Type *Type::stringType = new Type("string");
Type *Type::errorType  = new Type("error"); 

Type::Type(const char *n) {
    Assert(n);
    typeName = strdup(n);
}


bool Type::conformsTo(Type * other) {
	PrintDebug("conforms", "normal\n");
	PrepareScope();
	int n = other -> getNum();
	if (this == Type::nullType && (n == 2 || n == 1)) return true;
	if (this == Type::errorType || other == Type::errorType) return true;
	if (IsEquivalentTo(other)) return true;
	//if (other == Type::doubleType && this == Type::intType) return true;
	return false;
}

bool NamedType::conformsTo(Type * other) {
	PrintDebug("conforms", "named\n");
	PrepareScope();
	if (this == Type::errorType || other == Type::errorType) return true;
	if (this == Type::nullType) return true;
	if (getNum() != other -> getNum()) return false;
	// both named types
	NamedType * t = this;
	NamedType * o = (NamedType *)other;
	PrintDebug("conforms", "does %s conform to %s\n", this->id->GetName(), o->id->GetName());
	Decl * d = t->GetDeclForType();
	PrintDebug("conforms", "interface\n");
	if (d && d->IsInterfaceDecl()) return (!strcmp(d->GetName(), o->GetId()->GetName()));


	PrintDebug("conforms", "trying to find other in this' inheritance\n");
	while (d) {
		PrintDebug("conforms", "here 1\n");

		if (!d->IsClassDecl()) {
			PrintDebug("conforms", "here 2\n");
			return false;
		}
		ClassDecl * cd = (ClassDecl *)d;

		if (!strcmp(o->GetId()->GetName(), cd->GetId()->GetName())) {
			PrintDebug("conforms", "here 3\n");
			return true;
		}

		PrintDebug("conforms", "here 4\n");
		List<InterfaceDecl *> *imp = cd->getInterfaces();
		if (imp != NULL) {
			PrintDebug("conforms", "here 5\n");
			for(int i = 0; i < imp->NumElements(); ++i) {
				PrintDebug("conforms", "here 6\n");
				if(!strcmp(o->GetId()->GetName(), imp->Nth(i)->GetName())) {
					PrintDebug("conforms", "here 7\n");
					return true;
				}	       
			}
		}
		t = cd->getExtend();
		if (!t)  {
			break;
		}
		d = t->GetDeclForType();

	}

	PrintDebug("conforms", "here 8\n");
	return false;
}

bool ArrayType::conformsTo(Type * other) {
	PrintDebug("conforms", "array\n");
	PrepareScope();
	if (this == Type::errorType || other == Type::errorType) {
		PrintDebug("conforms", "here 1\n");
		return true;
	}
	if (getNum() != other -> getNum()) {
		PrintDebug("conforms", "here 2\n");
		return false;
	}

	ArrayType * at = (ArrayType *) other;

	PrintDebug("conforms", "here 3\n");
	return elemType->conformsTo(at->elemType);
}
	
NamedType::NamedType(Identifier *i) : Type(*i->GetLocation()) {
    Assert(i != NULL);
    (id=i)->SetParent(this);
} 

void NamedType::Check() {
    if (!GetDeclForType()) {
        isError = true;
        ReportError::IdentifierNotDeclared(id, LookingForType);
    }
}
Decl *NamedType::GetDeclForType() {
    if (!cachedDecl && !isError) {
        Decl *declForName = FindDecl(id);
        if (declForName && (declForName->IsClassDecl() || declForName->IsInterfaceDecl())) 
            cachedDecl = declForName;
    }
    return cachedDecl;
}

bool NamedType::IsInterface() {
    Decl *d = GetDeclForType();
    return (d && d->IsInterfaceDecl());
}

bool NamedType::IsClass() {
    Decl *d = GetDeclForType();
    return (d && d->IsClassDecl());
}

bool NamedType::IsEquivalentTo(Type *other) {
    NamedType *ot = dynamic_cast<NamedType*>(other);
    return ot && strcmp(id->GetName(), ot->id->GetName()) == 0;
}

ArrayType::ArrayType(yyltype loc, Type *et) : Type(loc) {
    Assert(et != NULL);
    (elemType=et)->SetParent(this);
}

void ArrayType::Check() {
    elemType->Check();
}

bool ArrayType::IsEquivalentTo(Type *other) {
    ArrayType *o = dynamic_cast<ArrayType*>(other);
    return (o && elemType->IsEquivalentTo(o->elemType));
}

