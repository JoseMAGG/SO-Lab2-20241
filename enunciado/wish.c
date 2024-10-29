#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **parseInput(char *input)
{
    char *token;
    char *array[20]; // Suponiendo un máximo de 20 palabras
    int i = 0;

    // Obtener el primer token
    token = strtok(input, " ");

    // Iterar sobre el resto de los tokens
    while (token != NULL)
    {
        array[i++] = token; // Guardar el token en el array
        token = strtok(NULL, " ");
    }
    return array;
}

int main(int argc, char *argv[])
{
    const char *mypath[] = {
        "./",
        "/usr/bin/",
        "/bin/",
        NULL};

    char input[51];
    char **inputArray;

    /** Dentro del main del intérprete*/
    while (1)
    {
        char *command = NULL;
        /* Wait for input */
        printf("wish> ");
        gets(input);

        if (strcmp(input, "exit") == 0)
            exit(0);

        inputArray = parseInput(input);

        // Imprime los resultados
        for (int j = 0; j < 10; j++)
        {
            printf("%s\n", inputArray[j]);
        }

        if (strcmp(command, "cd") == 0)
        {
            printf("Cd recognized\n");
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
