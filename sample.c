#include<stdio.h>

int main()
{
    int num1 = 0;
    int num2 = 0b0101;      
    int num3 = 0017;        
    int num4 = 0xABD;      
    char ch = 'A';
    // Errors :
    // (
    // {
    // [
    // 'a
    // "a
    // "a'
    /* 'a"
    int num5 = 019;
    float f  = 12.34d; */

    int num6 = 10;
    float f  = 12.34f; 
    int num7 = 10, i=10; 
    int arr[2] = {10, 20};

    if( (i!=50) || (i&&60) || (i==70) )
    {
        printf("hii\n");
    }

    for(int i = 0; i <= num7; i++)
    {
        for(int j=0; j<5; j++)
        {
            printf("hello\n");
        }
    }
    return 0;
}