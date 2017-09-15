#include "configloader.h"

void cleanconfigsettings(struct configsettings * config)
{
  if(config->rootdirectory)
    free(config->rootdirectory);
  if(config->requesthandling)
    free(config->requesthandling);
}

void insertoption(char * input,struct configsettings * conf,int index)
{
	int size = strlen(input);

	switch(index)
	{
		case 0:
			conf->port = atoi(input);
			break;
		case 1:
			conf->rootdirectory = calloc(sizeof(char),size+1);
			memcpy(conf->rootdirectory,input,size);
			break;
		case 2:
			for(int i = 0; i < size; i++)
				input[i] = tolower(input[i]);
			conf->requesthandling = calloc(sizeof(char),size+1);
			char * validOptions[] = {"thread","fork"};
			int valid = 0;
			for(int i = 0; i < 2; i++)
				if(strcmp(input,validOptions[i]) == 0)
					valid = 1;
			if(!valid)
				printf("Invalid request handling option %s \n",input);
			memcpy(conf->requesthandling,input,size);
			break;
	}
}

int validoption(char * input) //returns index for switch case
{
	char * options[] = {"PORT","ROOTDIRECTORY","REQUESTHANDLING"};
	for(int i = 0; i < 3; i++)
	{
		if(strcmp(input,options[i]) == 0)
		return i;
	}
	return -1;
}

void loadconfiguration(struct configsettings * conf)
{
	FILE *fp = fopen("config.txt","r");

	conf->port = 0;
	conf->rootdirectory = NULL;
	conf->requesthandling = NULL;

	char c, linebuffer[100];
	int index = 0, result = -1;

	memset(linebuffer,0,100);
	while((c = fgetc(fp)) != EOF)
	{
		if(c != '\n' && c != '=')
			linebuffer[index++] = c;
		else
		{
			if(c == '=')
				result = validoption(linebuffer);
			else if(c == '\n')
				insertoption(linebuffer,conf,result);
			memset(linebuffer,0,100);
			index = 0;
		}
	}
	fclose(fp);
}
