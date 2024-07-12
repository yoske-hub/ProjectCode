// PROGRAMA DESARROLLADO 13/06/2024 PARA LA RASPBERRY PI
// USANDO UN ACOPLAMIENTO DE VOLTAJES PARA LOS GPIO CON EL MOSFET 2N7000
// DESTINADO A USARSE CON LA PLACA SUPERIOR DEL T-154 CON LOS PINES NOMBRADOS A CONTINUACION

//	---------------------	v4.0   ---------------------
// LECTURA INSTANTANEA DE LOS DISPLAY 7 SEGMENTOS DEL T-154.
// Codigo revisado completamente de la version v2 y modificado
////////	Cambios:
// Error del PUNTERO: Al verificar el display en procedimiento() al comparar puntero anterior
//	con puntero actual, ambos punteros siempre seran igual por ser direccion de memoria. 
// SOLUCION: almacenar valores en variables separadas.
// 
// Funcionamiento. Se hace lectura primero del canal en la etapa 4
// Se hace lectura de los display y se retorna a la etapa 4 para verificar que seguimos en el mismo canal (ACK)
// Luego de ello se almacena el canal verificado para lo volver a leerlo.
// Una vez leidos los 3 canales se imprime todo y se termina el programa

#include <wiringPi.h>	// usar los gpio
#include <signal.h>		// limpiar los gpio
#include <stdio.h>		// print, printf
#include <stdlib.h>		// funcion exit
#include <time.h>			// time_t
#include <locale.h>			// hora local
// #include <unistd.h>		// usleep 
#include <string.h>		// strcat strlen snprintf

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
//const int pinDP = GPIO10;
// const int pinG = GPIO9;
// const int pinF = GPIO11;
// const int pinE = GPIO5;
// const int pinD = GPIO6;
// const int pinC = GPIO13;
// const int pinB = GPIO19;
// const int pinA = GPIO26;

// const int pinA0 = GPIO16;
// const int pinA1 = GPIO20;

const int pinG = GPIO2;
const int pinF = GPIO3;
const int pinE = GPIO4;

const int pinD = GPIO17;
const int pinC = GPIO27;

const int pinB = GPIO11;

const int pinA = GPIO5;
const int pinA0 = GPIO6;
const int pinA1 = GPIO13;
const int pinA2 = GPIO19;

const int segment_pins[] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG};

int caracteres7Segmentos[][8] = {    
	// a  b  c  d  e  f  g  ASCII
    {1, 1, 1, 1, 1, 1, 0, 48},   // 0
    {0, 1, 1, 0, 0, 0, 0, 49},   // 1
    {1, 1, 0, 1, 1, 0, 1, 50},   // 2
    {1, 1, 1, 1, 0, 0, 1, 51},   // 3
    {0, 1, 1, 0, 0, 1, 1, 52},   // 4
    {1, 0, 1, 1, 0, 1, 1, 53},   // 5
    {1, 0, 1, 1, 1, 1, 1, 54},   // 6
    {1, 1, 1, 0, 0, 0, 0, 55},   // 7
    {1, 1, 1, 1, 1, 1, 1, 56},   // 8
    {1, 1, 1, 1, 0, 1, 1, 57},   // 9
    {1, 1, 1, 0, 1, 1, 1, 65},   // A
    {0, 0, 1, 1, 1, 1, 1, 98},   // b min
    {0, 0, 0, 1, 1, 0, 1, 99},   // c min
    {0, 1, 1, 1, 1, 0, 1, 100},  // d min
    {1, 0, 0, 1, 1, 1, 1, 69},   // E
    {1, 0, 0, 0, 1, 1, 1, 70},   // F
    {1, 1, 1, 1, 0, 1, 1, 103},  // g min
    {0, 0, 1, 0, 1, 1, 1, 104},  // h min
    {0, 0, 1, 0, 0, 0, 0, 105},  // i min
    {0, 1, 1, 1, 1, 0, 0, 74},   // J
    {0, 0, 0, 1, 1, 1, 0, 76},   // L
    {0, 0, 1, 0, 1, 0, 1, 110},  // n min
    {0, 0, 1, 1, 1, 0, 1, 111},  // o min
    {1, 1, 0, 0, 1, 1, 1, 80},   // P
    {1, 1, 1, 0, 0, 1, 1, 81},   // Q
    {0, 0, 0, 0, 1, 0, 1, 114},  // r min
    {1, 0, 1, 1, 0, 1, 1, 83},   // S
    {0, 0, 1, 1, 1, 0, 0, 117},  // u min
    {0, 1, 1, 1, 0, 1, 1, 121},  // y min
    {0, 0, 0, 0, 0, 0, 1, 45},   // -
    {0, 0, 0, 0, 0, 0, 0, 0}    // caracter nulo
};

