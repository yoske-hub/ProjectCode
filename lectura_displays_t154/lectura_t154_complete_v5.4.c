/* 	PROGRAMA DESARROLLADO 13/06/2024 PARA LA RASPBERRY PI
 	USANDO UN ACOPLAMIENTO DE VOLTAJES PARA LOS GPIO CON EL MOSFET 2N7000
 	DESTINADO A USARSE CON LA PLACA SUPERIOR DEL T-154 CON LOS PINES NOMBRADOS A CONTINUACION
*/

// LECTURA DE DATOS DEL CONTROLADOR T-154.
// VERSION ACTUAL:	-------------	v5.1   ---------------------
// Codigo reconstruido desde v4.0

/* *** ----- Cambios v5.0 ----- ***
	- Internamente lectura_t154 ya no se ejecuta mas 20:29 23/06/2024
	- Nuevo formato de salida de datos y adicion de datos completa: 	
		Si se encuentra en modo scan se mostrara (4) lineas:
			(1)\t	"Canal"	\t	"Temp"	\t	"Trip"	\t	"Alarm"	\t	"Fan"	\t	"Fault"	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(2)\t	"Canal"	\t	"Temp"	\t	"Trip"	\t	"Alarm"	\t	"Fan"	\t	"Fault"	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(3)\t	"Canal"	\t	"Temp"	\t	"Trip"	\t	"Alarm"	\t	"Fan"	\t	"Fault"	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(4)*linea en blanco*
		o si no esta en el modo scan se mostrara (2) lineas:
			(1)\t	Error: Fuera de Modo SCAN	\t	Modo actual: "xx" \t\t\t\t	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(2)*linea en blanco*
	- Eliminacion de debug, ya no se habilitara la opcion de mostrar tiempos de ejecucion de instrucicones. Internamente se sigue ejecutando
	- Nuevo Diagrama de Flujo
							⬭	--INICIO DEL PROGRAMA--
		╔═══╦════════════>	□	Leer modo
		║	║				◇	¿Modo Scan?		═══ no ═══> □	"Mensaje Modo Actual" ═══> ⬭ --FIN DEL PROGRAMA--	
		║	║				□	Leer canal, guardar canal
		║	╠══ no ═══════	◇	¿Canal nuevo?
		║	║	╔════════>	□	Leer display 1
		║	║	╠══ no ═══	◇	¿Verificacion OK?
		║	║	║			□	Leer display 2
		║	║	╠══ no ═══	◇	¿Verificacion OK?
		║	║	║			□	Leer display 3
		║	║	╚══ no ═══	◇	¿Verificacion OK?
		║	║	╔════════>	□	Leer alertas
		║	║	╚══ no ═══	◇	¿Verificacion OK?
		║	║				□	Leer canal
		║	╚══════ no ═══	◇	¿Mismo Canal?
		║					□	Canal actual CHECK
		║					□	Acumular buffer: canal, temperatura, alertas, hora y fecha, errores
		╚══════ no ═══════	◇	¿Canales leidos = 3?
							□	Imprimir buffer acumulado
							⬭	--FIN DEL PROGRAMA--
		** Cada vez que una verificacion es fallida o error, se vuelve a consultar el MODO SCAN, pero no se incluye en el diagrama para disminuir la complejidad 

*** ----- Parche v5.1 ----- *** 
	- Adicion de tiempo maximo de operacion. funcion: tiempo_max()
		Si se excede el tiempo maximo de ejecucion se mostrará (2) lineas:
			(1)\t	Error: Tiempo máximo de ejecución de "xx"s excedido.	\t	Tiempo transcurrido: "xx" \t\t\t\t	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(2)*linea en blanco*
*** ----- Parche v5.2 ----- *** 
	- Anteriormente se leian 3 de los 4 canales existentes. Ahora se establece que los unicos canales fisicos legibles son 1, 2, 3.
*** ----- Parche v5.3 ----- *** 29/06/2024
	- Se corrigio el error de falsa lectura en la funcion "scan_mode" por error de desfase. while -> if (default case)
	- Se adicionó el indicador de CT a hacer lectura. Se muestra al inicio de la fila en la salida. Variable manipulable "CT_num".
		Si se encuentra en modo scan se mostrara (4) lineas:
			(1)"CT"	\t	"Canal"	\t	"Temp"	\t	"Trip"	\t	"Alarm"	\t	"Fan"	\t	"Fault"	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(2)"CT"	\t	"Canal"	\t	"Temp"	\t	"Trip"	\t	"Alarm"	\t	"Fan"	\t	"Fault"	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(3)"CT"	\t	"Canal"	\t	"Temp"	\t	"Trip"	\t	"Alarm"	\t	"Fan"	\t	"Fault"	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(4)*linea en blanco*
		o si no esta en el modo scan se mostrara (2) lineas:
			(1)"CT"	\t	Error: Fuera de Modo SCAN	\t	Modo actual: "xx" \t\t\t\t	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(2)*linea en blanco*
		Si se excede el tiempo maximo de ejecucion se mostrará (2) lineas:
			(1)"CT"	\t	Error: Tiempo máximo de ejecución de "xx"s excedido.	\t	Tiempo transcurrido: "xx" \t\t\t\t	\t	"Hora"	\t	"Fecha"	\t	"ErroresDeTiempo"	\t	"DesfasesDeLectura"
			(2)*linea en blanco*
*/


