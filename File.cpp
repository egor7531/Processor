#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "File.h"

int GetFileSize(FILE * fp)
{
    struct stat st;

    fstat(fileno(fp), &st);

    return st.st_size;
}

void FileRead(char * buf, FILE * fp, const int fileSize)
{
    if(fread(buf, sizeof(char), fileSize, fp) != fileSize)
    {
        if(feof(fp))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }
}

char * GetFileContent(FILE * fp, const int fileSize)
{
    char * buf = (char *)calloc(fileSize, sizeof(char));

    if(buf == NULL)
        printf("buf in NULL\n");

    FileRead(buf, fp, fileSize);

    return buf;
}

int GetCountLine(char * buf, const int fileSize)
{
    int nLine = 0;

    for(int i = 0; i < fileSize; i++)
    {
        if(buf[i] == '\n')
            nLine++;
    }

    return nLine;
}

char ** WriteArrayPointers(char * buf, const int fileSize, const int nLine)
{
    char ** text = (char **)calloc(nLine, sizeof(char*));

    assert(text != NULL);

    int line = 1;

    text[0] = buf;

    for(int i = 0; i < fileSize; i++)
    {
        if(buf[i] == '\n')
        {
            buf[i] = '\0';
            text[line++] = &buf[i+1];
        }
    }

    return text;
}
