#include <wiringPi.h>	// usar los gpio
//#include <signal.h>		// limpiar los gpio
#include <stdio.h>		// print, printf
//#include <stdlib.h>		// funcion exit
//#include <time.h>			// time_t
//#include <locale.h>			// hora local
#include <unistd.h>		// usleep
#include <sys/time.h>

unsigned long getMicroseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}
int main(void){

    // wiringpi
    int a = 0;
    int b = 0;
    int c = 0;
    unsigned long diff[] = {0,0,0,0,0,0,0,0,0,0};
    diff[0] = micros();
    a = 10;
    b = a + 10;
    c = b + 20;
    diff[1] = micros();
    //printf("%d\t%d\t%d\n",a,b,c);
    diff[2] = micros();
    a = b*c;
    b = c/a;
    c = b*a/(b-a);
    diff[3] = micros();
    //printf("%d\t%d\t%d\n",a,b,c);
    diff[4] = micros();
    while (micros() - diff[4] < 100){  
        diff[5] = micros();
    }
    usleep(1);
    diff[6]=micros();
    delayMicroseconds(1);
    diff[7]=micros();


    printf("\t%lu",diff[2]-diff[1]);
    printf("\t%lu",diff[3]-diff[2]);
    printf("\t%lu",diff[4]-diff[3]);
    printf("\t%lu",diff[5]-diff[4]);
    printf("\t%lu",diff[6]-diff[5]);
    printf("\t%lu\n",diff[7]-diff[6]);


    // // unistd
    // int a = 0;
    // int b = 0;
    // int c = 0;
    // unsigned long diff[] = {0,0,0,0,0,0,0,0,0,0};
    // diff[0] = getMicroseconds();
    // a = 10;
    // b = a + 10;
    // c = b + 20;
    // diff[1] = getMicroseconds();
    // //printf("%d\t%d\t%d\n",a,b,c);
    // diff[2] = getMicroseconds();
    // a = b*c;
    // b = c/a;
    // c = b*a/(b-a);
    // diff[3] = getMicroseconds();
    // //printf("%d\t%d\t%d\n",a,b,c);
    // diff[4] = getMicroseconds();
    // while (getMicroseconds() - diff[4] < 100){  
    //     diff[5] = getMicroseconds();
    // }
    // usleep(100);
    // diff[6]=getMicroseconds();
    // delayMicroseconds(100);
    // diff[7]=getMicroseconds();


    // printf("\t%lu",diff[2]-diff[1]);
    // printf("\t%lu",diff[3]-diff[2]);
    // printf("\t%lu",diff[4]-diff[3]);
    // printf("\t%lu",diff[5]-diff[4]);
    // printf("\t%lu",diff[6]-diff[5]);
    // printf("\t%lu\n",diff[7]-diff[6]);
    
    
}