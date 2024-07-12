#include <wiringPi.h>	// usar los gpio
#include <signal.h>		// limpiar los gpio
#include <stdio.h>		// print, printf
#include <stdlib.h>		// funcion exit
//////Equivalencias de wiringPi para BCM////////
typedef enum {
// Columna izquierda        //  Columna derecha
/* 3.3v */                  //  5v
GPIO2  = 8,                 //  5v
GPIO3  = 9,                 //  0v
GPIO4  = 7,                 GPIO14 = 15,         
/* 0v */                    GPIO15 = 16,
GPIO17 = 0,                 GPIO18 = 1,
GPIO27 = 2,                 // 0V
GPIO22 = 3,                 GPIO23 = 4,
/* 3.3V */                  GPIO24 = 5,
GPIO10 = 12,                // 0V    
GPIO9  = 13,                GPIO25 = 6,
GPIO11 = 14,                GPIO8  = 10,
/* 0V */                    GPIO7  = 11,
GPIO0  = 30,                GPIO1  = 31,
GPIO5  = 21,                // 0V
GPIO6  = 22,                GPIO12 = 26,
GPIO13 = 23,                // 0V
GPIO19 = 24,                GPIO16 = 27,
GPIO26 = 25,                GPIO20 = 28,
/* 0V */                    GPIO21 = 29
} GPIOMappings;
////////////////////////////////////////////////
void cleanup() {
    // Array de todos los pines de la enumeración
    int pins[] = {
		GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5, 
		GPIO6, GPIO7, GPIO8, GPIO9, GPIO10, GPIO11, 
		GPIO12, GPIO13, GPIO14, GPIO15, GPIO16, GPIO17, 
		GPIO18, GPIO19, GPIO20, GPIO21, GPIO22, GPIO23, 
		GPIO24, GPIO25, GPIO26, GPIO27
    };
    int numPins = sizeof(pins) / sizeof(pins[0]);

    // Recorrer todos los pines y configurarlos como entrada y en estado bajo
    for (int i = 0; i < numPins; i++) {
        pinMode(pins[i], INPUT);  // Establecer el pin en modo de entrada
        digitalWrite(pins[i], LOW); // Asegurarse de que el pin esté en estado bajo (apagado)
    }
    printf("GPIO cleanup done. All pins reset to input mode and LOW state.\n");
}

void programfinish(int sig) {
    printf("Caught signal %d\n", sig);
    cleanup();
    exit(0);
}

// Pines
const int pinDP = GPIO10;
const int pinA = GPIO26;
const int pinA0 = GPIO16;
const int pinA1 = GPIO20;
const int pinA2 = GPIO21;

int a2 =0;
int a1 =0;
int a0 =0;
unsigned long time1 = 0;
unsigned long time2 = 0;
unsigned long time3 = 0;
unsigned long x1 = 0;
unsigned long x2 = 0;
unsigned long x3 = 0;
unsigned long x4 = 0;

int main(void){
    // accion de la funcion si finaliza el programa
    signal(SIGINT, programfinish);
	signal(SIGTERM, programfinish);
    if (wiringPiSetup() == -1) {
        printf("WiringPi setup failed.\n");
        return 1;
    }
    pinMode(GPIO4,OUTPUT);
    pinMode(pinA0,INPUT);
	pullUpDnControl(pinA0, PUD_UP);
    pinMode(pinA1,INPUT);
	pullUpDnControl(pinA1, PUD_UP);
    pinMode(pinA2,INPUT);
	pullUpDnControl(pinA2, PUD_UP);


    a2=digitalRead(pinA2);
    a1=digitalRead(pinA1);
    a0=digitalRead(pinA0);
    for(;;){
        if(micros()<100000){        
            if ( digitalRead(pinA2)!=a2 || digitalRead(pinA1)!=a1 || digitalRead(pinA0)!=a0 ){
                a2=digitalRead(pinA2);
                time1=micros();
                a1=digitalRead(pinA1);
                digitalWrite(GPIO4,HIGH);
                digitalWrite(GPIO4,LOW);
                time2=micros();
                x1=time2-time1;
                a0=digitalRead(pinA0);
                time3=micros();       
                printf("%u%u%u %lu %lu %lu\n",a2,a1,a0,time1,x1,time3-time2);
                // printf("%d%d%d\t%lu\n",a2,a1,a0,micros());                
            }
        }else{exit(0);}

    }
}