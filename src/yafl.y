/*
 *  This file is part of the YAFL (Yet Another Free Language) Interpreter.
 *
 *  Copyright (C) 2010  Efstathios Chatzikyriakidis (stathis.chatzikyriakidis@gmail.com)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * interpreter's parser module (syntactical, semantic analysis).
 */

%{

/* C libraries. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "symbol-table.h"                 /* symbol table. */
#include "virtual-machine.h"              /* virtual machine. */
#include "data-code-generator.h"          /* code generator. */

/* errors counter. */
int errors;

%}

/* semantic records. */
%union
{
  int intval;                   /* number values. */
  char *id;                     /* identifiers. */
  struct lbs *lbls;             /* backpatching. */
}

/* tokens. */

%token TOKEN_ASSGNOP         /* assign operator. */

%token TOKEN_OROP            /* logical operators. */
%token TOKEN_ANDOP
%token TOKEN_NOTOP

%token TOKEN_OPENBRACKET     /* block operators. */
%token TOKEN_CLOSEBRACKET
%token TOKEN_OPENPAREN   
%token TOKEN_CLOSEPAREN

%token TOKEN_MINUS           /* arithmetic operators. */
%token TOKEN_ADDITION       
%token TOKEN_MULTIPLICATION
%token TOKEN_DIVISION
%token TOKEN_MODULO
%token TOKEN_POWER

%token TOKEN_BINAND          /* binary operators. */
%token TOKEN_BINOR
%token TOKEN_BINXOR
%token TOKEN_BINSHL
%token TOKEN_BINSHR

%token TOKEN_SEMICOLON       /* separation operators. */
%token TOKEN_COMMA

%token TOKEN_LESS            /* comparison operators. */
%token TOKEN_GREATER
%token TOKEN_LESSEQUAL
%token TOKEN_GREATEREQUAL
%token TOKEN_EQUAL
%token TOKEN_NOTEQUAL

%token <intval> TOKEN_NUMBER /* number value. */

%token <id> TOKEN_IDENTIFIER /* identifier name value. */

%token <lbls> TOKEN_IF       /* control flow keywords. */
%token TOKEN_ELSE
%token <lbls> TOKEN_WHILE
%token TOKEN_SKIP

%token TOKEN_PROGRAM         /* program structure keywords. */
%token TOKEN_INTEGER
%token TOKEN_START
%token TOKEN_FINISH

%token TOKEN_INPUT           /* i/o keywords. */
%token TOKEN_OUTPUT

/* operators precedence. */

%left TOKEN_COMMA
%right TOKEN_ASSGNO
%left TOKEN_OROP
%left TOKEN_ANDOP
%left TOKEN_BINOR
%left TOKEN_BINXOR
%left TOKEN_BINAND
%left TOKEN_EQUAL TOKEN_NOTEQUAL
%left TOKEN_LESS TOKEN_LESSEQUAL TOKEN_GREATER TOKEN_GREATEREQUAL
%left TOKEN_BINSHL TOKEN_BINSHR
%left TOKEN_ADDITION TOKEN_MINUS
%left TOKEN_MULTIPLICATION TOKEN_DIVISION TOKEN_MODULO
%right TOKEN_NOTOP UMINUS
%right TOKEN_POWER

/* grammar rules of the language. */

%%

GRULE_program : TOKEN_PROGRAM
  GRULE_declarations
  TOKEN_START { assign_code (OP_DATA, data_location () - 1); }
    GRULE_commands
  TOKEN_FINISH { assign_code (OP_HALT, 0);
                 freeSymbolTable ();
                 YYACCEPT; /* successful parse. */
               }
;

GRULE_declarations : /* empty */
  | TOKEN_INTEGER GRULE_idseq TOKEN_IDENTIFIER TOKEN_SEMICOLON { if (!installSymbol ($3)) errors++; }
;

GRULE_idseq : /* empty */
  | GRULE_idseq TOKEN_IDENTIFIER TOKEN_COMMA { if (!installSymbol ($2)) errors++; }
;

GRULE_commands : /* empty */
  | GRULE_commands GRULE_command TOKEN_SEMICOLON
;

