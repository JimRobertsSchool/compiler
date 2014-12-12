/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>
#include "tac.h"

Location * BoolConstant::cgen() {
	Location * loc;
	loc = cg -> GenLoadConstant(value ? 1 : 0);
	booleans->Enter(loc->GetName(), (void *)1, false);
	types->Enter(loc->GetName(), Type::boolType, false);
	return loc;
}

Location * StringConstant::cgen() {
	Location * loc;
	loc = cg -> GenLoadConstant(value);
	strings->Enter(loc->GetName(), (void *)1, false);
	types->Enter(loc->GetName(), Type::stringType, false);
	return loc;
}

Location * IntConstant::cgen() {
	Location * loc;
	loc = cg -> GenLoadConstant(value);
	ints->Enter(loc->GetName(), (void *)1, false);
	types->Enter(loc->GetName(), Type::intType, false);
	return loc;
}

Location * AssignExpr::cgen() {

	PrintDebug("d", "here 1\n");
	Location * loc, *l, *r, *t;
	left->write = true;
	l = left -> cgen();
	r = right -> cgen();

	if (dynamic_cast<ArrayAccess*>(left)) {
		cg->GenStore(l, r);
		return r;
	}

	loc = l;

	cg->GenAssign(loc, r);
	types->Enter(loc->GetName(), types->Lookup(r->GetName()), false);
	swapType(r, loc);
	return loc;


}

Location * LogicalExpr::cgen() {

	Location * loc, *l, *r;

	const char * oper = op->getOp()[0] == '!' ? "==" : op->getOp();

	if (!left) {
		r = right->cgen();
	}

	if (!left) {
		l = cg->GenLoadConstant(0);
	} else {
		l = left->cgen();
	}

	if (left) {
		r = right->cgen();
	}

	loc = cg->GenBinaryOp(oper, l, r);
	types->Enter(loc->GetName(), Type::boolType, false);
	swapType(r, loc);

	return loc;
}

Location * ArithmeticExpr::cgen() {

	Location * loc, *l, *r;

	if (!left) {
		l = cg->GenLoadConstant(0);
	} else {
		l = left->cgen();
	}
	r = right->cgen();

	loc = cg->GenBinaryOp(op->getOp(), l, r);
	types->Enter(loc->GetName(), Type::intType, false);
	swapType(r, loc);

	return loc;
}

Location * RelationalExpr::cgen() {

	Location * loc, *l, *r, *res1, *res2;

	l = left->cgen();
	r = right->cgen();

	const char * ops[] = {"<", ">", "<=", ">="};
	const char * availble[] = {"<", "==", "||"};
	char * o = op->getOp();

	if (!strcmp(o, ops[0])) {
		res1 = cg->GenBinaryOp(availble[0], l, r);
		loc = res1;
	} else if (!strcmp(o, ops[1])) {
		res1 = cg->GenBinaryOp(availble[0], r, l);
		loc = res1;
	} else if (!strcmp(o, ops[2])) {
		res1 = cg->GenBinaryOp(availble[0], l, r);
		res2 = cg->GenBinaryOp(availble[1], l, r);
		loc = cg->GenBinaryOp(availble[2], res1, res2);
	} else if (!strcmp(o, ops[3])) {
		res1 = cg->GenBinaryOp(availble[0], r, l);
		res2 = cg->GenBinaryOp(availble[1], r, l);
		loc = cg->GenBinaryOp(availble[2], res1, res2);
	}

	booleans->Enter(loc->GetName(), (void *)1, false);
	types->Enter(loc->GetName(), Type::boolType, false);
	return loc;

}

Location * EqualityExpr::cgen() {

	Location * loc, *l, *r;

	l = left->cgen();
	r = right->cgen();

	if (types->Lookup(l->GetName()) == Type::stringType) {
	//if (strings->Lookup(l->GetName()) != NULL) {
		// stirng comparison
		loc = cg->GenBuiltInCall(StringEqual, l, r);
	} else {
		//normal
		loc = cg->GenBinaryOp("==", l, r);
	}

	booleans->Enter(loc->GetName(), (void *)1, false);
	types->Enter(loc->GetName(), Type::boolType, false);
	return loc;
}

Location * ArrayAccess::cgen() {

	Location *index, *addr, *loc, *ret;

	index = subscript->cgen();
	addr = base->cgen();
	loc = cg->arrayIndex(addr, index);

	if (!write) {
		//load
		ret = cg->GenLoad(loc);
	} else {
		//store
		ret = loc;
	}

	ArrayType * at;
	Type * t;
	at = (ArrayType*)types->Lookup(addr->GetName());
	t = at->getBase();

	types->Enter(ret->GetName(), t);
	return ret;

	/*
	Location *index, *addr, *zero, *four, *check1, *check2, *check3, *check, *length, *error, *offset, *loc;
	char * l1;

	index = subscript->cgen();
	zero = cg->GenLoadConstant(0);
	check1 = cg->GenBinaryOp("<", index, zero);

	length = cg->GenTempVar();
	addr = base->cgen();
	cg->GenStore(addr, length, -4);

	check2 = cg->GenBinaryOp("<", index, length);
	check3 = cg->GenBinaryOp("==", check2, zero);
	check = cg->GenBinaryOp("||", check1, check3);


	l1 = cg->NewLabel();

	cg->GenIfZ(check, l1);
	error = cg->GenLoadConstant("Decaf runtime error: Array subscript out of bound...");
	cg->GenBuiltInCall(PrintString, error, NULL);

	cg->GenBuiltInCall(Halt, NULL, NULL);

	cg->GenLabel(l1);
	four = cg->GenLoadConstant(4);

	offset = cg->GenBinaryOp("*", four, index);

	loc = cg -> GenBinaryOp("+", addr, offset);
	loc = cg->GenLoad(loc);

	//types->Enter(loc->GetName(), 

	return loc;
	*/

}


Location * FieldAccess::cgen() {
	Location * loc = NULL;

	if (!base) {
		loc = locations->Lookup(field->getName());
		PrintDebug("d", "Looking for %s, found at %d\n", field->getName(), loc->GetOffset());
	}

	return loc;
}

Location * NewArrayExpr::cgen() {
	
	Location * num, * zero, *check, *error, *one, *sum, *four, *mul, *array, *fin;
	char * l1;

	l1 = cg->NewLabel();

	num = size->cgen();
	zero = cg->GenLoadConstant(0);
	check = cg->GenBinaryOp("<", num, zero);

	cg->GenIfZ(check, l1);

	error = cg->GenLoadConstant("Decaf runtime error: Array size is <= 0\\n");

	cg->GenBuiltInCall(PrintString, error, NULL);

	cg->GenBuiltInCall(Halt, NULL, NULL);

	cg->GenLabel(l1);

	one = cg->GenLoadConstant(1);
	sum = cg->GenBinaryOp("+", one, num);
	four = cg->GenLoadConstant(4);
	mul = cg->GenBinaryOp("*", sum, four);

	array = cg->GenBuiltInCall(Alloc, mul);

	cg->GenStore(array, num, 0);

	fin = cg->GenBinaryOp("+", array, four);

	// CHECK HERE FOR PRIMITIVE TYPES
	Type * t = new ArrayType(*location, elemType);
	t -> SetParent(this);
	types->Enter(fin->GetName(), t, false);

	return fin;


}

/* original below */


IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
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

       