#include <wiringPi.h>	// usar los gpio
#include <signal.h>		// limpiar los gpio
#include <stdio.h>		// print, printf
#include <stdlib.h>		// funcion exit
#include <time.h>			// time_t
#include <locale.h>			// hora local
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

int etapa = 6;					//	 establece el inico de orden de lectura
int etapa_next=4;
int finish = 0;
const size_t FILAS = sizeof(caracteres7Segmentos) / sizeof(caracteres7Segmentos[0]);
const size_t COLUMNAS = sizeof(caracteres7Segmentos[0]) / sizeof(caracteres7Segmentos[0][0]);
int n_error=0;					// almacena numero de errores de tiempo al ejecutar. Son iguales a los re-intentos de lectura.
int n_desfase_error = 0;		// falla o desfase en lectura de canales
int channel = 0;
int ack_channel[]={0,0,0,0};	// se almacena los canales ya leidos y confirmados
int value_display[]={0,0,0};		// almacenamiento temporal de display
char buffer_canal[300];			// buffer temporal para datos acumulados de los canales
char buffer_display[10];		// buffer temporal para datos de display
char buffer_alert[10];			// buffer temporal para alertas

////////////////////	 variables manipulables	///////////////////////////////////////////
	/**/	const int CT_num = 1 ;		//	Variable de CT objetivo
const int tiempo_max = 10;		// (s) tiempo maximo de ejecucion de programa
const int asentamiento = 500;  	// (us) 1ms retraso anitrebote de lectura de cada display
const int it_a2a1a0 = 8;		//	iteraciones para confirmar lectura correcta de A0A1A2
const int it_7seg = 3;			//  iteraciones para confirmar lectura correcta de 7 segmentos
const int step_it = 15;			// tiempo en (us) intervalo entre cada iteracion
/////////////////
const int tiempo = 1;			// habilita el tiempo actual donde se obtiene la lectura  utiliza el formato H:M:S D/M/A
								
///////////////////////////////////////////////////////////////////////////////////////////

/*Definicion optima para delay en microsegundos*/
void delay_Micro(unsigned int us) {
    unsigned long start_time = micros(); // Obtenemos el tiempo actual en microsegundos
    while (micros() - start_time < us) {		
    }
}

void tiempo_maximo(unsigned long time_start){
	if (micros()-time_start > tiempo_max*1000000){
		double tiempo_transcurrido = (double)(micros()-time_start) / 1000000.0;
		printf("%u\tError: Tiempo máximo de ejecución de %ds excedido.\tTiempo transcurrido: %.6f",CT_num,tiempo_max, tiempo_transcurrido);
		printf("\t\t\t\t");
		if (tiempo)	{
			time_t current_time;
			time(&current_time);
			struct tm *time_info = localtime(&current_time);
			// Formatear la hora y fecha
			char datetime_str[20]; // Espacio suficiente para "HH:MM:SS DD/MM/YY" y el carácter nulo
			strftime(datetime_str, sizeof(datetime_str), "%H:%M:%S\t%d/%m/%y", time_info);
			printf("\t%s", datetime_str);			
		}
		printf("\t%u",n_error);
		printf("\t%u\n",n_desfase_error);
		printf("\n");
		exit(0);	
	}	
}

