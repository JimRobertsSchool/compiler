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
S : AS {}
  | CD {}
  | 's' {}
;

A : 'a' {}
  | 'E' {}
;

B : 's' {}
;

C : 'c' {}
  | 'E' {}
;

D : 'd' {}
  | 's' {}
;
