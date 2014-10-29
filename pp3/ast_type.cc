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
	PrintDebug("entry", "Entering named type");
	initLevel(NULL, true);
	printLevel();
	Decl* inTree = lookup(id, true);
	if (inTree == NULL) {
		PrintDebug("ntype", "Not found: %s\n", id->getName());
		ReportError::IdentifierNotDeclared(id, LookingForType);
	};
	PrintDebug("entry", "Entering named type");

};

void ArrayType::Check() {
	PrintDebug("entry", "Entering array");
/*
	initLevel(NULL, true);
	Decl * inTree = lookup(elemType->getName(), true);
	if(inTree == NULL) {
		ReportError::IdentifierNotDeclared(id, LookingForType);
	}
	*/
	PrintDebug("entry", "leaving array");
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


