#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

void connection(int sd){
    char read_buffer[1024],write_buffer[1024],temp[1024];
    int bytes_read,bytes_written;
    do{
        bzero(read_buffer,sizeof(read_buffer));
        bzero(temp,sizeof(temp));

        bytes_read = read(sd,read_buffer,sizeof(read_buffer));
        if(bytes_read == -1){
            perror("Error reading from client");
        }
        else if(bytes_read == 0){
            printf("No data received from server");
        }
        else if (strchr(read_buffer, '^') != NULL){
            strncpy(temp, read_buffer, strlen(read_buffer) - 1);
            printf("%s\n", temp);
            bytes_written = write(sd, "^", strlen("^"));
            if (bytes_written == -1){
                perror("Error while writing to client socket!");
                break;
            }
        }
        else if(strchr(read_buffer,'$') || strchr(read_buffer,'@')){
            strncpy(temp, read_buffer, strlen(read_buffer) - 2);
            printf("%s\n", temp);
            printf("Closing the connection to the server\n");
            break;
        }
        else{
            bzero(write_buffer,sizeof(write_buffer));

            if(strchr(read_buffer,'#')!=NULL){
                strcpy(write_buffer,getpass(read_buffer));
            }
            else{
                printf("%s\n", read_buffer);
                scanf("%[^\n]%*c", write_buffer);
            }
            
            bytes_written = write(sd,write_buffer,strlen(write_buffer));
            if(bytes_written == -1){
                perror("Error writing toserver");
                break;
            }
        }
    }while(bytes_read > 0);
    close(sd);
}

int main(){
    int nsd;
    nsd = socket(AF_INET,SOCK_STREAM,0);
    if(nsd == -1){
        perror("Error creating server socket");
        exit(0);
    }


    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(9009);
    serv.sin_addr.s_addr = INADDR_ANY;

    int status = connect(nsd,(struct sockaddr*)&serv,sizeof(serv));
    if(status == -1){
        perror("Error connecting to server");
        close(nsd);
        exit(0);
    }

    connection(nsd);
    close(nsd);

    // char buff[200];



    // int bytes_read = read(nsd,buff,sizeof(buff));
    // while(bytes_read>1){
    //     printf("%s",buff);
    //     char in;
    //     scanf("%c",&in);
    //     write(nsd,&in,sizeof(in));
    //     bytes_read = read(nsd,buff,sizeof(buff));
    // }

    // read(nsd,buff,sizeof(buff));
    // printf("\n%s",buff);
    return 0;

}