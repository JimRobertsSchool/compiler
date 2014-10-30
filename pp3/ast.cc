/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h> // strdup
#include <stdio.h>  // printf

Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
    lvl = NULL;
}

Node::Node() {
    location = NULL;
    parent = NULL;
    lvl = NULL;
}
	 
Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
} 

Level* Node::initLevel(Level * p, bool link) {
	if(lvl==NULL) {
		lvl = new Level();
	//	PrintDebug("entry", "one");
	}
	if (link) {
		lvl->link(p);
	//	PrintDebug("entry", "two");
	}
	if (link && p==NULL) {
		lvl->link(GetParent()->lvl);
	//	PrintDebug("entry", "three");
	}
	//PrintDebug("entry", "returning");
	return lvl;
};

Decl* Node::lookup(Identifier * id, bool parents) {
	Decl* toReturn = NULL;
	toReturn = lvl->find(id);

	if (toReturn == NULL && parents) {
		PrintDebug("find", "Looking in parents");
		Level * p = lvl->getParent();
		while(p!=NULL && toReturn == NULL) {
			PrintDebug("find", "up scope");
			p->print();
			toReturn=p->find(id);
			p = p->getParent();
		}
	}
	if (toReturn != NULL) {
		PrintDebug("find", toReturn->getId()->getName());
	} else {
		PrintDebug("find", "ID not found :(\n");
	}

	return toReturn;
};

void Node::printLevel() {

	Level * cur = this->lvl;
	while(cur != NULL) {
		/*
		Iterator<Decl*> it = cur->lvl->getScope()->GetIterator();
		Decl * temp;
		while(temp = it.GetNextValue()) {
			PrintDebug("scope", "STUFF %s\n", temp->getId()->getName());
		}
		*/
		cur->print();
		cur = cur->getParent();
	}
	PrintDebug("scope", "\n");


}
