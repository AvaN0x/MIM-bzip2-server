#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "client.h"
#include "../shared/buffer.h"
#include "../shared/stream.h"
#include "../shared/font_colors.h"

#include "../shared/bzip2/bzip2.h"

#define ADDRESS "127.0.0.1"

/**
 * Main function that connect to the server and start the conversation
 * @return exit status (EXIT_FAILURE || EXIT_SUCCESS)
 */
int main()
{
    // Get the socket
    int fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSocket < 0)
    {
        printf(FONT_RED "Incorrect socket\n" FONT_DEFAULT);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverCoords;
    memset(&serverCoords, 0x00, sizeof(struct sockaddr_in)); // allocate memory
    serverCoords.sin_family = PF_INET;                       // Set protocal family
    inet_aton(ADDRESS, &serverCoords.sin_addr);              // put server address to our struct
    serverCoords.sin_port = htons(PORT);                     // set address port

    if (connect(fdSocket, (struct sockaddr *)&serverCoords, sizeof(serverCoords)) == -1)
    {
        printf(FONT_RED "Connection failed\n" FONT_DEFAULT);
        exit(EXIT_FAILURE);
    }
    printf(FONT_GREEN "Connected to %s:%d\n" FONT_DEFAULT, ADDRESS, PORT);

    // call the function that handle the connection
    connectedToServer(fdSocket);

    close(fdSocket);
    return EXIT_SUCCESS;
}

/**
 * Function that handle the client connection
 * @param fdSocket the socket of the communication
 */
void connectedToServer(int fdSocket)
{
    stream_t stream = create_stream(); // received stream

    size_t serStreamSize;           // variable that will contain the size of setStream
    char serStream[STREAM_SIZE];    // serialized stream
    char bufferString[BUFFER_SIZE]; // buffer for strings
    int8_t promptedInt;             // integer that is used when an integer is prompted to the client
    bool loop = 1;                  // loop that let the client do something until he leave

    do
    {
        printf(FONT_BLUE "\n*------- BZIP2 -------*" FONT_DEFAULT "\n" FONT_RED "0/" FONT_DEFAULT " Quitter\n1/ Demander un fichier\nChoix : ");
        promptedInt = promptInt(bufferString, BUFFER_SIZE, 0, 1); // prompt the client an int (choice)

        switch (promptedInt)
        {
        case 0:
            loop = 0;                             // set the loop at false, will disconnect the client
            init_stream(&stream, END_CONNECTION); // tell the server that this client left
            serStreamSize = serialize_stream(&stream, serStream);
            send(fdSocket, serStream, serStreamSize, 0); // send buffer to server
            break;
        case 1:
            if (!askForFile(fdSocket, &stream, bufferString, serStream))
            {
                printf(FONT_RED "\n/!\\ Erreur lors de la récupération du contenu du fichier.\nCela peut potentiellement casser le programme et nécessiter son relancement.\n" FONT_DEFAULT);
                // Clear recv buffer
                do
                {
                    recv(fdSocket, serStream, STREAM_SIZE, 0);
                    unserialize_stream(serStream, &stream);
                } while (stream.type != NULL_CONTENT);
            }
            break;
        default:
            break;
        }
    } while (loop == 1);

    destroy_stream(&stream);
}

/**
 * Function that handle when the client want to ask for a file from server
 * @param communicationID the id of the communication
 * @param stream the stream to send
 * @param bufferString the buffer that contain the string
 * @param serStream the buffer that will contain the serialized stream
 */
