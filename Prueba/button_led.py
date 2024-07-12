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
    # Variables
led=4
button=17
    # Configurar los pines de entrada y salida
GPIO.setup(led, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(button, GPIO.IN)
    # Variable auxiliar para guardar el estado del Led
state_led= 0
state_button_prev = 0
    
    # ciclo infinito para iniciar la intermitencia del led
while True:
    # boton configurado en pull-up
    sleep(0.0001) # 100us de rebote al pulsar boton
    state_button = GPIO.input(button)
    if (not state_button)==1 and state_button_prev==0:
        state_led = 1 - state_led
        if state_led == 1: print("led encendido")
        if state_led == 0: print("led apagado")
    state_button_prev = not state_button
    if state_led==1:
        GPIO.output(led,1)
    else:
        GPIO.output(led,0)
    
# Si ocurre un error al ejecutar try: se ejecutara except:
# Esto para evitar que el programa se detenga
try:
    # Mantener el programa en ejecución
    while True:
        time.sleep(1)

except KeyboardInterrupt:
    # Limpiar los pines GPIO
    GPIO.cleanup()
