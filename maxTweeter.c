#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * TWEETER_COL_NAME = "name";

// typedef struct {
//   char ** col_names;
//   int col_num;
//   int tweeter_idx;
// } Header;

typedef struct {
  char * name;
  int n;
} SingleTweeter;

typedef struct {
  SingleTweeter * tweeter[20000];
  int size;
} AllTweeters;


void throw_invalid_input()
{
  printf("Invalid Input Format\n");
  exit(1);
}

void remove_quotes(char ** str, int len)
{
  if ((*str)[0] == '"') {
    if ((*str)[len-1] != '"') {
      throw_invalid_input();
    }

    (*str)[len-1] = '\0';
    (*str) = (*str) + 1;
    return;
  }

  if ((*str)[len-1] == '"') {
    if ((*str)[0] != '"') {
      throw_invalid_input();
    }
    
    (*str)[len-1] = '\0';
    (*str) = (*str) + 1;

    return;
  }
}

void print_tweet_counts(AllTweeters * all_tweeters)
{
  int i;
  for (i=0; i<all_tweeters->size && i < 10; i++) {
    SingleTweeter * single_tweeter = all_tweeters->tweeter[i];
    printf("%s: %i\n", single_tweeter->name, single_tweeter->n);
  }
}

void sort_tweet_counts(AllTweeters * all_tweeters) //Bubble sort
{
  int i;
  for (i=0; i<all_tweeters->size; i++) {
    int j;
    for (j=0; j<all_tweeters->size-i; j++) {
        int a = all_tweeters->tweeter[i]->n;
        int b = all_tweeters->tweeter[j]->n;
        if (a >= b) {
            SingleTweeter * hold = all_tweeters->tweeter[i];
            all_tweeters->tweeter[i] = all_tweeters->tweeter[j];
            all_tweeters->tweeter[j] = hold; 
        }
    }
  }
}


int find_tweeter(AllTweeters* tweetCounts, char* name)
{
  if(tweetCounts == NULL || name == NULL){
    printf("find_tweeter @ Null Argument\n");
    return -1;
  }

  int i = 0; 
  int idx = -1; 

  for(i = 0; i < tweetCounts->size; i++){
    if(strcmp(name, tweetCounts->tweeter[i]->name) == 0){
        idx = i;
        break; 
    }
  }

  return idx;
}

void * parse_header(char ** line, int * ret_name_idx)
{
  int name_idx_match = -1;

  char * col_name = NULL;
  int name_found = 0; 
  int curr_header_idx = 0;

  while( (col_name = strsep(&line, ",")) != NULL ) {

    int len = strlen(col_name);
    
    remove_quotes(&col_name, len);

    
    if (strcmp(col_name, TWEETER_COL_NAME) == 0) {
      // printf("found name @ %i\n", i);

      if(name_found == 0){
        name_idx_match = curr_header_idx;
        name_found = 1; 
      } else {
        throw_invalid_input();
      }
    }

    curr_header_idx++;
  }

  if (name_idx_match == -1) {
    printf("Name col not found\n");
    exit(1);
  }

  *ret_name_idx = name_idx_match;
}

void parse_row(char * line, AllTweeters * tweet_counts, int name_idx)
{
  char * col_data = NULL;

  int col_idx = 0;
  while( (col_data = strsep(&line, ",")) != NULL ) {

    if (col_idx != name_idx) { // Ignore all cols except name col
      col_idx++;
      continue;
    }

    char * tweeter = col_data;

    int len = strlen(tweeter);

    remove_quotes(&tweeter, len);

    int match_idx = find_tweeter(tweet_counts, tweeter);

    if (match_idx == -1) {
      // Not Found
      int last = tweet_counts->size;
      SingleTweeter * new_tweeter_count = malloc(sizeof(SingleTweeter));
      if (new_tweeter_count == NULL) {
        printf("Malloc fail\n"); 
        exit(1);
      }

      int name_len = strlen(tweeter);
      char * tweeter_name = malloc(name_len + 1);
      if (tweeter_name == NULL) {
        printf("Malloc fail\n"); 
        exit(1);
      }

      memcpy(tweeter_name, tweeter, name_len+1);

      new_tweeter_count->name = tweeter_name;
      new_tweeter_count->n = 1;

      tweet_counts->tweeter[last] = new_tweeter_count;

      tweet_counts->size = tweet_counts->size + 1;
    } else {
      int prev_count = tweet_counts->tweeter[match_idx]->n;
      tweet_counts->tweeter[match_idx]->n = prev_count + 1;
      }

    col_idx++;
  }

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

  // printf("filename: %s\n\n", filename);

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

  /*
   * Parse Header
   */


  char * header_line;
  header_line = strsep(&buffer, "\n");
  if (header_line == NULL) {
    printf("Buffer Parse failed\n"); 
    return 1;
  }

  int name_idx = -1;
  parse_header(header_line, &name_idx);

  /*
   * Counting
   */

  AllTweeters * counts = malloc(sizeof(AllTweeters));
  if (counts == NULL) {
    printf("Malloc fail\n"); 
    return 1;
  }

  char * entry_row;
  while ( (entry_row = strsep(&buffer, "\n")) != NULL ) {
    parse_row(entry_row, counts, name_idx);
  }

  sort_tweet_counts(counts);
  print_tweet_counts(counts);



  return 0;
}