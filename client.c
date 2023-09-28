#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "common.h"
#define pAssert(a,b) {if (!(a)) {printf("Line %d in File %s: %s\n", __LINE__, __FILE__, b); exit(1);}}
#define pError(e,a) {if (e) {perror(a); exit(1);}}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;

    char client_msg[MAX_STR_SIZE];
    char server_msg1[MAX_STR_SIZE];
    char server_msg2[MAX_STR_SIZE];
    char magic_str[] = MAGIC_STRING;
    char *serv_magic_check = 0x0, *serv_mess_type, *addr_string = 0x0, *rand_num_str = 0x0, *rand_num_str2 = 0x0, *gold_str = 0x0;


    if (argc < 2) {
       fprintf(stderr,"usage %s cid\n", argv[0]);
       exit(0);
    }

    /* Establish Connection*/
    portno = SERVER_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

	//Complete the missing codes
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    /* Communication */
    /* construct HELLO message */
    bzero(client_msg, MAX_STR_SIZE);
    snprintf(client_msg, MAX_STR_SIZE, "%s HELLO %s\n", magic_str, argv[argc-1]);	
    printf("%s HELLO %s\n", magic_str, argv[argc-1]);

    /* send HELLO message */
    send(sockfd, client_msg, strlen(client_msg), 0);

    /* receive STATUS message */
    //bzero(server_msg1, MAX_STR_SIZE);					//receive STATUS message
    n = read(sockfd, server_msg1, MAX_STR_SIZE);
    if (n < 0) 
         error("ERROR reading from socket");
    
    /* parse STATUS message */
    //Do Stuff
    
    char * token = strtok(server_msg1, " ");
    int count = 0;
    while(token != NULL){
        if(count == 0){
            serv_magic_check = token;
        } else if(count == 1){
            serv_mess_type = token;
        } else if(count == 2){
            rand_num_str = token;
        } else if (count == 3){
            rand_num_str2 = token;
        } else if (count == 4){
            addr_string = token;
        }
        token = strtok(NULL, " ");
        count++;
    }

    if(!(!strcmp(serv_magic_check, MAGIC_STRING) && !strcmp(serv_mess_type, "STATUS"))){
        error("ERROR server response had wrong MAGIC_STRING or message type");
    }
    printf("%s STATUS %s %s %s\n", magic_str, rand_num_str, rand_num_str2, addr_string);


    /* construct BYE message */
    bzero(client_msg, MAX_STR_SIZE);
    snprintf(client_msg, MAX_STR_SIZE,"%s BYE %d\n", magic_str, atoi(rand_num_str) + atoi(rand_num_str2)); 
    printf("%s BYE %d\n", magic_str, atoi(rand_num_str) + atoi(rand_num_str2));


    /* send BYE message */
    send(sockfd, client_msg, strlen(client_msg), 0);


    /* receive CONFIRM_BYE message */
    bzero(server_msg2,MAX_STR_SIZE);					
    n = read(sockfd, server_msg2, MAX_STR_SIZE);
    if (n < 0) 
         error("ERROR reading from socket");
    
    /* parse CONFIRM_BYE message */
    //Do Stuff
    char * token2 = strtok(server_msg2, " ");
    count = 0;
    while(token2!= NULL){
        if(count == 0){
            serv_magic_check = token2;
        } else if(count == 1){
            serv_mess_type = token2;
        } else if(count == 2){
            gold_str = token2;
        }
        token2 = strtok(NULL, " ");
        count++;
    }

    if(!(!strcmp(serv_magic_check, MAGIC_STRING) && !strcmp(serv_mess_type, "CONFIRM_BYE"))){
        error("ERROR server response had wrong MAGIC_STRING or message type");
    }

    printf("%s CONFIRM_BYE %s\n", magic_str, gold_str);
    //printf("%s\n", server_msg2);

    /* Close Connection */
    close(sockfd);
    return 0;
}