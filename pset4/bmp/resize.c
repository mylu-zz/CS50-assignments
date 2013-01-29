/****************************************************************************
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Resizes an image by a factor
 *
 * Max Lu
 ***************************************************************************/
       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: resize scale infile outfile\n");
        return 1;
    }

    // remember filename
    char* infile = argv[2];
    char* outfile = argv[3];

    //checks to see user inputted an int into scale
    for (int i = 0,n = strlen(argv[1]); i < n; i++)
        if (isdigit(argv[1][i])==0)
        {
            printf("scale must be integer in [1,100]\n");
            return 2;
        }
    //sets scale to 1st command line parameter
    int scale = atoi(argv[1]);
    //checks to see if scale in appropriate range
    if (scale < 1 || scale > 100)
    {
        printf("scale must be integer in [1,100]\n");
        return 2;
    }

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }
    
    // stores dimensions and padding of input image
    int inputWidth = bi.biWidth;
    int inputHeight = bi.biHeight;
    int inputPadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    //change dimensions and padding of output image
    bi.biHeight = bi.biHeight*scale;
    bi.biWidth = bi.biWidth*scale;
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = abs((bi.biWidth*sizeof(RGBTRIPLE) + padding) * bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(inputHeight); i < biHeight; i++)
    {   
        // iterates over each scanline a scale number of times (scale up height)
        for (int j = 0; j < scale; j++)
        {
            // iterate over pixels in scanline
            for (int k = 0; k < inputWidth; k++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile scale number of times (scale up width)
                for(int l = 0; l < scale; l++)
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }
            // adds correct padding to output image
            for (int m = 0; m < padding; m++)
                fputc(0x00, outptr);
            
            //goes back to the beginning position of input image's scanline so it can write it scale number of times
            fseek(inptr, -1*(inputWidth*sizeof(RGBTRIPLE)), SEEK_CUR);
        }
        //goes to the next scanline (row) of input image
        fseek(inptr,inputWidth*sizeof(RGBTRIPLE)+inputPadding, SEEK_CUR);

    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
