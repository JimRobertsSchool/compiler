%lex

%%
\s+	/* */;
"a"	return 'a';
"b"	return 'b';
"c"	return 'c';
"d"	return 'd';
""	return 'E';
<<EOF>>	return 'EOF';

/lex

%start S

%%
S : A {}
;

A : B'b' {}
  | C'd' {}
;

B : 'a'B {}
  | 'E' {}
;

C : 'c'C {}
  | 'E' {}
;
