#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
   // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
  //  printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr,0,SEEK_END);
    uint size= ftell(fptr);
        fseek(fptr,0,SEEK_SET);

        return size;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    
    //check if the source image is of .bmp format
    int src_ext_pos=0;
    int size=strlen(argv[2]);
    for(int i=0;i<size;i++)
    {
         if(argv[2][i]=='.')
            {
                src_ext_pos=i;
                break;
            }
    }

    if(src_ext_pos==0 || strcmp(argv[2]+src_ext_pos,".bmp")!= 0) 
    {
         printf("\nsource extension invalid\n");
        return e_failure;
    }
    //if valid copy to structure 
     encInfo->src_image_fname=argv[2];



      
     //check if the secret text is of .txt fromat
    int secret_ext_pos=0;
     size=strlen(argv[3]);
    for(int i=0;i<size;i++)
    {
         if(argv[3][i]=='.')
            {
                secret_ext_pos=i;
                break;
            }
    }

    if(secret_ext_pos==0 && ( !strcmp(argv[3]+secret_ext_pos,".txt" )  || !strcmp(argv[3]+secret_ext_pos,".c") || !strcmp(argv[3]+secret_ext_pos,".csv")|| !strcmp(argv[3]+secret_ext_pos,".sh"))) 
    {
        printf(" secret file extension  failure");
        return e_failure;
    } 
       strcpy(encInfo->extn_secret_file,argv[3]+secret_ext_pos);
        encInfo->secret_fname=argv[3];



    //check if  destination file name is valid or not

    if(argv[4]!=NULL)
    {
                int dest_ext_pos=0;
                int size=strlen(argv[4]);
                for(int i=0;i<size;i++)
                {
                    if(argv[4][i]=='.')
                        {
                            dest_ext_pos=i;
                            break;
                        }
                }

                if(dest_ext_pos==0 || strcmp(argv[4]+dest_ext_pos,".bmp")!= 0)
                return e_failure;
                encInfo->stego_image_fname=argv[4];
    }
    else
     encInfo->stego_image_fname="default.bmp";

   return e_success;

}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    //store image capacity to structure
    encInfo->image_capacity= get_image_size_for_bmp(encInfo->fptr_src_image);
    //getting size of file who's data need to be encoded 
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity> (54+ 16 + 32 + 32 +32 + (encInfo->size_secret_file*8) )) // magic string+ extexnsion size + extension data +  secret_data 
          return e_success;
          else
          {
                 printf("Enough space not available \n");
                 return e_failure;
          }
    
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
     //copy the 54 bytes if header as it is to stegno_image
     char ImageBuffer[54];
     fseek(fptr_src_image, 0 , SEEK_SET) ;
     fread(ImageBuffer , 54,1 , fptr_src_image) ;
     fwrite(ImageBuffer, 54,1,fptr_dest_image);
     return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
      
      char imageBuffer[8];
      //get magic string length 
      int magic_string_size=strlen(magic_string);
      // run loop "string length" times 
     for(int i=0;i<magic_string_size;i++)
     {
         fread(imageBuffer,1,8,encInfo->fptr_src_image);
         encode_byte_to_lsb(magic_string[i],imageBuffer);
         fwrite(imageBuffer,1,8,encInfo->fptr_stego_image);
            
     }
     return e_success;

}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    //buffer of 32 bytes used , as it is a integer
    char imageBuffer[32];
    //read 32 bytes from source 
    fread(imageBuffer,32,1,encInfo->fptr_src_image);
         encode_size_to_lsb(size,imageBuffer);
    //write the enoded data to stego_image 
    fwrite(imageBuffer,32,1,encInfo->fptr_stego_image);
      return e_success;
   
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char imageBuffer[8];
    int size=strlen(file_extn);

    //loop run "size" times 
    for(int i=0;i<size;i++)
    {
        fread(imageBuffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],imageBuffer);
        fwrite(imageBuffer,8,1,encInfo->fptr_stego_image);
    }

    return e_success;

    
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
     char imageBuffer[32];
     //read file size, encode and write to destination 
    fread(imageBuffer,32,1,encInfo->fptr_src_image);
         encode_size_to_lsb(file_size,imageBuffer);
         fwrite(imageBuffer,32,1,encInfo->fptr_stego_image);
      return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
     //buffer with size same as the secret data of source 
     char databuffer[encInfo->size_secret_file];
      fread(databuffer,encInfo->size_secret_file,1,encInfo->fptr_secret);

       //encoding image buffer of 8 bytes 
      char imageBuffer[8];
     
      //run the loop "secert file size " times 
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        
        fread(imageBuffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(databuffer[i],imageBuffer);
        fwrite(imageBuffer,8,1,encInfo->fptr_stego_image);
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
   // get the  size of remaining source data that need to copied 
    int cur_pos=ftell(fptr_src);
    fseek(fptr_src,0,SEEK_END);
    int end_pos=ftell(fptr_src);
    long  remaining_size= end_pos-cur_pos;
 
    //move back the file pointer to last charcater extracted position 
    fseek(fptr_src,-remaining_size,SEEK_CUR);

    // copying remaining data from source to destination
     char databuffer[remaining_size];
        fread(databuffer,remaining_size,1,fptr_src);
        fwrite(databuffer,remaining_size,1,fptr_dest);
    

    return e_success;
    
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
      //to enode one byte of data
      for(int i=0;i<=7;i++)
        { 
            image_buffer[i]=image_buffer[i]&~1 | data>>(7-i)&1;

        }
   
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    //encode 32 bytes of data 
    for(int i=0;i<=31;i++)
    {
         imageBuffer[i]=imageBuffer[i]&~1 | size>>(31-i)&1;

    }
   
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{

   if( open_files(encInfo))
   {
      if( check_capacity(encInfo) )
      {  
          // printf("check capacity done \n");
              if(   copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image))
              {
            //     printf("copy bmp header done \n");
                if( encode_magic_string(MAGIC_STRING, encInfo) )
                {
              //       printf("encode magic_string done \n");
                        if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo))
                        {
                //             printf("encode_secret_file_extn_size done \n");
                            if(encode_secret_file_extn(encInfo->extn_secret_file , encInfo))
                            {
                  //               printf("encode_secret_file_extn done \n");
                                if(encode_secret_file_size(encInfo->size_secret_file, encInfo))
                                {
                    //                     printf("encode_secret_file_size done \n");
                                        if(encode_secret_file_data(encInfo))
                                        {
                      //                       printf("encode_secret_file_data done \n");
                                            if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image))
                                            {
                        //                        printf("copy_remaining_img_data done \n");
                                                     fclose(encInfo->fptr_src_image);
                                                     fclose(encInfo->fptr_stego_image);
                                                     return e_success;
                                            }
                                        }
                                }
                            }
                        }
                }
                }



      }

     
   }
    return e_failure;
   
}
