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
	PrintDebug("find", "Looking up %s\n", id->getName());
	return scope->Lookup(id->getName());
};

bool Level::add(Decl * toAdd) {
	//if old and conflicting then return false
	PrintDebug("level", "adding\n");
	Decl * old = find(toAdd->getId());
	PrintDebug("level", "Level adding: %s\n", toAdd->getId()->getName());
	/*
	if (old == NULL) {
		PrintDebug("level", "in the loop\n");

		Level * p = parent;
		while (p != NULL && old == NULL) {
			old = p->find(toAdd->getId());
			p = p->parent;
		}
	}
	PrintDebug("level", "Done loop\n");
	*/
	if (old != NULL && old->sGetT() != s_FDecl) {
		ReportError::DeclConflict(toAdd, old);
		PrintDebug("level", "in the if (T=%d)\n", old->sGetT());
		return false;
	}
	scope->Enter(toAdd->getId()->getName(), toAdd);
	PrintDebug("level", "normal return\n");
	return true;
};

//might instead copy all from other level
void Level::link(Level * top) {
	parent = top;
};

void Level::print() {
	Iterator<Decl*> it = scope->GetIterator();
	Decl * temp;
	while(temp = it.GetNextValue()) {
		PrintDebug("level", "STUFF %s\n", temp->getId()->getName());
	}
	PrintDebug("level", "\n");
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
