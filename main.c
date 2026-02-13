/* name: Adithyan mb (25013_008)
   date: 29/08/25
   title: LSB IMAGE STEGANOGRAPHY project
*/


#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "decode.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
   // checking if there are  correct number of arguments passed through CLA
  if(argc<3)
  {
     printf("invalid number of arguments \n");
     return e_failure;
  }
     
  //Encoding Code 
      if(argc>=4 && check_operation_type(argv[1])==e_encode)
      {
         EncodeInfo enc_info;
         if( read_and_validate_encode_args(argv,&enc_info))
         {
             printf("\n------------------------------Starting Encoding---------------------- \n");
            if( do_encoding(&enc_info))
            {
                printf("\n Details : \n");
                printf("\n Operation type : %s\n","Encoding");
                printf("\n Source File name : %s\n",enc_info.src_image_fname);
                printf("\n Secret file name : %s\n",enc_info.secret_fname);
                printf("\n Encoded file name : %s\n",enc_info.stego_image_fname);

               printf("\n \n ***************************   ENCODED   ***************************\n");
            }
            else
            {
               printf("\n \n XXXXXXXXXXXXXXXXXXXXXXXXXX   NOT ENCODED   XXXXXXXXXXXXXXXXXXXXXXXXXXn");
            }
         }
         

      }  
      //if operation is invalid 
      else if(check_operation_type(argv[1])==e_unsupported)
       {
         printf(" INVALID OPERATION , EXITING \n");
            return 0;
       }

    //Decoding  code
    else if(  argc>=3 && check_operation_type(argv[1])==e_decode)
    {     
       
        DecodeInfo dec_info;
       if( Read_and_validate_decode_args(argv,&dec_info))
       {     
           printf("\n------------------------------Starting Decoding---------------------- \n"); 
          if( do_decoding(&dec_info))
          {
             printf("\n Details : \n");
                printf("\n Operation type : %s\n","Decoding");
                printf("\n Encoded file name : %s\n",dec_info.stego_image_fname);
                printf("\n Secret file name : %s\n",dec_info.secret_fname);
             
             printf("\n \n ***************************   DECODED   ***************************\n");
          }
          else
          {
             printf("\n \n XXXXXXXXXXXXXXXXXXXXXXXXXX   NOT DECODED   XXXXXXXXXXXXXXXXXXXXXXXXXXn");
          }
       }
       else printf("validation failed");
        

    }  
     //if operation is invalid 
     else if(check_operation_type(argv[1])==e_unsupported)
       {
         printf(" INVALID OPERATION , EXITING \n");
            return 0;
       }
        
}


//check if the operation given through CLS is a valid operation 
OperationType check_operation_type(char *symbol)
{
       if(!strcmp("-e",symbol))
           return e_encode;

           else if (!strcmp("-d",symbol))                      //check if the operation is encding, decoding or unsupported
               return e_decode;

               else
                return e_unsupported;
   
}
