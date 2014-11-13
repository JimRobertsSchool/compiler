/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>

#include "errors.h"

bool Expr::insideClass() {
	Node * n = this;
	while (n) {
		ClassDecl * cd = dynamic_cast<ClassDecl *>(n);
		if (cd) {
			return true;
		}
		n = n->getParent();

	}
	return false;
}

bool Expr::idInClass(Identifier * i) {
	Node * n = this;
	while (n) {
		ClassDecl * cd = dynamic_cast<ClassDecl*>(n);
		if (cd) {
			if (cd->FindDecl(i, kShallow)) {
				return true;
			}
			return false;
		}
		n = n->getParent();
	}
	return false;
}

/*
void CompoundExpr::Check() {
	PrepareScope();

	right->Check();
	Type * r = right->getType();

	if (left == NULL) {
		if (!r->isNumeric()) {

		}
		type = r;
	}
}
*/
void ArithmeticExpr::Check() {
	PrintDebug("arith", "entering arith\n");
	PrepareScope();

	right -> Check();
	Type * r = right -> getType();
	if (left == NULL) {
		// uunary
		if (!(r->isNumeric())) {
			type = Type::errorType;
			ReportError::IncompatibleOperand(op, r);
			PrintDebug("arith", "leaving arith\n");
			return;
		} 
		type = r;
		PrintDebug("arith", "leaving arith\n");
		return;
	}
	left -> Check();
	Type * l = left -> getType();
	
	if (l != r && l!=Type::errorType && r!=Type::errorType) {
		type = Type::errorType;
		ReportError::IncompatibleOperands(op, l, r);
		PrintDebug("arith", "leaving arith\n");
		return;
	}

	type = l;
	PrintDebug("arith", "leaving arith\n");

}

void RelationalExpr::Check() {
	PrintDebug("relation", "entering relational\n");
	PrepareScope();

	left -> Check();
	right -> Check();

	Type * l, * r, * e = Type::errorType, * b = Type::boolType;
	l = left -> getType();
	r = right -> getType();
	if (l == r && (l == Type::intType || l == Type::doubleType)) {
		type = Type::boolType;
	} else if (l == e || r == e) {
		type = b;
	} else {
		type = Type::errorType;
		ReportError::IncompatibleOperands(op, l, r);
		PrintDebug("relation", "leaving relational\n");
	}

	PrintDebug("relation", "leaving relational\n");
	
}

void EqualityExpr::Check() {
	PrintDebug("equal", "entering equality\n");

	Type * e = Type::errorType;
	left->Check();
	right->Check();

	Type * l = left->getType(), * r = right->getType();

	//if (l->IsEquivalentTo(r)) {
	if (r->conformsTo(l) || l->IsEquivalentTo(r)) {
		type = Type::boolType;
	} else if (l == e || r == e) {
		type = Type::boolType;
	} else {
		type = e;
		ReportError::IncompatibleOperands(op, l, r);
	}

	PrintDebug("equal", "leaving equality\n");
}

void LogicalExpr::Check() {
	PrintDebug("logic", "entering logical\n");

	Type * e = Type::errorType, * b = Type::boolType;
	right -> Check();
	Type * r, *l;
	r = right->getType();

	if (left) {

		left -> Check();
		l = left -> getType();


		if (l == b && r == b) {
			type = b;
		} else if ( l == e || r == e) {
			type = b;
		} else {
			type = e;
			ReportError::IncompatibleOperands(op, l, r);
		}
		return;
	}

	if ( r == b) {
		type = b;
	} else if ( r == e) {
		type = e;
	} else {
		type = e;
		ReportError::IncompatibleOperand(op, r);
	}


	PrintDebug("logic", "leaving logical\n");
	return;
}


void AssignExpr::Check() {
	PrepareScope();
	PrintDebug("assign", "entering assign\n");

	PrintDebug("assign", "checking left\n");
	left -> Check();
	PrintDebug("assign", "left checked\n");

	PrintDebug("assign", "checking right\n");
	right -> Check();
	PrintDebug("assign", "checked\n");

	Type * l, * r;
	l = left -> getType();
	r = right -> getType();
	PrintDebug("assign", "got types\n");

	//conformance if (l->isNu
	PrintDebug("arith", "here 7\n");
	if (!r->conformsTo(l)) {
		type = Type::errorType;
		//Error here
		ReportError::IncompatibleOperands(op, l, r);
		PrintDebug("assign", "leaving assign\n");
		return;
	}

	type = l;
	PrintDebug("assign", "leaving assign\n");
}

