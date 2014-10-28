%lex

%%
\s+	/* */;
"("	return '(';
")"	return ')';
"["	return '[';
"]"	return ']';
"c"	return 'c';
<<EOF>>	return 'EOF';

/lex

%start S

%%
S : '(' X {}
  | E ']' {}
  | F ')' {}
;

X : E ')' {}
  | F ')' {}
;

E : A {}
;

F : A {}
;

A : 'c' {}
;
