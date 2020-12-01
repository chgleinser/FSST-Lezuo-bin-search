/*##############################################################################
    PROJEKT: bin_search_GleinserC.c
    NAME: CHRISTOPH GLEINSER
    KLASSE: 5BHEL
    DATUM: 30.11.2020
##############################################################################*/
/*
Aufgabenstellung:

Implementiere rekursive bin‰re Suche in C, benutze die Funktion strcmp(3)
    studiere das Format des wortbuffer files (unter Linux: hexdump -C wortbuffer)
    lese das wortbuffer file an (wie in file_copy) und schreibe alles in einen groﬂen Buffer (malloc, wie in rle - Bonus: realloc)
    baue einen Sucharray auf (char *search_index[]) mit der Anzahl der Worte (malloc)
    der User gibt ein Wort, beantworte die Frage ob das Wort im Buffer ist oder nicht

*/
//############################### INCLUDES #####################################
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//################################ DEFINES #####################################
//##############################################################################
//############################## SUBROUTINES ###################################
/*##############################################################################
    Funktion: iFileSize
    Erklaerung: berechnet die Groesse des Files
##############################################################################*/
int iFileSize(const char* cPath)
{
	struct stat sFile;

	if(stat(cPath, &sFile) < 0)
	{
		perror("Filegroesse konnte nicht gefunden werden:(\n");
		return -1;
	}
	else
		return sFile.st_size;
}

/*##############################################################################
    Funktion: cWortliste
    Erklaerung: ordnet die Liste damit das Prinzip der binaeren Suche
    funktionieren kann
##############################################################################*/
char **cWortliste(const char* cPath)
{
	int iFileOpen, iSize, iBytes;
    int iRunVar1, iRunVar2;


	if((iSize = iFileSize(cPath))  < 0)
		return NULL;

	char tmp[iSize];
	char cWord[100] = { 0 };
	char **cWordlist = (char **) malloc(iSize * sizeof(*cWordlist));

	// allocate rest of the list (unless you want a seg. fault at strcpy)
	for(iRunVar1 = 0; iRunVar1 < iSize; iRunVar1++)
		cWordlist[iRunVar1] = (char *) malloc(100 * sizeof(**cWordlist));

	// File handling
	if((iFileOpen = open(cPath, O_RDONLY)) < 0)
	{
		perror("File konnte nicht gefunden werden :(\n");
		return NULL;
	}
	while(read(iFileOpen, tmp, iSize) > 0);

	// store values to string array
	for(iRunVar1 = 0, iRunVar2 = 0, iBytes = 0; iBytes < iSize; iRunVar1++)
	{
		for(iRunVar2 = 0; tmp[iBytes] != '\0'; iRunVar2++)
        cWord[iRunVar2] = tmp[iBytes++];
		cWord[iRunVar2] = '\0';

		strcpy(cWordlist[iRunVar1], cWord);

		if(tmp[iBytes] == '\0')
			iBytes++;
	}
	cWordlist[iRunVar1] = NULL;

	// reallocating
	for(iRunVar1 = 0; cWordlist[iRunVar1] != NULL; iRunVar1++);
	cWordlist = (char **) realloc(cWordlist, iRunVar1 * sizeof(*cWordlist));
	for(iRunVar2 = 0; iRunVar2 <= iRunVar1; iRunVar2++)
		cWordlist[iRunVar2] = (char *) realloc(cWordlist[iRunVar2], 100 * sizeof(**cWordlist));
	cWordlist[iRunVar1] = NULL;

	return cWordlist;
}
/*##############################################################################
    Funktion: cBinsearch
    Erklaerung: findet ein Wort in einer geordneten Liste
##############################################################################*/
char* cBinsearch(char** cSearch_index, char* cSearch, int iStart, int iStop)
{
	if(iStop < iStart)
		return NULL;

	int iMid = (iStart + iStop) / 2;

	if(strcmp(cSearch_index[iMid], cSearch) == 0)
		return cSearch_index[iMid];
	else if(strcmp(cSearch_index[iMid], cSearch) < 0)
		return cBinsearch(cSearch_index, cSearch, iMid + 1, iStop);
	else
		return cBinsearch(cSearch_index, cSearch, iStart, iMid - 1);
}
//################################# MAIN #######################################
//Hinweis: Die for-schleife wurde aus der Aufgabenstellung 1:1 ¸bernommen
int main(int argc, char **argv)
{
	int iEnd;
	char **cSearch_index = cWortliste("wortbuffer");

	if(cSearch_index == NULL)
	{
		perror("I the program don't like what you're trying to do >:(\n");
		return -1;
	}

	for(iEnd = 0; cSearch_index[iEnd] != NULL; iEnd++);

	for(;;)
	{
		char input[100];

		fgets(input, sizeof(input), stdin);
		input[strlen(input)-1] = 0;

		if(!strlen(input))
			break;

		struct timeval tv_begin, tv_end, tv_diff;

		gettimeofday(&tv_begin, NULL);
		char *res = cBinsearch(cSearch_index, input, 0, iEnd-1);
		gettimeofday(&tv_end, NULL);

		timersub(&tv_end, &tv_begin, &tv_diff);

		if(res != NULL)
			printf("found %s", res);
		else
			printf("not found");

		printf(" in (%ld seconds %ld microseconds)\n",tv_diff.tv_sec,tv_diff.tv_usec);
	}

	return 0;
}

