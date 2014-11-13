/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h> // strdup
#include <stdio.h>  // printf
#include "errors.h"
#include "scope.h"

Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
    nodeScope = NULL;
}

Node::Node() {
    location = NULL;
    parent = NULL;
    nodeScope = NULL;
}

Decl *Node::FindDecl(Identifier *idToFind, lookup l) {
    PrintDebug("find", "FINDING %s\n", idToFind->GetName());
    Decl *mine;
    if (!nodeScope) PrepareScope();
    if (nodeScope && (mine = nodeScope->Lookup(idToFind))) {
	PrintDebug("find", "FOUND %s\n", idToFind->GetName());
        return mine;
    }
    if (l == kDeep && parent) {
        return parent->FindDecl(idToFind, l);
    }
    PrintDebug("find", "NOT FOUND\n");
    return NULL;
}
	 
Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
    cached = NULL;
} 

