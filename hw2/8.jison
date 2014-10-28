%lex

%%
\s+	/* */;
"a"	return 'a';
"e"	return 'e';
"d"	return 'd';
"&"	return '&';
"%"	return '%';
"^"	return '^';
<<EOF>>	return 'EOF';

/lex

%start S

%%
S : A'&'B {}
  | B {}
;

A : 'e' {}
;

B : B'%''d' {}
  | C {}
;

C : C'^' {}
  | 'a' {}
  | 'd' {}
;
