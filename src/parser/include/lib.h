#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>

char *fcontent(char *filename);

int isbrace(char c);
int iscomma(char c);
int iscolon(char c);
int isquote(char c);
int ispoint(char c);
int isroot(char c);

#endif