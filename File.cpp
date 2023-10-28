#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "File.h"

int get_file_size(FILE * fp)
{
    struct stat st;

    fstat(fileno(fp), &st);

    return st.st_size;
}

void file_read(char * buf, FILE * fp, const int fileSize)
{
    if(fread(buf, sizeof(char), fileSize, fp) != fileSize)
    {
        if(feof(fp))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }
}

char * get_file_content(int * fileSize)
{
    const char * nameFile  = "ASM1.txt";
    FILE * fp = fopen(nameFile, "rb");

    if(fp == NULL)
    {
        printf("can't open file\n");
        abort();
    }

    *fileSize = get_file_size(fp);

    char * buf = (char *)calloc(*fileSize, sizeof(char));

    if(buf == NULL)
    {
        printf("buf in NULL\n");
        abort();
    }

    file_read(buf, fp, *fileSize);

    fclose(fp);

    return buf;
}

int get_count_line(char * buf, const int fileSize)
{
    int nLine = 0;

    for(int i = 0; i < fileSize; i++)
    {
        if(buf[i] == '\n')
            nLine++;
    }

    return nLine;
}

char ** write_array_pointers(char * buf, const int fileSize, const int nLine)
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
