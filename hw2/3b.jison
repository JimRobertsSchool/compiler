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
S : AB {}
;

A : 'a' {}
  | 'E' {}
;

B : 'b' {}
  | 'E' {}
;
