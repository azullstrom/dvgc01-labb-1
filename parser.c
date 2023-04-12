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
/* #include "optab.h"       */       /* when the optab     is added   */

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
static void operand();
static void factor();
static void term();
static void expr();
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
   match(program); match(id); match('('); match(input);
   match(','); match(output); match(')'); match(';');
   if (DEBUG) printf("\n *** Out program_header\n");
}

/* VAR_PART */
static void type() {
   if (DEBUG) printf("\n *** In  type");
   switch(lookahead) {
      case integer:
         match(integer);
         break;
      case boolean:
         match(boolean);
         break;
      case real:
         match(real);
         break;
      default:
         printf("Error in type\n");
         is_parse_ok = 0;
         break;
   }
   if (DEBUG) printf("\n *** Out  type");
}

static void id_list() {
   if (DEBUG) printf("\n *** In  id_list");
   if(lookahead == id) {
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
   match(':');
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
   match(var); 
   var_dec_list();
   if (DEBUG) printf("\n *** Out var_part\n");
}

/* STAT_PART */
static void operand() {
   if (DEBUG) printf("\n *** In  operand");
   switch(lookahead) {
   case id:
      match(id);
      break;
   case number:
      match(number);
      break;
   default:
      printf("Error in operand\n");
      is_parse_ok = 0;
      break;
   }
   if (DEBUG) printf("\n *** Out operand");
}

static void factor() {
   if (DEBUG) printf("\n *** In  factor");
   if(lookahead == '(') {
      match('(');
      expr();
      match(')');
   } else {
      operand();
   }
   if (DEBUG) printf("\n *** Out factor");
}

static void term() {
   if (DEBUG) printf("\n *** In  term");
   factor();
   if(lookahead == '*') {
      match('*');
      term();
   }
   if (DEBUG) printf("\n *** Out term");
}

static void expr() {
   if (DEBUG) printf("\n *** In  expr");
   term();
   if(lookahead == '+') {
      match('+');
      expr();
   }
   if (DEBUG) printf("\n *** Out expr");
}

static void assign_stat() {
   if (DEBUG) printf("\n *** In  assign_stat");
   if(lookahead == id) {
      match(id);
   }
   if(lookahead == assign) {
      match(assign);
      expr();
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
   match(begin); 
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
   return is_parse_ok;             // status indicator
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/