void This::Check() {
	PrintDebug("this", "entering this\n");
	PrepareScope();

	Node * p = parent;

	while(p) {
		ClassDecl * cd = dynamic_cast<ClassDecl *>(p);	
		if (cd) {
			type = cd -> getType();
			PrintDebug("this", "leaving this\n");
			return;
		}
		p = p -> getParent();
	}

	ReportError::ThisOutsideClassScope(this);
	type = Type::errorType;

	PrintDebug("this", "leaving this\n");
}

void ArrayAccess::Check() {
	PrintDebug("arrayaccess", "entering array access\n");

	base->Check();
	if (base->getType() == Type::errorType) {
		type = Type::errorType;
		return;
	}
	ArrayType *t = dynamic_cast<ArrayType*>(base->getType());

	if (!t) {
		type = Type::errorType;
		ReportError::BracketsOnNonArray(base);
		return;
	}
	
	subscript->Check();
	if (subscript->getType() != Type::intType && subscript->getType() != Type::errorType) {
		type = Type::errorType;
		ReportError::SubscriptNotInteger(subscript);
		return;
	}

	if (subscript->getType() == Type::errorType) {
		type = Type::errorType;
		return;
	}

	type = t->getBase();

	PrintDebug("arrayaccess", "leaving array access\n");
}

void FieldAccess::Check() {
	PrintDebug("fieldaccess", "entering field access\n");
	PrepareScope();

	// Should fix this
	PrintDebug("fieldaccess", "check for location\n");
	if(insideClass() && idInClass(field) && NULL==base) {
		base = new This(*location);
		base->SetParent(this);
	}

	Type * tt;
	//
	if (base) {
		PrintDebug("fieldaccess", "checking base\n");
		base -> Check();
		//Here
		tt = base->getType();
		//ReportError::FieldNotFoundInBase(field, tt);
	} else {
		PrintDebug("fieldaccess", "looking for %s", field->GetName());
		Decl * d = FindDecl(field, kDeep);
		if (false && d) {
			PrintDebug("fieldaccess", "found %s\n", d->GetName());
			PrintDebug("fieldaccess", "type: %d", d->IsVarDecl());
			PrintDebug("fieldaccess", "type: %d", d->IsClassDecl());
			PrintDebug("fieldaccess", "type: %d", d->IsInterfaceDecl());
			PrintDebug("fieldaccess", "type: %d", d->IsFnDecl());
			PrintDebug("fieldaccess", "type: %d", d->IsMethodDecl());
		}
		VarDecl * vd = dynamic_cast<VarDecl *>(d);
		if (!vd) {
			PrintDebug("fieldaccess", "not a variable");
		}
		if (d && d->IsVarDecl()) {
			PrintDebug("fieldaccess", "found vardecl");
			tt = ((VarDecl*)d)->GetDeclaredType();
			type = tt;
			PrintDebug("fieldaccess", "leaving field access 5\n");
			return;
		} else {
			type = Type::errorType;
			PrintDebug("fieldaccess", "check here 1\n");
			ReportError::IdentifierNotDeclared(field, LookingForVariable);
			return;
		}
	}

	if (tt->getNum() == 0) {
		//type = tt;
		if (tt != Type::errorType) {
			//problem?
			ReportError::FieldNotFoundInBase(field, tt);
			type = Type::errorType;
		}
		PrintDebug("fieldaccess", "leaving field access 11111111111111111\n");
		return;

	}

	NamedType * nt = dynamic_cast<NamedType *>(tt);
	ArrayType * at = dynamic_cast<ArrayType *>(tt);
	if (at) {
		if (!strcmp(field->GetName(), "length")) {
			type = Type::intType;
			return;
		}
		type = Type::errorType;
		ReportError::FieldNotFoundInBase(field, at);
		return;
	}
	if (tt == Type::errorType || !nt) {
		type = Type::errorType;
		PrintDebug("fieldaccess", "leaving field access 2\n");
		return;
	}
	PrintDebug("fieldaccess", "finding %s\n", nt->GetId()->GetName());
	Decl * d = FindDecl(nt->GetId(), kDeep);
	PrintDebug("fieldaccess", "done finding %s\n", nt->GetId()->GetName());

	if (d) {
	
		PrintDebug("fieldaccess", "here 41\n");
		if(d->IsClassDecl() && idInClass(field)) {

			//type = ((ClassDecl*)d)->getType();
			ClassDecl * cd = (ClassDecl*)d;

			Decl *  fd = cd->FindDecl(field, kShallow);
			if (fd) {
				VarDecl * vd = dynamic_cast<VarDecl*>(fd);
				if (vd) {
					type = vd ->GetDeclaredType();
					return;
				}
			}
			

			PrintDebug("fieldaccess", "leaving field access 3\n");
			return;
		} else if (d->IsClassDecl()) {
			ClassDecl * cd = (ClassDecl *)d;
			Decl * dd = cd->FindDecl(field, kShallow);
			type = Type::errorType;

			if (dd && dd->IsVarDecl()) {
				ReportError::InaccessibleField(field, nt);
			} else {
				ReportError::FieldNotFoundInBase(field, nt);
			}



			return;
		}

		if(d->IsVarDecl() && idInClass(field)) {
			type = ((VarDecl*)d)->GetDeclaredType();
			PrintDebug("fieldaccess", "leaving field access 4\n");
			return;
		}
	       /*
		 else {
			type = Type::errorType;
			ReportError::FieldNotFoundInBase(field, tt);
			return;

		}
		*/
	}

	type = Type::errorType;
	/*
	if (insideClass() && idInClass(field)) {
		ReportError::k
		*/
	//ReportError::FieldNotFoundInBase(field, tt);
	ReportError::InaccessibleField(field, tt);
	PrintDebug("fieldaccess", "leaving field access 40\n");


}

