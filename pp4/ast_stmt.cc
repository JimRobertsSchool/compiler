/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "scope.h"
#include "errors.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::Check() {
    List<char *> debugs = List<char *>();
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    if (false) {
        //debugs.Append("arith");
        //debugs.Append("conforms");
        debugs.Append("call");
        //debugs.Append("assign");
        //debugs.Append("equal");
        //debugs.Append("this");
        //debugs.Append("relational");
        //debugs.Append("logic");
        //debugs.Append("arrayaccess");
        debugs.Append("fieldaccess");
        //debugs.Append("new");
        //debugs.Append("newarray");
        //debugs.Append("readint");
        //debugs.Append("readline");
        debugs.Append("find");
        //debugs.Append("block");
        debugs.Append("scope");
        //debugs.Append("type");
        //debugs.Append("cond");
        //debugs.Append("if");
        //debugs.Append("print");
        //debugs.Append("return");
        //debugs.Append("fdecl");
        //debugs.Append("cdecl");
    }
    #pragma GCC diagnostic pop
    for (int i = 0; i < debugs.NumElements(); ++i) {
        SetDebugForKey(debugs.Nth(i), true);
    }

    PrintDebug("arith", "here 1\n");
    nodeScope = new Scope();
    decls->DeclareAll(nodeScope);
    decls->CheckAll();
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}
void StmtBlock::Check() {
    PrintDebug("block", "entering block\n");
    nodeScope = new Scope();
    decls->DeclareAll(nodeScope);
    decls->CheckAll();
    PrintDebug("block", "Checking all\n");
    stmts->CheckAll();
    PrintDebug("block", "leaving block\n");
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

void ConditionalStmt::Check() {
    PrintDebug("cond", "entering conditional\n");
    PrepareScope();
    test->Check();
    Type * t = test->getType();
    if (t!=Type::boolType && t!=Type::errorType) {
	    ReportError::TestNotBoolean(test);
    }
    body->Check();
    PrintDebug("cond", "leaving conditional\n");
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

void ForStmt::Check() {
	ConditionalStmt::Check();
	init->Check();
	step->Check();

}

void WhileStmt::Check() {
	ConditionalStmt::Check();
	//test->Check();
	//body->Check();
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}
void IfStmt::Check() {
    ConditionalStmt::Check();
    PrintDebug("if", "entering if\n");
    if (elseBody) elseBody->Check();
    PrintDebug("if", "entering if\n");
}

void BreakStmt::Check() {
	PrintDebug("break", "entering break\n");

	Node * n = this;
	while (n) {
		if (dynamic_cast<LoopStmt *>(n)) {
			return;
		}
		n = n->getParent();
	}
	ReportError::BreakOutsideLoop(this);

	PrintDebug("break", "leaving break\n");
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::Check() {
	PrintDebug("return", "entering return\n");

	Node * n = this;
	while (n) {
		PrintDebug("return", "here 8\n");

		FnDecl * fd = dynamic_cast<FnDecl*>(n);
		PrintDebug("return", "here 7\n");
		if (fd) {

			PrintDebug("return", "here 9\n");
			// expr might be null
			if (fd->getReturn() == Type::voidType) {
				PrintDebug("return", "here 10\n");
				if (expr) {
					PrintDebug("return", "here 11\n");

					expr -> Check();
					PrintDebug("return", "here 12\n");

					if(!expr->getType()) {
						PrintDebug("return", "its null\n");
						return;

					}

					if (expr->getType() != Type::voidType) {
						PrintDebug("return", "here 13\n");
						ReportError::ReturnMismatch(this, expr->getType(), Type::voidType);
						PrintDebug("return", "here 14\n");
						return;
					}
					PrintDebug("return", "here 15\n");
					return;

				}
				PrintDebug("return", "here 16\n");
				return;
			}
			PrintDebug("return", "check\n");
			expr -> Check();
			PrintDebug("return", "done check\n");
			Type * a = expr->getType(), * f = fd->getReturn();
			PrintDebug("return", "here 1\n");

			if (!a->conformsTo(f)) {
			PrintDebug("return", "here 2\n");
				ReportError::ReturnMismatch(this, a, f);
				PrintDebug("return", "here 3\n");
				return;
			} else {
				PrintDebug("return", "here 4\n");
				return;
			}

		}
			PrintDebug("return", "here 5\n");
		n = n->getParent();
	}
	//ReportError::ReturnMismatch(this, );
			PrintDebug("return", "here 6\n");

}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}

void PrintStmt::Check() {
	PrintDebug("print", "entering print\n");
	PrepareScope();
	args->CheckAll();

	for(int i = 0; i<args->NumElements(); ++i) {
		Expr * e = args->Nth(i);
		Type * t = e->getType();

		if (t!=Type::stringType && t!=Type::intType && t!=Type::boolType && t!=Type::errorType) {
			ReportError::PrintArgMismatch(e, i+1, t);

		}
	}

	PrintDebug("print", "leaving print\n");

}

