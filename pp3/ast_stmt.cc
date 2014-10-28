/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */
	List<char *> debugs = List<char *>();
	if(true) {
		//debugs.Append("program");
		//debugs.Append("find");
		//debugs.Append("scope");
		//debugs.Append("level");
		//debugs.Append("cdecl");
		//debugs.Append("vdecl");
		//debugs.Append("fdecl");
		//debugs.Append("sblock");
		//debugs.Append("ntype");
	}
	for(int i = 0; i < debugs.NumElements(); ++i) {
		SetDebugForKey(debugs.Nth(i), true);
	}
		
	PrintDebug("program", "Entering program\n");
	PrintDebug("program", "adding\n");
	//Level * globals = new Level();
	initLevel();
	/*
	Hashtable<ClassDecl *> * c = new Hashtable<ClassDecl *>();
	for(int i = 0; i < decls->NumElements(); ++i) {
		Decl * cur = decls->Nth(i);
		//globals->add(decls->Nth(i));
		switch (cur->sGetT()) {
			case s_CDecl: {
					PrintDebug("program", "Found Class %s\n", ((ClassDecl *)cur)->getId()->getName());
					c->Enter(cur->getId()->getName(), ((ClassDecl *)cur));
				      }; break;
		}
	}
	*/
	//globals->addList(decls, globals);
	decls->addList(lvl);

	printLevel();
	PrintDebug("program", "done adding\n");

	/*
	for(int i = 0; i < decls->NumElements(); ++i) {
		decls->Nth(i)->Check();
	}
	*/
	//globals->checkList(decls);
	decls->checkList();
	
}

void StmtBlock::Check() {
	PrintDebug("sblock", "entering sblock\n");
	initLevel(NULL, true);
	//lvl = new Level();
	/*
	lvl->addList((List<Decl *>*)decls, lvl);
	lvl->checkList((List<Decl *>*)decls);
	lvl->checkList((List<Decl *>*)stmts);
	*/
	decls->addList(lvl);
	decls->checkList();
	stmts->checkList();
	PrintDebug("sblock", "leaving sblock\n");
};

void ConditionalStmt::Check() {
	body->Check();
};

void IfStmt::Check() {
	body->Check();	
};

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}


ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}


