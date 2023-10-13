#include<stdio.h>
#include<stdlib.h>
#include<string.h>
// #include<math.h>
void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}
int main(){
    char a[]="Hello",b[10];
    char c[10];
    int i=0;
    for(;i<strlen(a);i++)
        c[i] = a[i];
    printf("%s",c);
    tostring(b,1);
    for(int j=0;j<strlen(b);j++){
        c[i++] = b[j];
    }
    printf("\n%s",c);
    return 0;
}