#include "markov_chain.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define PARAMETERS_NUM_ERROR "Usage: parameter num 3 or 4 only.\n"
#define FILE_PATH_INVALID "Error: Given file path is invalid.\n"
#define MIN_ARGC 4
#define MAX_ARGC 5
#define ARG_FILE_PATH 3
#define ARG_WORDS_TO_READ 4
#define MAX_LINE_LENGTH 1002
#define FILL_SUCCESS 0
#define FILL_FAILED 1
#define DEFAULT_VALUE 0
#define DOT '.'
#define SPACE " \n"
#define SEED 1
#define MAX_TWEETS_ARG 2
#define TWEET_MAX_LENGTH 20
#define BASE 10
#define RUN_TILL_END -1

int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain);

void chain_initializing(MarkovChain *chain)
{
  chain->database->first = NULL;
  chain->database->last = NULL;
  chain->database->size = DEFAULT_VALUE;
}

int main(int argc, char *argv[]){
  if((argc < MIN_ARGC)||(argc > MAX_ARGC))
  {
    printf(PARAMETERS_NUM_ERROR);
    return EXIT_FAILURE;
  }
  FILE *txt_file;
  txt_file = fopen (argv[ARG_FILE_PATH], "r");
  if(!txt_file) //if file opening failed
  {
    printf(FILE_PATH_INVALID);
    fclose (txt_file);
    return EXIT_FAILURE;
  }
  MarkovChain *chain = malloc (sizeof(MarkovChain));
  if(!chain){
    return EXIT_FAILURE;
  }
  chain->database = malloc (sizeof (LinkedList));
  if(!chain->database) //if allocation of MarkovChain failed
  {
    free(chain);
    return EXIT_FAILURE;
  }
  chain_initializing(chain);
  int num_words_to_read = DEFAULT_VALUE;
  if(argc == MAX_ARGC){
    num_words_to_read = strtol (argv[ARG_WORDS_TO_READ], NULL, BASE);
  }
  else{
    num_words_to_read = RUN_TILL_END;
  }
  int fill_status = FILL_FAILED;
  fill_status = fill_database (txt_file, num_words_to_read, chain);
  fclose (txt_file);
  if(fill_status == FILL_FAILED){
    free_database (&chain);
    return EXIT_FAILURE;
  }
  int seed = strtol(argv[SEED], NULL, BASE);
  srand (seed);
  int max_tweets = strtol(argv[MAX_TWEETS_ARG], NULL, BASE);
  for(int i=0; i<max_tweets;i++){
    MarkovNode *first_rand_node = get_first_random_node (chain);
    printf ("Tweet %d: ", i+1);
    generate_tweet (chain, first_rand_node, TWEET_MAX_LENGTH);
    printf ("\n");
  }
  free_database (&chain);
}

/**
This function takes in a file pointer, an integer representing the number of
 words to read, and a pointer to a Markov chain data structure. It reads the
 specified number of words from the file, adds them to the Markov chain, and
 updates their frequency. The function returns either FILL_SUCCESS or
 FILL_FAILED.
 */
int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain){
  char sentence[MAX_LINE_LENGTH];
  int added_words_counter = DEFAULT_VALUE;
  Node *cur = NULL;
  Node *prev = NULL;
  char *word = NULL;
  while(fgets (sentence, MAX_LINE_LENGTH, fp)){
    word = strtok (sentence, SPACE);
    while(word != NULL){
      if(words_to_read == added_words_counter){
        return FILL_SUCCESS;
      }
      cur = add_to_database (markov_chain, word);
      if(!cur){
        return FILL_FAILED;
      }
      added_words_counter++;
      if(prev != NULL){
        if(add_node_to_frequencies_list (prev->data, cur->data) == false){
          return FILL_FAILED;
        }
      }
      if(cur->data->last_word == true){
        prev = NULL;
      }
      prev = cur;
      word = strtok (NULL, SPACE);
    }
  }
  return FILL_SUCCESS;
}