#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Función que devuelve un vector de tres números aleatorios del 1 al 9
int* numeros() {
    static int nums[3];

    // Genera tres números aleatorios del 1 al 9 y los guarda en el vector
    for (int i = 0; i < 3; ++i) {
        nums[i] = rand() % 9 + 1;
    }
    return nums;
}

// Función que multiplica los números de dos vectores
void multiplicar(int* vectorx) {    
    int vectorNumeros1[]={vectorx[0],vectorx[1],vectorx[2]};

    numeros();
    int vectorNumeros2[]={vectorx[0],vectorx[1],vectorx[2]};

    printf("VectorNumeros1: %d, %d, %d\n", vectorNumeros2[0], vectorNumeros2[1], vectorNumeros2[2]);

    // Comparar ambos vectores
    bool sonIguales = true;
    for (int i = 0; i < 3; ++i) {
        if (vectorNumeros1[i] != vectorNumeros2[i]) {
            sonIguales = false;
            break;
        }
    }

    if (sonIguales) {
        printf("Los vectores son iguales.\n");
    } else {
        printf("Los vectores son diferentes.\n");
    }

    // Multiplicar de manera escalar ambos vectores
    int resultado[3];
    for (int i = 0; i < 3; ++i) {
        resultado[i] = vectorNumeros1[i] * vectorNumeros2[i];
    }

    // Mostrar el resultado de la multiplicación escalar
    printf("El resultado de la multiplicación escalar es: %d, %d, %d\n",
           resultado[0], resultado[1], resultado[2]);
}

int main() {
    // Inicializa la semilla para los números aleatorios
    srand(time(NULL));
    int* vectorNumeros1=numeros();
    
    
    //int vectorNumeros1[] = numeros();

    // Mostrar los números del primer vector
    printf("VectorNumeros1: %d, %d, %d\n", vectorNumeros1[0], vectorNumeros1[1], vectorNumeros1[2]);

    // Llamada a la función para multiplicar los números
    multiplicar(vectorNumeros1);

    return 0;
}


// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <unistd.h>
// // #include <wiringPi.h>

// // Función que genera un vector de tres números aleatorios del 1 al 9
// // 1 o 2;
// int* numeros(int nume) {
//     static int resultado[6]={0,0,0,0,0,0};
//     int init_i = 0;
//     int fin_i = 0;
//     if (nume==0){
//         init_i=0;
//         fin_i=3;
//     }else{
//         init_i=3;
//         fin_i=6;
//     }
    
//     // Inicializar la semilla para generar números aleatorios

//     for (int i = init_i; i < fin_i; ++i) {
//         resultado[i] = rand() % 9 + 1; // Números aleatorios del 1 al 9
//         //sleep(1);
//     }
//     return resultado;
// }

// // Función para imprimir un vector
// void imprimirVector(const char* nombre, int* vector) {
//     printf("%s: %d, %d, %d\n", nombre, vector[0], vector[1], vector[2]);
// }

// int main() {
//     // Definir dos vectores para almacenar los resultados
//     int* num;
//     int vector1[3];
//     int vector2[3];

//     // Llamada a la función para obtener el primer vector de números
//     num = numeros(0);
//     vector1[0] = num[0];
//     vector1[1] = num[1];
//     vector1[2] = num[2];

//     // Mostrar el primer vector
//     imprimirVector("Vector1", vector1);

    
//     // Llamada a la función para obtener el segundo vector de números
//     num = numeros(1);
//     vector2[0] = num[3];
//     vector2[1] = num[4];
//     vector2[2] = num[5];

//     // Mostrar el segundo vector
//     imprimirVector("Vector2", vector2);
//     printf("num: %d, %d, %d, %d, %d, %d;\n", num[0], num[1], num[2], num[3], num[4], num[5]);


//     // Comprobación de igualdad
//     if (vector1[0] == vector2[0] && vector1[1] == vector2[1] && vector1[2] == vector2[2]) {
//         printf("Los vectores son iguales.\n");
//     } else {
//         printf("Los vectores son diferentes.\n");
//     }

//     return 0;
// }