#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname; // To store the src image name
    FILE *fptr_src_image;  // To store the address of the src image
    

    /* Secret File Info */
    char secret_fname[50];       // To store the secret file name
    FILE *fptr_secret;        // To store the secret file address
    int extn_length;          //to store extension length
    char extn_secret_file[5]; // To store the Secret file extension
    char secret_data[100];    // To store the secret data
    long size_secret_file;    // To store the size of the secret data

    /* Stego Image Info */
    char *stego_image_fname; // To store the dest file name
    FILE *fptr_stego_image;  // To store the address of stego image

} DecodeInfo;

/* decoding function prototype */

/* Read and validate Decode args from argv */
Status Read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status Open_files(DecodeInfo *decInfo);


/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status decode_magic_string( DecodeInfo *decInfo);

/*Decode extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn( DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_byte_to_lsb(char * data, char *image_buffer);

// Decode a size to lsb
Status decode_size_to_lsb(int *size, char *imageBuffer);



#endif
