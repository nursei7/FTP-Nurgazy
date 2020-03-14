#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>



int main(int argc, char* argv[]) {
    int server_port;
    char server_addr[100];

    if(argc != 3){
        printf("Some arguments are missing, enter server address  and port number\n");
        return 1;
    }
    else{
        server_port = atoi(argv[2]);
        strcpy(server_addr, argv[1]);
        printf("Server's IP address: %s\n", server_addr);
        printf("Server's port number: %d\n", server_port);
    }
    char buf[1024];// to send and receive messages
    int sockfd;//Communication socket
    int sockfdt;//Transport socket
    struct sockaddr_in address;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Cannot open socket \n");
        exit(1);
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(server_port);
    inet_aton(server_addr, &(address.sin_addr));
    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("Connection failed\n");
        exit(1);
    }
    printf("Connected to the server\n");
    while(1){
        char *arg;
        char *cmd;
        while(1){
            printf("ftp> ");
            char in[1024];
            fgets(buf,1024, stdin);
            strcpy(in, buf);
            cmd = strtok(in, "\n");
            if(NULL == cmd){
                continue;
            }
            cmd = strtok(cmd, " ");
            if(NULL == cmd){
                continue;
            }
            break;
        }
        
        
        if((strcmp(cmd, "USER")) == 0){
            write(sockfd, buf, strlen(1+buf));
            memset(buf, 0, strlen(buf));
            if (read(sockfd, buf, 1024) == 0) {
                printf("Connection closed by server\n");
                exit(0);
            }
            if((strcmp(buf, "Username does not exist")) == 0){
                printf("Username does not exist\n");
            }
            else if((strcmp(buf, "Username OK, password required")) == 0){
                printf("Username OK, password required\n");
            }
            else{
                printf("Incorrect response from server\n");
            }
        }
        else if((strcmp(cmd, "PASS")) == 0){
            write(sockfd, buf, strlen(1+buf));
            memset(buf, 0, strlen(buf));
            if (read(sockfd, buf, 1024) == 0) {
                printf("Connection closed by server\n");
                exit(0);
            }
            if((strcmp(buf, "wrong password")) == 0){
                printf("wrong password\n");
            }
            else if((strcmp(buf, "Authentication complete")) == 0){
                printf("Authentication complete\n");
            }
            else if((strcmp(buf, "set USER first")) == 0){
                printf("set USER first\n");
            }
            else{
                printf("Incorrect response from server\n");
            }
        }
        else if ((strcmp(cmd, "PUT")) == 0) {
            arg = strtok(NULL, " ");//name of the file to be send
            printf("This is the command: %s\n", cmd);
            printf("This is the argument: %s\n", arg);
            FILE *filepointer;
            char *line;
            if(NULL == arg){
                printf("Please enter PUTT fileName");
                continue;
            } 
            filepointer = fopen(arg, "r");// open the file with name arg
            if(filepointer == 0){ // if fails
                printf("Error opening the file \n");
                continue;
            }
            char *aux;
            char *response;
            aux = strtok(buf, "\n");
            memset(buf, 0, strlen(buf));//put zeros in buf
            sprintf(buf, "PUT %s", arg);
            write(sockfd, buf, strlen(1+buf));// send the command to the server
            if (read(sockfd, buf, 1024) == 0) {
                printf("Connection closed by server\n");
                exit(0);
            }
            response = strtok(buf, "\n");
            if(NULL == response){
                printf("Corrupted response from server\n");
                continue;
            }
            else if(strcmp(response, "Autentication required") == 0){
                printf("Autentication required\n");
                continue;
            }
            else{
                response = strtok(response, " ");
                if(strcmp(response, "Ready") != 0){
                    printf("Server is not ready\n");
                    continue;
                }
                else if(strcmp(response,"Ready") == 0){
                    response = strtok(NULL, " ");
                    if(response == NULL){
                        printf("Port is missing\n");
                        continue;
                    }
                }
            }
            //once server is ready and port number is provided ...
            if((sockfdt = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                printf("Cannot open socket\n");
                exit(0);
            }
            address.sin_port = htons(atoi(response));
            size_t length = 0;
            ssize_t read;
            if (connect(sockfdt, (struct sockaddr *)&address, sizeof(address)) < 0) {
                printf("Can't connect to port\n");
                continue;
            }
            while((read = getline(&line, &length, filepointer)) != -1){
                printf("%s\n", line);
                write(sockfdt, line, strlen(line+1));
            }
            close(filepointer);
            close(sockfdt);



            //open file
            //read it and send it to server
        }











        else if ((strcmp(cmd, "GET")) == 0) {
            write(sockfd, buf, strlen(1+buf));
            memset(buf,0, strlen(buf));
            if(read(sockfd, buf, 1024) == 0){
                printf("Connection closed by server\n");
                exit(0);
            }
            //check if the fiile is there
        }
        else if(strcmp(cmd, "QUIT") == 0){
            printf("Shutting down...\n");

            if(close(sockfd) == 0){//quits the connection
            printf("Socket closed\n");
        } else {
            printf("Error while closing the socket\n");
        }
            return(1);
    }
  }



    return 0;
}

