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
 * interpreter's scanner module (lexical analysis).
 */

%{

/* tokens and yylval. */
#include "yafl.tab.h" 

/* errors counter. */
extern int errors;

%}

/* token definitions. */

DIGIT [0-9]
ID [a-zA-Z_][a-zA-Z0-9_]*

/* regular epxressions defining the tokens. */

%%

":="     { return (TOKEN_ASSGNOP);        } /* assign operator. */

"||"     { return (TOKEN_OROP);           } /* logical operators. */
"&&"     { return (TOKEN_ANDOP);          }
"!"      { return (TOKEN_NOTOP);          }

"{"      { return (TOKEN_OPENBRACKET);    } /* block operators. */
"}"      { return (TOKEN_CLOSEBRACKET);   }
"("      { return (TOKEN_OPENPAREN);      }
")"      { return (TOKEN_CLOSEPAREN);     }

"-"      { return (TOKEN_MINUS);          } /* arithmetic operators. */
"+"      { return (TOKEN_ADDITION);       }
"*"      { return (TOKEN_MULTIPLICATION); }
"/"      { return (TOKEN_DIVISION);       }
"%"      { return (TOKEN_MODULO);         }
"**"     { return (TOKEN_POWER);          }

"&"      { return (TOKEN_BINAND);         } /* binary operators. */
"|"      { return (TOKEN_BINOR);          }
"^"      { return (TOKEN_BINXOR);         }
"<<"     { return (TOKEN_BINSHL);         }
">>"     { return (TOKEN_BINSHR);         }

";"      { return (TOKEN_SEMICOLON);      } /* separation operators. */
","      { return (TOKEN_COMMA);          }

"<"      { return (TOKEN_LESS);           } /* comparison operators. */
">"      { return (TOKEN_GREATER);        }
"<="     { return (TOKEN_LESSEQUAL);      }
">="     { return (TOKEN_GREATEREQUAL);   }
"="      { return (TOKEN_EQUAL);          }
"!="     { return (TOKEN_NOTEQUAL);       }

{DIGIT}+ { /* number value. */
           yylval.intval = atoi (yytext);
           return (TOKEN_NUMBER);
         }

if       { return (TOKEN_IF);             } /* control flow keywords. */
else     { return (TOKEN_ELSE);           }
while    { return (TOKEN_WHILE);          }
skip     { return (TOKEN_SKIP);           }

program  { return (TOKEN_PROGRAM);        } /* program structure keywords. */
integer  { return (TOKEN_INTEGER);        }
start    { return (TOKEN_START);          }
finish   { return (TOKEN_FINISH);         }

input    { return (TOKEN_INPUT);          } /* i/o keywords. */
output   { return (TOKEN_OUTPUT);         }

{ID}     { /* identifier name value. */
           yylval.id = (char *) strdup (yytext);
           return (TOKEN_IDENTIFIER);
         }

[ \t\n]+ /* eat up whitespace. */

.        { return (yytext[0]);            } /* characters left. */

"/*"     { /* ignore multiline comments. */
           register int c;
           for (;;)
             {
               while ((c = input ()) != '*' && c != EOF)
                 ; /* eat up text of comment. */

               if (c == '*')
                 {
                   while ((c = input ()) == '*')
                     ;

                   if (c == '/')
                     break; /* found the end. */
                 }

               if (c == EOF)
                 {
                   printf("Error: EOF in comment.\n");
                   errors++;
                   break;
                 }
             }
         }                    

%%

int
yywrap (void) {
}