int etapa = 4;					//	 establece el inico de orden de lectura
int etapa_next=0;
int finish = 0;
const size_t FILAS = sizeof(caracteres7Segmentos) / sizeof(caracteres7Segmentos[0]);
const size_t COLUMNAS = sizeof(caracteres7Segmentos[0]) / sizeof(caracteres7Segmentos[0][0]);
int n_error=0;					// almacena numero de errores de tiempo al ejecutar. Son iguales a los re-intentos de lectura.
int n_desfase_error = 0;		// falla o desfase en lectura de canales
int channel = 0;
int ack_channel[]={0,0,0,0};	// se almacena los canales ya leidos y confirmados
int value_display[]={0,0,0};
char buffer_canal[100];			// buffer temporal para datos de canal
char buffer_display[100];		// buffer temporal para datos de display

////////////////////	 variables manipulables	///////////////////////////////////////////
const int asentamiento = 500;  	// (us) 1ms retraso anitrebote de lectura de cada display
const int it_a2a1a0 = 8;		//	iteraciones para confirmar lectura correcta de A0A1A2
const int it_7seg = 3;			//  iteraciones para confirmar lectura correcta de 7 segmentos
const int step_it = 15;			// tiempo en (us) intervalo entre cada iteracion
/////////////////
const int debug = 0;			// habilita mostrar tiempos entre cada funcion 
								// asentamiento ::: lectura7segmentos ::: decodificarCaracter 
const int tiempo = 1;			// habilita el tiempo actual donde se obtiene la lectura  utiliza el formato H:M:S D/M/A
								
///////////////////////////////////////////////////////////////////////////////////////////

/*Definicion optima para delay en microsegundos*/
void delayMicro(unsigned int us) {
    unsigned long start_time = micros(); // Obtenemos el tiempo actual en microsegundos
    while (micros() - start_time < us) {		
    }
}

// A2_A1_A0
int* lectura_selector() {		
    static int data_vector[] = {0, 0, 0};	// PUNTERO
	int data_prev[] = {1, 1, 1};    
    int count = 0;
	///////////// confirmar lectura
    while (count < it_a2a1a0) { // 10 iteraciones de lectura para antirebote
		delayMicro(step_it);
		//printf("lectura bucle. count=%d\n",count);
		data_vector[0] = digitalRead(pinA2);
		data_vector[1] = digitalRead(pinA1);
		data_vector[2] = digitalRead(pinA0);
		int igual = 1;
		for (size_t i=0; i<3; i++){
			// pullup
			if (data_prev[i] != data_vector[i]) {
			igual = 0;
			break;
			}			
		}
		if (igual == 1){
			count++;
		}else{
			count=0;
		}
		for (size_t i = 0; i < 3; i++) {
			data_prev[i] = data_vector[i];
		}
    }	
    return data_vector;
}

// A B C D E F G
int* lectura7Segmentos(int num) {
    static int data_vector[]={0, 0, 0, 0, 0, 0, 0};	
	int data_prev[] = {1, 1, 1, 1, 1, 1, 1};
    int count = 0;
    while (count < it_7seg) { // 10 iteraciones de lectura para antirebote
		delayMicro(step_it);	
		for (size_t i=0; i< num; i++) {
			data_vector[i] = digitalRead(segment_pins[i]);
		}		
		int igual = 1;	
		for (size_t i=0; i<num; i++){
			// anodo comun
			if (data_prev[i] != data_vector[i]) {
			igual = 0;
			break;
			}			
		}										
		if (igual == 1){
			count++;
		}else{
			count=0;
		}
		for (size_t i = 0; i < num; i++) {
			data_prev[i] = data_vector[i];
		}
    }
    return data_vector;
}

// caracter legible 7 segmentos
int decodificarCaracter(int estados[]){	
    for (size_t i=0;i<(FILAS);i++){
		int igual=1;
		for (size_t j=0;j<(COLUMNAS-1);j++){
			// PULLUP
			if(estados[j] == caracteres7Segmentos[i][j]){
				igual=0;
				break;
			}
		}		
		if (igual){
			return caracteres7Segmentos[i][7];
		}		
	}	
	return 63;	// '?' ascci code
}

