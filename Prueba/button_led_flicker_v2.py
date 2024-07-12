# Importa la libreria  de contorl del GPIO de la Raspberry Pi
import RPi.GPIO as GPIO
#Importa la funcion sleep del modulo time
import time

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
intermitencia = 0
last_toogle_time = time.time()
    #######
    # el boton detiene la intermitencia del led en cualquier momento
    #######
    # ciclo infinito para iniciar la intermitencia del led
try:
    while True:
        # boton configurado en pull-up
        time.sleep(0.00015) # 150us de rebote al pulsar boton
        state_button = GPIO.input(button)
        if (not state_button)==1 and state_button_prev==0:
            intermitencia = not intermitencia
        state_button_prev = not state_button
        if intermitencia:
            if time.time()-last_toogle_time >= 0.5:
                last_toogle_time = time.time()
                state_led = not state_led
                GPIO.output(led,state_led)
                if state_led:
                    print("led intermitente encendido")
                else:
                    print("led intermitente apagado")
        else:
            pass    
    
# Si ocurre un error al ejecutar try: se ejecutara except:
# Esto para evitar que el programa se detenga
    # Mantener el programa en ejecución
    
except KeyboardInterrupt:
    # Limpiar los pines GPIO
    GPIO.cleanup()
