/**********************************************************************/
/* lab 1 DVG C01 - Parser OBJECT                                      */
/**********************************************************************/

/**********************************************************************/
/* Include files                                                      */
/**********************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/**********************************************************************/
/* Other OBJECT's METHODS (IMPORTED)                                  */
/**********************************************************************/
#include "keytoktab.h"
#include "lexer.h"          
#include "symtab.h"
#include "optab.h"

/**********************************************************************/
/* OBJECT ATTRIBUTES FOR THIS OBJECT (C MODULE)                       */
/**********************************************************************/
#define DEBUG 0
static int  lookahead=0;
static int  is_parse_ok=1;

/**********************************************************************/
/* RAPID PROTOTYPING - simulate the token stream & lexer (get_token)  */
/**********************************************************************/
/* define tokens + keywords NB: remove this when keytoktab.h is added */
/**********************************************************************/
// enum tvalues { program = 257, id, input, output, var, integer, begin, defined_as, number, end };
/**********************************************************************/
/* Simulate the token stream for a given program                      */
/**********************************************************************/
// static int tokens[] = {program, id, '(', input, ',', output, ')', ';',
// var, id, ',', id, ',', id, ':', integer, ';', begin, id, predef,
// id, '+', id, '*', number, end, '.', '$' };

/**********************************************************************/
/*  Simulate the lexer -- get the next token from the buffer          */
/**********************************************************************/
// static int pget_token() {  
//    static int i=0;
//    if (tokens[i] != '$') return tokens[i++]; else return '$';
// }

/**********************************************************************/
/*  PRIVATE METHODS for this OBJECT  (using "static" in C)            */
/**********************************************************************/

/* PROGRAM_HEADER */
static void program_header();

/* VAR_PART */
static void type();
static void id_list();
static void var_dec();
static void var_dec_list();
static void var_part();

/* STAT_PART */
static toktyp operand();
static toktyp factor();
static toktyp term();
static toktyp expr();
static void assign_stat();
static void stat();
static void stat_list();
static void stat_part();

/* ERROR HANDLING */
static void psyntax_error(char *expected, int line) {
   printf("\nSyntax Error on line %d: Expected %s found %s", line, expected, tok2lex(lex2tok(get_lexeme())));
   is_parse_ok = 0;
} 

static void psemantic_error_not_declared(int line) {
   printf("\nSemantic Error on line %d: var %s is not declared", line, get_lexeme());
   is_parse_ok = 0;
} 

static void psemantic_error_duplicate(int line) {
   printf("\nSemantic Error on line %d: Duplicate id declared as %s", line, get_lexeme());
   is_parse_ok = 0;
}

static void psemantic_error_assign(char *left_type, char *right_type, int line) {
   printf("\nSemantic Error on line %d: Assign types: %s := %s", line, left_type, right_type);
   is_parse_ok = 0;
} 

/**********************************************************************/
/* The Parser functions                                               */
/**********************************************************************/
static void match(int t) {
    if(DEBUG) printf("\n --------In match expected: %s found: %s", tok2lex(t), tok2lex(lookahead));
    if (lookahead == t) lookahead = get_token();
    else {
        is_parse_ok=0;
        printf("\n *** Unexpected Token: expected: %4d found: %4d (in match)", t, lookahead);
    }
}

/**********************************************************************/
/* The grammar functions                                              */
/**********************************************************************/
static void program_header() {
   if (DEBUG) printf("\n *** In  program_header");
   /* Check first set for program_header() */
   if(lookahead == program) {
      match(program); 
   } else {
      psyntax_error("program", __LINE__);
   }
   if(lookahead == id) {
      addp_name(get_lexeme());
      match(id);
   } else {
      psyntax_error("id", __LINE__);
   }
   match('('); match(input);
   match(','); match(output); 
   match(')'); match(';');
   if (DEBUG) printf("\n *** Out program_header\n");
}

/* VAR_PART */
static void type() {
   if (DEBUG) printf("\n *** In  type");
   switch(lookahead) {
      case integer:
         setv_type(integer);
         match(integer);
         break;
      case boolean:
         setv_type(boolean);
         match(boolean);
         break;
      case real:
         setv_type(real);
         match(real);
         break;
      default:
         psyntax_error("type", __LINE__);
         setv_type(undef);
         break;
   }
   /* Check follow set ';' for type() */
   if(lookahead != ';') {
      psyntax_error(";", __LINE__);
   }
   if (DEBUG) printf("\n *** Out  type");
}

static void id_list() {
   if (DEBUG) printf("\n *** In  id_list");
   if(lookahead == id) {
      if(!find_name(get_lexeme())) {
         addv_name(get_lexeme());
      } else {
         psemantic_error_duplicate(__LINE__);
      }
      match(id);
   }
   if(lookahead == ',') {
      match(',');
      id_list();
   }
   if (DEBUG) printf("\n *** Out  id_list");
}

static void var_dec() {
   if (DEBUG) printf("\n *** In  var_dec");
   id_list();
   /* Check follow set ':' for id_list() */
   if(lookahead == ':') {
      match(':');
   } else {
      psyntax_error(":", __LINE__);
   }
   type();
   match(';');
   if (DEBUG) printf("\n *** Out  var_dec");
}

