	// PROGRAMA DESARROLLADO 24/04/2024 PARA LA RASPBERRY PI
// USANDO UN ACOPLAMIENTO DE VOLTAJES PARA LOS GPIO CON EL MOSFET 2N7000
// DESTINADO A USARSE CON LA PLACA SUPERIOR DEL T-154 CON LOS PINES NOMBRADOS A CONTINUACION

//	---------------------	v1.0   ---------------------
// LECTURA INSTANTANEA DE LOS DISPLAY 7 SEGMENTOS DEL T-154.

#include <wiringPi.h>	// usar los gpio
#include <signal.h>		// limpiar los gpio
#include <stdio.h>		// print, printf
#include <stdlib.h>		// funcion exit
#include <time.h>			// time_t
#include <locale.h>			// hora local
// #include <unistd.h>		// usleep

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
const int pinG = GPIO9;
const int pinF = GPIO11;
const int pinE = GPIO5;
const int pinD = GPIO6;
const int pinC = GPIO13;
const int pinB = GPIO19;
const int pinA = GPIO26;

const int pinA0 = GPIO16;
const int pinA1 = GPIO20;
const int pinA2 = GPIO21;

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

int ciclo = 0;	//	 establece el inico de orden de lectura
int ciclo_next=0;
const size_t FILAS = sizeof(caracteres7Segmentos) / sizeof(caracteres7Segmentos[0]);
const size_t COLUMNAS = sizeof(caracteres7Segmentos[0]) / sizeof(caracteres7Segmentos[0][0]);
int n_error=0;	// almacena numero de errores de tiempo al ejecutar. Son iguales a los re-intentos de lectura.

////////////////////	 variables manipulables	///////////////////////////////////////////		
const int asentamiento = 500;  	// (us) 1ms retraso anitrebote de lectura de cada display
const int it_a0a1a2 = 4;		//	iteraciones para confirmar lectura correcta de A0A1A2
const int it_7seg = 4;			//  iteraciones para confirmar lectura correcta de 7 segmentos
const int step_it = 20;			// tiempo en (us) intervalo entre cada iteracion
/////////////////
const int debug = 1;			// habilita mostrar tiempos entre cada funcion 
								// asentamiento ::: lectura7segmentos ::: decodificarCaracter 
const int tiempo = 1;			// habilita el tiempo actual donde se obtiene la lectura
const int formato_reducido = 1; // utiliza el formato H:M:S D/M/A
								




///////////////////////////////////////////////////////////////////////////////////////////

