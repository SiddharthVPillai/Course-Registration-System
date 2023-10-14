#define ROOT "root"
#define PASS "1234"
#include "structures.h"
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>

void tostring(char str[], int num){
    int i, rem, len = 0, n;
    n = num;
    while (n != 0){
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++){
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

int validate(char id[],char pass[]){
    if(strcmp(ROOT,id)!=0)  return -1;
    if(strcmp(PASS,pass)!=0)  return -2;
    return 0;
}

void addStudent(int);
void viewStudent(int);
void addFaculty(int);
void viewFaculty(int);
void activateStudent(int);
void blockStudent(int);
void modifyStudent(int);
void modifyFaculty(int);

void admin(int cl){
    char id[5],pass[5];
    write(cl,"Enter login id:",15);
    int bytes_read = read(cl,id,sizeof(id));
    id[bytes_read] = '\0';
    write(cl,"Enter password:",15);
    bytes_read = read(cl,pass,sizeof(pass));
    pass[bytes_read] = '\0';
    
    int val = validate(id,pass);
    if(val == -1){
        write(cl,INVALID_LOGIN,strlen(INVALID_LOGIN));
        return;
    }
    else if(val == -2){
        write(cl,INVALID_PASSWORD,strlen(INVALID_PASSWORD));
        return;
    }

    do{
        write(cl,adminMenu,strlen(adminMenu));
        char opt;
        bytes_read = read(cl,&opt,sizeof(opt));
        if(bytes_read == -1){
            perror("Error getting option");
            exit(0);
        }
        int op = atoi(&opt);
        
        switch(op){
            case 1: addStudent(cl);
            break;
            case 2: viewStudent(cl);
            break;
            case 3: addFaculty(cl);
            break;
            case 4: viewFaculty(cl);
            break;
            case 5: activateStudent(cl);
            break;
            case 6: blockStudent(cl);
            break;
            case 7: modifyStudent(cl);
            break;
            case 8: modifyFaculty(cl);
            break;
            case 9: write(cl,LOGOUT,strlen(LOGOUT));
            return;
            break;
            default:write(cl,WRONG,strlen(WRONG)); 
        }
    }while(1);
}



void addStudent(int cl){
    int bytes_read,bytes_written;
    char read_buffer[1024],write_buffer[1024];

    struct student s ,p;

    write(cl,"Enter degree (MT/IM/MS): ",25);
    bytes_read = read(cl,read_buffer,sizeof(read_buffer));

    int fd;
    int cr;
    
    if(read_buffer[0] == 'I'){
        fd = open(IM_STUDENT_DATA,O_RDWR);
        cr = 1;
    }
    else if(read_buffer[1] == 'T'){
        fd = open(MT_STUDENT_DATA,O_RDWR);
        cr = 2;
    }
    else if(read_buffer[1]=='S'){
        fd = open(MS_STUDENT_DATA,O_RDWR);
        cr = 3;
    }
    else{
        write(cl,INVALID_COURSE,strlen(INVALID_COURSE));
    }

    if(fd == -1 && errno == ENOENT){
        s.id = 1;
    }
    else if(fd == -1){
        perror("Error opening data file");
        return;
    }
    else{
        int os =  lseek(fd , -sizeof(struct student),SEEK_END);
        if(os == -1){
            perror("Error seeking last student id");
            return;
        }

        struct flock lock = {F_RDLCK,SEEK_SET,os,sizeof(struct student),getpid()};
        int ls = fcntl(fd,F_SETLKW, &lock);
        if(ls == -1){
            perror("Error obtaining lock");
            return;
        }
        
        bytes_read = read(fd,&p,sizeof(struct student));
        if(bytes_read == -1){
            perror("Error reading from file");
            return;
        }

        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
        close(fd);
        s.id = p.id+1;
    }
    read_buffer[bytes_read] = '\0';
    strcpy(s.course,read_buffer);

    bytes_written = write(cl,"Enter student name: ",20);
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
    strcpy(s.name,read_buffer);

    bytes_written = write(cl,"Enter age: ",11);
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
    s.age = atoi(read_buffer);

    bytes_written = write(cl,"Enter mail id: ",15);
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
    strcpy(s.mail,read_buffer);

    bytes_written = write(cl,"Enter state: ",13);
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
    strcpy(s.state,read_buffer);

    char roll[10];
    strcpy(roll,s.course);
    // int i=0;
    // for(;i<strlen(s.course);i++){
    //     roll[i] = s.course[i];
    // } 
    char num[4];
    tostring(num,s.id);
    // for(int j=0;j<=strlen(num);j++){
    //     roll[i++] = num[j];
    // }
    // roll[i] = '\0';
    strcat(roll,num);
    strcpy(s.rollNo,roll);

    strcpy(s.pass,DEFAULT_PASS);
    s.blocked = 0;
    // printf("\nID: %d",s.id);
    // printf("\nLogged: %d",s.logged);
    // printf("Name: %s",s.name);
    // printf("\nnum: %s",num);
    // printf("\nAge: %d",s.age);
    // printf("\nMail: %s",s.mail);
    // printf("\ncourse: %s",s.course);
    // printf("\nroll: %s",s.rollNo);
    // printf("\nPass: %s",s.pass);
    // printf("\nLocation: %s",s.state);

    char path[25];
    if(cr == 1){
        strcpy(path,IM_STUDENT_DATA);
    }
    else if(cr == 2){
        strcpy(path,MT_STUDENT_DATA);
    }
    else{
        strcpy(path,MS_STUDENT_DATA);
    }
    fd = open(path,O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if(fd == -1){
        perror("Error opening file");
        return;
    }

    bytes_written = write(fd,&s,sizeof(struct student));
    if(bytes_written == -1){
        perror("Error writing data");
        return;
    }
    close(fd);

    bzero(write_buffer,sizeof(write_buffer));
    sprintf(write_buffer,"\n-------Student Successfully created-------\nName: %s\nLoginId: %s\nPassword: %s",s.name,s.rollNo,s.pass);
    strcat(write_buffer,"^");

    bytes_written = write(cl,write_buffer,strlen(write_buffer));
    if(bytes_written == -1){
        perror("Error writing to client");
        return;
    }
    read(cl,read_buffer,sizeof(read_buffer));
}


void viewStudent(int cl){
    int bytes_read,bytes_written;
    char read_buffer[1024],write_buffer[1024],temp[1024];

    struct student s;
    bytes_written = write(cl,"Enter roll no of student: ",26);
    if(bytes_written == -1){
        perror("Error sending data to client");
        return;
    }
    bzero(read_buffer,sizeof(read_buffer));
    bytes_read = read(cl,read_buffer,sizeof(read_buffer));

    int fd,cs;
    if(read_buffer[0] == 'I'){
        cs = 1;
        fd = open(IM_STUDENT_DATA,O_RDONLY);
    }
    else if(read_buffer[1] == 'T'){
        cs = 2;
        fd = open(MT_STUDENT_DATA,O_RDONLY);
    }
    else if(read_buffer[1] == 'S'){
        cs = 3;
        fd = open(MS_STUDENT_DATA,O_RDONLY);
    }
    else{
        write(cl,INVALID_ROLL,strlen(INVALID_ROLL));
    }

    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct student), getpid()};
    int id = atoi(read_buffer+2);

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

    int os = lseek(fd,(id-1)*sizeof(struct student),SEEK_SET);
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

    bytes_read = read(fd,&s,sizeof(struct student));
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

    bzero(write_buffer,sizeof(write_buffer));
    sprintf(write_buffer,"-------Student Details-------\nRoll No: %s\nName: %s\nE-mail: %s\nAge: %d\nBlocked: %d",s.rollNo,s.name,s.mail,s.age,s.blocked);
    strcat(write_buffer,"^");

    bytes_written = write(cl,write_buffer,strlen(write_buffer));
    if(bytes_written == -1){
        perror("Error sending to client");
        return;
    }

    read(cl,read_buffer,sizeof(read_buffer));
}

void addFaculty(int cl){
    int bytes_read,bytes_written;
    char read_buffer[1024],write_buffer[1024];

    struct faculty f ,p;

    int fd;
    fd = open(FACULTY_DATA,O_RDONLY);    
    if(fd == -1 && errno == ENOENT){
        f.id = 1;
    }
    else if(fd == -1){
        perror("Error opening data file");
        return;
    }
    else{
        int os =  lseek(fd , -sizeof(struct faculty),SEEK_END);
        if(os == -1){
            perror("Error seeking last faculty id");
            return;
        }

        struct flock lock = {F_RDLCK,SEEK_SET,os,sizeof(struct faculty),getpid()};
        int ls = fcntl(fd,F_SETLKW, &lock);
        if(ls == -1){
            perror("Error obtaining lock");
            return;
        }
        
        bytes_read = read(fd,&p,sizeof(struct faculty));
        if(bytes_read == -1){
            perror("Error reading from file");
            return;
        }

        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
        close(fd);
        f.id = p.id+1;
    }
    
    bytes_written = write(cl,"Enter faculty name: ",20);
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
    strcpy(f.name,read_buffer);

    bytes_written = write(cl,"Enter Department: ",18);
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
    strcpy(f.dept,read_buffer);

    bytes_written = write(cl,"Enter mail id: ",15);
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
    strcpy(f.mail,read_buffer);

    bytes_written = write(cl,"Enter Designation: ",19);
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
    strcpy(f.designation,read_buffer);

    bytes_written = write(cl,"Enter Address: ",15);
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
    strcpy(f.address,read_buffer);
    
    char login[10] = "iiitb";
    char num[4];
    tostring(num,f.id);
    strcat(login,num);
    strcpy(f.login,login);

    strcpy(f.pass,DEFAULT_PASS);

    fd = open(FACULTY_DATA,O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if(fd == -1){
        perror("Error opening file");
        return;
    }

    bytes_written = write(fd,&f,sizeof(struct faculty));
    if(bytes_written == -1){
        perror("Error writing data");
        return;
    }
    close(fd);

    bzero(write_buffer,sizeof(write_buffer));
    sprintf(write_buffer,"\n-------Faculty Successfully created-------\nName: %s\nLoginId: %s\nPassword: %s",f.name,f.login,f.pass);
    strcat(write_buffer,"^");

    bytes_written = write(cl,write_buffer,strlen(write_buffer));
    if(bytes_written == -1){
        perror("Error writing to client");
        return;
    }
    read(cl,read_buffer,sizeof(read_buffer));
}

void viewFaculty(int cl){
    int bytes_read,bytes_written;
    char read_buffer[1024],write_buffer[1024],temp[1024];

    struct faculty f;
    bytes_written = write(cl,"Enter Id no of faculty: ",24);
    if(bytes_written == -1){
        perror("Error sending data to client");
        return;
    }
    bzero(read_buffer,sizeof(read_buffer));
    bytes_read = read(cl,read_buffer,sizeof(read_buffer));

    int fd;
    fd = open(FACULTY_DATA,O_RDONLY);

    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct faculty), getpid()};
    int id = atoi(read_buffer);

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

    bzero(write_buffer,sizeof(write_buffer));
    sprintf(write_buffer,"-------Faculty Details-------\nName: %s\nDepartment: %s\nDesignation: %s\nE-mail: %s\nAddress: %s\nLogin id: %s",f.name,f.dept,f.designation,f.mail,f.address,f.login);
    strcat(write_buffer,"^");

    bytes_written = write(cl,write_buffer,strlen(write_buffer));
    if(bytes_written == -1){
        perror("Error sending to client");
        return;
    }

    read(cl,read_buffer,sizeof(read_buffer));
}