// procedimiento para displays
void procedimiento(int* d_selector){  
	unsigned long tempo[] ={0,0,0,0};
	unsigned long diff[] = {0,0,0};
		int selector_temp1[] = {d_selector[0],d_selector[1],d_selector[2]};
	tempo[3]=micros();
	delayMicro(asentamiento);
	tempo[0]=micros();		
	int* estados = lectura7Segmentos(7);
		lectura_selector();
		int selector_temp2[] = {d_selector[0],d_selector[1],d_selector[2]};
		tempo[1]=micros();
		/* Verificacion de selector/transistor de display*/
		if (selector_temp1[0] == selector_temp2[0] &&
			selector_temp1[1] == selector_temp2[1] &&
			selector_temp1[2] == selector_temp2[2]){
				// confirma correcta lectura de transistor de display
		}else{
			n_desfase_error++;		// error de exceso de tiempo de procesamiento
			etapa = 0;				// etapa transitoria
			etapa_next = 1;			// volver a empezar lectura
				buffer_display[0] = '\0'; // vaciar buffer
			return;
		}
		////////
	int caracter = decodificarCaracter(estados);
		tempo[2]=micros();	
		diff[0]=tempo[0]-tempo[3];
		diff[1]=tempo[1]-tempo[0];
		diff[2]=tempo[2]-tempo[1];
		//////// verificacion de tiempo de operación.
		if (diff[0]>asentamiento*1.6 || diff[1]>300 || diff[2]>150){
			n_error++;		// error de exceso de tiempo de procesamiento
			etapa = 0;		// etapa transitoria
			etapa_next = 1;	// volver a empezar lectura
				buffer_display[0] = '\0'; // vaciar buffer
			return;
		}
		/* Verificacion de selector */
		
		////////////		
		if (debug==1){		
			// Concatenar más contenido al buffer
    			snprintf(buffer_display + strlen(buffer_display), sizeof(buffer_display) - strlen(buffer_display),  "\t%lu::%lu::%lu", diff[0], diff[1], diff[2]);
		}
	value_display[etapa-1]=caracter;
	if (etapa == 3){
			snprintf(buffer_display + strlen(buffer_display), sizeof(buffer_display) - strlen(buffer_display), "\t%c%c%c",value_display[0],value_display[1],value_display[2]);
		if (tiempo)	{
			time_t current_time;
			time(&current_time);
			struct tm *time_info = localtime(&current_time);
			// Formatear la hora y fecha
			char datetime_str[20]; // Espacio suficiente para "HH:MM:SS DD/MM/YY" y el carácter nulo
			strftime(datetime_str, sizeof(datetime_str), "%H:%M:%S\t%d/%m/%y", time_info);
				snprintf(buffer_display + strlen(buffer_display), sizeof(buffer_display) - strlen(buffer_display), "\t%s", datetime_str);
		}
			snprintf(buffer_display + strlen(buffer_display), sizeof(buffer_display) - strlen(buffer_display), "\tErrores: %u",n_error);
		n_error = 0;	
		etapa = 4; // despues de las 3 lecturas confirmamos
	} else{		
		etapa++;
	}
}

