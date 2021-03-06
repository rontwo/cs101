
/**
This program reads a block of memory and pulls out all JPG files. This is intended to be a forensic tool that allows the user to recover 
JPGs from raw memory. It assumes the following:
1) the first three bytes of every JPG start with 0xff/0xd8/0xff
2) the fourth bye of every JPG can range from 0xe0 to 0xef
3) JPGs are written onto memory in contiguous blocks of size 512 bytes, so that you can iterate over each block of 512bytes
(i.e. JPGs can be found on the byte 512, 1024, 1536, etc...)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

//Define block as 512 bytes
#define BYTE uint8_t
#define BLOCK 512

int main(int argc, char *argv[])
{
    //ensure proper usage
    if(argc != 2)
    {
        fprintf(stderr, "Usage error: Open file for recovery\n");
        return 1;
    }

    //Check that file is opened sucessfully
    FILE *inptr = fopen(argv[1],"r");
    if(inptr == NULL)
    {
        fprintf(stderr, "Could not open %s\n", argv[1]);
        return 2;
    }

    //Declare function to open & write new file and counter for jpgs
    FILE *outptr = NULL;
    int jpgcount = 0;

    //Declare jpg checks
    bool in_jpg = false;

    //Declare temp variable storage and read infile
    BYTE buffer[BLOCK];

    //While loop to run until ptr is at EOF
    while(fread(buffer, BLOCK, 1, inptr) > 0)
    {
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
            {
                //If already in JPG, close outfile;
                if(in_jpg)
                {
                    fclose(outptr);
                }

                //Set in_jpg to true
                in_jpg = true;

                //Name new file
                char newfile[8];
                //Name new output file
                jpgcount++;
                sprintf(newfile,"%03i.JPG", jpgcount);

                //Create and open new outfile
                outptr = fopen(newfile, "w");
                if(outptr == NULL)
                {
                    fclose(inptr);
                    fprintf(stderr, "Could not create %s\n", newfile);
                    return 3;
                }
                //Write current blcok to outfile
                fwrite(buffer,BLOCK,1,outptr);
            }

        //Check to see if already in jpg; if so, write current block to outfile
        else if(in_jpg)
            {
                fwrite(buffer,BLOCK,1,outptr);
            }
    }
    //Free buffer after while loop exits
    free(buffer);

    //Close input and output file after last
    fclose(inptr);
    fclose(outptr);

    //Woohoo!
    return 0;
}
