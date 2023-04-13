/**********************************************************************/
/* lab 1 DVG C01 - Parser OBJECT                                      */
/**********************************************************************/

/**********************************************************************/
/* Include files                                                      */
/**********************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

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
      printf("\nSyntax Error: Expected <program> as first set for program_header().");
      is_parse_ok = 0;
   }
   if(lookahead == id) {
      addp_name(get_lexeme());
      match(id);
   } else {
      printf("\nSyntax Error: Expected ID after <program>.");
      is_parse_ok = 0;
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
         printf("\nSemantic Error: Invalid type <%s>.", get_lexeme());
         setv_type(error);
         match(id);
         is_parse_ok = 0;
         break;
   }
   /* Check follow set ';' for type() */
   if(lookahead != ';') {
      printf("\nSyntax Error: Expected ';' as follow set for type().");
      is_parse_ok = 0;
   }
   if (DEBUG) printf("\n *** Out  type");
}

static void id_list() {
   if (DEBUG) printf("\n *** In  id_list");
   if(lookahead == id) {
      if(!find_name(get_lexeme())) {
         addv_name(get_lexeme());
      } else {
         printf("\nSemantic Error: Duplicate ID's declared as %s.", get_lexeme());
         is_parse_ok = 0;
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
      printf("\nSyntax Error: Expected ':' as follow set for id_list().");
      is_parse_ok = 0;
   }
   type();
   match(';');
   if (DEBUG) printf("\n *** Out  var_dec");
}

static void var_dec_list() {
   if (DEBUG) printf("\n *** In  var_dec_list");
   var_dec();
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
      printf("\nSyntax Error: Expected <var> as first set for var_part().");
      is_parse_ok = 0;
   }
   var_dec_list();
   if (DEBUG) printf("\n *** Out var_part\n");
}

/* STAT_PART */
static toktyp operand() {
   if (DEBUG) printf("\n *** In  operand");
   switch(lookahead) {
   case id:
      match(id);
      return get_ntype(get_lexeme());
      break;
   case number:
      match(number);
      return get_ntype(get_lexeme());
      break;
   default:
      printf("\nSyntax Error: Expected operand.");
      is_parse_ok = 0;
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
   toktyp type = factor();
   if(lookahead == '*') {
      match('*');
      type = get_otype('*', type, term());
   }
   if (DEBUG) printf("\n *** Out term");
   return type;
}

static toktyp expr() {
   if (DEBUG) printf("\n *** In  expr");
   if(!find_name(get_lexeme()) && !atoi(get_lexeme())) {
      printf("\nSemantic Error: var %s is not declared.", get_lexeme());
      is_parse_ok = 0;
   }

   toktyp type = term();
   if(lookahead == '+') {
      match('+');
      type = get_otype('+', type, expr());
   }
   if (DEBUG) printf("\n *** Out expr");
   return type;
}

static void assign_stat() {
   if (DEBUG) printf("\n *** In  assign_stat");
   if(lookahead == id) {
      if(!find_name(get_lexeme())) {
         printf("\nSemantic Error: var %s is not declared.", get_lexeme());
         is_parse_ok = 0;
      }
      match(id);
   } else {
      printf("\nSyntax Error: Expected ID before := definition.");
      is_parse_ok = 0;
   }
   if(lookahead == assign) {
      match(assign);
      expr();
   } else {
      printf("\nSyntax Error: Expected := after ID definition.");
      is_parse_ok = 0;
   }
   /* If there is more lines of code than the example code */
   if(lookahead == id) {
      assign_stat();
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
      printf("\nSyntax Error: Expected <begin> as first set for stat_part().");
      is_parse_ok = 0;
   }
   stat_list();
   match(end);
   match('.');
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
