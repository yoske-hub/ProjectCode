#include <wiringPi.h>

int contador = 0;

int main(){

	wiringPiSetup();
	pinMode(1, OUTPUT);
	pinMode(7, INPUT);
	for(;;){
		contador++;
		if(contador > 100){
		break;
		}
		else{
			digitalWrite(0,HIGH);
			delay(500);
	
			digitalWrite(0, LOW);
			delay(500);
		}
	}
	return 0;
}
