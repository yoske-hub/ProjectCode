import time

tiempo_inicio = time.time()
time.sleep(0.000001)
tiempo_fin = time.time()
tiempo_fin2 = time.time()
tiempo_transcurrido= tiempo_fin - tiempo_inicio
tiempo_transcurrido2= tiempo_fin2 - tiempo_fin
print("Tiempo transcurrido:", tiempo_transcurrido, "s")
print("Tiempo transcurrido2:", tiempo_transcurrido2, "s")
