#include <stdio.h>

int read_int(){
    int a;
    scanf("%d",&a);
    return a;
}

int read_real(){
    float a;
    scanf("%f",&a);
    int b = *(int*)(&a);
    return b;
}

void print_int( int a ){
    printf("%d",a);
}

void print_real( int a ){
    float b = *(float*)(&a);
    printf("%d",a);
}

void print_str( int a ){
    char* p = a;
    printf("%s",a);
}

void print_line(){
    printf("\n");
}

void MainEntry();

int main(){
    MainEntry();
    return 0;
}
