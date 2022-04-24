#######
#
#  Makefile for the YAFL (Yet Another Free Language) Interpreter.
#
#  Copyright (C) 2010  Efstathios Chatzikyriakidis (stathis.chatzikyriakidis@gmail.com)
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program. If not, see <http://www.gnu.org/licenses/>.
#
#######

# information which help the compilation part.

# the compiler which is used to build YAFL.
CC = gcc

# the name of the executable interpreter.
EXEC = yafl

# the targets of the objects.
OBJS = yafl-parser yafl-scanner

# build the YAFL interpreter.
yafl-interpreter: $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) *.o -lm

# build the syntactical analyzer.
yafl-parser:
	bison -d yafl.y
	$(CC) $(CFLAGS) -c yafl.tab.c

# build the lexical analyzer.
yafl-scanner:
	flex yafl.lex
	$(CC) $(CFLAGS) -c lex.yy.c

# clean target.
clean:
	rm -f $(EXEC) lex.yy.c yafl.tab.c yafl.tab.h *.o