void activateStudent(int cl){
    int bytes_read,bytes_written;
    char read_buffer[1024],write_buffer[1024],temp[1024];

    struct student s;
    bytes_written = write(cl,"Enter roll no of student: ",26);
    if(bytes_written == -1){
        perror("Error sending data to client");
        return;
    }
    bzero(read_buffer,sizeof(read_buffer));
    bytes_read = read(cl,read_buffer,sizeof(read_buffer));

    int fd,cs;
    if(read_buffer[0] == 'I'){
        cs = 1;
        fd = open(IM_STUDENT_DATA,O_RDWR);
    }
    else if(read_buffer[1] == 'T'){
        cs = 2;
        fd = open(MT_STUDENT_DATA,O_RDWR);
    }
    else if(read_buffer[1] == 'S'){
        cs = 3;
        fd = open(MS_STUDENT_DATA,O_RDWR);
    }
    else{
        write(cl,INVALID_ROLL,strlen(INVALID_ROLL));
    }

    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct student), getpid()};
    int id = atoi(read_buffer+2);

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

    int os = lseek(fd,(id-1)*sizeof(struct student),SEEK_SET);
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

    bytes_read = read(fd,&s,sizeof(struct student));
    if(bytes_read == -1){
        perror("Error reading from file");
        return;
    }
    s.blocked = 0;
    lseek(fd,-1*sizeof(s),SEEK_CUR);
    bytes_written = write(fd,&s,sizeof(struct student));
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
    sprintf(write_buffer,"-------Student unblocked successfully-------");
    strcat(write_buffer,"^");

    bytes_written = write(cl,write_buffer,strlen(write_buffer));
    if(bytes_written == -1){
        perror("Error sending to client");
        return;
    }

    read(cl,read_buffer,sizeof(read_buffer));
}

