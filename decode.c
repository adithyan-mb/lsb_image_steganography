#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>

 //check file extension is bmp or not
Status Read_and_validate_decode_args(char *argv[],DecodeInfo *dec_info)
{

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
                                return e_failure;
                        dec_info->stego_image_fname=argv[2];




 //if user provides a filename, copy that to secret_fname
                    if(argv[3]!=NULL)
                    {
                            strcpy(dec_info->secret_fname,strtok(argv[3],"."));
                          //  printf("secret_fname %s",dec_info->secret_fname);                     
                            return e_success;
                    }
                    else
                        strcpy(dec_info->secret_fname,"output");           // if not provided , use "output"  as defualt name 
                     
                        return e_success;

}

 //opening stegno image 
Status Open_files(DecodeInfo *decInfo)
{
             decInfo->fptr_stego_image =fopen(decInfo->stego_image_fname,"r");
            if(decInfo->fptr_stego_image==NULL)
            {
                printf("secret file cannot be opened");
                return e_failure;                         
            }
            else
              return  e_success;
}

  //buffer to store MAGIC string from encoded image
Status decode_magic_string( DecodeInfo *decInfo)
{
        char imagebuffer[8];
        char databuffer[3]={0};           
         for(int j=0;j<strlen(MAGIC_STRING);j++)
          {
                fread(imagebuffer,8,1,decInfo->fptr_stego_image);
                decode_byte_to_lsb(&databuffer[j],imagebuffer);

          }

       //checking if the MAGIC STRING that we have and from enoded file are same   
        if(!strcmp(databuffer,MAGIC_STRING))
          return e_success;                            
          else                                
               return  e_failure;

}
Status decode_secret_file_extn_size(DecodeInfo* decInfo)
{
    char imagebuffer[32];

      int extn_size=0;   
    fread(imagebuffer,32,1,decInfo->fptr_stego_image);                         //getting file extension length size from encoded file 
      decode_size_to_lsb(&extn_size,imagebuffer);

      decInfo->extn_length=extn_size;                      //storing it to   decInfo->extn_length
                return e_success;
        
       
  
}
Status  decode_secret_file_extn( DecodeInfo *decInfo)
{
 
         char extension[decInfo->extn_length+1];

        char imagebuffer[8];
                                                             //getting file extension  from encoded file
        for(int i=0;i<decInfo->extn_length;i++)
        {

             fread(imagebuffer,8,1,decInfo->fptr_stego_image);
               decode_byte_to_lsb(&extension[i],imagebuffer);

        }
          extension[decInfo->extn_length]='\0';

         strcat(decInfo->secret_fname,extension);         //storing it to  full file name (with extension)
                    return e_success;
           


}

Status decode_secret_file_size(DecodeInfo *decInfo)
{

     char imagebuffer[32];

      int size=0;   
    fread(imagebuffer,32,1,decInfo->fptr_stego_image);     //getting file data file  size
      decode_size_to_lsb(&size,imagebuffer);

      decInfo->size_secret_file=size;
                return e_success;                     
}

Status decode_secret_file_data(DecodeInfo * decInfo)
{
    char imagebuffer[8];
    
          //getting file data from enoded file and write to secret_file 
         for(int j=0;j<decInfo->size_secret_file;j++)
          {
                 char data=0;
                fread(imagebuffer,8,1,decInfo->fptr_stego_image);
                decode_byte_to_lsb(&data,imagebuffer);
                fputc(data,decInfo->fptr_secret);

          }

                    return e_success;

}

Status decode_byte_to_lsb(char* data,char imagebuffer[])
{
     *data=0;
             for(int i=0;i<=7;i++)                                // decode the byte
                *data=*data | (imagebuffer[i]&1)<<(7-i) ;   
                          return e_success; 
}

Status decode_size_to_lsb(int * extn_size,char imagebuffer[])
{
     for(int i=0;i<=31;i++)
                *extn_size=*extn_size | (imagebuffer[i]&1)<<(31-i) ;    //decode the size 
                          return e_success;
}


Status do_decoding(DecodeInfo *dec_info)                     // all function calls related to decoding file 
{  if(Open_files(dec_info))
    {
                // skip header
                 fseek(dec_info->fptr_stego_image,54,SEEK_SET);
                if( decode_magic_string(dec_info) )
                 {

                    if(decode_secret_file_extn_size(dec_info))
                    {

                          if(  decode_secret_file_extn(dec_info))
                            {
                                          if(decode_secret_file_size(dec_info))
                                          {
                                                dec_info->fptr_secret =fopen(dec_info->secret_fname,"w");
                                                     
                                                 if(decode_secret_file_data(dec_info))
                                                 {
                                                        fclose(dec_info->fptr_stego_image);
                                                        fclose(dec_info->fptr_secret);
                                                 }

                                          } 
                                       
                            }
                    }
                 }
                  return   e_success;
    }
 
  else
  return e_failure;
}