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
 * interpreter's data/code segment generator module.
 */

/* declarations. */

static int data_offset = 0;     /* initial data segment offset location. */
static int code_offset = 0;     /* initial code segment offset location. */

/* backpatching support, labels for data, if and while. */
typedef struct lbs
{
  int for_goto;
  int for_jmp_false;
} lbs;

/* operations. */

/* reserves a data offset location. */
int
data_location (void)
{
  return data_offset++;
}

/* reserves a code offset location. */
static int
code_location (void)
{
  return code_offset++;
}

/* returns current code offset location. */
static int
gen_label (void)
{
  return code_offset;
}

/* assigns code at current location. */
static void
assign_code (code_ops operation, int arg)
{
  code[code_offset].op = operation;
  code[code_offset++].arg = arg;
}

/* assigns code at a reserved location. */
static void
back_patch (int addr, code_ops operation, int arg)
{
  code[addr].op = operation;
  code[addr].arg = arg;
}

/* if identifier is defined, assign code. */
static int
context_check (code_ops operation, char *symbolName)
{
  STR *identifier = getSymbol (symbolName);
  if (identifier == NULL)
    {
      printf ("Error: identifier %s is not defined.\n", symbolName);
      return 0;
    }

  assign_code (operation, identifier->offset);
  return 1;
}

/* allocate space for the labels. */
lbs *
newlblrec (void)
{
  return (lbs *) malloc (sizeof (lbs));
}

/* prints program code to standard output. */
static void
print_code (void)
{
  // for indexing the code segment locations.
  int i;

  printf ("Program's Code Segment:\n");

  // parse until the end of the code segment (for DEBUGGING).
  for (i = 0; i < code_offset; i++)
    {
      // show the operation name and the argument.
      printf ("%3ld: %20s%4ld\n", i, op_name[(int) code[i].op], code[i].arg);
    }
}
