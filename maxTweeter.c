#include <stdio.h>
#include <stdlib.h>
#include <string.h> 


int main(int argc, const char* argv[]) {
  FILE *filename; 
  if(argc < 2){
    printf("No pathname specified\n"); 
    return 1; 
  }

  filename = argv[1];

  printf("filename: %s\n", filename);

  char * buffer = 0;
  long length;
  FILE * file = fopen(filename, "rb");

  if (file)
  {
    fseek(file, 0, SEEK_END);
    length = ftell(file);

    fseek(file, 0, SEEK_SET);
    buffer = malloc(length);

    if (buffer == NULL)
    {
      return 1;
    }

    fread(buffer, 1, length, file);
    fclose (file);
  }

  if (buffer)
  {
    printf("contents: %s\n", buffer);
  }

  return 0;
}