/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "errors.h"
#include <cstring>

void Program::Check() {
    /* You can use your pp3 semantic analysis or leave it out if
     * you want to avoid the clutter.  We won't test pp5 against 
     * semantically-invalid programs.
     */
}
void Program::Emit() {
    /* pp5: here is where the code generation is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, generating instructions as you go.
     *      Each node can have its own way of translating itself,
     *      which makes for a great use of inheritance and
     *      polymorphism in the node classes.
     */

	List<const char*>debugs = List<const char *>();

	if (false) {
		debugs.Append("d");
	}
	for (int i = 0; i < debugs.NumElements(); ++i) {
		SetDebugForKey(debugs.Nth(i), true);
	}
	
	// may loop to find class decls and fill them out
	//
	varSize = CodeGenerator::VarSize;
	locations = new Hashtable<Location*>();
	strings = new Hashtable<void *>();
	ints = new Hashtable<void *>();
	booleans = new Hashtable<void *>();
	types = new Hashtable<Type*>();
	cg = new CodeGenerator();
	tempNumb = CodeGenerator::tempNum;
	
	FnDecl * main = NULL;
	List<VarDecl*> globals = List<VarDecl*>();
	List<FnDecl*> globalFunctions = List<FnDecl*>();
	List<ClassDecl*> classes = List<ClassDecl*>();

	
	for (int i = 0; i < decls->NumElements(); ++i) {
		Decl * temp = decls->Nth(i);
		int t = temp->getDeclType();

		if (t == classDecl) {
			classes.Append((ClassDecl*)temp);
		} else if (t == varDecl) {
			// global var decl
			globals.Append((VarDecl*)temp);
		} else if (t == functionDecl) {
			FnDecl * fd = (FnDecl*)temp;	
			globalFunctions.Append(fd);
			if (!strcmp(fd->getName(), "main")) {
				main = fd;
			}	
		} else {
			// interface?
		}
	}

	if (main == NULL) {
		ReportError::NoMainFound();
		return;
	}

	for (int i = 0; i < globals.NumElements(); ++i) {

		VarDecl * vd = globals.Nth(i);
		Location * loc = new Location(gpRelative, CodeGenerator::VarSize * i, vd->getName());
		locations->Enter(vd->getName(), loc, false);
		types->Enter(vd->getName(), vd->getType(), false);

	}

	for (int i = 0; i < globalFunctions.NumElements(); ++i) {

		FnDecl * fd = globalFunctions.Nth(i);
		fd -> Emit();

	}

	cg -> DoFinalCodeGen();

}

void StmtBlock::Emit() {

	List<Location *> locals = List<Location *>();
	for (int i = 0; i < decls->NumElements(); ++i) {
		VarDecl * vd = decls -> Nth(i);
		Location * loc = new Location(fpRelative, -1 * varSize * i + CodeGenerator::OffsetToFirstLocal, vd->getName());
		locals.Append(loc);
		PrintDebug("d", "inserting %s at %s%d\n", vd->getName(), loc->segChar(), loc->GetOffset());
		locations->Enter(vd->getName(), loc, false);
		types->Enter(vd->getName(), vd->getType(), false);
	}

	cg -> increaseStack(locals.NumElements());

	for (int i = 0; i < stmts->NumElements(); ++i) {
		//stmts -> Nth(i) -> help();
		stmts -> Nth(i) -> Emit();
	}

	for (int i = 0; i < decls->NumElements(); ++i) {
		VarDecl * vd = decls -> Nth(i);
		Location * loc = locals.Nth(i);
		locations->Remove(vd->getName(), loc);
		types->Remove(vd->getName(), vd->getType());
	}


}


void ForStmt::Emit() {

	Location * ini, * check, * update;

	l1 = cg->NewLabel();
	l2 = cg->NewLabel();

	ini = init->cgen();

	cg->GenLabel(l1);

	check = test->cgen();

	cg->GenIfZ(check, l2);

	body -> Emit();

	update = step->cgen();
	cg->GenGoto(l1);

	cg->GenLabel(l2);

}

void WhileStmt::Emit() {

	Location * check;

	l1 = cg->NewLabel();
	l2 = cg->NewLabel();

	cg->GenLabel(l1);

	check = test -> cgen();

	cg->GenIfZ(check, l2);

	body->Emit();

	cg->GenGoto(l1);

	cg->GenLabel(l2);

}
void IfStmt::Emit() {
	Location * check;
	char * l1, * l2;

	check = test->cgen();
	l1 = cg->NewLabel();
	if (elseBody) {
		l2 = cg->NewLabel();
	}
	cg->GenIfZ(check, l1);

	body->Emit();

	if (elseBody) {
		cg->GenGoto(l2);
	}

	cg->GenLabel(l1);

	if (elseBody) {
		elseBody->Emit();

		cg->GenLabel(l2);
	}


}

void BreakStmt::Emit() {
	Node * cur = this;

	while (cur != NULL) {
		LoopStmt * l = dynamic_cast<LoopStmt *>(cur);

		if (l) {
			cg -> GenGoto(l->l2);
		}

		cur = cur->GetParent();
	}
}

void PrintStmt::Emit() {
	for (int i = 0; i < args -> NumElements(); ++i) {
		// wrongish
		Location * loc;
		Expr * e = args -> Nth(i);
		loc = e -> cgen();

		Type * t = types->Lookup(loc->GetName());

		if (t == Type::intType) {
			cg->GenBuiltInCall(PrintInt, loc, NULL);
		} else if (t == Type::stringType) {
			cg->GenBuiltInCall(PrintString, loc, NULL);
		} else if (t == Type::boolType) {
			cg->GenBuiltInCall(PrintBool, loc, NULL);
		} else {
			Assert(false);
		}
		/*
		if (ints->Lookup(loc->GetName()) != NULL) {
			cg->GenBuiltInCall(PrintInt, loc, NULL);
		} else if (strings->Lookup(loc->GetName()) != NULL) {
			cg->GenBuiltInCall(PrintString, loc, NULL);
		} else if (booleans->Lookup(loc->GetName()) != NULL) {
			cg->GenBuiltInCall(PrintBool, loc, NULL);
		} else {
			Assert(false);
		}
		*/
	}
}

/* changes above */

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}


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


