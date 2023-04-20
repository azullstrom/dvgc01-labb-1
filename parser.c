/* Anders Ullström */

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
#define DEBUG 1
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
static void psyntax_error_symbol(char *expected) {
   printf("\nSYNTAX: Symbol expected %s found %s", expected, get_lexeme());
   is_parse_ok = 0;
} 

static void psyntax_error_id() {
   printf("\nSYNTAX: ID expected found %s", get_lexeme());
   is_parse_ok = 0;
} 

static void psyntax_error_type() {
   printf("\nSYNTAX: Type name expected found %s", get_lexeme());
   is_parse_ok = 0;
} 

static void psyntax_error_operand() {
   printf("\nSYNTAX: Operand Expected");
   is_parse_ok = 0;
} 

static void psyntax_error_extra_symbol() {
   printf("\nSYNTAX: Extra symbols after end of parse!\n");
   while(!strstr(get_lexeme(), "$")) {
      printf("%s ", get_lexeme());
      lookahead = get_token();
   }
   printf("\n");
   is_parse_ok = 0;
} 

static void psemantic_error_not_declared() {
   printf("\nSEMANTIC: ID NOT declared: %s", get_lexeme());
   is_parse_ok = 0;
} 

static void psemantic_error_duplicate() {
   printf("\nSEMANTIC: ID already declared: %s", get_lexeme());
   is_parse_ok = 0;
}

static void psemantic_error_assign(char *left_type, char *right_type) {
   printf("\nSEMANTIC: Assign types: %s := %s", left_type, right_type);
   is_parse_ok = 0;
} 

/**********************************************************************/
/* The Parser functions                                               */
/**********************************************************************/
static void match(int t) {
    if(DEBUG) printf("\n --------In match expected: %s found: %s", tok2lex(t), get_lexeme());
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
      psyntax_error_symbol("program");
   }
   if(lookahead == id) {
      addp_name(get_lexeme());
      match(id);
   } else {
      psyntax_error_id();
   }
   if(lookahead == '(') match('('); else psyntax_error_symbol("(");
   if(lookahead == input) match(input); else psyntax_error_symbol("input");
   if(lookahead == ',') match(','); else psyntax_error_symbol(",");
   if(lookahead == output) match(output); else psyntax_error_symbol("output");
   if(lookahead == ')') match(')'); else psyntax_error_symbol(")");
   if(lookahead == ';') match(';'); else psyntax_error_symbol(";");
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
         psyntax_error_type();
         setv_type(error);
         break;
   }
   if (DEBUG) printf("\n *** Out  type");
}

static void id_list() {
   if (DEBUG) printf("\n *** In  id_list");
   if(lookahead == id) {
      if(!find_name(get_lexeme())) {
         addv_name(get_lexeme());
      } else {
         psemantic_error_duplicate();
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
      psyntax_error_symbol(":");
   }
   type();
   /* Check follow set ';' for type() */
   if(lookahead != ';') {
      psyntax_error_symbol(";");
   } else {
      match(';');
   }
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
      psyntax_error_symbol("var");
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
         if(!find_name(get_lexeme())) {
            psemantic_error_not_declared();
         }
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
         psyntax_error_operand();
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
   toktyp type;

   type = factor();
   
   if(lookahead == '*') {
      match('*');
      type = get_otype('*', type, term());
   }
   if (DEBUG) printf("\n *** Out term");
   return type;
}

static toktyp expr() {
   if (DEBUG) printf("\n *** In  expr");
   toktyp type;

   /* Check first set id or number for expr() */
   type = term();

   if(lookahead == '+') {
      match('+');
      type = get_otype('+', type, expr());
   }
   if (DEBUG) printf("\n *** Out expr");
   return type;
}

static void assign_stat() {
   if (DEBUG) printf("\n *** In  assign_stat");
   toktyp right_type = error, left_type = get_ntype(get_lexeme());

   if(lookahead == id) {
      match(id);
   } else {
      psyntax_error_id();
   }
   if(lookahead == assign) {
      match(assign);
   } else {
      psyntax_error_symbol(":=");
   }
   right_type = expr();
   
   /* If x := y is not the same */
   if(left_type != right_type) {
      psemantic_error_assign(tok2lex(left_type), tok2lex(right_type));
   }
   if (DEBUG) printf("\n *** Out assign_stat");
}

static void stat() {
   if (DEBUG) printf("\n *** In  stat");
   assign_stat();
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
      psyntax_error_symbol("begin");
   }
   stat_list();
   if(lookahead == end) {
      match(end);
   } else {
      psyntax_error_symbol("end");
   }
   if(lookahead == '.') {
      match('.');
   } else {
      psyntax_error_symbol(".");
   }
   
   if (DEBUG) printf("\n *** Out stat_part\n");
}
   
/**********************************************************************/
/*  PUBLIC METHODS for this OBJECT  (EXPORTED)                        */
/**********************************************************************/

int parser() {
   if (DEBUG) printf("\n *** In  parser");
   lookahead = get_token();       // get the first token
   if(lookahead < 0) {
      printf("\nWARNING: Input file is empty");
   } else {
      program_header();               // call the first grammar rule
      var_part();
      stat_part();
      if(lookahead != '$') {
         psyntax_error_extra_symbol();
      }
      if(is_parse_ok) {
         printf("\nPARSE SUCCESSFUL!");
      } else {
         printf("\nPARSE FAILED!");
      }
   }
   p_symtab();
   return is_parse_ok;             // status indicator
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/
