# Importa la libreria  de contorl del GPIO de la Raspberry Pi
import RPi.GPIO as GPIO
#Importa la funcion sleep del modulo time
from time import sleep

    #Desactivar advertencias (warnings) opcional
#GPIO.setwarnings(False)
    #Configurar la libreria para usar el numero de pin
    #Llame GPIO.setmode(GPIO.BCM) para usar el canal SOC definido por Broadcom (GPIOx)
    #Llame GPIO.setmode(GPIO.BOARD) para usar la numeracion fisica de los 40 pines
GPIO.setmode(GPIO.BCM)
    #Configurar el pin 7 (GPIO4) como salida y habilitar en bajo
GPIO.setup(4, GPIO.OUT, initial=GPIO.LOW)

    # El siguiente codigo hace parpadear el led
while True: #Bucle infinito
    sleep(0.5) # Espera 500ms
    GPIO.output(4, GPIO.HIGH) # Enciende el led
    print("led encendido")
    sleep(0.5) # Espera 500ms
    GPIO.output(4, GPIO.LOW) # Apaga el led
    print("led apagado")
    
# Si ocurre un error al ejecutar try: se ejecutara except:
# Esto para evitar que el programa se detenga
try:
    # Mantener el programa en ejecución
    while True:
        time.sleep(1)

except KeyboardInterrupt:
    # Limpiar los pines GPIO
    GPIO.cleanup()