void Call::Check() {
	PrintDebug("call", "entering call\n");
	PrepareScope();

	if(insideClass() && idInClass(field) && NULL==base) {
		base = new This(*location);
		base->SetParent(this);
	}


	if (base) {
		PrintDebug("call", "base found %s\n", field->GetName());
		base -> Check();

		// Get name of class
		Type * tt = base->getType();
		NamedType * nt = dynamic_cast<NamedType *>(tt);
		//if (tt == Type::errorType || !nt) {
		if (tt == Type::errorType) {
			type = Type::errorType;
			PrintDebug("call", "leaving call error\n");
			return;
		} else if (!nt) {
			if (dynamic_cast<ArrayType *>(tt) && !strcmp("length", field->GetName())) {
				type = Type::intType;
				return;
			}
			ReportError::FieldNotFoundInBase(field, tt);
			type = Type::errorType;
			return;
		}
		PrintDebug("call", "here 1\n");
		if (!nt->GetId()) {
			PrintDebug("call", "null?\n");
		}
		PrintDebug("call", "here 9\n");
		Decl * d = FindDecl(nt->GetId(), kDeep);
		PrintDebug("call", "here 2\n");

		if(!d) {
			type = Type::errorType;
			return;
		}
		if(d->IsMethodDecl()) {
		}
		PrintDebug("call", "here 8\n");

		if(d->IsInterfaceDecl()) {
			PrintDebug("call", "HERE INTERFACE\n");
			InterfaceDecl * idd = (InterfaceDecl*)d;
			idd->Check();
			Decl * md = idd->FindDecl(field, kDeep);
			if (md && md->IsFnDecl()) {
				type = this->compare(md);
				return;
			}
			actuals->CheckAll();
			type = Type::errorType;
			PrintDebug("call", "leaving call this 1\n");
			ReportError::FieldNotFoundInBase(field, new NamedType(idd->GetId()));
			return;
		}

		if(d->IsClassDecl()) {
			PrintDebug("call", "here 3\n");
			while (d) {
				PrintDebug("call", "here 4\n");
				if (!d->IsClassDecl()) {
					//Errorr
					type = Type::errorType;
					PrintDebug("call", "leaving call\n");
					return;
				}
				ClassDecl * cd = (ClassDecl*)d;
				//May need to check for null type
				PrintDebug("call", "here 5\n");
				cd -> PrepareScope();
				PrintDebug("call", "here 6\n");
				Decl * md = cd->FindDecl(field, kDeep);
				PrintDebug("call", "here 7\n");
				
				if (md && md->IsFnDecl()) {
					//FnDecl * fd = (FnDecl*)md;
					type = this->compare(md);
					/*
					List<VarDecl*>* formals = fd->getFormals();
					actuals -> CheckAll();
					if (actuals -> NumElements() != formals->NumElements()) {
						type = Type::errorType;
						ReportError::NumArgsMismatch(field, formals->NumElements(), actuals->NumElements());
						PrintDebug("call", "leaving call\n");
						return;

					}
					*/
					/*
					for (int i = 0; i < actuals->NumElements(); ++i) {
						Type * form = formals->Nth(i)->GetDeclaredType();
						Type * act = actuals->Nth(i)->getType();

						if (!act->conformsTo(form)) {
							type = Type::errorType;
							ReportError::ArgMismatch(actuals->Nth(i), i+1, act, form);
						}
						if (type == Type::errorType) {
							PrintDebug("call", "leaving call\n");
						       	return;
						}

					}
					*/
					//type = ((FnDecl*)md)->getReturn();


					PrintDebug("call", "leaving call 11\n");
					return;
				}
				actuals->CheckAll();
				type = Type::errorType;
				PrintDebug("call", "leaving call this 2\n");
				ReportError::FieldNotFoundInBase(field, cd->getType());
				return;

			}
		}
	}

	PrintDebug("call", "no base\n");
	Decl* d = FindDecl(field, kDeep);
	
	if (d) {
		FnDecl * fd = dynamic_cast<FnDecl*>(d);
		if (fd) {
			/*
			   List<VarDecl*> * formals = fd->getFormals();
			   actuals->CheckAll();


			   if (actuals -> NumElements() != formals->NumElements()) {
			   type = Type::errorType;
			   ReportError::NumArgsMismatch(field, formals->NumElements(), actuals->NumElements());
			   PrintDebug("call", "leaving call\n");
			   return;

			   }
			   for (int i = 0; i < actuals->NumElements(); ++i) {
			   Type * form = formals->Nth(i)->GetDeclaredType();
			   Type * act = actuals->Nth(i)->getType();

			   if (!act->conformsTo(form)) {
			   type = Type::errorType;
			   ReportError::ArgMismatch(actuals->Nth(i), i+1, act, form);
			   }
			   if (type == Type::errorType) {
			   PrintDebug("call", "leaving call\n");
			   return;
			   }

			   }

			   type = fd->getReturn();
			   */
			PrintDebug("call", "Function: %s\n", fd->GetName());
			/*if (!strcmp(fd->GetName(), "Print")) {
				//check for print arguments
				for (int i = 0; i < actuals->NumElements(); ++i) {

					Expr * e = actuals->Nth(i);
					e -> Check();
					Type * t = e->getType();

					if (t->getNum() != 0) {
						ReportError::PrintArgMismatch(e, i+1, t);
						type = Type::errorType;
					}

				}
				if (type == Type::errorType) {
					return;
				}
				type = Type::voidType;
				return;
			} else {
			*/
				type = compare(fd);
				PrintDebug("call", "leaving call 12\n");

				return;
			
				/*
				}
				*/
		}
	}


	type = Type::errorType;
	PrintDebug("call", "here 1\n");
	ReportError::IdentifierNotDeclared(field, LookingForFunction);
	PrintDebug("call", "here 2\n");
	PrintDebug("call", "leaving call\n");

}