void blockStudent(int cl){
    int bytes_read,bytes_written;
    char read_buffer[1024],write_buffer[1024],temp[1024];

    struct student s;
    bytes_written = write(cl,"Enter roll no of student: ",26);
    if(bytes_written == -1){
        perror("Error sending data to client");
        return;
    }
    bzero(read_buffer,sizeof(read_buffer));
    bytes_read = read(cl,read_buffer,sizeof(read_buffer));

    int fd,cs;
    if(read_buffer[0] == 'I'){
        cs = 1;
        fd = open(IM_STUDENT_DATA,O_RDWR);
    }
    else if(read_buffer[1] == 'T'){
        cs = 2;
        fd = open(MT_STUDENT_DATA,O_RDWR);
    }
    else if(read_buffer[1] == 'S'){
        cs = 3;
        fd = open(MS_STUDENT_DATA,O_RDWR);
    }
    else{
        write(cl,INVALID_ROLL,strlen(INVALID_ROLL));
    }

    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct student), getpid()};
    int id = atoi(read_buffer+2);

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

    int os = lseek(fd,(id-1)*sizeof(struct student),SEEK_SET);
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

    bytes_read = read(fd,&s,sizeof(struct student));
    if(bytes_read == -1){
        perror("Error reading from file");
        return;
    }
    s.blocked = 1;
    lseek(fd,-1*sizeof(s),SEEK_CUR);
    bytes_written = write(fd,&s,sizeof(struct student));
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
    sprintf(write_buffer,"-------Student blocked successfully-------");
    strcat(write_buffer,"^");

    bytes_written = write(cl,write_buffer,strlen(write_buffer));
    if(bytes_written == -1){
        perror("Error sending to client");
        return;
    }

    read(cl,read_buffer,sizeof(read_buffer));
}

