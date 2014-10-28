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

%start Ap

%%
Ap : A {}
;

A : B 'a' C {}
   | C {}
;

B : C 'b' {}
  | 'b' {}
;

C : A B {}
  | 'a' {}
;
