/* File:  scanner.l
 * ----------------
 * Lex inupt file to generate the scanner for the compiler.
 */

%{

/* The text within this first region delimited by %{ and %} is assumed to
 * be C/C++ code and will be copied verbatim to the lex.yy.c file ahead
 * of the definitions of the yylex() function. Add other header file inclusions
 * or C++ variable declarations/prototypes that are needed by your code here.
 */

#include <string.h>
#include "scanner.h"
#include "utility.h" // for PrintDebug()
#include "errors.h"

#include <sstream>
#include <iostream>

/* Global variable: yylval
 * -----------------------
 * This global variable is how we get attribute information about the token
 * just scanned to the client. The scanner sets the global variable
 * appropriately and since it's global the client can just read it.  In the
 * future, this variable will be declared for us in the y.tab.c file
 * produced by Yacc, but for now, we declare it manually.
 */
YYSTYPE yylval;  // manually declared for pp1, later Yacc provides

/* Global variable: yylloc
 * -----------------------
 * This global variable is how we get position information about the token
 * just scanned to the client. (Operates similarly to yylval above)
 */
struct yyltype yylloc; // manually dclared for pp1, later Yacc provides

int yycolumn = 1;

/* Macro: YY_USER_ACTION 
 * ---------------------
 * This flex built-in macro can be defined to provide an action which is
 * always executed prior to any matched rule's action. Basically, it is
 * a way of having a piece of code common to all actions factored out to
 * this routine.  We already defined it for you and left the empty
 * function DoBeforeEachAction ready for your use as needed. It will
 * be called once for each pattern scanned from the file, before
 * executing its action.
 */
static void DoBeforeEachAction(); 
#define YY_USER_ACTION DoBeforeEachAction();


int intToInt(char * in) {
	int toReturn;
	std::stringstream ss;
	ss << in;
	ss >> toReturn ? toReturn : 0;
	return toReturn;
}

int hexToInt(char * in) {
	int toReturn;
	std::stringstream ss;
	ss << std::hex << in;
	ss >> toReturn ? toReturn : 0;
	return toReturn;
}

int octToInt(char * in) {
	int toReturn;
	std::stringstream ss;
	ss << std::oct << in;
	ss >> toReturn ? toReturn : 0;
	return toReturn;
}

double strToDbl(char * in) {
	double toReturn;
	std::stringstream ss;
	ss << in;
	ss >> toReturn ? toReturn : 0;
	return toReturn;
}

%}


 /* The section before the first %% is the Definitions section of the lex
  * input file. Here is where you set options for the scanner, define lex
  * states, and can set up definitions to give names to regular expressions
  * as a simple substitution mechanism that allows for more readable
  * entries in the Rules section later. 
  */

