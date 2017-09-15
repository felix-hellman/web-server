#ifndef configloader_h
#define configloader_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"



void cleanconfigsettings(struct configsettings * conf);
void loadconfiguration(struct configsettings * conf);
void insertoption(char * input,struct configsettings * conf,int index);
int validoption(char * input);




#endif
