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
 * interpreter's virtual machine module.
 */

/* declarations. */

#define CODE_SIZE 999           /* size of the code segment. */
#define STCK_SIZE 999           /* size of the stack segment. */

/* OPERATIONS: internal representation. */
typedef enum code_ops
{
  OP_HALT,
  OP_STORE,

  OP_JMP_FALSE,
  OP_GOTO,

  OP_DATA,
  OP_LD_INT,
  OP_LD_VAR,

  OP_INPUT_INT,
  OP_OUTPUT_INT,

  OP_OR,
  OP_AND,
  OP_NOT,

  OP_LT,
  OP_LE,
  OP_EQ,
  OP_NE,
  OP_GT,
  OP_GE,

  OP_BINAND,
  OP_BINOR,
  OP_BINXOR,
  OP_BINSHL,
  OP_BINSHR,

  OP_ADD,
  OP_SUB,
  OP_MOD,
  OP_MUL,
  OP_DIV,
  OP_PWR,

  OP_UMINUS
} code_ops;

/* OPERATIONS: external representation. */
static char *op_name[] = {
  "Halt",
  "Store",

  "Jump If False",
  "Goto",

  "Data",
  "Load Integer",
  "Load Variable",

  "Input Integer",
  "Output Integer",

  "OR",
  "AND",
  "NOT",

  "Less Than",
  "Less Or Equal",
  "Equal",
  "Not Equal",
  "Greater",
  "Greater Or Equal",

  "Binary AND",
  "Binary OR",
  "Binary XOR",
  "Binary SHL",
  "Binary SHR",

  "Addition",
  "Subtraction",
  "Modulo",
  "Multiplication",
  "Division",
  "Power",

  "Minus"
};

/* structure of an instruction code. */
typedef struct instruction
{
  code_ops op;                  // code operation.
  int arg;                      // operation argument.
} instruction;

/* CODE segment as static array. */
static instruction code[CODE_SIZE];

/* RUN-TIME stack as static array. */
static int stack[STCK_SIZE];

/* structure of virtual machine registers. */
typedef struct registers
{
  int pc;                       // program counter register.
  instruction ir;               // instruction register.
  int top;                      // top register of the stack.
  int ar;                       // activation record register.
} registers;

/* virtual machine registers. */
static registers rs;

/* init virtual machine's registers. */
static void
initRegisters (registers * r)
{
  // init registers default to zero.
  r->pc = r->top = r->ar = 0;
}

static void
printRegisters (registers r)
{
  printf ("PC=%3d IR.arg=%8d AR=%3d Top=%3d,%8d\n", r.pc,
                                                    r.ir.arg,
                                                    r.ar,
                                                    r.top,
                                                    stack[r.top]);
}

/* execute the code segment. */
static void
execute_code (void)
{
  // set virtual machine's registers values to zero.
  initRegisters (&rs);

  // for each operation in the code segment.
  do
    {
      // display the values of the registers (for DEBUGGING).
      // printRegisters (rs);

      // fetch cycle - next instruction from code segment.
      rs.ir = code[rs.pc++];

      // execute cycle - run the instruction code.
      switch (rs.ir.op)
        {
          case OP_HALT:
            break;

          case OP_INPUT_INT:
            printf ("Input: ");
            scanf ("%ld", &stack[rs.ar + rs.ir.arg]);
            break;

          case OP_OUTPUT_INT:
            printf ("Output: %d\n", stack[rs.top--]);
            break;

          case OP_STORE:
            stack[rs.ir.arg] = stack[rs.top--];
            break;

          case OP_JMP_FALSE:
            if (!stack[rs.top--])
              rs.pc = rs.ir.arg;
            break;

          case OP_GOTO:
            rs.pc = rs.ir.arg;
            break;

          case OP_DATA:
            rs.top += rs.ir.arg;
            break;

          case OP_LD_INT:
            stack[++rs.top] = rs.ir.arg;
            break;

          case OP_LD_VAR:
            stack[++rs.top] = stack[rs.ar + rs.ir.arg];
            break;

          case OP_OR:
            if (stack[rs.top - 1] || stack[rs.top])
              stack[--rs.top] = 1;
            else
              stack[--rs.top] = 0;
            break;

          case OP_AND:
            if (stack[rs.top - 1] && stack[rs.top])
              stack[--rs.top] = 1;
            else
              stack[--rs.top] = 0;
            break;

          case OP_NOT:
            stack[rs.top] = !stack[rs.top];
            break;

          case OP_UMINUS:
            stack[rs.top] = -stack[rs.top];
            break;

          case OP_BINAND:
            stack[rs.top - 1] = stack[rs.top - 1] & stack[rs.top];
            rs.top--;
            break;

          case OP_BINOR:
            stack[rs.top - 1] = stack[rs.top - 1] | stack[rs.top];
            rs.top--;
            break;

          case OP_BINXOR:
            stack[rs.top - 1] = stack[rs.top - 1] ^ stack[rs.top];
            rs.top--;
            break;

          case OP_BINSHR:
            stack[rs.top - 1] = stack[rs.top - 1] >> stack[rs.top];
            rs.top--;
            break;

          case OP_BINSHL:
            stack[rs.top - 1] = stack[rs.top - 1] << stack[rs.top];
            rs.top--;
            break;

          case OP_LT:
            if (stack[rs.top - 1] < stack[rs.top])
              stack[--rs.top] = 1;
            else
              stack[--rs.top] = 0;
            break;

          case OP_LE:
            if (stack[rs.top - 1] <= stack[rs.top])
              stack[--rs.top] = 1;
            else
              stack[--rs.top] = 0;
            break;

          case OP_EQ:
            if (stack[rs.top - 1] == stack[rs.top])
              stack[--rs.top] = 1;
            else
              stack[--rs.top] = 0;
            break;

          case OP_NE:
            if (stack[rs.top - 1] != stack[rs.top])
              stack[--rs.top] = 1;
            else
              stack[--rs.top] = 0;
            break;

          case OP_GT:
            if (stack[rs.top - 1] > stack[rs.top])
              stack[--rs.top] = 1;
            else
              stack[--rs.top] = 0;
            break;

          case OP_GE:
            if (stack[rs.top - 1] >= stack[rs.top])
              stack[--rs.top] = 1;
            else
              stack[--rs.top] = 0;
            break;

          case OP_ADD:
            stack[rs.top - 1] = stack[rs.top - 1] + stack[rs.top];
            rs.top--;
            break;

          case OP_SUB:
            stack[rs.top - 1] = stack[rs.top - 1] - stack[rs.top];
            rs.top--;
            break;

          case OP_MOD:
            if (!stack[rs.top]) {
              printf ("Arithmetic Error: Zero Division.\n");
              exit (EXIT_FAILURE);
            }

            stack[rs.top - 1] = stack[rs.top - 1] % stack[rs.top];
            rs.top--;
            break;

          case OP_MUL:
            stack[rs.top - 1] = stack[rs.top - 1] * stack[rs.top];
            rs.top--;
            break;

          case OP_DIV:
            if (!stack[rs.top]) {
              printf ("Arithmetic Error: Zero Division.\n");
              exit (EXIT_FAILURE);
            }

            stack[rs.top - 1] = stack[rs.top - 1] / stack[rs.top];
            rs.top--;
            break;

          case OP_PWR:
            stack[rs.top - 1] = pow (stack[rs.top - 1], stack[rs.top]);
            rs.top--;
            break;

          default:
            printf ("Error: internal memory dump.\n");
            break;
        }
    }
  while (rs.ir.op != OP_HALT);
}