// A2_A1_A0
int* lectura_selector() {		
    static int data_vector[] = {0, 0, 0};	// PUNTERO
	int data_prev[] = {1, 1, 1};    
    int count = 0;
	///////////// confirmar lectura
    while (count < it_a2a1a0) { // 10 iteraciones de lectura para antirebote
		delay_Micro(step_it);
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
		delay_Micro(step_it);	
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
void procedimiento_display(int* d_selector){  
/*	Procedimiento anti-errores	*/
	unsigned long tempo[] ={0,0,0,0};
	unsigned long diff[] = {0,0,0};
		int selector_temp1[] = {d_selector[0],d_selector[1],d_selector[2]};
	tempo[3]=micros();
	delay_Micro(asentamiento);
	tempo[0]=micros();		
	int* estados = lectura7Segmentos(7);
		lectura_selector();
		int selector_temp2[] = {d_selector[0],d_selector[1],d_selector[2]};
		tempo[1]=micros();
	int caracter = decodificarCaracter(estados);
	
		tempo[2]=micros();	
		diff[0]=tempo[0]-tempo[3];
		diff[1]=tempo[1]-tempo[0];
		diff[2]=tempo[2]-tempo[1];
	/* Verificacion de tiempo: si es la duracion esta fuera de rango ↓*/
		if (diff[0]>asentamiento*1.6 || diff[1]>300 || diff[2]>150){
			n_error++;		// error de exceso de tiempo de procesamiento
			etapa = 0;		// etapa transitoria
			etapa_next = 1;	// volver a empezar lectura
				buffer_display[0] = '\0'; // vaciar buffer
			return;
		}

	/* Verificacion de selector/transistor: si es diferente ↓*/
	if (selector_temp1[0] != selector_temp2[0] ||
		selector_temp1[1] != selector_temp2[1] ||
		selector_temp1[2] != selector_temp2[2]){
		n_desfase_error++;		// error de exceso de tiempo de procesamiento
		etapa = 0;				// etapa transitoria
		etapa_next = 1;			// volver a empezar lectura
			buffer_display[0] = '\0'; // vaciar buffer
		return;
	}

/*	Procedimiento relevante de la funcion	*/
	if (etapa==1){
		buffer_display[0] = '\0' ;	// vaciar buffer cuando empieza a leer
	}	
	value_display[etapa-1]=caracter;
	if (etapa == 3){
			snprintf(buffer_display, sizeof(buffer_display), "\t%c%c%c",value_display[0],value_display[1],value_display[2]);
		etapa = 5; // despues de las 3 lecturas leemos las alertas
	} else{		
		etapa++;
	}
}

// procedimiento para canales
void procedimiento_channel(){	
/*	Procedimiento anti-errores	*/
	unsigned long tempo[] ={0,0,0,0};
	unsigned long diff[] = {0,0};
	// variable ack para confirmacion de lectura correcta entre canal y display's
	static int ack = 0;		// sintaxis.  ack(canal) lectura_display ack(canal) // 1:en confirmacion 0:confirmado
	tempo[3]=micros();
	delay_Micro(asentamiento);
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
	// Por otra parte el 4to led (canal 4) no puede ser leido.
	if ( (estados[0] + estados[1] + estados[2] + estados[3]) != 3 || estados[3] != 1){
		n_error++;			// error de estados de led de canal
		etapa = 0 ;			// etapa de transicion
		etapa_next = 4;		// reinicio de lectura de canal
		return;
	}else{

/*	Procedimiento relevante de la funcion	*/
		// Encontrar canal activo
		for (size_t i = 0; i < 4; i++){
			// anodo comun
			if (estados[i] == 0){
				if(ack_channel[i] == 0){	// CANAL NO CONFIRMADO
					if(ack == 0){		//	SIN VERIFICACION
						channel = i+1;
						ack = 1;
						etapa = 0;		// etapa de transicion
						etapa_next = 1;	// empezar lectura de displays
						return;
					}else{				//	VERIFICACION PENDIENTE 
						if (channel == i+1){	// VERIFICACION EXITOSA
							ack = 0;			// canal y lectura confirmada
							ack_channel[i] = 1;	// canal confirmado
							etapa = 0;
							etapa_next = 6;		// verificar modo scan
						}else{	// OCURRE DESFASE DE CANAL
							ack = 0;
							etapa = 0;		// etapa de transicion
							etapa_next = 4;	// reinicio, nueva lectura de canal completo
							n_desfase_error++;
							return;
						}						
					}					
				}else{		// CANAL CONFIRMADO
					etapa = 0;		// etapa de transicion
					etapa_next = 4;	// reinicio de lectura de canal
					return;
				}
			}
		}		
	}
		snprintf(buffer_canal + strlen(buffer_canal), sizeof(buffer_canal) - strlen(buffer_canal), "%u\t%u",CT_num,channel);
		strcat(buffer_canal, buffer_display);
		strcat(buffer_canal, buffer_alert);
		if (tiempo)	{
			time_t current_time;
			time(&current_time);
			struct tm *time_info = localtime(&current_time);

			// Formatear la hora y fecha
			char datetime_str[20]; // Espacio suficiente para "HH:MM:SS DD/MM/YY" y el carácter nulo
			strftime(datetime_str, sizeof(datetime_str), "%H:%M:%S\t%d/%m/%y", time_info);
				snprintf(buffer_canal + strlen(buffer_canal), sizeof(buffer_canal) - strlen(buffer_canal), "\t%s", datetime_str);
		}
		snprintf(buffer_canal + strlen(buffer_canal), sizeof(buffer_canal) - strlen(buffer_canal), "\t%u\t%u\n",n_error,n_desfase_error);
		n_error = 0;
		n_desfase_error = 0;
	    etapa = 0;					// etapa de transicion
		etapa_next = 4;				// en busqueda de nuevo canal
	if ((ack_channel[0] + ack_channel[1] + ack_channel[2]) == 3){
		printf("%s\n", buffer_canal);
		finish = 1;
	}
}

// procedimiento para detectar modo scan
int scan_mode(int* d_selector){
/*	Procedimiento anti-errores	*/
	unsigned long tempo[] ={0,0,0,0};
	unsigned long diff[] = {0,0,0};
	int selector_temp1[] = {d_selector[0],d_selector[1],d_selector[2]};
	tempo[3]=micros();
	delay_Micro(asentamiento);
	tempo[0]=micros();		
	int* estados = lectura7Segmentos(4);
	lectura_selector();
	int selector_temp2[] = {d_selector[0],d_selector[1],d_selector[2]};
	
	tempo[1]=micros();
	diff[0]=tempo[0]-tempo[3];
	diff[1]=tempo[1]-tempo[0];
	//////// verificacion de tiempo de operación.
	if (diff[0]>asentamiento*1.6 || diff[1]>300){
		n_error++;		// error de exceso de tiempo de procesamiento
		return 1;
	}

	/* Verificacion de selector/transistor: si es diferente ↓*/
	if (selector_temp1[0] != selector_temp2[0] ||
		selector_temp1[1] != selector_temp2[1] ||
		selector_temp1[2] != selector_temp2[2]){
		n_desfase_error++;		// error de exceso de tiempo de procesamiento
		return 1;
	}

/*	Procedimiento relevante de la funcion	*/
	if (estados[0]==0 &&		// scan
		estados[1]==1 &&		// auto
		estados[2]==1 &&		// man
		estados[3]==1){			// tmax		
	}else{
		printf("%u\tError: Fuera de Modo SCAN\tModo actual: ",CT_num);
		if ( (estados[0]+estados[1]+estados[2]+estados[3]) == 4){
			printf("Programación");
		}else if ((estados[0]+estados[1]+estados[2]+estados[3]) == 0){
			printf("Reinicio");			
		}else{
			if (estados[1]==0){printf("AUTO");}	
			if (estados[2]==0){printf("MAN");}	
			if (estados[3]==0){printf("T.MAX");}
		}		
			
		printf("\t\t\t\t");
		if (tiempo)	{
			time_t current_time;
			time(&current_time);
			struct tm *time_info = localtime(&current_time);
			// Formatear la hora y fecha
			char datetime_str[20]; // Espacio suficiente para "HH:MM:SS DD/MM/YY" y el carácter nulo
			strftime(datetime_str, sizeof(datetime_str), "%H:%M:%S\t%d/%m/%y", time_info);
			printf("\t%s", datetime_str);			
		}
		printf("\t%u",n_error);
		printf("\t%u\n",n_desfase_error);
		printf("\n");
		finish = 1;		
	}
	return 0;
}

// procedimiento para detectar alertas
void procedimiento_alert(int* d_selector){
/*	Procedimiento anti-errores	*/
	unsigned long tempo[] ={0,0,0,0};
	unsigned long diff[] = {0,0,0};
	int selector_temp1[] = {d_selector[0],d_selector[1],d_selector[2]};
	tempo[3]=micros();
	delay_Micro(asentamiento);
	tempo[0]=micros();
	int* estados = lectura7Segmentos(4);
	lectura_selector();
	int selector_temp2[] = {d_selector[0],d_selector[1],d_selector[2]};
	
	tempo[1]=micros();
	diff[0]=tempo[0]-tempo[3];
	diff[1]=tempo[1]-tempo[0];
	/* Verificacion de tiempo: si es la duracion esta fuera de rango ↓*/
	if (diff[0]>asentamiento*1.6 || diff[1]>300){
		n_error++;		// error de exceso de tiempo de procesamiento
		etapa = 0;				// etapa transitoria
		etapa_next = 5;			// volver a empezar lectura
			buffer_alert[0] = '\0'; // vaciar buffer}
		return;
	}

	/* Verificacion de selector/transistor: si es diferente ↓*/
	if (selector_temp1[0] != selector_temp2[0] ||
		selector_temp1[1] != selector_temp2[1] ||
		selector_temp1[2] != selector_temp2[2]){
		n_desfase_error++;		// error de exceso de tiempo de procesamiento
		etapa = 0;				// etapa transitoria
		etapa_next = 5;			// volver a empezar lectura
			buffer_alert[0] = '\0'; // vaciar buffer}
		return;
	}
	
/*	Procedimiento relevante de la funcion	*/
	buffer_alert[0] = '\0';

	int a_trip = 0;
	int a_alarm = 0;
	int a_fan = 0;
	int a_fault = 0;

	if (estados[3]==0){	a_trip = 1;}		// 	TRIP
	if (estados[2]==0){	a_alarm = 1;}		//	ALARM
	if (estados[1]==0){	a_fan = 1;}			//	FAN
	if (estados[0]==0){	a_fault = 1;}		//	FAULT

	snprintf(buffer_alert, sizeof(buffer_alert), "\t%d\t%d\t%d\t%d",a_trip,a_alarm,a_fan,a_fault);
	etapa = 0;			// etapa de transicion
	etapa_next = 4;		// verificar canal
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

	unsigned long time_start = micros();

    for (;;){
		delay_Micro(10);	
		/*	Tiempo maximo de ejecucion de programa	*/
		tiempo_maximo(time_start);			

		int* data_selector = lectura_selector();
		/* ordenado por etapas naturales del microcontrolador */
		switch (etapa){
			case 1:
				/* display 1 */							
				if (data_selector[0]==0 &&
					data_selector[1]==0 &&
					data_selector[2]==0){
					procedimiento_display(data_selector);
				}
				break;
			case 2:
				/* display 2 */
				if (data_selector[0]==0 &&
					data_selector[1]==0 &&
					data_selector[2]==1){
					procedimiento_display(data_selector);
				}	
				break;
			case 3:
				/* display 3 */
				if (data_selector[0]==0 &&
					data_selector[1]==1 &&
					data_selector[2]==0){
					procedimiento_display(data_selector);						
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
			case 5:
				/* lectura de alertas */
				if (data_selector[0]==1 &&
					data_selector[1]==0 &&
					data_selector[2]==0){
					procedimiento_alert(data_selector);
				}
				break;
			case 6:
				/* inicio de programa */
				if (data_selector[0]==0 &&
					data_selector[1]==0 &&
					data_selector[2]==0){
					etapa = 0;
				}
				break;			
			default:	// etapa transitoria scanmode
				/*	etapas reales A2A1A0: 000 001 010 011 100 101 // 111 es lectura de boton
					Importante: Empezar en una etapa donde no se ejecute nada para mnimizar errores */
				if (data_selector[0]==1 &&
					data_selector[1]==0 &&
					data_selector[2]==1){	
					if (scan_mode(data_selector) == 0){
						if (finish){						
							exit(0);
						}
						etapa = etapa_next;
					}
				}				
				break;			
		}
	}
}