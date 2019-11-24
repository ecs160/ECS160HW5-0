#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * TWEETER_COL_NAME = "name";

typedef struct {
  char ** col_names;
  int col_num;
  int tweeter_idx;
} Header;

typedef struct {
  char * name;
  int n;
} TweeterCount;

typedef struct {
  TweeterCount * count[20000];
  int size;
} TotalCounts;

void print_tweet_counts(TotalCounts * counts)
{
  // printf("\n---- PRINTING ALL ----\n");
  int i;
  for (i=0; i<counts->size && i < 10; i++) {
    TweeterCount * this_count = counts->count[i];
    printf("%s: %i\n", this_count->name, this_count->n);
  }
}

void sort_tweet_counts(TotalCounts * counts)
{
  int i;
  for (i=0; i<counts->size; i++) {
    int j;
    for (j=0; j<counts->size-i; j++) {
        int a = counts->count[i]->n;
        int b = counts->count[j]->n;
        if (a >= b) {
            TweeterCount * hold = counts->count[i];
            counts->count[i] = counts->count[j];
            counts->count[j] = hold; 
        }
    }
  }
}


int find_tweeter(TotalCounts* tweetCounts, char* name)
{
  if(tweetCounts == NULL || name == NULL){
    // printf("find_tweeter @ Null Argument\n");
    return -1;
  }

  int i = 0; 
  int idx = -1; 

  for(i = 0; i < tweetCounts->size; i++){
    if(strcmp(name, tweetCounts->count[i]->name) == 0){
        idx = i;
        break; 
    }
  }

  return idx;
}

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
    // printf("col entry: %s\n", col);

    int removed_quotes = 0;

    char* UCol; 

    int len = strlen(col);
    if (*col == '"') {
      if (col[len-1] != '"') {
        // // printf("Bad quote\n");
        exit(1);
      }

      UCol = (char*)malloc(sizeof(char)*(len - 2)); 
      strncpy(UCol, col, len-2); 
      removed_quotes = 1;
      // printf("col name: %s\n", col);
    } else {
      UCol = (char*)malloc(sizeof(char)*(len)); 
      strncpy(UCol, col, len); 
    }
    
    int cmp = strcmp(UCol, TWEETER_COL_NAME);
    if (cmp == 0) {
      // printf("found name @ %i\n", i);
      header->tweeter_idx = i;
      break;
    }

    // if (removed_quotes) {
    //   col = col - 1;
    //   col[len-1] = '"';
    // }

    i++;
  }

  if (header->tweeter_idx == -1) {
    // printf("Name col not found\n");
    exit(1);
  }


  return header;
}

void parse_row(char * line, TotalCounts * tweet_counts)
{
  char * col = NULL;

  int i = 0;
  while( (col = strsep(&line, ",")) != NULL ) {

    if (i++ != 8) {
      continue;
    }

    // printf("col: %s\n", col);

    int len = strlen(col);
    char* UCol; 

    int removed_quotes = 0;
    // Check for quotes
    if (*col == '"') {
      if (col[len-1] != '"') {
        // printf("Bad quote\n");
        exit(1);
      }

      UCol = (char*)malloc(sizeof(char)*(len - 2)); 
      strncpy(UCol, col, len-2); 
      removed_quotes = 1;
      // printf("col name: %s\n", col);
    } else {
      UCol = (char*)malloc(sizeof(char)*(len)); 
      strncpy(UCol, col, len); 
    }

    int match_idx = find_tweeter(tweet_counts, UCol);

    if (match_idx == -1) {
      // Not Found
      int last = tweet_counts->size;
      TweeterCount * new_tweeter_count = malloc(sizeof(TweeterCount));
      if (new_tweeter_count == NULL) {
        //todo error
        exit(1);
      }

      int name_len = strlen(UCol);
      char * tweeter_name = malloc(name_len + 1);
      if (tweeter_name == NULL) {
        // todo error
        exit(1);
      }

      memcpy(tweeter_name, UCol, name_len+1);

      new_tweeter_count->name = tweeter_name;
      new_tweeter_count->n = 1;

      tweet_counts->count[last] = new_tweeter_count;

      // printf("tweeter %s has %i tweets\n", tweet_counts->count[last]->name, tweet_counts->count[last]->n);
      tweet_counts->size = tweet_counts->size + 1;
    } else {
      int count = tweet_counts->count[match_idx]->n;

      tweet_counts->count[match_idx]->n = count + 1;

      // printf("tweeter %s has %i tweets\n", tweet_counts->count[match_idx]->name, tweet_counts->count[match_idx]->n);
      }

    // Bring back the quotes
    // if (removed_quotes) {
    //   col = col - 1;
    //   col[len-1] = '"';
    // }

    i++;
  }

  return;
}

int main(int argc, const char* argv[]) {
  FILE *filename; 

  // Open file
  if(argc < 2){
    // printf("No pathname specified\n"); 
    return 1; 
  }

  filename = argv[1];

  // printf("filename: %s\n\n", filename);

  char * buffer = 0;
  long length;
  FILE * file = fopen(filename, "rb");

  if (file == NULL) {
    // printf("File failed to open\n");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  length = ftell(file);

  fseek(file, 0, SEEK_SET);
  buffer = malloc(length);

  if (buffer == NULL)
  {
    // printf("Couldn't allocate buffer\n");
    return 1;
  }

  /*
   * Read file Contents
   */

  fread(buffer, 1, length, file);
  fclose (file);
  // // printf("contents: %s\n\n", buffer);

  /*
   * Parse Contents
   */

  // Header

  // printf("\n---- HEADER ----\n");

  char * header_line;
  header_line = strsep(&buffer, "\n");
  if (header_line == NULL) {
    // todo
    return 1;
  }

  Header* header = parse_header(header_line);

  // printf("header name idx: %i\n", header->tweeter_idx);
  

  // printf("\n---- ENTRIES ----\n");

  /*
   * Counting
   */

  TotalCounts * counts = malloc(sizeof(TotalCounts));
  if (counts == NULL) {
    //todo
    return 1;
  }

  char * entry_row;
  while ( (entry_row = strsep(&buffer, "\n")) != NULL ) {
    parse_row(entry_row, counts);
  }

  sort_tweet_counts(counts);
  print_tweet_counts(counts);



  return 0;
}