/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>
#include "tac.h"

using namespace std;
Location * EmptyExpr::cgen() {
	return NULL;
}

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

Location * NullConstant::cgen() {
	Location * toReturn;

	toReturn = cg->GenLoadConstant(0);
	types->Enter(toReturn->GetName(), Type::nullType, false);
	
	return toReturn;
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
	if (true && inClass() && dynamic_cast<FieldAccess*>(left)) {
		FieldAccess * fa = (FieldAccess*)left;
		ClassDecl * cd = inClass();
		Location * temp = cd->locs->Lookup(fa->getName());
		if (temp) {
			int off = temp -> GetOffset();
			cg->GenStore(l, r, off);
			return r;
		} else {
			
		}
	}
	loc = l;

	cg->GenAssign(loc, r);
	types->Enter(loc->GetName(), types->Lookup(r->GetName()), false);
	swapType(r, loc);
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

Location * This::cgen() {
	return cg->ThisPtr;
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
		// no base, check parameters
		loc = locations->Lookup(field->getName());
		if (!loc) {
			goto out0;
		}
		loc = loc && loc->GetSegment() == gpRelative ? NULL : loc;
		//PrintDebug("d", "Looking for %s, found at %d\n", field->getName(), loc->GetOffset());
	} 
	out0:

	if (!loc && (inClass() || base)) {
		// want to just check instance variables next
		Location * b;
		b = base ? base->cgen() : cg->ThisPtr;
		ClassDecl * cd = inClass();
		loc = cd->locs->Lookup(field->getName());
		if (!loc) goto out1;
		//Location * t = cg->GenLoad(b, 7);
		//loc = cg->GenLoad(t, loc->GetOffset());
		if (!write) {
			loc = cg->GenLoad(b, loc->GetOffset());
			types->Enter(loc->GetName(), cd->vdecs->Lookup(field->getName()));
		} else {
			loc = b;
		}
	}
	out1:

	if (!loc) {
		loc = locations->Lookup(field->getName());
	}

	return loc;
}

Location * Call::cgen() {

	List<Location *> ll = List<Location *>();
	Location * b = NULL;
	Location * m = NULL;
	Type * rt = NULL;
	int offset = 0;

	if (!base) {
		if (inClass()) {
			ClassDecl * cd = inClass();
			if (cd->locs->Lookup(field->getName())) {
				// method
				b = cg->ThisPtr;
				m = cd->locs->Lookup(field->getName());	
				rt = cd->fdecs->Lookup(field->getName())->getRet();
			} else {
				// function

			}
		} else {
			// function
			
		}
	} else {
		b = base->cgen();
		Type * t = types->Lookup(b->GetName());
		if (dynamic_cast<NamedType *>(t)) {
			NamedType * nt = dynamic_cast<NamedType *>(t);
			ClassDecl * cd = hclass->Lookup(nt->getName());
			m = cd->locs->Lookup(field->getName());
			rt = cd->fdecs->Lookup(field->getName())->getRet();
		} else {
			// array length
			Location * toRet = cg->GenLoad(b, -4);
			types->Enter(toRet->GetName(), Type::intType);
			return toRet;
		}
	}

	for (int i = 0; i < actuals->NumElements(); ++i) {
		Location * l = actuals->Nth(i)->cgen();
		ll.InsertAt(l, 0);
	}

	if (m) {
		ll.Append(b);	
		Location * toUse = new Location(fpRelative, b->GetOffset(), b->GetName());
		Location * one = cg->GenLoad(b, 0);
		m = cg->GenLoad(one, m->GetOffset());
	} else {
		rt = hfns->Lookup(field->getName())->getRet();
	}

	for (int i = 0; i < ll.NumElements(); ++i) {
		cg->GenPushParam(ll.Nth(i));
	}

	Location * toReturn = NULL;

	if (m) {
		toReturn = cg->GenACall(m, rt);
	} else {
		string s = "_";
		s.append(field->getName());
		toReturn = cg->GenLCall(s.c_str(), rt);
	}

	cg->GenPopParams(4*ll.NumElements());

	if (toReturn) {
		types->Enter(toReturn->GetName(), rt);
	}

	return toReturn;
}

Location * NewExpr::cgen() {
	Location * toReturn;

	char * name = cType->getName();
	ClassDecl * cd = hclass->Lookup(name);

	Location * s = cg->GenLoadConstant(cd->size + 4);
	toReturn = cg->GenBuiltInCall(Alloc, s,NULL);

	Location * vt = cg->GenLoadLabel(name);
	cg->GenStore(toReturn, vt);

	types->Enter(toReturn->GetName(), new NamedType(cd->getId()));

	return toReturn;
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

Location * ReadIntegerExpr::cgen() {
	
	Location * toReturn;

	toReturn = cg->GenBuiltInCall(ReadInteger, NULL, NULL);

	types->Enter(toReturn->GetName(), Type::intType, false);

	return toReturn;
}

Location * ReadLineExpr::cgen() {

	Location * toReturn;

	toReturn = cg->GenBuiltInCall(ReadLine, NULL, NULL);

	types->Enter(toReturn->GetName(), Type::stringType, false);

	return toReturn;
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

       
