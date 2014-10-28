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

%start Sp

%%
Sp : Sp {}
;

S : 'a'A'a' {}
   | 'E' {}
;

A : 'a''b'S {}
  | 'c' {}
;
