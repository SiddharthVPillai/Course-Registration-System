# Course-Registration-System
A course registration system made in c which uses system call and files as database.
The system uses file locking mechanism and socket programming for its functioning.

There are three roles in the system:
1. Admin
2. Faculty
3. Student

Admin is a super user that can add/view/block/unblock/update details od students and add/view/update faculty details.
userid: root
password: 1234

While creating student and faculty each of them is assigned with default password i.e. pass123.

Faculty can view add courses and view and modify the courses.
Students can see the courses being offered and can enroll and deenroll from the courses.

-----------------------------------------------------------------------------------------------------------
In order to run the software first compile the server by running following command in terminal
cc -o server server.c

Now compile the client by running
cc -o client client.c

Now the out file is names as server and client

Run server in one terminal and client in other and you will get data from server to client.
