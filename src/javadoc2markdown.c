/*
 ============================================================================
 Name        : javadoc2markdown.c
 Author      : Brian Walton (brian@riban.co.uk)
 Version     :
 Copyright   : riban 2015 GPL V2
 Description : Convert javadoc to markdown
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum WIKI_STYLE
{
	STYLE_TIDDLY,
	STYLE_GITHUB
};

int findNextChar(char* pString)
{
	int i = 0;
	while(*(pString + i) != 0 && (*(pString + i) == ' ' || *(pString + i) == '\t'))
		++i;
	return i;
}

int findNextSpace(char* pString)
{
	int i = 0;
	while((*(pString + i) != 0) && (*(pString + i) != ' '))
		++i;
	return i;
}

int main(int argc, char **argv)
{
	int nStyle = STYLE_TIDDLY;

    if(argc < 2)
        {
            perror("Insufficient parameters\n");
            return -1;
        }
    FILE *pFile;
    char line[255];

    pFile = fopen(argv[1], "rt");
    if(NULL == pFile)
    {
        perror("Cannot open input file\n");
        exit(-1);
    }

    int nState = 0; //Are we inside a block?
    //Scan each line of file
    while(fgets(line, sizeof(line), pFile) != NULL)
    {
        char sBrief[255] = {0};
        char sParam[2550] = {0};
        char* pParam = sParam;
        char sNote[2550] = {0};
        char* pNote = sNote;
        char sReturn[255] = {0};
        char sTodo[255] = {0};
        char* pTodo= sTodo;
    	int nPos = 0;
    	//Scan line looking for start of javadoc block "/**"
    	while(0 != line[nPos] && 0 != line[nPos + 1] && 0 != line[nPos + 2])
    	{
    		if(line[nPos++] != '/' || line[nPos++] != '*' || line[nPos++] != '*')
    			continue;
    		//We are within a javadoc block
    		do
    		{
    			nPos = 0;
    			//Scan each line of a block
    			while(0 != line[nPos] && 0 != line[nPos + 1])
    			{
    				if(line[nPos] == '*' && line[++nPos] == '/')
    				{
    					nState = 1;
    					break;
    				}
       				if(line[nPos++] == '@')
					{
       					//Found tag
       					if(0 == strncmp(line + nPos, "brief", 5))
       					{
       						//Found @brief
       						nPos += 5;
       						nPos += findNextChar(line + nPos);
       						strncpy(sBrief, line + nPos, sizeof(sBrief));
       					}
       					else if(0 == strncmp(line + nPos, "param", 5))
       					{
       						//Found @param
       						nPos += 5;
       						nPos += findNextChar(line + nPos);
       						*(pParam++) = '>';
       						switch(nStyle)
       						{
       							case STYLE_TIDDLY:
       								*(pParam++) = '\'';
       								*(pParam++) = '\'';
       								break;
       							case STYLE_GITHUB:
       								*(pParam++) = '*';
       								*(pParam++) = '*';
       								break;
       						}
       						int nSpace = findNextSpace(line + nPos);
       						strncpy(pParam, line + nPos, nSpace);
       						pParam += nSpace;
       						nPos += nSpace;
       						switch(nStyle)
       						{
       							case STYLE_TIDDLY:
       								*(pParam++) = '\'';
       								*(pParam++) = '\'';
       								break;
       							case STYLE_GITHUB:
       								*(pParam++) = '*';
       								*(pParam++) = '*';
       								break;
       						}
       						strncpy(pParam, line + nPos, sParam + sizeof(sParam) - pParam);
       						pParam += strlen(line + nPos);
       						(*pParam++) = '\n';
       					}
       					else if(0 == strncmp(line + nPos, "return", 6))
       					{
       						//Found @return
       						nPos += 6;
       						nPos += findNextChar(line + nPos);
       						strncpy(sReturn, line + nPos, sizeof(sReturn));
       					}
       					else if(0 == strncmp(line + nPos, "note", 4))
       					{
       						//Found @note
       						nPos += 4;
       						nPos += findNextChar(line + nPos);
       						strncpy(pNote, line + nPos, sNote + sizeof(sNote) - pNote);
       						pNote += strlen(line + nPos);
       						*(pNote++) = '\n';
       					}
       					else if(0 == strncmp(line + nPos, "todo", 4))
       					{
       						//Found @todo
       						nPos += 4;
       						nPos += findNextChar(line + nPos);
       						strncpy(pTodo, line + nPos, sTodo + sizeof(sTodo) - pTodo);
       						pTodo += strlen(line + nPos);
       					}
       					else if(0 == strncmp(line + nPos, "author", 6))
       					{
       						//Found @author
       						nPos += 6;
       						nPos += findNextChar(line + nPos);
       					}
       					else if(0 == strncmp(line + nPos, "version", 7))
       					{
       						//Found @version
       						nPos += 7;
       						nPos += findNextChar(line + nPos);
       					}
       					else if(0 == strncmp(line + nPos, "see", 3))
       					{
       						//Found @see
       						nPos += 3;
       						nPos += findNextChar(line + nPos);
       					}
       					else
       						fprintf(stderr, "Found unknown tag: @%s\n", line + nPos);
					}
    			}
    		} while(0 == nState && fgets(line, sizeof(line), pFile) != NULL);
    		if(0 == nState)
    		{
    			//Should be inside a block so must be an error in source file
    			fprintf(stderr, "Syntax error in source file\n");
    			exit(-1);
    		}
    		char sFunction[255] = {0};
    		fgets(sFunction, sizeof(line), pFile);
    		if(strlen(sFunction) > 2)
    		{
				//Output markdown
    			switch(nStyle)
    			{
    				case STYLE_TIDDLY:
					{
						printf("''`%s`''\n\n%s\n\n", sFunction, sBrief);
						if(strlen(sParam) > 2)
							printf("''Parameters''\n\n%s\n", sParam);
						if(strlen(sReturn) > 2)
							printf("''Returns''\n\n>%s\n\n", sReturn);
						if(strlen(sNote) > 2)
							printf("%s\n", sNote);
						if(strlen(sTodo) > 2)
							printf("''TODO''\n\n%s\n", sTodo);
						printf("---\n");
					}
					break;
    				case STYLE_GITHUB:
					{
						printf("**`%s`**\n\n", sFunction);
						printf("%s\n\n", sBrief);
						if(strlen(sParam) > 2)
							printf("**Parameters**\n\n%s\n", sParam);
						if(strlen(sReturn) > 2)
							printf("**Returns**\n\n>%s\n\n", sReturn);
						if(strlen(sNote) > 2)
							printf("%s\n", sNote);
						if(strlen(sTodo) > 2)
							printf("**TODO**\n\n%s\n", sTodo);
						printf("***\n");
					}
					break;

    			}
    		}
    		nState = 0;
    	}
    }
    fclose(pFile);

	return 0;
}
