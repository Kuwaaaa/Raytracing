#include "immintrin.h"
#include "stdio.h"

void _mm256_print_epi32(__m256i p){
     int *p1 = (int*)&p;
     printf("%d %d %d %d %d %d %d %d\n",p1[0],p1[1],p1[2],p1[3],p1[4],p1[5],p1[6],p1[7]);
};

int main()
{
    __m256i a = _mm256_set_epi32(7,6,5,4,3,2,1,0);//从由地址高到低的顺序装载
    _mm256_print_epi32(a);
    __attribute__((aligned(32))) int d1[8] = {-1,-2,-3,-4,-5,-6,-7,-8};
    __m256i d = _mm256_load_si256((__m256i*)d1);//装在int可以使用指针类型转换 必须32位对齐
   _mm256_print_epi32(d);
    __m256i d2 = _mm256_add_epi32(d,a);
   _mm256_print_epi32(d2);
    return 0;
}
