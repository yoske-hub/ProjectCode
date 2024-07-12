#include <wiringPi.h>	// usar los gpio
#include <signal.h>		// limpiar los gpio
#include <stdio.h>		// print, printf
#include <stdlib.h>		// funcion exit

//////Equivalencias de wiringPi para BCM////////
typedef enum {
// Columna izquierda
// 3.3v
GPIO2  = 8,
GPIO3  = 9,
GPIO4  = 7,
// 0V
GPIO17 = 0,
GPIO27 = 2,
GPIO22 = 3,
// 3.3V
GPIO10 = 12,
GPIO9  = 13,
GPIO11 = 14,
// 0V
GPIO0  = 30,
GPIO5  = 21,
GPIO6  = 22,
GPIO13 = 23,
GPIO19 = 24,
GPIO26 = 25,
// 0V

// Columna derecha
// 5V
// 5V
// 0V
GPIO14 = 15,
GPIO15 = 16,
GPIO18 = 1,
// 0V
GPIO23 = 4,
GPIO24 = 5,
// 0V
GPIO25 = 6,
GPIO8  = 10,
GPIO7  = 11,
GPIO1  = 31,
// 0V
GPIO12 = 26,
// 0V
GPIO16 = 27,
GPIO20 = 28,
GPIO21 = 29,
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

int main(void){
	// accion de la funcion si finaliza el programa
	signal(SIGINT, programfinish);
    signal(SIGTERM, programfinish);
    if (wiringPiSetup() == -1) {
        printf("WiringPi setup failed.\n");
        return 1;
    }
    
    ///// Your code setup
    
    for (;;){
		//// Your code loop
	}
    
}
