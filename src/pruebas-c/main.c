#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Variables de control globales
int continuar = 0;  // Indica si el hilo debe continuar (1) o detenerse (0)
int cancelar = 0;   // Indica si el hilo debe ser cancelado

// Definir el mutex y la variable de condición
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *threadFunction(void *arg) {
    printf("Hilo iniciado.\n");

    pthread_mutex_lock(&mutex);

    // Mientras no se cancele, el hilo espera a que se le permita continuar
    while (!cancelar) {
        while (!continuar) {  // Pausar el hilo hasta que 'continuar' sea 1
            printf("Hilo pausado.\n");
            pthread_cond_wait(&cond, &mutex);  // Espera a la señal para continuar o cancelar
        }

        if (cancelar) {
            break;  // Salir si se ha solicitado la cancelación
        }

        // Simular trabajo en el hilo
        printf("Hilo en ejecución.\n");
        sleep(2);  // Simula tiempo de ejecución del hilo

        // Resetear la bandera para volver a pausar el hilo
        continuar = 0;
    }

    pthread_mutex_unlock(&mutex);
    printf("Hilo terminado.\n");
    return NULL;
}

int main() {
    pthread_t thread;

    // Crear el hilo
    pthread_create(&thread, NULL, threadFunction, NULL);

    // Simular un tiempo antes de pausar y reanudar el hilo
    sleep(2);

    // Pausar y luego reanudar el hilo
    for (int i = 0; i < 3; i++) {
        // Reanudar el hilo
        pthread_mutex_lock(&mutex);
        continuar = 1;  // Permitir que el hilo continúe
        pthread_cond_signal(&cond);  // Despertar al hilo
        pthread_mutex_unlock(&mutex);

        sleep(3);  // Dar tiempo para que el hilo haga su trabajo antes de pausar de nuevo
    }

    // Solicitar la cancelación del hilo
    pthread_mutex_lock(&mutex);
    cancelar = 1;  // Señal para cancelar
    pthread_cond_signal(&cond);  // Despertar al hilo para que vea la señal de cancelación
    pthread_mutex_unlock(&mutex);

    // Esperar a que el hilo termine
    pthread_join(thread, NULL);

    printf("Hilo principal terminado.\n");
    return 0;
}