letter			[a-zA-Z]
digit			[0-9]
underscore		[_]
space			[ ]
tab			[\t]
newline			[\n]
b_true			true
b_false			false
dot			[\.]
quote			[\"]
string_char		[^\n\"]

hex_int			0[xX][0-9a-fA-F]+
oct_int			0[0-7]+
sci			[eE]({plus}|{minus})?{digit}+

plus			[+]
minus			[\-]
star			[*]
divide			[/]
mod			[%]
not			[!]
assign			[=]
less_than		[<]
less_equal		{less_than}{assign}
greater_than		[>]
greater_equal		{greater_than}{assign}
equal			{assign}{assign}
not_equal		{not}{assign}
and			[&]{2}
or			[|]{2}
semi			[;]
comma			[,]
l_bracket		[\[]
r_bracket		[\]]
l_brace			[\{]
r_brace			[\}]
l_paren			[\(]
r_paren			[\)]

single			{divide}{divide}
multi_start		{divide}{star}
multi_end		{star}{divide}

not_star		[^*\n]
not_end			{star}[^*/\n]

identifier		{letter}({letter}|{digit}|{underscore}){0,30}
long_id			{letter}({letter}|{digit}|{underscore}){31,}

whitespace		({space}|{tab})
boolean_constant	({b_true}|{b_false})
integer_constant	{digit}+
double_constant		({digit})+{dot}({digit})*{sci}?
half_string		{quote}({string_char}*)
string_constant		{half_string}{quote}


%x MULTI

%%             /* BEGIN RULES SECTION */
 /* All patterns and actions should be placed between the start and stop
  * %% markers which delimit the Rules section. 
  */ 


void			{return(T_Void);}
int			{return(T_Int);}
double			{return(T_Double);}
bool			{return(T_Bool);}
string			{return(T_String);}
class			{return(T_Class);}
interface		{return(T_Interface);}
null			{return(T_Null);}
this			{return(T_This);}
extends			{return(T_Extends);}
implements		{return(T_Implements);}
for			{return(T_For);}
while			{return(T_While);}
if			{return(T_If);}
else			{return(T_Else);}
return			{return(T_Return);}
break			{return(T_Break);}
New			{return(T_New);}
NewArray		{return(T_NewArray);}

{plus}			{return('+');}
{minus}			{return('-');}
{star}			{return('*');}
{divide}		{return('/');}
{mod}			{return('%');}
{less_than}		{return('<');}
{less_equal}		{return(T_LessEqual);}
{greater_than}		{return('>');}
{greater_equal}		{return(T_GreaterEqual);}
{assign}		{return('=');}
{equal}			{return(T_Equal);}
{not_equal}		{return(T_NotEqual);}
{and}			{return(T_And);}
{or}			{return(T_Or);}
{not}			{return('!');}
{semi}			{return(';');}
{comma}			{return(',');}
{dot}			{return('.');}
{l_bracket}		{return('[');}
{r_bracket}		{return(']');}
{l_paren}		{return('(');}
{r_paren}		{return(')');}
{l_brace}		{return('{');}
{r_brace}		{return('}');}

{boolean_constant}	{yylval.boolConstant=yytext[0]=='t'?true:false; return(T_BoolConstant);}

{hex_int}		{yylval.integerConstant=hexToInt(yytext); return(T_IntConstant);}
{integer_constant}	{yylval.integerConstant=intToInt(yytext); return(T_IntConstant);}
{double_constant}	{yylval.doubleConstant=strToDbl(yytext); return(T_DoubleConstant);}
{string_constant}	{yylval.stringConstant=yytext; return(T_StringConstant);}
{half_string}		{ReportError::UntermString(&yylloc, yytext);}

{long_id}		{ReportError::LongIdentifier(&yylloc, yytext); return(T_Identifier);}
{identifier}		{return(T_Identifier);}


{whitespace}		{}
{newline}		{yycolumn = 1; yylineno++;}
.			{ReportError::UnrecogChar(&yylloc, yytext[0]);}
%%

/* removed rules
{single}.*		{}

{multi_start}		{BEGIN(MULTI);}
<MULTI>{not_star}	{}
<MULTI>{not_end}	{}
<MULTI>{newline}	{yylineno++;}
<MULTI>{multi_end}	{BEGIN(INITIAL);}
<MULTI><<EOF>>		{ReportError::UntermComment();}

*/

/* The closing %% above marks the end of the Rules section and the beginning
 * of the User Subroutines section. All text from here to the end of the
 * file is copied verbatim to the end of the generated lex.yy.c file.
 * This section is where you put definitions of helper functions.
 */


/* Function: InitScanner
 * ---------------------
 * This function will be called before any calls to yylex().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the scanner (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yy_flex_debug that controls whether flex prints debugging information
 * about each token and what rule was matched. If set to false, no information
 * is printed. Setting it to true will give you a running trail that might
 * be helpful when debugging your scanner. Please be sure the variable is
 * set to false when submitting your final version.
 */
void InitScanner()
{
    PrintDebug("lex", "Initializing scanner");
    yy_flex_debug = false;
}


/* Function: DoBeforeEachAction()
 * ------------------------------
 * This function is installed as the YY_USER_ACTION. This is a place
 * to group code common to all actions.
 */
static void DoBeforeEachAction()
{
	yylloc.first_line = yylloc.last_line = yylineno;
	yylloc.first_column = yycolumn;
	yylloc.last_column = yycolumn + yyleng - 1;
	yycolumn += yyleng;
	strncpy(yylval.identifier, yytext, 32);
	yylval.identifier[31] = '\0';
}
