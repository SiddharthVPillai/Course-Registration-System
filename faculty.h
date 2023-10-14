#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
// #include "structures.h"
// #include "const.h"

void viewCourse(int);
void addCourse(int);
void removeCourse(int);
void updateCourse(int);
void changePass(int,int);

int faculty_validate(struct faculty f,char id[],char pass[]){
    if(strcmp(f.login,id)!=0)   return -1;
    if(strcmp(f.pass,pass)!=0)  return -2;
    return 0;
}

void faculty(int cl){
    char id[10], pass[10];
    int bytes_read , bytes_written;
    char write_buffer[1024],read_buffer[1024];
    bytes_written = write(cl,"Enter login id: ",16);
    if(bytes_written == -1){
        perror("Error writing to client");
        return;
    }
    bytes_read = read(cl,id,sizeof(id));
    if(bytes_read == -1){
        perror("Error reading from client");
        return;
    }
    id[bytes_read] = '\0';

    bytes_written = write(cl,"Enter password: ",16);
    if(bytes_written == -1){
        perror("Error writing to client");
        return;
    }
    bytes_read = read(cl,pass,sizeof(pass));
    if(bytes_read == -1){
        perror("Error reading from client");
        return;
    }
    pass[bytes_read] = '\0';
    int i = atoi(id+5);
    
    struct faculty f;
    int fd;
    fd = open(FACULTY_DATA,O_RDONLY);
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct faculty), getpid()};

    int os = lseek(fd,(i-1)*sizeof(struct faculty),SEEK_SET);
    if(errno == EINVAL){
        bzero(write_buffer, sizeof(write_buffer));
        strcpy(write_buffer, INVALID_ID);
        strcat(write_buffer, "^");
        bytes_written = write(cl, write_buffer, strlen(write_buffer));
        if (bytes_written == -1)
        {
            perror("Error sending message to client");
            return;
        }
        bytes_read = read(cl, read_buffer, sizeof(read_buffer)); 
        return;
    }
    else if (os == -1){
        perror("Error while seeking file");
        return;
    }

    lock.l_start = os;
    int status = fcntl(fd,F_SETLKW,&lock);
    if(status == -1){
        perror("Error getting lock");
        return;
    }

    bytes_read = read(fd,&f,sizeof(struct faculty));
    if(bytes_read == -1){
        perror("Error reading from file");
        return;
    }

    lock.l_type = F_UNLCK;
    status = fcntl(fd,F_SETLK,&lock);
    if(status == -1){
        perror("Error releasing lock");
        return;
    }

    int val = faculty_validate(f,id,pass);
    if(val == -1){
        write(cl,INVALID_LOGIN,strlen(INVALID_LOGIN));
        return;
    }
    else if(val == -2){
        write(cl,INVALID_PASSWORD,strlen(INVALID_PASSWORD));
        return;
    }

    do{
        bytes_written = write(cl,FACULTY_MENU,strlen(FACULTY_MENU));
        if(bytes_written == -1){
            perror("Error writing to client");
            return;
        }
        char opt;
        bytes_read = read(cl,&opt,sizeof(opt));
        if(bytes_read == -1){
            perror("Error reading from client");
            return;
        }

        int op = atoi(&opt);
        switch(op){
            case 1: break;
            case 2: break;
            case 3: break;
            case 4: break;
            case 5: changePass(cl,i);
                    break;
            case 6: write(cl,LOGOUT,strlen(LOGOUT));
                    return;
                    break;
            default: write(cl,WRONG,strlen(WRONG)); 
        }
    }while(1);
}

void viewCourse(int cl){

}
void addCourse(int cl){

}
void removeCourse(int cl){

}
void updateCourse(int cl){

}

void changePass(int cl,int id){
    int bytes_read,bytes_written;
    char read_buffer[1024],write_buffer[1024],temp[1024];

    struct faculty f;
    
    int fd;
    fd = open(FACULTY_DATA,O_RDWR);

    struct flock lock = {F_WRLCK, SEEK_SET, 0, sizeof(struct faculty), getpid()};

    if(fd == -1){
        bzero(write_buffer, sizeof(write_buffer));
        strcpy(write_buffer, INVALID_ID);
        strcat(write_buffer, "^");
        bytes_written = write(cl, write_buffer, strlen(write_buffer));
        if (bytes_written == -1){
            perror("Error sending message to client");
            return;
        }
        bytes_read = read(cl, read_buffer, sizeof(read_buffer)); 
        return;
    }

    int os = lseek(fd,(id-1)*sizeof(struct faculty),SEEK_SET);
    if(errno == EINVAL){
        bzero(write_buffer, sizeof(write_buffer));
        strcpy(write_buffer, INVALID_ID);
        strcat(write_buffer, "^");
        bytes_written = write(cl, write_buffer, strlen(write_buffer));
        if (bytes_written == -1){
            perror("Error sending message to client");
            return;
        }
        bytes_read = read(cl, read_buffer, sizeof(read_buffer)); 
        return;
    }
    else if (os == -1){
        perror("Error while seeking file");
        return;
    }

    lock.l_start = os;
    bytes_read = read(fd,&f,sizeof(struct faculty));
    if(bytes_read == -1){
        perror("Error reading from file");
        return;
    }
    int status = fcntl(fd,F_SETLKW,&lock);
    if(status == -1){
        perror("Error getting lock");
        return;
    }
    ////////
    bytes_written = write(cl,"Enter new password: ",21);
    if(bytes_written == -1){
        perror("Error writing to client");
        return;
    }
    bytes_read = read(cl,read_buffer,sizeof(read_buffer));
    if(bytes_read == -1){
        perror("Error reading from client");
        return;
    }
    read_buffer[bytes_read] = '\0';
    strcpy(f.pass,read_buffer);
    ////////

    lseek(fd,-1*sizeof(f),SEEK_CUR);
    bytes_written = write(fd,&f,sizeof(struct faculty));
    if(bytes_written == -1){
        perror("Error writing into file");
        return;
    }
    lock.l_type = F_UNLCK;
    status = fcntl(fd,F_SETLK,&lock);
    if(status == -1){
        perror("Error releasing lock");
        return;
    }

    bzero(write_buffer,sizeof(write_buffer));
    sprintf(write_buffer,"-------Password changed successfully-------");
    strcat(write_buffer,"^");

    bytes_written = write(cl,write_buffer,strlen(write_buffer));
    if(bytes_written == -1){
        perror("Error sending to client");
        return;
    }

    read(cl,read_buffer,sizeof(read_buffer));
}