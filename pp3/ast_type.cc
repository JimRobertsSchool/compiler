/* File: ast_type.cc
 * -----------------
 * Implementation of type node classes.
 */
#include "ast_type.h"
#include "ast_decl.h"
#include "errors.h"
#include <string.h>

 
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

void NamedType::Check() {
	//PrintDebug("entry", "Entering named type");
	initLevel(NULL, true);
	printLevel();
	Decl* inTree = lookup(id, true);
	if (inTree == NULL) {
		//PrintDebug("ntype", "Not found: %s\n", id->getName());
		reasonT r = LookingForType;
		int s = parent->sGetT();
		if(s == s_CDecl) {
			ClassDecl * p = (ClassDecl *)parent;
			if (p->getExtends() != NULL && strcmp(p->getExtends()->getId()->getName(), id->getName())) {
				//PrintDebug("ntype", "found: %s, %s\n", id->getName(), p->getExtends()->getId()->getName());
				ReportError::IdentifierNotDeclared(id, LookingForInterface);
			} else {
				ReportError::IdentifierNotDeclared(id, LookingForClass);
			}
			/*
			   List<NamedType *> *t = ((ClassDecl *)parent)->getInterface();
			   for(int i = 0; i < t->NumElements(); ++i) {
			   if (!strcmp(t->Nth(i)->getId()->getName(), id->getName())) {
			   //PrintDebug("ntype", "found: %s\n", id->getName());
			   ReportError::IdentifierNotDeclared(id, LookingForClass);
			   return;
			   }
			   }
			   */

		}
		//else if (s == s_CDecl) ReportError::IdentifierNotDeclared(id, LookingForClass);
		else ReportError::IdentifierNotDeclared(id, LookingForType);
	};
	//PrintDebug("entry", "Entering named type");

};

void ArrayType::Check() {
	//PrintDebug("entry", "Entering array");
	initLevel(NULL, true);
	/*
	//PrintDebug("atype", "here zero %d", location->first_line);
	yyltype l = yyltype();
	l.first_line =   location->first_line;
	l.last_line =    location->last_line;
	l.first_column = location->first_column;
	l.last_column =  location->last_column;
	PrintDebug("atype", "here zero %s", elemType->getName());
	Identifier * fake = new Identifier(l, elemType->getName());
	//PrintDebug("atype", "here one");
	Decl * inTree = lookup(fake, true);
	PrintDebug("atype", "here two");
	if (inTree == NULL) {
		//PrintDebug("atype", "here three");
		ReportError::IdentifierNotDeclared(fake, LookingForType);
	}
	*/
	if(elemType->sGetT() == s_NType) {
		NamedType * t = ((NamedType *)elemType);
		Decl * inTree = lookup(t->getId(), true);
		if (inTree == NULL) {
			//PrintDebug("ntype", "Not found: %s\n", t->getId()->getName());
			reasonT r = LookingForType;
			int s = parent->sGetT();
			if(s == s_CDecl) {
				ClassDecl * p = (ClassDecl *)parent;
				if (p->getExtends() != NULL && strcmp(p->getExtends()->getId()->getName(), t->getId()->getName())) {
					ReportError::IdentifierNotDeclared(t->getId(), LookingForInterface);
				} else {
					ReportError::IdentifierNotDeclared(t->getId(), LookingForClass);
				}
			}
				   else ReportError::IdentifierNotDeclared(t->getId(), LookingForType);
		};
	} else if(elemType->sGetT() == s_AType) {
		((ArrayType *)elemType)->Check();
	}
	//PrintDebug("entry", "leaving array");
};

bool NamedType::IsEquivalentTo(Type * other) {
	//PrintDebug("entry", "Entering named equivalence check");

	if(other == NULL) return false;

	if (other-> sGetT() == s_DType) {
		//PrintDebug("entry", "leaving named equivalence check");
		return false;

	} else if(other-> sGetT() == s_NType) { 
		//PrintDebug("entry", "leaving named equivalence check");
		const char * one = id->getName();
		//PrintDebug("entry", "two %s", one);
		const char * two = ((NamedType *)other)->id->getName();

		bool toReturn  = !(strcmp(one, two));
		//PrintDebug("entry", "Leaving named equivalence check %s, %s, %s", toReturn ? "true" : "false", one, two);
		return toReturn;

	} else if (other-> sGetT() == s_AType) {
		//PrintDebug("entry", "leaving named equivalence check");
		return false;
	}

	//PrintDebug("entry", "leaving named equivalence check");
	return false;
};

bool ArrayType::IsEquivalentTo(Type * other) {
	//PrintDebug("entry", "Entering array equivalence check");

	if(other == NULL) return false;

	if (other-> sGetT() == s_DType) {
		//PrintDebug("entry", "Leaving array equivalence check false");
		return false;

	} else if(other-> sGetT() == s_NType) { 
		//PrintDebug("entry", "Leaving array equivalence check false");
		return false;

	} else if (other-> sGetT() == s_AType) {
		//PrintDebug("entry", "one");

		bool toReturn =(elemType->IsEquivalentTo(((ArrayType *)other)->elemType)); 
		//PrintDebug("entry", "Leaving array equivalence check %s", toReturn ? "true" : "false");
		return toReturn;
	}

	//PrintDebug("entry", "Leaving array equivalence check false");
	return false;
};

Type::Type(const char *n) {
    Assert(n);
    typeName = strdup(n);
}



	
NamedType::NamedType(Identifier *i) : Type(*i->GetLocation()) {
    Assert(i != NULL);
    (id=i)->SetParent(this);
} 


ArrayType::ArrayType(yyltype loc, Type *et) : Type(loc) {
    Assert(et != NULL);
    (elemType=et)->SetParent(this);
}


