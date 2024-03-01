#include "markov_chain.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define EQUEL 0
#define FIRST 0
#define INITIAL 1
#define EMPTY 0
#define DEFAULT_VALUE 0
#define FAILED 1
#define DOT '.'



int get_random_number(int max_number){
  return rand() % max_number;
}

/**
Description:
The function takes a MarkovChain struct pointer and a char
pointer as input arguments. It searches through the linked list in the
MarkovChain's database for a node that matches the given char pointer.
If it finds a match, it returns a pointer to the matching node.
If it does not find a match, it creates a new node with the given char
pointer as data and appends it to the end of the linked list in the database.
Finally, the function returns a pointer to the newly added node.
Parameters:
A pointer to a MarkovChain struct named "markov_chain"
A char pointer named "data_ptr"
Returns:
A pointer to a Node struct.
 */

Node* add_to_database(MarkovChain *markov_chain, char *data_ptr){
  Node *cur = get_node_from_database (markov_chain, data_ptr);
  if(cur!=NULL){
    return cur;
  }
  MarkovNode *new_markov_node = malloc (sizeof(MarkovNode));
  if(!new_markov_node){
    return NULL;
  }
  bool last_word_c = false;
  if(data_ptr[strlen (data_ptr)-1] == DOT){
    last_word_c = true;
  }
  new_markov_node->data = malloc (sizeof (char) * (strlen (data_ptr)+1));
  if(!new_markov_node->data){
    return NULL;
  }
  strcpy (new_markov_node->data, data_ptr);
  new_markov_node->frequencies_list = NULL;
  new_markov_node->frequencies_list_length = DEFAULT_VALUE;
  new_markov_node->total_frequencies = DEFAULT_VALUE;
  new_markov_node->last_word = last_word_c;
  int check = add (markov_chain->database, new_markov_node);
  if(check == FAILED){
    free (new_markov_node->data);
    free (new_markov_node);
    return NULL;
  }
  markov_chain->database->last->next = NULL;
  return markov_chain->database->last;
}

/**
Description:
The "get_node_from_database" function takes a MarkovChain struct pointer and
 a char pointer as input arguments. It searches through the linked list in
 the MarkovChain's database for a node that matches the given char pointer.
 If it finds a match, it returns a pointer to the matching node. If it does
 not find a match, it returns a NULL pointer.
Parameters:
A pointer to a MarkovChain struct named "markov_chain"
A char pointer named "data_ptr"
Returns:
A pointer to a Node struct or NULL.
 */

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr){
  Node *cur = markov_chain->database->first;
  while(cur){
    if(strcmp (cur->data->data, data_ptr) == EQUEL){
      return cur;
    }
    cur = cur->next;
  }
  return NULL;
}

/**
Description:
The "add_node_to_frequencies_list" function takes two parameters, both of type
 MarkovNode pointer. It first checks if the second node already exists in the
 first node's "frequencies_list" array. If it does, the function increments
 the frequency value of the corresponding MarkovNodeFrequency struct and
 returns true. If it does not exist, the function adds the second node to
 the end of the "frequencies_list" array and returns true. If memory
 allocation fails, the function returns false.
Parameters:
A pointer to a MarkovNode struct named "first_node"
A pointer to a MarkovNode struct named "second_node"
Returns:
A boolean value: true if the operation was successful, false otherwise.
 */

bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
*second_node){
  if(first_node->frequencies_list_length == DEFAULT_VALUE)
  {
    first_node->frequencies_list = malloc (sizeof(MarkovNodeFrequency));
    if(!first_node->frequencies_list){
      return NULL;
    }
    first_node->frequencies_list_length++;
    first_node->total_frequencies++;
    first_node->frequencies_list[FIRST].frequency = INITIAL;
    first_node->frequencies_list[FIRST].markov_node = second_node;
    return true;
  }
  for(int i=0; i<first_node->frequencies_list_length; i++)//if second_node
    // is already exits in first_node's list
  {
    if(first_node->frequencies_list[i].markov_node==second_node){
      first_node->frequencies_list[i].frequency++;
      first_node->total_frequencies++;
      return true;
    }
  }
  first_node->frequencies_list_length++;
  first_node->total_frequencies++;
  MarkovNodeFrequency *frq_lst = first_node->frequencies_list;
  int the_size = sizeof (MarkovNodeFrequency) *
      (first_node->frequencies_list_length);
  MarkovNodeFrequency *new_frq_lst = realloc (frq_lst,the_size);
  if(!(first_node->frequencies_list)){
    return false;
  }
  first_node->frequencies_list = new_frq_lst;
  first_node->frequencies_list[(first_node->frequencies_list_length)-1]
  .frequency = INITIAL;
  first_node->frequencies_list[(first_node->frequencies_list_length)-1]
  .markov_node = second_node;
  return true;
}

/**
Description:
The function gets a pointer to pointer to markovchain and free all its memory.
Parameters:
A pointer to pointer to MarkovChain.
Returns: None.
 */

void free_database(MarkovChain ** ptr_chain){
  Node *cur_1 = (*ptr_chain)->database->first;
  Node *cur_2 = NULL;
  while(cur_1){
    free(cur_1->data->frequencies_list);//release MNF
    free (cur_1->data->data);
    free(cur_1->data); // release MarkovNodes
    cur_2 = cur_1->next;
    free (cur_1); //release Node
    cur_1 = cur_2;
    }
  free ((*ptr_chain)->database); //release LinkedList
  free (*ptr_chain); // release MarkovChain
  *ptr_chain = NULL;
  }


MarkovNode* get_first_random_node(MarkovChain *markov_chain){
  int chain_length = markov_chain->database->size;
  Node *chosen_node = NULL;
  int chosen_number = INITIAL;
  bool is_last_word = true;
  while(is_last_word)
  {
    chosen_number = get_random_number (chain_length);
    chosen_node = markov_chain->database->first;
    for(int i=0; i<chosen_number; i++){
      chosen_node = chosen_node->next;
    }
    if(chosen_node->data->last_word == false){
      is_last_word = false;
    }
  }
  return chosen_node->data;
}

MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr){
  int freq_lst_length = state_struct_ptr->total_frequencies;
  int random_number = get_random_number (freq_lst_length);
  int counter = DEFAULT_VALUE;
  for(int i=0; i<freq_lst_length; i++){
    counter += state_struct_ptr->frequencies_list[i].frequency;
    if(counter > random_number){
      return state_struct_ptr->frequencies_list[i].markov_node;
    }
  }
  return NULL;
}

void generate_tweet(MarkovChain *markov_chain, MarkovNode *first_node, int
max_length){
  if(max_length == DEFAULT_VALUE){
    return;
  }
  while(!first_node){
    first_node = get_first_random_node (markov_chain);
  }
  printf("%s ", first_node->data);
  MarkovNode *cur = first_node;
  int counter = INITIAL;
  while(cur->last_word==false){
    if(counter == max_length){
      return;
    }
    cur = get_next_random_node (cur);
    if(cur->last_word){
      printf("%s", cur->data);
    }
    else{
      printf("%s ", cur->data);
    }
    counter++;
  }
}