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
                printf(FONT_RED "\n/!\\ Erreur lors de la récupération du contenu du fichier.\n" FONT_DEFAULT);
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
        printf("\nLe fichier demandé a bien été trouvé, son contenu va s'afficher ci-dessous :\n");

        while (stream->type != NULL_CONTENT)
        {
            // printf("%s", stream->content);
            int idx;
            int charFrequences[128];
            unsigned char *encodedBZIP2;
            uint64_t encodedBZIP2Size;

            // Get idx
            if (stream->type != INT_CONTENT)
                return false;
            idx = *(int *)stream->content;
            printf("idx : %d\n", idx);

            // Get charFrequences

            // Get encodedBZIP2 and encodedBZIP2Size
            bufSize = recv(fdSocket, serStream, STREAM_SIZE, 0);
            if (bufSize < 1)
                return false;
            unserialize_stream(serStream, stream);
            if (stream->type != SEND_GZIP2_STRING)
                return false;
            encodedBZIP2 = (unsigned char *)stream->content;
            encodedBZIP2Size = stream->contentSize;

            printf("(%ld) \"", encodedBZIP2Size);
            for (int i = 0; i < encodedBZIP2Size; i++)
                printf("%u ", encodedBZIP2[i]);
            printf("\"\n");

            // Receive another time for next while iteration
            bufSize = recv(fdSocket, serStream, STREAM_SIZE, 0);
            if (bufSize < 1)
                return false;
            unserialize_stream(serStream, stream);
        }
        break;
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