#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

int GetFileSize(FILE * fp);
char * GetFileContent(FILE * fp, const int fileSize);
int GetCountLine(char * buf, const int fileSize);
char ** WriteArrayPointers(char * buf, const int fileSize, const int nLine);

#endif // FILE_H_INCLUDED
