/*
 *  This file is part of the YAFL (Yet Another Free Language) Interpreter.
 *
 *  Copyright (C) 2010  Efstathios Chatzikyriakidis (contact@efxa.org)
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
 * interpreter's symbol table module.
 */

/* declarations. */

/* declare the symbol table record node (STR). */
typedef struct STR
{
  char *name;                   // name of the symbol.
  int offset;                   // data location offset.
  struct STR *next;             // next STR node in the list.
} STR;

/* type definition name for pointer to STR node. */
typedef STR * PSTR;

/* symbol table entry. */
static PSTR identifier = NULL;

/* pointer to the symbol table. */
static PSTR symbolTable = NULL;

/* operations. */

/* put a symbol to the symbol table. */
static PSTR
putSymbol (char *symbolName)
{
  // define a STR node pointer.
  PSTR newSTR = NULL;

  // allocate memory for a STR node.
  newSTR = (PSTR) malloc (sizeof (STR));

  // allocate enough memory for the name of the symbol.
  newSTR->name = (char *) malloc (strlen (symbolName) + 1);

  // copy the symbol name inside to the STR node.
  strcpy (newSTR->name, symbolName);

  // get the location offset of the symbol.
  newSTR->offset = data_location ();

  // STR node points next to the symbol table.
  newSTR->next = symbolTable;

  // make the STR node the head of the list.
  symbolTable = newSTR;

  // return also the pointer of STR node.
  return newSTR;
}

/* get a symbol from the symbol table. */
static PSTR
getSymbol (char *symbolName)
{
  // define a STR node pointer.
  PSTR current = NULL;

  // parse the symbol table starting from the head.
  for (current = symbolTable; current != NULL; current = current->next)
    // if the symbol name exists in the symbol table.
    if (!strcmp (current->name, symbolName))
      // return the STR node pointer.
      return current;

  // otherwise, return unchanged NULL pointer (not found).
  return current;
}

/* install identifier & check if previously defined. */
static int
installSymbol (char *symbolName)
{
  // try to get the symbol from the symbol table.
  if (getSymbol (symbolName) == NULL)
    {
      // try to put the symbol to the symbol table.
      if (putSymbol (symbolName) == NULL)
        {
          printf ("Error: identifier %s cannot be stored.\n", symbolName);
          return 0;
        }

      return 1;
    }

  printf ("Error: identifier %s is already defined.\n", symbolName);
  return 0;
}

/* free the allocated memory of the symbol table. */
static void
freeSymbolTable (void)
{
  // define a STR node pointer.
  PSTR nextSTR = NULL;

  // parse the symbol table starting from the head.
  while (symbolTable != NULL)
    {
      // get the next STR node.
      nextSTR = symbolTable->next;

      // deallocate the name of the symbol.
      free (symbolTable->name);

      // deallocate the STR node.
      free (symbolTable);

      // point to the next STR node.
      symbolTable = nextSTR;
    }
}
