/**********************************************************************/
/* lab 1 DVG C01 - Lexer OBJECT                                       */
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

/**********************************************************************/
/* OBJECT ATTRIBUTES FOR THIS OBJECT (C MODULE)                       */
/**********************************************************************/
#define BUFSIZE 1024
#define LEXSIZE   30
static char buffer[BUFSIZE];
static char lexbuf[LEXSIZE];
static int  pbuf  = 0;               /* current index program buffer  */
static int  plex  = 0;               /* current index lexeme  buffer  */

/**********************************************************************/
/*  PRIVATE METHODS for this OBJECT  (using "static" in C)            */
/**********************************************************************/
/**********************************************************************/
/* buffer functions                                                   */
/**********************************************************************/
/**********************************************************************/
/* Read the input file into the buffer                                */
/**********************************************************************/

static void get_prog() {
   int c, i = 0;
   while((c = getchar()) != EOF) {
      buffer[i++] = c;
   }
   buffer[i++] = '\n';
   buffer[i++] = '$';
}     

/**********************************************************************/
/* Display the buffer                                                 */
/**********************************************************************/  

static void pbuffer() {   
   printf("\n________________________________________________________");
   printf("\n THE PROGRAM TEXT");
   printf("\n________________________________________________________");
   printf("\n%s", buffer);
   printf("\n________________________________________________________");
}

/**********************************************************************/
/* Copy a character from the program buffer to the lexeme buffer      */
/**********************************************************************/

static void get_char() {   
   lexbuf[plex++] = buffer[pbuf++];
}

/**********************************************************************/
/* End of buffer handling functions                                   */
/**********************************************************************/

/**********************************************************************/
/*  PUBLIC METHODS for this OBJECT  (EXPORTED)                        */
/**********************************************************************/
/**********************************************************************/
/* Return a token                                                     */
/**********************************************************************/
int get_token() {  

   if(pbuf == 0) {
      get_prog();
      pbuffer();
   }

   /* Initialize lexeme buffer */
   memset(lexbuf, 0, LEXSIZE);
   plex = 0;

   /* Ignore white space */
   while(isspace(buffer[pbuf])) {
      pbuf++;
   }


   /* Get the first character from the buffer */
   get_char();

   /* If numerical */
   if(isdigit(lexbuf[0])) {
      while(!isspace(buffer[pbuf]) && !ispunct(buffer[pbuf])) {
         get_char();
      }
      return lex2tok(lexbuf);
   }

   /* If alphanumerical */
   if(isalpha(lexbuf[0])) {
      while(!isspace(buffer[pbuf]) && !ispunct(buffer[pbuf])) {
         get_char();
      }
      return key2tok(lexbuf);
   }

   /* If not numerical and not alphanumerical */
   if((lexbuf[0] == ':') && (buffer[pbuf] == '=')) {
      get_char();
   }
   return lex2tok(lexbuf);
}

/**********************************************************************/
/* Return a lexeme                                                    */
/**********************************************************************/
char * get_lexeme() {  
   return lexbuf;
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/
