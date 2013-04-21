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
#include <stdlib.h>
#include <string.h>

FILE *file_file = NULL;
char file_buffer[BUFSIZ];
char *file_line = NULL;
char *file_key = NULL;
char *file_val = NULL;
char *file_val_next = NULL;
int file_val_cursor = 0;

// implement strdup here since C99 doesn't have it
char *strdup(const char *str) {
    char *new_str = NULL;
    if (str != NULL) {
        new_str = malloc(strlen(str)+1);
        if (new_str)
        strcpy(new_str, str);
    }
    return new_str;
}

FILE* fileOpen(char *filename) {
    file_file = fopen(filename,"r");
    return file_file;
}

char* fileNext() {
    file_key = NULL;
    file_val = NULL;
    if (file_val_next != NULL) {
        free(file_val_next);
        file_val_next = NULL;
    }
    file_val_cursor = 0;

    if (file_file == NULL) return NULL;

    char *tmp = fgets(file_buffer,BUFSIZ,file_file);
    if (tmp != NULL) file_line = strdup(tmp);
    else return NULL;

    if (file_line != NULL && file_line[0] != '#') {
        file_key = strtok(file_line,"=");
        file_val = strtok(NULL,"\n");
    }

    return file_line;
}

char* fileGetKey() {
    if (file_key) return file_key;
    else return "";
}

char* fileGetVal() {
    if (file_val) return file_val;
    else return "";
}

char* fileGetValNext() {
    if (file_val_next != NULL) {
        free(file_val_next);
        file_val_next = NULL;
    }
    int i;
    int len = strlen(file_val);
    int new_cursor = 0;
    for (i=file_val_cursor; i<=len; i++) {
        if (file_val[i] != ',' && file_val[i] != '\n') {
            file_val_cursor++;
            new_cursor++;
            file_val_next = realloc(file_val_next, file_val_cursor);
            file_val_next[new_cursor-1] = file_val[i];
        } else {
            file_val_cursor++;
            new_cursor++;
            file_val_next = realloc(file_val_next, file_val_cursor);
            file_val_next[new_cursor-1] = '\0';
            break;
        }
    }
    if (file_val_next) return file_val_next;
    else return "";
}

char* fileGetLine() {
    if (file_line) return file_line;
    else return "";
}

void fileClose() {
    if (file_val_next != NULL)
        free(file_val_next);
    if (file_line != NULL)
        free(file_line);
    if (file_file != NULL)
        fclose(file_file);

    file_key = NULL;
    file_val = NULL;
    file_val_next = NULL;
    file_val_cursor = 0;
}

