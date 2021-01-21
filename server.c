#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include<openssl/md5.h>
#include<openssl/aes.h>
void compute_md5(char *string, unsigned char digest[16]) 
{
    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, string, strlen(string));
    MD5_Final(digest, &context);
}
int check (char *h1, char *h2)
{
    return !(strcmp(h1,h2));
}

//the thread function
void *connection_handler(void *);
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

   //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
    listen(socket_desc , 3);
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;
while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        //printf("Handler assigned for %lu\n",thread_id);
    }
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }    
    return 0;
}

 /**
 * This will handle connection for each client
 * */

void *connection_handler(void *socket_desc)
{
    char key[] = "thisisaserverkey";
    char md_hash[1024];
    char hash[16];
    AES_KEY enc_key;
    AES_KEY dec_key;

    //Encrypted and Decrypted messege holders
    unsigned char dec_out[80];
    unsigned char enc_out[80];

    
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size,n;
    char *message , user[10], client_message[2000];
    message="Enter username> ";
    write(sock, message, 1024);
    read(sock,user, (sizeof(user)));
    printf("%s is Connected\n", user );
    //Receive a message from client
    while((read_size=read(sock,enc_out,80))>0)
    { 
        int n=1;
        //Server encryption
        
        AES_set_encrypt_key(key, 128, &enc_key);
        //Client decrption
        
        AES_set_decrypt_key(key, 128, &dec_key);
        while(n>0)
        {
            read(sock,md_hash,16);
            AES_decrypt(enc_out, dec_out, &dec_key);
            compute_md5(dec_out, hash);
            n--;
        }
        if(!(strncmp(md_hash,hash,7)))
            printf("%s: %s\n",user,dec_out);
        printf("Reply %s> ",user);
        scanf("%s",client_message);
        compute_md5(client_message,md_hash);
        AES_encrypt(client_message, enc_out, &enc_key); 
        printf("Encrypted: %s\n",enc_out); 
        write(sock, enc_out, 1024);
        write(sock, md_hash, 1024);  
        bzero(client_message,2000);  
    }
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    return 0;
}
