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
    char a[] = "MT102";
    printf("%d",atoi(a+2));
    return 0;
}