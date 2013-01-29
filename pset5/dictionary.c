/****************************************************************************
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 *
 * Max Lu
 
 ***************************************************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "dictionary.h"


// Declares a node struct that represents a trie
typedef struct node
{
    bool isWord;
    struct node* children[27];
}
node;

// Global variables store a root trie (dictionary) and counter (size of dictionary)
node* root;
unsigned int counter = 0;

// Function prototypes 
void freeNode(node* currentNode);

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // point node to global variable root
    node* trie = root;
    
    // iterate through characters in word
    int i = 0;
    while(word[i] != '\0')
    {
        // search for character in its appropriate bin
        int childrenIndex = word[i] == '\''? 26: tolower(word[i])- 'a';
        // pointer to character has been found, now move to next level of trie
        if (trie != NULL)
            trie = trie->children[childrenIndex];
        i++;
    }
    // if pointer does not exist
    if (trie == NULL)
        return false;
    // else return isWord of the last pointer
    return trie->isWord;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // initialize a root
    root = malloc(sizeof(node));
    // if malloc fails, then return false    
    if (root== NULL)
        return false;
    
    // open dictionary file, return false if failed
    FILE* fp = fopen(dictionary, "r");
    if (fp == NULL)
        return false;
    
    // declare buffer word of length LENGTH + 2 to store value of each line in dictionary
    char word[LENGTH + 2];
    
    // while we have not reached the end of the file
    while (fgets(word, LENGTH + 2, fp) != NULL)
    {
        // declare a pointer
        node* trie = root;
        
        // iterate over each character in word
        int i = 0;
        while (word[i] != '\n')
        {
            // give each character a corresponding position
            int childrenIndex = (word[i] == '\'') ? 26: tolower(word[i])- 'a';
            // don't overwrite any old indices
            if (trie->children[childrenIndex] == NULL)
                // allocate memory for new node
                trie->children[childrenIndex] = malloc(sizeof(node));
            // update trie pointer
            trie = trie->children[childrenIndex];
            i++;
            
        }
        // set isWord to true so check recognizes there is word there
        trie->isWord = true;
        // update global to get size of dictionary
        counter++;
    }
    fclose(fp);
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    // counter is global storing number of words added in load
    return counter;
    
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    // calls recursive function
    freeNode(root);
    return true;
}

/*
 * Recursive function to free nodes to prevent leaks by freeing lowest level of trie first
 */
void freeNode(node* currentNode)
{
    // for every child
    for (int i = 0; i < 27; i++)
    {
        // calls freenode on children if they are not null
        if (currentNode->children[i] != NULL)
            freeNode(currentNode->children[i]);
    }
    free(currentNode);
}
