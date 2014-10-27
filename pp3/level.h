#ifndef LEVEL_H
#define LEVEL_H

#include "hashtable.h"
#include "list.h"

class Decl;
class Identifier;

class Level {

	protected:
		Hashtable<Decl *> *scope;
		Level * parent;

	public:
		Level();
		~Level();

		Decl * find(Identifier * id);
		bool add(Decl * toAdd);
		void link(Level * top);

		/*
		static void checkList(List<Decl *> * l);
		static void addList(List<Decl *> * l, Level * lvl);
		*/

};




#endif