int* lectura_a0a1a2() {		
    int data_prev[] = {1, 1, 1};
    static int data_vector[] = {0, 0, 0};
    int count = 0;
	///////////// confirmar lectura
    while (count < it_a0a1a2) { // 10 iteraciones de lectura para antirebote
		unsigned long start_time = micros();
		while((micros()-start_time) < step_it){
			delayMicroseconds(1);
			}	
		//printf("lectura bucle. count=%d\n",count);
		data_vector[0] = digitalRead(pinA2);
		data_vector[1] = digitalRead(pinA1);
		data_vector[2] = digitalRead(pinA0);
		int igual = 1;
		for (int i=0; i<3; i++){
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
		for (int i = 0; i < 3; i++) {
			data_prev[i] = data_vector[i];
		}
    }
    return data_vector;
}

int* lectura7Segmentos() {
	//printf("lectura 7\n");
    int data_prev[] = {1, 1, 1, 1, 1, 1, 1, 1};
    static int data_vector[]={0,0,0,0,0,0,0,0};	
    int count = 0;
    while (count < it_7seg) { // 10 iteraciones de lectura para antirebote
	unsigned long start_time = micros();
	while((micros()-start_time) < step_it){
		//delayMicroseconds(1);
	    }	
	for (int i=0; i<8; i++) {
	    data_vector[i] = digitalRead(segment_pins[i]);
	}
	
	int igual = 1;	
	for (int i=0; i<8; i++){
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
	for (int i = 0; i < 8; i++) {
	    data_prev[i] = data_vector[i];
        }
    }
    return data_vector;
}

int decodificarCaracter(int estados[]){
	//int ascii_code=0;
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
		//delayMicroseconds(1);			
	}
	return 63;	// '?' ascci code
}

void procedimiento(){
	unsigned long tempo[] ={0,0,0,0};
	unsigned long diff[] = {0,0,0};
	tempo[3]=micros();
	while(micros()-tempo[3]<asentamiento){
		tempo[0]=micros();
	}	
	int* estados = lectura7Segmentos();
		tempo[1]=micros();
	int caracter = decodificarCaracter(estados);
		tempo[2]=micros();	
		diff[0]=tempo[0]-tempo[3];
		diff[1]=tempo[1]-tempo[0];
		diff[2]=tempo[2]-tempo[1];		
		//////// verificacion de tiempo de operación.
		if (tempo[0]-tempo[3]>asentamiento*1.6 || tempo[1]-tempo[0]>300 || tempo[2]-tempo[1]>150){
			n_error++;
			ciclo = 0;
			ciclo_next = 1;
			return;
		}
		////////////
		if (debug==1){						
			printf("\t%lu::%lu::%lu\t",tempo[0]-tempo[3],tempo[1]-tempo[0],tempo[2]-tempo[1]);			
		}
	printf("%c",caracter);
	if (ciclo < 3){
		ciclo++;
	} else{		
		ciclo = 4;	// despues de las 3 lecturas de display leemos el canal
			if (tiempo)	{
				if (formato_reducido){
					time_t current_time;
					time(&current_time);
					struct tm *time_info = localtime(&current_time);

					// Formatear la hora y fecha
					char datetime_str[20]; // Espacio suficiente para "HH:MM:SS DD/MM/YY" y el carácter nulo
					strftime(datetime_str, sizeof(datetime_str), "%H:%M:%S %d/%m/%y", time_info);
					printf("\t%s", datetime_str);
				}else{
					// Establece el locale a español
					setlocale(LC_TIME, "es_PE.UTF-8");
						
					// Obtiene el tiempo actual
					time_t current_time;
					time(&current_time);

					// Convierte el tiempo actual a una estructura de tiempo
					struct tm *time_info = localtime(&current_time);

					// Buffer para almacenar la cadena de fecha y hora formateada
					char date_time_str[100];

					// Formatea la fecha y hora de acuerdo con el locale actual
					strftime(date_time_str, sizeof(date_time_str), "%A, %d de %B de %Y, %H:%M:%S", time_info);

					// Imprime la fecha y hora formateada
					printf("\t%s", date_time_str);
				}			
			}
		printf("\tErrores: %d",n_error);
		fflush(stdout);	// imprimo datos alamcenados en buffer
		//printf("\n");
	}
}

void procedimiento_channel(){
	unsigned long tempo[] ={0,0,0,0};
	tempo[3]=micros();
	while(micros()-tempo[3]<asentamiento){
		tempo[0]=micros();
		//delayMicroseconds(10);
	}	
	int* estados = lectura7Segmentos();
	tempo[1]=micros();
	if (debug==1){		
		printf("\t%lu::%lu\t",tempo[0]-tempo[3],tempo[1]-tempo[0]);				
	}
	//////// verificacion de tiempo de operación.
	if (tempo[0]-tempo[3]>asentamiento*1.6 || tempo[1]-tempo[0]>300){
		printf("\x1b[2K"); // Borra el buffer actual con defectos de tiempo de ejecucion
		printf("\r");	//	 Mueve el  cursor al inicio
		n_error++;
		//ciclo=0;
		return;
	}
	////////////	
	// En un estado scan normalmente 1 led siempre estara encendido y la suma de bit seria siempre 3
	int channel = 0;
	if ( (estados[0] + estados[1] + estados[2] + estados[3]) != 3 ){
		n_error++;
		return;
	}else{
		// Encontrar canal activo
		for (int i = 0; i < 4; i++){
			if (estados[i] == 0){
				channel = i+1;
				break;
			}
		}		
	}
	printf("%d",channel);
	if (ciclo < max_ciclo){
		ciclo++;
	} else{
		ciclo = 1;	
		if (tiempo)	{
			if (formato_reducido){
				time_t current_time;
				time(&current_time);
				struct tm *time_info = localtime(&current_time);

				// Formatear la hora y fecha
				char datetime_str[20]; // Espacio suficiente para "HH:MM:SS DD/MM/YY" y el carácter nulo
				strftime(datetime_str, sizeof(datetime_str), "%H:%M:%S %d/%m/%y", time_info);
				printf("\t%s", datetime_str);
			}else{
				// Establece el locale a español
				setlocale(LC_TIME, "es_PE.UTF-8");
					
				// Obtiene el tiempo actual
				time_t current_time;
				time(&current_time);

				// Convierte el tiempo actual a una estructura de tiempo
				struct tm *time_info = localtime(&current_time);

				// Buffer para almacenar la cadena de fecha y hora formateada
				char date_time_str[100];

				// Formatea la fecha y hora de acuerdo con el locale actual
				strftime(date_time_str, sizeof(date_time_str), "%A, %d de %B de %Y, %H:%M:%S", time_info);

				// Imprime la fecha y hora formateada
				printf("\t%s", date_time_str);
			}			
		}
		printf("\tErrores: %d",n_error);
		fflush(stdout);	// imprimo datos alamcenados en buffer
		printf("\n");
	}
}


int main(void){
    // accion de la funcion si finaliza el programa
    signal(SIGINT, programfinish);
	signal(SIGTERM, programfinish);
    if (wiringPiSetup() == -1) {
        printf("WiringPi setup failed.\n");
        return 1;
    }else{
	//printf("inicializado\n");
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
	//printf("inicio de bucle \n");
    for (;;){
	int* data_vector = lectura_a0a1a2();
	int ciclo_prev = ciclo;
		switch (ciclo){
			case 1:
				/* display 1 */			
				if (data_vector[0]==0 &&
					data_vector[1]==0 &&
					data_vector[2]==0){
					procedimiento();
				}
				break;
			case 2:
				/* display 2 */
				if (data_vector[0]==0 &&
					data_vector[1]==0 &&
					data_vector[2]==1){
					procedimiento();
				}	
				break;
			case 3:
				/* display 3 */
				if (data_vector[0]==0 &&
					data_vector[1]==1 &&
					data_vector[2]==0){
					procedimiento();
				}	
				break;
			case 4:
				/* led channel */
				if (data_vector[0]==0 &&
					data_vector[1]==1 &&
					data_vector[2]==1){
					procedimiento_channel();

				}
				break;
			default:
				/*	ciclos reales A2A1A0: 000 001 010 011 100 101 */
				/*	Important: Empezar en el ultimo ciclo imperdira que
					se haga mal lectura del primer ciclo */
				if (data_vector[0]==1 &&
					data_vector[1]==0 &&
					data_vector[2]==1){
					ciclo = 1;
				}
				break;
		}
	}
}