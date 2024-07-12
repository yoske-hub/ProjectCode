#include <stdio.h>
#include <unistd.h>
// este codigo elimina una linea impresa
// SI no funca :v, guardar todo en el buffer y borrar para sobreescribir
int main(void){
    // //
        // imprimir
        printf("Hola mundox alskdhaklshlkbahslkjahbsrklj aklsj ajsfhlka jsh kas \t \t asdasdas");
        fflush(stdout);

        // Esperar un momento para demostrar el cambio
        sleep(2);

        // Mover el cursor una línea arriba y al inicio de la línea
        printf("\x1b[A"); // Secuencia de escape ANSI para mover el cursor una línea arriba
        printf("\rHola mundo\n"); // Sobrescribir con la nueva línea
        //fflush(stdout); // Asegurarse de que la salida se imprima inmediatamente

        return 0;

}