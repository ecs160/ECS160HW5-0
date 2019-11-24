#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char ** col_names;
  int col_num;
  int tweeter_idx;
} Header;

char * TWEETER_COL_NAME = "name";

Header * parse_header(char ** line)
{
  Header * header = malloc(sizeof(Header));
  if (header == NULL) {
    return NULL;
  }

  header->tweeter_idx = -1;

  char * col = NULL;
  int i = 0;

  while( (col = strsep(&line, ",")) != NULL ) {
    printf("col entry: %s\n", col);

    int len = strlen(col);
    if (*col == '"') {
      if (col[len-1] != '"') {
        printf("Bad quote\n");
        exit(1);
      }

      col[len-1] = '\0';
      col = col + 1;
      printf("col name: %s\n", col);
    }
    
    int cmp = strcmp(col, TWEETER_COL_NAME);
    if (cmp == 0) {
      printf("found name @ %i\n", i);
      header->tweeter_idx = i;
      break;
    }

    col = col - 1;
    col[len-1] = '"';

    i++;
  }

  if (header->tweeter_idx == -1) {
    printf("Name col not found\n");
    exit(1);
  }


  return header;
}

void parse_row(char * line)
{
  printf("entry: %s\n", line);
  return;
}

int main(int argc, const char* argv[]) {
  FILE *filename; 

  // Open file
  if(argc < 2){
    printf("No pathname specified\n"); 
    return 1; 
  }

  filename = argv[1];

  printf("filename: %s\n\n", filename);

  char * buffer = 0;
  long length;
  FILE * file = fopen(filename, "rb");

  if (file == NULL) {
    printf("File failed to open\n");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  length = ftell(file);

  fseek(file, 0, SEEK_SET);
  buffer = malloc(length);

  if (buffer == NULL)
  {
    printf("Couldn't allocate buffer\n");
    return 1;
  }

  /*
   * Read file Contents
   */

  fread(buffer, 1, length, file);
  fclose (file);
  // printf("contents: %s\n\n", buffer);

  /*
   * Parse Contents
   */

  // Header

  printf("\n---- HEADER ----\n");

  char * header_line;
  header_line = strsep(&buffer, "\n");
  if (header_line == NULL) {
    // todo
    return 1;
  }

  Header* header = parse_header(header_line);

  printf("header name idx: %i\n", header->tweeter_idx);
  
  printf("\n---- ENTRIES ----\n");
  // New line
  char * entry_row;
  while ( (entry_row = strsep(&buffer, "\n")) != NULL ) {
    // printf("entry: %s\n", entry_line);
    parse_row(entry_row);
  }

  return 0;
}