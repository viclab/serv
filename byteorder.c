//判断机器字节序的大小端模式
#include <stdio.h>

union byte_union
{
    short value;
    char bytes[sizeof(short)];
};

void byteorder()
{
    union byte_union test;
    test.value = 0x0102;
    if(1 == test.bytes[0] && 2 == test.bytes[1])
        printf("本机采用大端字节序\n");
    else if(2 == test.bytes[0] && 1 == test.bytes[1])
        printf("本机采用小端字节序\n");
    else
        printf("无法判断\n");
}

int main(void)
{
    byteorder();
    return 0;
}
    