bool askForFile(int fdSocket, stream_t *stream, char *bufferString, char *serStream)
{
    size_t serStreamSize; // variable that will contain the size of setStream
    int bufSize;          // contain the return value of recv()

    do
    {
        printf("\nVeuillez saisir le nom du fichier voulu ('q' pour abandonner) : ");
        promptString(bufferString, BUFFER_SIZE);

        if (bufferString[1] == '\0' && (bufferString[0] == 'q' || bufferString[0] == 'Q'))
            break;

        init_stream(stream, SEND_FILE_NAME); // send file name to server
        set_content(stream, bufferString, 0);
        serStreamSize = serialize_stream(stream, serStream);
        send(fdSocket, serStream, serStreamSize, 0); // send buffer to server

        bufSize = recv(fdSocket, serStream, STREAM_SIZE, 0);
        if (bufSize < 1)
            return false;
        unserialize_stream(serStream, stream);

        if (stream->type == FILE_DO_NOT_EXIST)
        {
            printf("\nLe fichier demandé n'existe pas, veuillez réessayer.\n");
            continue;
        }
        printf(FONT_GREEN "\n> " FONT_DEFAULT "Le fichier demandé a bien été trouvé, son contenu va s'afficher ci-dessous :\n");

        char *filePath = (char *)malloc(sizeof(char) * (strlen(bufferString) + 5));
        strcpy(filePath, "serv_");
        strcat(filePath, bufferString);

#ifdef DEBUG_SEND_FILE
        // printf("filePath : %s\n", filePath);
#endif

        FILE *file = fopen(filePath, "w");
        if (!file)
        {
            fprintf(stderr, FONT_RED "\nLe programme n'a pas les droits pour créer des fichiers ici.\n" FONT_DEFAULT);
            return false;
        }

        // Get next values from server, this is outside of the while loop because there is also one at the end of the loop for the next iteration
        // this is to know when the file is over
        bufSize = recv(fdSocket, serStream, STREAM_SIZE, 0);
        if (bufSize < 1)
        {
            fprintf(stderr, FONT_RED "\nLe buffer reçu n'a pas de taille valide." FONT_DEFAULT);
            return false;
        }
        unserialize_stream(serStream, stream);

        printf(FONT_YELLOW "\"" FONT_DEFAULT);
        while (stream->type != NULL_CONTENT)
        {
            int32_t idxBWT;
            int32_t charFrequences[128];
            unsigned char *encodedBZIP2;
            uint64_t encodedBZIP2Size;

            // Get idx
            if (stream->type != INT_CONTENT)
            {
                fprintf(stderr, FONT_RED "\nLe stream reçu n'a pas le type attendu, attendu INT_CONTENT (%d), reçu %d." FONT_DEFAULT, INT_CONTENT, stream->type);
                return false;
            }
            idxBWT = *(int *)stream->content;

            // Tell the server that we received these data
            init_stream(stream, DATA_RECEIVED);
            serStreamSize = serialize_stream(stream, serStream);
            send(fdSocket, serStream, serStreamSize, 0); // send buffer to server

            // Get charFrequences
            bufSize = recv(fdSocket, serStream, STREAM_SIZE, 0);
            if (bufSize < 1)
            {
                fprintf(stderr, FONT_RED "\nLe buffer reçu n'a pas de taille valide." FONT_DEFAULT);
                return false;
            }
            unserialize_stream(serStream, stream);
            if (stream->type != SEND_CHAR_FREQUENCES)
            {
                fprintf(stderr, FONT_RED "\nLe stream reçu n'a pas le type attendu, attendu SEND_CHAR_FREQUENCES (%d), reçu %d." FONT_DEFAULT, SEND_CHAR_FREQUENCES, stream->type);
                return false;
            }
            memcpy(charFrequences, stream->content, 128 * sizeof(int32_t));

            // Tell the server that we received these data
            init_stream(stream, DATA_RECEIVED);
            serStreamSize = serialize_stream(stream, serStream);
            send(fdSocket, serStream, serStreamSize, 0); // send buffer to server

            // Get encodedBZIP2 and encodedBZIP2Size
            bufSize = recv(fdSocket, serStream, STREAM_SIZE, 0);
            if (bufSize < 1)
            {
                fprintf(stderr, FONT_RED "\nLe buffer reçu n'a pas de taille valide." FONT_DEFAULT);
                return false;
            }
            unserialize_stream(serStream, stream);
            if (stream->type != SEND_GZIP2_STRING)
            {
                fprintf(stderr, FONT_RED "\nLe stream reçu n'a pas le type attendu, attendu SEND_GZIP2_STRING (%d), reçu %d." FONT_DEFAULT, SEND_GZIP2_STRING, stream->type);
                return false;
            }
            encodedBZIP2 = (unsigned char *)stream->content;
            encodedBZIP2Size = stream->contentSize;

#ifdef DEBUG_SEND_FILE
            printf("idx : %d\n", idxBWT);
            for (int i = 0; i < 128; i++)
            {
                if (charFrequences[i] > 0)
                    printf("char(%2d) %c : %d\n ", i, i, charFrequences[i]);
            }

            printf("(%ld) \"", encodedBZIP2Size);
            for (int i = 0; i < encodedBZIP2Size; i++)
                printf("%u ", encodedBZIP2[i]);
            printf("\"\n");
#endif

            // All data are received, decode the data
            char *decodedString;
            int decodedStringSize; // Not needed here
            decodeBZIP2(encodedBZIP2, encodedBZIP2Size, idxBWT, charFrequences, &decodedString, &decodedStringSize);

#ifdef DEBUG_SEND_FILE
            printf(FONT_YELLOW "\"" FONT_DEFAULT);
#endif

            // Write content to console
            printf("%s", decodedString);
            // Write content to file
            fputs(decodedString, file);

#ifdef DEBUG_SEND_FILE
            printf(FONT_YELLOW "\"\n" FONT_DEFAULT);
#endif

            // Tell the server that we received these data
            init_stream(stream, DATA_RECEIVED);
            serStreamSize = serialize_stream(stream, serStream);
            send(fdSocket, serStream, serStreamSize, 0); // send buffer to server

            // Receive another time for next while iteration
            bufSize = recv(fdSocket, serStream, STREAM_SIZE, 0);
            if (bufSize < 1)
            {
                fprintf(stderr, FONT_RED "\nLe buffer reçu n'a pas de taille valide." FONT_DEFAULT);
                return false;
            }
            unserialize_stream(serStream, stream);
        }
        printf(FONT_YELLOW "\"\n" FONT_DEFAULT);

        printf(FONT_BLUE "\n> " FONT_DEFAULT "Le contenu du fichier a également été écrit dans le fichier suivant \"" FONT_BLUE "%s" FONT_DEFAULT "\" situé à l'endroit où vous avez exécuté le client.\n", filePath);

        fclose(file);
        free(filePath);
    } while (true);
    return true;
}

