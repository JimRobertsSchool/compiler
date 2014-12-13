/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h> // strdup
#include <stdio.h>  // printf

void Node::swapType(Location * loc, Location * r) {
	if (ints->Lookup(loc->GetName()) != NULL) {
		ints->Enter(r->GetName(), (void *)1, false);
	} else if (strings->Lookup(loc->GetName()) != NULL) {
		strings->Enter(r->GetName(), (void *)1, false);
	} else if (booleans->Lookup(loc->GetName()) != NULL) {
		booleans->Enter(r->GetName(), (void *)1, false);
	}
}

Hashtable<Location *> *Node::locations;
Hashtable<void *> *Node::strings;
Hashtable<void *> *Node::booleans;
Hashtable<void *> *Node::ints;
Hashtable<Type *> *Node::types;
Hashtable<ClassDecl*> *Node::hclass;
Hashtable<FnDecl*> *Node::hfns;
CodeGenerator *Node::cg;
int Node::varSize;
int *Node::tempNumb;

Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
}

Node::Node() {
    location = NULL;
    parent = NULL;
}
	 
Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
} 

ClassDecl * Node::inClass() {

	Node * current = parent;

	while (current != NULL) {

		ClassDecl * cd = dynamic_cast<ClassDecl*>(current);
		if (cd != NULL) return cd;
		current = current->GetParent();
		
	}

	return NULL;
}


