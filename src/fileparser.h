/*
    Espada 2
    Copyright (C) 2013 Justin Jacobs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>

FILE *file_file;
char file_buffer[BUFSIZ];
char *file_line;
char *file_key;
char *file_val;
char *file_val_next;
int file_val_cursor;

char *strdup(const char *str);
FILE* fileOpen(char *filename);
char* fileNext();
char* fileGetKey();
char* fileGetVal();
char* fileGetValNext();
char* fileGetLine();
void fileClose();

