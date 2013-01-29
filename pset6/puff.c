/****************************************************************************
 * puff.c                                                                   *
 *                                                                          *
 * Uncompresses a text file that has been compressed by a Huffman algorithm *
 *                                                                          *    
 * Max Lu                                                                   *
 ****************************************************************************/
 
#include "huffile.h"
#include "forest.h"
#include "tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//function prototypes
int sum(int array[]);

int main(int argc, char* argv[])
{
    // checks user input for correct usage
    if (argc != 3)
    {
        printf("Usage: ./puff input output\n");
        return 1;
    }
    
    // opens up input file and stores as Huffile
    Huffile* input = hfopen(argv[1],"r");
    if (input == NULL)
    {
        printf("Could not open input file %s\n",argv[1]);
        return 2;
    }
        
    // stores file header and checks to see its a huffed file    
    Huffeader header;
    if (!hread(&header, input)&&(header.magic!=MAGIC)&&(header.checksum != sum(header.frequencies)))
    {
        printf("Input file not valid huffed file\n");
        hfclose(input);
        return 1;
    }
    
    // declares and initializes a forest
    Forest* newforest = mkforest();
    if (newforest == NULL)
    {
        hfclose(input);
        printf("Could not allocate enough space\n");
        return 1;
    }
    
    // fills forests with symbols used in text file
    for (int i = 0; i < SYMBOLS; i++)
    {
        // only include symbols with nonzero frequencies
        if (header.frequencies[i] != 0)
        {
            // declares and initializes temporary tree for each symbol
            Tree* temptree = mktree();
            if (temptree == NULL)
            {
                printf("Could not allocate enough space\n");
                hfclose(input);
                if(!rmforest(newforest))
                    printf("Also could not remove forest\n");

                return 1;
            }
            
            // sets temptree to take frequency and character of each symbol
            temptree->frequency = header.frequencies[i];
            temptree->symbol = i;
            
            // plants tree in forest, exits if unsuccessful
            if (!plant(newforest,temptree))
            {   
                printf("Error while planting tree\n"); 
                hfclose(input);
                rmtree(temptree);
                if(!rmforest(newforest))
                    printf("Also could not remove forest\n");
                return 1;
            }
        }
    }
    
    // builds tree 
    Tree* lower;
    // while condition picks tree from forest and stores in lower, breaks if no more trees
    while((lower=pick(newforest))!= NULL)
    {
        // picks second lowest tree from forest, but if forest is empty, break out of loop
        Tree* higher=pick(newforest);
        if (higher==NULL)
            break;
        // declare parent tree and set its left and right children to lower and higher
        Tree* parent = mktree();
        parent->left = lower;
        parent->right = higher;
        // adds frequency of children and sets as frequency of parent
        parent->frequency = lower->frequency + higher->frequency;
        // plants tree into forest, "replaces" original lower and higher tree. exits program on error
        if (!plant(newforest,parent))
        {    
            printf("Error while planting tree\n");
            hfclose(input);
            rmtree(lower);
            rmtree(higher);
            if(!rmforest(newforest))
                printf("Also could not remove forest\n");
            return 1;
        }
    }
    
    // opens output file, exits program if unsuccessful
    FILE* output = fopen(argv[2],"w");
    if (output == NULL)
    {
        printf("Could not open file %s\n",argv[2]);
        hfclose(input);
        rmtree(lower);
        if(!rmforest(newforest))
            printf("Also could not remove forest\n");
        return 1;
    }
    
    // iterates over each bit in input file
    int bit;
    Tree* htree = lower;
    while ((bit = bread(input)) != EOF)
    {
        // goes to left child of tree if bit is 0
        if (bit == 0)
            htree = htree->left;
        // goes to right child of tree if bit is 0
        else if(bit == 1)
            htree = htree->right;
        // if tree has no children, we are at a leaf
        if (htree->left == NULL && htree->right == NULL)
        {
            fprintf(output, "%c", htree->symbol);
            htree = lower;
        }
     
    }
    
    // frees memory
    hfclose(input);
    fclose(output);
    rmtree(lower);
    if(!rmforest(newforest))
    {
        printf("Could not remove forest\n");
        return 1;
    }
    return 0;
}

/**
 * Returns sum of each element in an integer array
 */
int sum(int array[])
{
    int counter = 0;
    for (int i = 0; i < SYMBOLS; i++)
        counter += array[i];
    return counter;
}