GRULE_command : TOKEN_SKIP
  | TOKEN_INPUT TOKEN_IDENTIFIER { if (!context_check (OP_INPUT_INT, $2)) errors++; }
  | TOKEN_OUTPUT GRULE_exp { assign_code (OP_OUTPUT_INT, 0); }
  | TOKEN_IDENTIFIER TOKEN_ASSGNOP GRULE_exp { if (!context_check (OP_STORE, $1)) errors++; }

  | TOKEN_IF GRULE_exp { $1 = (lbs *) newlblrec ();
                         $1->for_jmp_false = code_location ();
                       }
    TOKEN_OPENBRACKET
      GRULE_commands { $1->for_goto = code_location (); }
    TOKEN_CLOSEBRACKET TOKEN_ELSE TOKEN_OPENBRACKET { back_patch ($1->for_jmp_false, OP_JMP_FALSE, gen_label ()); }
      GRULE_commands
    TOKEN_CLOSEBRACKET { back_patch ($1->for_goto, OP_GOTO, gen_label()); }

  | TOKEN_WHILE { $1 = (lbs *) newlblrec ();
                  $1->for_goto = gen_label ();
                } GRULE_exp { $1->for_jmp_false = code_location (); }
    TOKEN_OPENBRACKET
      GRULE_commands
    TOKEN_CLOSEBRACKET { assign_code (OP_GOTO, $1->for_goto);
                         back_patch ($1->for_jmp_false, OP_JMP_FALSE, gen_label ());
                       }
;

GRULE_exp : TOKEN_NUMBER { assign_code (OP_LD_INT, $1); }
  | TOKEN_IDENTIFIER { if (!context_check (OP_LD_VAR, $1)) errors++; }

  | GRULE_exp TOKEN_OROP GRULE_exp           { assign_code (OP_OR, 0);  }
  | GRULE_exp TOKEN_ANDOP GRULE_exp          { assign_code (OP_AND, 0); }
  | TOKEN_NOTOP GRULE_exp                    { assign_code (OP_NOT, 0); }

  | GRULE_exp TOKEN_BINAND GRULE_exp         { assign_code (OP_BINAND, 0); }
  | GRULE_exp TOKEN_BINOR GRULE_exp          { assign_code (OP_BINOR, 0);  }
  | GRULE_exp TOKEN_BINXOR GRULE_exp         { assign_code (OP_BINXOR, 0); }
  | GRULE_exp TOKEN_BINSHL GRULE_exp         { assign_code (OP_BINSHL, 0); }
  | GRULE_exp TOKEN_BINSHR GRULE_exp         { assign_code (OP_BINSHR, 0); }

  | GRULE_exp TOKEN_LESS GRULE_exp           { assign_code (OP_LT, 0); }
  | GRULE_exp TOKEN_LESSEQUAL GRULE_exp      { assign_code (OP_LE, 0); }
  | GRULE_exp TOKEN_EQUAL GRULE_exp          { assign_code (OP_EQ, 0); }
  | GRULE_exp TOKEN_NOTEQUAL GRULE_exp       { assign_code (OP_NE, 0); }
  | GRULE_exp TOKEN_GREATER GRULE_exp        { assign_code (OP_GT, 0); }
  | GRULE_exp TOKEN_GREATEREQUAL GRULE_exp   { assign_code (OP_GE, 0); }

  | GRULE_exp TOKEN_ADDITION GRULE_exp       { assign_code (OP_ADD, 0); }
  | GRULE_exp TOKEN_MINUS GRULE_exp          { assign_code (OP_SUB, 0); }
  | GRULE_exp TOKEN_MULTIPLICATION GRULE_exp { assign_code (OP_MUL, 0); }
  | GRULE_exp TOKEN_DIVISION GRULE_exp       { assign_code (OP_DIV, 0); }
  | GRULE_exp TOKEN_MODULO GRULE_exp         { assign_code (OP_MOD, 0); }
  | GRULE_exp TOKEN_POWER GRULE_exp          { assign_code (OP_PWR, 0); }

  | TOKEN_MINUS GRULE_exp %prec UMINUS       { assign_code (OP_UMINUS, 0); }

  | TOKEN_OPENPAREN GRULE_exp TOKEN_CLOSEPAREN
;

%%

/* interpreter entry point. */
int
main (int argc, char *argv[])
{
  // possible input file as argument.
  extern FILE *yyin;

  // open the input file passed.
  yyin = fopen (argv[1], "r");

  // assume no errors at the beggining.
  errors = 0;

  printf ("Source Code Analysis Started.\n");

  // parse the source code (lexical, sentactical, semantic analysis).
  yyparse ();

  printf ("Source Code Analysis Finished.\n");

  // close the input file.
  fclose (yyin);

  // if there are no errors start executing the code.
  if (errors == 0)
    {
      // print the code segment of the program.
      print_code ();

      printf ("Program Execution Started.\n");

      // execute code segment.
      execute_code ();

      printf ("Program Execution Finished.\n");

      // program has executed normally.
      return EXIT_SUCCESS;
    }

  // program has executed abnormally.
  return EXIT_FAILURE;
}

/* function called by yyparse on error. */
int
yyerror (char *s)
{
  errors++;
  printf ("Error: syntactical error found.\n");
}
