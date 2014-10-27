#include "level.h"
#include "ast_decl.h"
#include "errors.h"
#include "ast.h"

Level::Level() {
	scope = new Hashtable<Decl *>();
	parent = NULL;
};

Level::~Level() {
	delete scope;
};

Decl * Level::find(Identifier * id) {
	return scope->Lookup(id->getName());
};

bool Level::add(Decl * toAdd) {
	Decl * old = scope->Lookup(toAdd->getId()->getName());
	if (old == NULL) {
		Level * p = parent;
		while (parent != NULL && old == NULL) {
			old = p->scope->Lookup(toAdd->getId()->getName());
			p = p->parent;
		}
	}
	if (old->sGetT() != s_FDecl) {
		ReportError::DeclConflict(toAdd, old);
		return false;
	}
	return true;
};

void Level::link(Level * top) {
	parent = top;
};

/*
void Level::checkList(List<Decl *> * l) {
	for (int i = 0; i < l->NumElements(); ++i) {
		l->Nth(i)->Check();
	}
};

void Level::addList(List<Decl *> * l, Level * lvl) {
	for (int i = 0; i < l->NumElements(); ++i) {
		lvl->add(l->Nth(i));
	}
};
*/
