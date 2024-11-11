#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 100

// Función para mostrar un mensaje de error
void throwError() {
    char errorMessage[] = "An error has occurred\n";
    write(STDERR_FILENO, errorMessage, strlen(errorMessage));
}

// Función para ejecutar comandos integrados como cd, path y exit
int executeBuiltIn(char **args, char **path) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || args[2] != NULL) {
            throwError();
        } else {
            if (chdir(args[1]) != 0) {
                throwError();
            }
        }
        return 1;
    } else if (strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL) {
            throwError();
        } else {
            exit(0);
        }
        return 1;
    } else if (strcmp(args[0], "path") == 0) {
        int i = 1;
        for (i = 1; args[i] != NULL; i++);
        path[0] = NULL;  // Resetear las rutas
        for (i = 1; args[i] != NULL; i++) {
            path[i-1] = strdup(args[i]);
        }
        path[i-1] = NULL;
        return 1;
    }
    return 0;
}

// Función para ejecutar comandos con redirección
void executeCommand(char **args, char **path, char *outFile) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Proceso hijo
        if (outFile != NULL) {
            int fd = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (fd < 0) {
                throwError();
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }

        // Probar las rutas para encontrar el comando
        for (int i = 0; path[i] != NULL; i++) {
            char comando[256];
            snprintf(comando, sizeof(comando), "%s/%s", path[i], args[0]);
            execv(comando, args);
        }

        // Si llegamos aquí, execv falló
        throwError();
        exit(1);
    } else if (pid < 0) {
        // Error al crear el proceso hijo
        throwError();
    } else {
        // Proceso padre espera a que termine el hijo
        waitpid(pid, &status, 0);
    }
}

// Función para ejecutar varios comandos en paralelo
// Función para ejecutar varios comandos en paralelo
void executeInParallel(char **commandArray, char **path) {
    char *args[MAX_ARGS];
    char *outFile = NULL;
    pid_t pid;
    int i = 0;

    for (i = 0; commandArray[i] != NULL; i++) {
        // Parsear el comando individual
        int idx = 0;
        char *token = strtok(commandArray[i], " \t");

        // Si el primer token es NULL, significa que no hay comando
        if (token == NULL) {
            // En este caso, simplemente retornamos 0
            continue; // Esto hará que no se ejecute ningún comando.
        }

        // Guardamos el primer token para verificar si hay un comando
        args[idx++] = token;

        // Procesar los siguientes tokens
        while ((token = strtok(NULL, " \t")) != NULL) {
            if (strcmp(token, ">") == 0) {
                // Redirección de salida
                token = strtok(NULL, " \t");
                if (token == NULL || strtok(NULL, " \t") != NULL) {
                    throwError();
                    return;
                }
                outFile = token;
                break; // Salimos del bucle para no añadir más argumentos.
            }
            args[idx++] = token; // Agregar el token a args.
        }
        args[idx] = NULL; // Finalizar la lista de argumentos.

        // Verificar si hay al menos un comando antes de intentar ejecutarlo
        if (args[0] != NULL && !executeBuiltIn(args, path)) {
            // Si no es un comando integrado, ejecutar el comando en un nuevo proceso
            pid = fork();
            if (pid == 0) {
                // Proceso hijo
                executeCommand(args, path, outFile);
                exit(0); // Termina el hijo después de ejecutar el comando
            }
        }
    }

    // Esperar a que todos los procesos hijos terminen
    while (wait(NULL) > 0);
}



// Función principal para leer, parsear y ejecutar comandos
void interactiveLoop(char **path) {
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;

    while (1) {
        printf("wish> ");
        nread = getline(&line, &size, stdin);
        if (nread == -1) {
            break;
        }

        // Eliminar salto de línea
        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }

        // Dividir comandos por '&' para ejecución en paralelo
        char *commandArray[MAX_ARGS];
        int idx = 0;
        char *command = strtok(line, "&");
        while (command != NULL) {
            commandArray[idx++] = command;
            command = strtok(NULL, "&");
        }
        commandArray[idx] = NULL;

        // Ejecutar los comandos en paralelo
        executeInParallel(commandArray, path);
    }

    free(line);
}

// Modo batch
void batchMode(char *fileName, char **path) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        throwError();
        exit(1);
    }

    char *line = NULL;
    size_t size = 0;
    ssize_t nread;

    while ((nread = getline(&line, &size, file)) != -1) {
        // Eliminar salto de línea
        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }

        // Dividir comandos por '&' para ejecución en paralelo
        char *commandArray[MAX_ARGS];
        int idx = 0;
        char *command = strtok(line, "&");
        while (command != NULL) {
            commandArray[idx++] = command;
            command = strtok(NULL, "&");
        }
        commandArray[idx] = NULL;

        // Ejecutar los comandos en paralelo
        executeInParallel(commandArray, path);
    }

    free(line);
    fclose(file);
}

// Función principal
int main(int argc, char *argv[]) {
    // Definir rutas iniciales
    char *path[MAX_ARGS] = {"/bin", NULL};

    // Verificar argumentos
    if (argc == 1) {
        interactiveLoop(path);
    } else if (argc == 2) {
        // Modo batch
        batchMode(argv[1], path);
    } else {
        throwError();
        return 1;
    }

    return 0;
}