/**
 * Allow to enter text, and if the text entered is too long, then it will clear the buffer
 * @param buffer the buffer to fill
 * @param length the max length of the string
 * @return exit status (EXIT_FAILURE || EXIT_SUCCESS)
 */
int promptString(char *buffer, int length)
{
    if (fgets(buffer, length, stdin) != NULL)
    {
        char *lastCharPos = strchr(buffer, '\n');
        if (lastCharPos != NULL)
            *lastCharPos = '\0';
        else
            clearBuffer();
        return EXIT_SUCCESS;
    }
    clearBuffer();
    return EXIT_FAILURE;
}

/**
 * Allow to enter an int
 * @param buffer the buffer to fill
 * @param length the max length of the string
 * @param min min value
 * @param max max value
 * @return the prompted int
 */
int promptInt(char *buffer, int length, int min, int max)
{
    int ret;      // the variable that will contain the prompted int
    char *endPtr; // this will allow us to check if the client specified a number or not
    while (1)     // infinite loop until there is a return
    {
        promptString(buffer, length); // prompt a string to the buffer

        ret = (int)strtol(buffer, &endPtr, 10); // convert to int
        // if strtol did not found a matching int, or if the int is now in the interval, then we ask the client to prompt a new int
        if (buffer == endPtr || ret < min || ret > max)
            printf("Veuillez rentrer un entier entre %d et %d : ", min, max);
        else // else we return the good int
            return ret;
    }
}

/**
 * Function that clear the buffer of its content
 */
void clearBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
        c = getchar();
}