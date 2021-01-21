#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include<openssl/aes.h>
#include<openssl/md5.h>

void compute_md5(char *string, unsigned char digest[16]) 
{
    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, string, strlen(string));
    MD5_Final(digest, &context);
}
char key[] = "thisisaserverkey";

int main()
{
	char md_hash[1024];
	char user[10];
	char hash[16];
	AES_KEY enc_key;
	AES_KEY dec_key;
	unsigned char dec_out[80];
	unsigned char enc_out[80];

	// Connection
	int socket_desc, val,n;
	struct sockaddr_in client_addr;
	char buff[2000];
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr =  INADDR_ANY; 
	client_addr.sin_port = htons(8888);
	if(connect(socket_desc, (struct sockaddr*)&client_addr, sizeof(client_addr)) == 0)
		printf("CONNECT STATE: Connected to Server on port 8888\n");
	else
		printf("Connection to server failed !\n");
	
	read(socket_desc, buff, sizeof(buff));
	printf("%s\n",buff );
	scanf("%s",user);
	write(socket_desc,user,sizeof(user));
	
	while(1)
	{
		//Encryption
		AES_set_encrypt_key(key, 128, &enc_key);
		//Decryption
		AES_set_decrypt_key(key, 128, &dec_key);
		printf("Message> ");
		scanf("%s",buff);
		//AES Encrypt
	    AES_encrypt(buff, enc_out, &enc_key);
	    compute_md5(buff,md_hash);
	    write(socket_desc, enc_out, sizeof(enc_out));
	    write(socket_desc,md_hash,16);
	    bzero(buff,2000);
		//AES decrypt
		if(read(socket_desc,enc_out,80)>0)
		{
			int n=1;
			while(n>0)
		    {
			    read(socket_desc,md_hash,1024);
				AES_decrypt(enc_out, dec_out, &dec_key);
			    compute_md5(dec_out, hash);
				n--;
			}
         }   
        /*
    	AES_decrypt(enc_out, dec_out, &dec_key);
    	printf("Server Message: %s\n",dec_out );
        compute_md5(dec_out, hash);
       */
        if(!(strncmp(md_hash,hash,7)))
            printf("Server: %s\n",dec_out);
	}
		close(socket_desc);
		return 0;	
}
