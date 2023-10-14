#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include "const.h"
#include "admin.h"
#include "faculty.h"
#include "student.h"

void connection(int cl){
    printf("Client connected to server\n");
    char read_buffer[1024],write_buffer[1024];
    int bytes_read,bytes_written;

    bytes_written = write(cl,mainMenu,strlen(mainMenu));
    if(bytes_written == -1){
        perror("Error sending msg to client");
    }
    
    bzero(read_buffer,sizeof(read_buffer));
    bytes_read = read(cl,read_buffer,sizeof(read_buffer));
    if(bytes_read == -1){
        perror("Error reading from client");
    }
    printf("Client sent :%s\n",read_buffer);
    int op = atoi(read_buffer);
    switch(op){
        case 1: admin(cl);
                break;
        case 2: faculty(cl);
                break;
        case 3: student(cl);
                break;
        default: write(cl,"Wrong option thank you :)",26);
    }
}

int main(){
    int sd;
    sd = socket(AF_INET,SOCK_STREAM,0);
    if(sd==-1){
        perror("Error creating server socket");
        exit(1);
    }

    struct sockaddr_in serv,cli;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(9009);
    serv.sin_addr.s_addr = INADDR_ANY;

    int b_status = bind(sd,(struct sockaddr*)&serv,sizeof(serv));
    if(b_status==-1){
        perror("Error binding server");
        exit(1);
    }

    int l_status = listen(sd,5);
    if(l_status==-1){
        perror("Error listening to client");
        close(sd);
        exit(1);
    }

    int c_size;
    while(1){
        int cl;
        c_size = (int)sizeof(cli);
        cl = accept(sd,(struct sockaddr *)&cli,&c_size);
        if(cl == -1){
            perror("Error connecting client");
            close(sd);
        }

        if(!fork()){
            connection(cl);
            // char buff;
            // read(cl,&buff,sizeof(buff));
            // printf("Message from client: %c\n",buff);

            // int n = atoi(&buff);
            // switch(n){
            //     case 1: admin(cl);
            //             break;
            //     case 2:break;
            //     case 3:break;
            //     default: write(cl,"Wrong option thank you :)",26);
            // }
        }
        close(cl);
    }
    close(sd);
    
    return 0;
}