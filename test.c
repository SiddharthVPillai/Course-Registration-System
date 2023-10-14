#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include "const.h"
#include "structures.h"
#include<fcntl.h>

int main(){
    // int fd = open(FACULTY_DATA,O_RDONLY);
    // struct faculty f;
    // while(read(fd,&f,sizeof(struct faculty))>0){
    //     printf("name: %s\n",f.name);
    //     printf("dept: %s\n",f.dept);
    //     printf("des: %s\n",f.designation);
    //     printf("mail: %s\n",f.mail);
    //     printf("address: %s\n",f.address);
    //     printf("login: %s\n",f.login);
    //     printf("pass: %s\n",f.pass);
    //     printf("id: %d\n",f.id);
    //     printf("---------------\n");

    //     char course[3];
    // char name[20];
    // char mail[50];
    // char state[20];
    // int id;
    // int age;
    // char rollNo[10];
    // char pass[10];
    // int blocked;
    // }
    struct student s;
    int fd = open(MT_STUDENT_DATA,O_RDONLY);
    while(read(fd,&s,sizeof(struct student))>0){
        printf("name: %s\n",s.name);
        printf("dept: %s\n",s.course);
        printf("mail: %s\n",s.mail);
        printf("address: %s\n",s.state);
        printf("login: %s\n",s.rollNo);
        printf("pass: %s\n",s.pass);
        printf("id: %d\n",s.id);
        printf("age: %d\n",s.age);
        printf("blocked: %d\n",s.blocked);
        printf("---------------\n");
    }
    return 0;
}