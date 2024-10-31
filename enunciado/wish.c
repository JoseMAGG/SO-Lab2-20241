#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parseInput(char *input, char *inputArray[])
{
    char *token;
    int i = 0;

    // Obtener el primer token
    token = strtok(input, " ");

    // Iterar sobre el resto de los tokens
    while (token != NULL)
    {
        inputArray[i++] = token; // Guardar el token en el array
        token = strtok(NULL, " ");
    }
    return i;
}

int main(int argc, char *argv[])
{
    // Inicialización de variables globales del programa
    const char *mypath[] = {
        "./",
        "/usr/bin/",
        "/bin/",
        NULL};

    char input[51];
    char *inputArray[20]; // Suponiendo un máximo de 20 palabras

    while (1)
    {
        // Inicialización de variables interactivas
        char *command = malloc(10 * sizeof(char));
        if (command == NULL)
            return 1;

        /* Wait for input */
        printf("wish> ");
        gets(input);

        if (strcmp(input, "exit") == 0)
            exit(0);

        int arrayLength = parseInput(input, inputArray);
        // Imprime los resultados
        for (int j = 0; j < arrayLength; j++)
        {
            printf("%s\n", inputArray[j]);
        }
        command = inputArray[0];
        if (strcmp(command, "cd") == 0)
        {
            if (arrayLength != 2) // Si el comando es cd sólo se debe ingresar un argumento
                exit(1);
            int cdResult = chdir(inputArray[2]);
            printf("Resultado de cd %d\n", cdResult);
            if (cdResult != 0)
            {
                printf("Error al ejecutar cd\n");
                exit(1);
            }
        }
        printf("Command not recognized: %s\n", input);

        // fgets(...);

        /* If necessary locate executable using mypath array */
        /* Launch executable */
        // if (fork() == 0)
        // {
        //     ... execv(...);
        //     ...
        // }
        // else
        // {
        //     wait(...);
        // }
    }
}