void modifyStudent(int cl){
    int bytes_read,bytes_written;
    char read_buffer[1024],write_buffer[1024],temp[1024];

    struct student s;
    bytes_written = write(cl,"Enter roll no of student: ",26);
    if(bytes_written == -1){
        perror("Error sending data to client");
        return;
    }
    bzero(read_buffer,sizeof(read_buffer));
    bytes_read = read(cl,read_buffer,sizeof(read_buffer));

    int fd,cs;
    if(read_buffer[0] == 'I'){
        cs = 1;
        fd = open(IM_STUDENT_DATA,O_RDWR);
    }
    else if(read_buffer[1] == 'T'){
        cs = 2;
        fd = open(MT_STUDENT_DATA,O_RDWR);
    }
    else if(read_buffer[1] == 'S'){
        cs = 3;
        fd = open(MS_STUDENT_DATA,O_RDWR);
    }
    else{
        write(cl,INVALID_ROLL,strlen(INVALID_ROLL));
    }

    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct student), getpid()};
    int id = atoi(read_buffer+2);

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

    int os = lseek(fd,(id-1)*sizeof(struct student),SEEK_SET);
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
///////////////////////
    bytes_read = read(fd,&s,sizeof(struct student));
    if(bytes_read == -1){
        perror("Error reading from file");
        return;
    }
    
    bytes_written = write(cl,"Enter student name: ",20);
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
    strcpy(s.name,read_buffer);

    bytes_written = write(cl,"Enter age: ",11);
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
    s.age = atoi(read_buffer);

    bytes_written = write(cl,"Enter mail id: ",15);
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
    strcpy(s.mail,read_buffer);

    bytes_written = write(cl,"Enter state: ",13);
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
    strcpy(s.state,read_buffer);
////////////////////////////
    lseek(fd,-1*sizeof(s),SEEK_CUR);
    bytes_written = write(fd,&s,sizeof(struct student));
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
    sprintf(write_buffer,"-------Student detail updated successfully-------");
    strcat(write_buffer,"^");

    bytes_written = write(cl,write_buffer,strlen(write_buffer));
    if(bytes_written == -1){
        perror("Error sending to client");
        return;
    }

    read(cl,read_buffer,sizeof(read_buffer));
}

void modifyFaculty(int cl){
    int bytes_read,bytes_written;
    char read_buffer[1024],write_buffer[1024],temp[1024];

    struct faculty f;
    bytes_written = write(cl,"Enter Id no of faculty: ",24);
    if(bytes_written == -1){
        perror("Error sending data to client1");
        return;
    }
    bzero(read_buffer,sizeof(read_buffer));
    bytes_read = read(cl,read_buffer,sizeof(read_buffer));

    int fd;
    fd = open(FACULTY_DATA,O_RDWR);

    struct flock lock = {F_WRLCK, SEEK_SET, 0, sizeof(struct faculty), getpid()};
    int id = atoi(read_buffer);

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
    ////////
    bytes_written = write(cl,"Enter faculty name: ",20);
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
    strcpy(f.name,read_buffer);

    bytes_written = write(cl,"Enter Department: ",18);
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
    strcpy(f.dept,read_buffer);

    bytes_written = write(cl,"Enter mail id: ",15);
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
    strcpy(f.mail,read_buffer);

    bytes_written = write(cl,"Enter Designation: ",19);
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
    strcpy(f.designation,read_buffer);

    bytes_written = write(cl,"Enter Address: ",15);
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
    strcpy(f.address,read_buffer);

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
    sprintf(write_buffer,"-------Faculty Details Modified-------");
    strcat(write_buffer,"^");

    bytes_written = write(cl,write_buffer,strlen(write_buffer));
    if(bytes_written == -1){
        perror("Error sending to client");
        return;
    }

    read(cl,read_buffer,sizeof(read_buffer));
}