Type * Call::compare(Decl * d) {

	if (d->IsFnDecl()) {

		FnDecl * fd = (FnDecl *)d;
		List<VarDecl *> * formals = fd->getFormals();
		if (actuals->NumElements() != formals->NumElements()) {
			ReportError::NumArgsMismatch(field, formals->NumElements(), actuals->NumElements());
			return Type::errorType;
		}

		actuals -> CheckAll();

		Type * toReturn = fd->getReturn();

		for (int i = 0; i < actuals->NumElements(); ++i) {

			if (!actuals->Nth(i)->getType()->conformsTo(formals->Nth(i)->GetDeclaredType())) {
				ReportError::ArgMismatch(actuals->Nth(i), i+1, actuals->Nth(i)->getType(), formals->Nth(i)->GetDeclaredType());
				toReturn = Type::errorType;
			}

		}

		return toReturn;

	} else {
		return Type::errorType;
	}

}

void NewExpr::Check() {
	PrintDebug("new", "entering new\n");

	PrepareScope();

	Decl * d = FindDecl(cType->GetId(), kDeep);

	if (d && d->IsClassDecl()) {
		type = cType;
		return;
	}

	//cType->Check();
	//type = cType;
	type = Type::errorType;
	ReportError::IdentifierNotDeclared(cType->GetId(), LookingForClass);
	PrintDebug("new", "leaving new\n");
}

void NewArrayExpr::Check() {
	PrintDebug("newarray", "entering new array\n");

	size->Check();
	if (!(size->getType()==Type::intType)) {
		ReportError::NewArraySizeNotInteger(size);
		type = Type::errorType;
		return;
	}

	elemType->Check();
	if(elemType != Type::errorType) {
		PrintDebug("newarray", "making new array type\n");
		type = new ArrayType(*location, elemType);
		type -> SetParent(this);
		return;
	}

	type=Type::errorType;
	PrintDebug("newarray", "leaving new array\n");

}

void ReadIntegerExpr::Check() {
	PrintDebug("readint", "entering read int\n");

	type = Type::intType;

	PrintDebug("readint", "leaving read int\n");
}

void ReadLineExpr::Check() {
	PrintDebug("readline", "entering read line\n");

	type = Type::stringType;

	PrintDebug("readline", "leaving read line\n");
}

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    type = Type::intType;
    value = val;
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    type = Type::doubleType;
    value = val;
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    type = Type::boolType;
    value = val;
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    type = Type::stringType;
    value = strdup(val);
}

NullConstant::NullConstant(yyltype loc) : Expr(loc) {
	type = Type::nullType;
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}
CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}
   
  
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}


Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}
 

NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}


NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
    (elemType=et)->SetParent(this);
}

       
