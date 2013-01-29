/****************************************************************************
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 *
 * Max Lu
 ***************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// declares BYTE type to represent size of a byte in memory
typedef uint8_t  BYTE;

int main(void)
{
    // opens file card.raw which contains raw image data and reads it
    FILE* fp = fopen("card.raw", "r");
    if (fp == NULL)
    {
        printf("Could not open");
        return 1;
    }
    
    // declares string of 8 letters to store name of output file
    char name[8];
    // counts number of images extracted
    int count = 0;
    // declares file pointer that will store each extracted image
    FILE* image;
    // decalres an array of 512 bytes that will store a 512 byte block of image data
    BYTE bytes[512];
    
    
    // sets pointer to last byte in file
    fseek(fp, 0, SEEK_END);
    // finds the "size-1" of the file in bytes
    int size = ftell(fp);
    // resets pointer to beginning of file
    fseek(fp, 0, SEEK_SET);
   
    // iterate through each block in raw image file
    for(int i= 0; i < size; i=i+512)
    {
        // sets bytes equal to the ith block of data
        fread(bytes, sizeof(BYTE),512,fp);
        // if first four bytes match jpeg header
        if ((bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff && bytes[3] == 0xe0)||(bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff && bytes[3] == 0xe1))
        {
            // closes previous image (if there is one) to avoid memory leaks
            if (count > 0)
                fclose(image);
            
            // sets and formats name to the desired style
            sprintf(name,"%.3d.jpg",count);
            count++;
            // creates a file called name
            image = fopen(name, "w");
            if (image == NULL)
            {
                printf("Could not open");
                // closes file to prevent memory leak
                fclose(fp);
                return 2;
            }
            // writes 512 byte to the file
            fwrite(bytes,sizeof(BYTE),512,image);
            
        } 
        // if the next block is not a new image, and we have already found at least one image, 
        // than it is part of the same image
        else if (count > 0)
            fwrite(bytes,sizeof(BYTE),512,image); 
    }
    // close file to prevent memory leaks
    fclose(fp);
    return 0;
}

