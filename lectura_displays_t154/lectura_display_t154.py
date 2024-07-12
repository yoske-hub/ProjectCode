# PROGRAMA DESARROLLADO 24/04/2024 PARA LA RASPBERRY PI
# USANDO UN ACOPLAMIENTO DE VOLTAJES PARA LOS GPIO CON EL MOSFET 2N7000
# DESTINADO A USARSE CON LA PLACA SUPERIOR T154 CON LOS PINES NOMBRADOS A CONTINUACION

import RPi.GPIO as GPIO
import time

# Pines
pinDP = 10
pinG = 9
pinF = 11
pinE = 5
pinD = 6
pinC = 13
pinB = 19
pinA = 26

pinA0 = 16
pinA1 = 20
pinA2 = 21
pin1OE = 19
segment_pins = [pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP]

timedelay = 0.1  # (s) retraso de lectura del grupo de display
ciclo = 3         # establece un orden de lectura de los display
asentamiento = 0.0005  # (s) 0.5ms retraso de lectura de cada display para evitar los rebotes
#asentamiento_micro = 0.00001 # 10us

caracteres7Segmentos = [
    # a  b  c  d  e  f  g  ASCII
    [1, 1, 1, 1, 1, 1, 0, 48],   # 0
    [0, 1, 1, 0, 0, 0, 0, 49],   # 1
    [1, 1, 0, 1, 1, 0, 1, 50],   # 2
    [1, 1, 1, 1, 0, 0, 1, 51],   # 3
    [0, 1, 1, 0, 0, 1, 1, 52],   # 4
    [1, 0, 1, 1, 0, 1, 1, 53],   # 5
    [1, 0, 1, 1, 1, 1, 1, 54],   # 6
    [1, 1, 1, 0, 0, 0, 0, 55],   # 7
    [1, 1, 1, 1, 1, 1, 1, 56],   # 8
    [1, 1, 1, 1, 0, 1, 1, 57],   # 9
    [1, 1, 1, 0, 1, 1, 1, 65],   # A
    [0, 0, 1, 1, 1, 1, 1, 98],   # b min
    [0, 0, 0, 1, 1, 0, 1, 99],   # c min
    [0, 1, 1, 1, 1, 0, 1, 100],  # d min
    [1, 0, 0, 1, 1, 1, 1, 69],   # E
    [1, 0, 0, 0, 1, 1, 1, 70],   # F
    [1, 1, 1, 1, 0, 1, 1, 103],  # g min
    [0, 0, 1, 0, 1, 1, 1, 104],  # h min
    [0, 0, 1, 0, 0, 0, 0, 105],  # i min
    [0, 1, 1, 1, 1, 0, 0, 74],   # J
    [0, 0, 0, 1, 1, 1, 0, 76],   # L
    [0, 0, 1, 0, 1, 0, 1, 110],  # n min
    [0, 0, 1, 1, 1, 0, 1, 111],  # o min
    [1, 1, 0, 0, 1, 1, 1, 80],   # P
    [1, 1, 1, 0, 0, 1, 1, 81],   # Q
    [0, 0, 0, 0, 1, 0, 1, 114],  # r min
    [1, 0, 1, 1, 0, 1, 1, 83],   # S
    [0, 0, 1, 1, 1, 0, 0, 117],  # u min
    [0, 1, 1, 1, 0, 1, 1, 121],  # y min
    [0, 0, 0, 0, 0, 0, 1, 45],   # -
    [0, 0, 0, 0, 0, 0, 0, 0],    # caracter nulo
]

def setup():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(segment_pins, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup([pinA0, pinA1, pinA2, pin1OE], GPIO.IN, pull_up_down=GPIO.PUD_UP)    

def lectura_a0a1a2():
    data_prev = [1,1,1]
    count = 0
    #tiempo_inicio = time.time()
    while count < 10: # 10 iteraciones de lectura para antirebote
        #time.sleep(asentamiento_micro)
        data = [
            GPIO.input(pinA2),
            GPIO.input(pinA1),
            GPIO.input(pinA0)
        ]
        if data_prev == data:
            count += 1
        else:
            count = 0
        data_prev = data
    #print(f"data: {data}", end='')
    #tiempo_fin = time.time()
    #tiempo_transcurrido = tiempo_fin - tiempo_inicio
    #print("Tiempo transcurrido:", tiempo_transcurrido, "s")
    return data        
    

def lectura7Segmentos():
    data = [GPIO.input(pin) for pin in segment_pins]
    #data=GPIO.input(segment_pins)
    
    #data_prev = [1,1,1,1,1,1,1,1]
    #count = 0
    #while count < 2: # iteraciones de lectura para antirebote
    #    data = [GPIO.input(pin) for pin in segment_pins]
    #    if data_prev == data:
    #        count += 1
    #    else:
    #        count = 0
    #    data_prev = data    
    return data

def decodificarCaracter(estados):
    for caracter in caracteres7Segmentos:
        if all(not estados[i] == caracter[i] for i in range(7)):
            ascii_code = caracter[7]
            if estados[7] == 0:  # dp (punto) activo
                return chr(ascii_code) + '.'
            else:
                return chr(ascii_code)
    return '?'

def loop():
    global ciclo
    while True:
        if ciclo == 3:
            data = lectura_a0a1a2();
            if [0,0,0] == data:
                start_time=time.time()
                while (time.time()-start_time) < asentamiento:
                    pass
                #print("  ",time.time()-start_time," s1",end='  ')
                estados = lectura7Segmentos()
                caracter = decodificarCaracter(estados)
                print(caracter, end='')
                #print(data)
                ciclo = 1

        elif ciclo == 1:
            data = lectura_a0a1a2();
            if [0,0,1] == data:
                start_time=time.time()
                while (time.time()-start_time) < asentamiento:
                    pass
                estados = lectura7Segmentos()
                caracter = decodificarCaracter(estados)
                print(caracter, end='')
                #print(data)
                ciclo = 2

        elif ciclo == 2:
            data = lectura_a0a1a2();
            if [0,1,0] == data:
                start_time=time.time()
                while (time.time()-start_time) < asentamiento:
                    pass
                #print("  ",time.time()-start_time," s1",end='  ')
                estados = lectura7Segmentos()
                caracter = decodificarCaracter(estados)
                print(caracter)
                #print(data)
                ciclo = 3
                start_time=time.time()
                while (time.time()-start_time) < timedelay:
                    pass
                #print("  ",time.time()-start_time," s2")

if __name__ == '__main__':
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        GPIO.cleanup()