// procedimiento para canales
void procedimiento_channel(){	
	unsigned long tempo[] ={0,0,0,0};
	unsigned long diff[] = {0,0};
	// variable ack para confirmacion de lectura correcta entre canal y display's
	static int ack = 0;		// sintaxis.  ack(canal) lectura_display ack(canal) // 1:en confirmacion 0:confirmado
	tempo[3]=micros();
	delayMicro(asentamiento);
	tempo[0]=micros();	
	// lectura de los led de canal son 4
	int* estados = lectura7Segmentos(4);
	tempo[1]=micros();
	diff[0]=tempo[0]-tempo[3];
	diff[1]=tempo[1]-tempo[0];
	//////// verificacion de tiempo de operación.
	if (diff[0]>asentamiento*1.6 || diff[1]>300){
		n_error++;		// error de exceso de tiempo de procesamiento
		etapa = 0;		// etapa de transicion
		etapa_next = 4;	// reinicio de lectura de canal
		return;
	}
	////////////
	// En un estado scan normalmente 1 led siempre estara encendido y la suma de bit seria siempre 3
	if ( (estados[0] + estados[1] + estados[2] + estados[3]) != 3 ){
		n_error++;			// error de estados de led de canal
		etapa = 0 ;			// etapa de transicion
		etapa_next = 4;		// reinicio de lectura de canal
		return;
	}else{
		// Encontrar canal activo
		for (size_t i = 0; i < 4; i++){
			// anodo comun
			if (estados[i] == 0){
				if(ack_channel[i] == 0){		// si canal no esta confirmado
					if(ack == 0){
						channel = i+1;
						ack = 1;
						etapa = 0;		// etapa de transicion
						etapa_next = 1;	// empezar lectura de displays
						return;
					}else{
						if (channel == i+1){
							ack = 0;			// canal y lectura confirmada
							ack_channel[i] = 1;	// canal confirmado
						}else{
							ack = 0;
							etapa = 0;		// etapa de transicion
							etapa_next = 4;	// reinicio de lectura de canal
							n_desfase_error++;
							buffer_display[0] = '\0';	// vaciar buffer
							return;
						}						
					}					
				}else{
					etapa = 0;		// etapa de transicion
					etapa_next = 4;	// reinicio de lectura de canal
					return;
				}
			}
		}		
	}
	if (debug==1){		
			snprintf(buffer_canal + strlen(buffer_canal), sizeof(buffer_canal) - strlen(buffer_canal), "\t%lu::%lu",diff[0],diff[1]);
	}
		snprintf(buffer_canal + strlen(buffer_canal), sizeof(buffer_canal) - strlen(buffer_canal), "\t%u",channel);
		if (tiempo)	{
			time_t current_time;
			time(&current_time);
			struct tm *time_info = localtime(&current_time);

			// Formatear la hora y fecha
			char datetime_str[20]; // Espacio suficiente para "HH:MM:SS DD/MM/YY" y el carácter nulo
			strftime(datetime_str, sizeof(datetime_str), "%H:%M:%S\t%d/%m/%y", time_info);
				snprintf(buffer_canal + strlen(buffer_canal), sizeof(buffer_canal) - strlen(buffer_canal), "\t%s", datetime_str);
		}
			snprintf(buffer_canal + strlen(buffer_canal), sizeof(buffer_canal) - strlen(buffer_canal), "\tErrores: %u",n_error);
		n_error = 0;
		n_desfase_error = 0;
		strcat(buffer_canal, buffer_display);
		snprintf(buffer_canal + strlen(buffer_canal), sizeof(buffer_canal) - strlen(buffer_canal), "\tDesfases: %u",n_desfase_error);
		n_desfase_error = 0;
		printf("%s\n", buffer_canal);
		buffer_canal[0] = '\0';		// vaciar buffer
		buffer_display[0] = '\0';	// vaciar buffer
	    etapa = 0;					// etapa de transicion
		etapa_next = 4;				// en busqueda de nuevo canal
	if ((ack_channel[0] + ack_channel[1] + ack_channel[2]) == 3){
		finish = 1;
	}
}

// programa principal
int main(void){
    // accion de la funcion si finaliza el programa
    signal(SIGINT, programfinish);
	signal(SIGTERM, programfinish);
    if (wiringPiSetup() == -1) {
        printf("WiringPi setup failed.\n");
        return 1;
    }

    // inicializa los pines de los 7 segmentos
	for (size_t i=0 ; i<(sizeof(segment_pins)/sizeof(segment_pins[0])) ; i++){
		pinMode(segment_pins[i],INPUT);
		pullUpDnControl(segment_pins[i], PUD_UP);
	}
	
	// inicializa los pines A0A1A2
	pinMode(pinA0,INPUT);pullUpDnControl(pinA0, PUD_UP);
	pinMode(pinA1,INPUT);pullUpDnControl(pinA1, PUD_UP);
	pinMode(pinA2,INPUT);pullUpDnControl(pinA2, PUD_UP);

    for (;;){
	delayMicro(10);
	int* data_selector = lectura_selector();
	/* ordenado por etapas naturales del microcontrolador */
		switch (etapa){
			case 1:
				/* display 1 */							
				if (data_selector[0]==0 &&
					data_selector[1]==0 &&
					data_selector[2]==0){
					procedimiento(data_selector);
				}
				break;
			case 2:
				/* display 2 */
				if (data_selector[0]==0 &&
					data_selector[1]==0 &&
					data_selector[2]==1){
					procedimiento(data_selector);
				}	
				break;
			case 3:
				/* display 3 */
				if (data_selector[0]==0 &&
					data_selector[1]==1 &&
					data_selector[2]==0){
					procedimiento(data_selector);						
				}				
				break;
			case 4:		
				/* check channel */
				if (data_selector[0]==0 &&
					data_selector[1]==1 &&
					data_selector[2]==1){
					procedimiento_channel();
				}
				break;
			default:
				/*	etapas reales A2A1A0: 000 001 010 011 100 101 */
				/*	Important: Empezar en el ultimo etapa imperdira que
					se haga mal lectura del primer etapa */
				if (data_selector[0]==1 &&
					data_selector[1]==0 &&
					data_selector[2]==1){	
					etapa = etapa_next;
				}
				if (finish==1){
					printf("\n");
					exit(0);
					}
				break;
		}
	}
}