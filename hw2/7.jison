%lex

%%
\s+	/* */;
"id"	return 'id';
"="	return '=';
";"	return ';';
"+"	return '+';
","	return ',';
"{"	return '{';
"}"	return '}';
"("	return '(';
")"	return ')';
<<EOF>>	return 'EOF';

/lex

%start S

%%
S : 'id''='A';' {}
;

A : 'id''='A {}
  | E {}
;

E : E '+' P {}
  | P
;

P : 'id' {}
  | '('A';'A')' {}
  | '('V','V')' {}
  | '{'A','A'}' {}
  | '{'V';'V'}' {}
;

V : 'id' {}
;
