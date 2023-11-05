#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

int get_file_size(FILE * fp);
char * get_file_content(const char * nameFile, int * fileSize);
int get_count_line(char * buf, const int fileSize);
char ** write_array_pointers(char * buf, const int fileSize, const int nLine);

#endif // FILE_H_INCLUDED