static void var_dec_list() {
   if (DEBUG) printf("\n *** In  var_dec_list");
   var_dec();
   /* If there is more lines of code (more var declarations) after ';' */
   if(lookahead == id) {
      var_dec_list();
   }
   if (DEBUG) printf("\n *** Out  var_dec_list");
}

static void var_part() {
   if (DEBUG) printf("\n *** In  var_part");
   /* Check first set <var> for var_part() */
   if(lookahead == var) {
      match(var); 
   } else {
      psyntax_error("var", __LINE__);
   }
   var_dec_list();
   if (DEBUG) printf("\n *** Out var_part\n");
}

/* STAT_PART */
static toktyp operand() {
   if (DEBUG) printf("\n *** In  operand");
   toktyp type;
   switch(lookahead) {
      case id:
         type = get_ntype(get_lexeme());
         match(id);
         return type;
         break;
      case number:
         type = integer;
         match(number);
         return type;
         break;
      default:
         psyntax_error("operand", __LINE__);
         return error;
         break;
      }
   if (DEBUG) printf("\n *** Out operand");
}

static toktyp factor() {
   if (DEBUG) printf("\n *** In  factor");
   toktyp type;
   if(lookahead == '(') {
      match('(');
      type = expr();
      match(')');
   } else {
      type = operand();
   }
   if (DEBUG) printf("\n *** Out factor");
   return type;
}

static toktyp term() {
   if (DEBUG) printf("\n *** In  term");
   /* If variable does not exist and is not a number */
   if(!find_name(get_lexeme()) && lookahead != number) {
      psemantic_error_not_declared(__LINE__);
   }
   toktyp type;

   /* Check first set id or number for expr() */
   if(lookahead == id || lookahead == number) {
      type = factor();
   } else {
      psyntax_error("id or number", __LINE__);
   }
   
   if(lookahead == '*') {
      match('*');
      type = get_otype('*', type, term());
   }
   if (DEBUG) printf("\n *** Out term");
   return type;
}

static toktyp expr() {
   if (DEBUG) printf("\n *** In  expr");
   /* If variable does not exist and is not a number */
   if(!find_name(get_lexeme()) && lookahead != number) {
      psemantic_error_not_declared(__LINE__);
   }
   toktyp type;
   
   /* Check first set id or number for expr() */
   if(lookahead == id || lookahead == number) {
      type = term();
   } else {
      psyntax_error("id or number", __LINE__);
   }
   if(lookahead == '+') {
      match('+');
      type = get_otype('+', type, expr());
   }
   if (DEBUG) printf("\n *** Out expr");
   return type;
}

static void assign_stat() {
   if (DEBUG) printf("\n *** In  assign_stat");
   if(!find_name(get_lexeme())) {
      psemantic_error_not_declared(__LINE__);
   }
   toktyp right_type = error, left_type = get_ntype(get_lexeme());

   match(id);
   if(lookahead == assign) {
      match(assign);
      right_type = expr();
   } else {
      psyntax_error(":=", __LINE__);
   }

   /* If x := y is not the same and the otype is not real. 
   integer := real => integer
   real := integer => real */
   toktyp allowed = get_otype('+', left_type, right_type);
   if(left_type != right_type && allowed != real) {
      psemantic_error_assign(tok2lex(left_type), tok2lex(right_type), __LINE__);
   }

   /* If there is more lines of code (more var definitions/assign) than the example code */
   if(lookahead == id) {
      assign_stat();
   }
   if (DEBUG) printf("\n *** Out assign_stat");
}

static void stat() {
   if (DEBUG) printf("\n *** In  stat");
   /* 
   Usually there are more x_stat() functions like if_stat(), while_stat()
   but for this lab only assign_stat() is necessary because the code has no if statements 
   and so on. That's why a if statement is defined in this stat() function just to simulate
   reality.
   */
   if(lookahead == id) {
      assign_stat();
   } else {
      psyntax_error("id", __LINE__);
   }
   if (DEBUG) printf("\n *** Out stat");
}

static void stat_list() {
   if (DEBUG) printf("\n *** In  stat_list");
   stat();
   if(lookahead == ';') {
      match(';');
      stat_list();
   }
   if (DEBUG) printf("\n *** Out stat_list");
}

static void stat_part() {
   if (DEBUG) printf("\n *** In  stat_part");
   /* Check first set <begin> for stat_part() */
   if(lookahead == begin) {
      match(begin);
   } else {
      psyntax_error("begin", __LINE__);
   }
   stat_list();
   if(lookahead == end) {
      match(end);
   } else {
      psyntax_error("end", __LINE__);
   }
   if(lookahead == '.') {
      match('.');
   } else {
      psyntax_error(".", __LINE__);
   }
   
   if (DEBUG) printf("\n *** Out stat_part\n");
}
   
/**********************************************************************/
/*  PUBLIC METHODS for this OBJECT  (EXPORTED)                        */
/**********************************************************************/

int parser() {
   if (DEBUG) printf("\n *** In  parser");
   lookahead = get_token();       // get the first token
   program_header();               // call the first grammar rule
   var_part();
   stat_part();
   if(is_parse_ok) {
      printf("\nPARSE SUCCESSFUL!");
   } else {
      printf("\nPARSE FAILED!");
   }
   p_symtab();
   return is_parse_ok;             // status indicator
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/
