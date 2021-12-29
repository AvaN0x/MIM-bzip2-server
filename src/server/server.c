#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <pthread.h>

#include "../shared/buffer.h"
#include "../shared/stream.h"
#include "server.h"

#include "process.h"

#include "../shared/font_colors.h"
#include "../shared/huffman/huff.h"
#include "../shared/bwt/bwt.h"
#include "../shared/m2f/m2f.h"
#include "../shared/rle/rle.h"

#include "../shared/huffman/count.h"

#ifdef TEST
#include "test.c"
#endif

/**
 * Function that create the socket and manage all clients connections
 * It also contains at the start, some tests for each encoder/decoder
 * @return exit status (EXIT_FAILURE || EXIT_SUCCESS)
 */
int main(int argc, char const *argv[])
{
    // Real server code
    int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        printf(FONT_RED "Incorrect socket\n" FONT_DEFAULT);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0x00, sizeof(struct sockaddr_in)); // allocate memory
    serverAddr.sin_family = PF_INET;                       // Set protocal family
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);        // set address
    serverAddr.sin_port = htons(PORT);                     // set address port

    if (bind(serverSocket, (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) == -1)
    {
        printf(FONT_RED "Bind error, PORT may already be in use.\n" FONT_DEFAULT);
        exit(EXIT_FAILURE);
    }
    if (listen(serverSocket, 5) == -1)
    {
        printf(FONT_RED "Listen error.\n" FONT_DEFAULT);
        exit(EXIT_FAILURE);
    }

    printf(FONT_GREEN "Server is listening on port %d\n" FONT_DEFAULT, PORT);
    // Main loop
    int sockaddr_in_size = sizeof(struct sockaddr_in);

#ifdef TEST
    test(serverSocket, sockaddr_in_size);
#endif

    while (1)
    {
        connectionStruct myConnectionStruct;

        // waiting for a client to connect
        if ((myConnectionStruct.communicationID = accept(serverSocket, (struct sockaddr *)&myConnectionStruct.connectedAddr,
                                                         (socklen_t *)&sockaddr_in_size)) != -1)
        {
            // create the thread that will handle the connection of this client
            pthread_t thread;
            pthread_create(&thread, NULL, connectionThread, (void *)&myConnectionStruct);
        }
        else
        {
            printf(FONT_RED "Connection acceptation error\n" FONT_DEFAULT);
        }
    }
    close(serverSocket);

    return EXIT_SUCCESS;
}

/**
 * The thread that will handle the connection
 * @param args instance of connectionStruct as (void *)
 * @return void*
 */
void *connectionThread(void *args)
{
    connectionStruct connection = *(connectionStruct *)args;
    printf("%d | Connected client : %s\n", connection.communicationID, inet_ntoa(connection.connectedAddr.sin_addr)); // display client IP

    // function that will manage the client
    clientConnected(connection.communicationID);

    close(connection.communicationID);
    pthread_exit(NULL);
}

/**
 * Call function that manage the client connection
 * @param communicationID the id of the communication
 */
void clientConnected(int communicationID)
{
    stream_t stream = create_stream(); // stream that is used with this client
    char serStream[STREAM_SIZE];       // serialized stream
    size_t serStreamSize;              // buffer that contain the serialized stream
    bool loop = 1;
    char fileName[BUFFER_SIZE];

    while (loop)
    {
        // wait to receive a message from the client
        int bufSize = recv(communicationID, serStream, STREAM_SIZE, 0);
        if (bufSize < 1)
        {
            loop = 0;
            continue;
        }

        unserialize_stream(serStream, &stream);

        // check the type of the stream
        switch (stream.type)
        {
        case END_CONNECTION:
            // stop the loop that manage the client
            loop = 0;
            break;

        case SEND_FILE_NAME:
            memcpy(fileName, (char *)stream.content, BUFFER_SIZE);
            char *tmpFileName = (char *)malloc(sizeof(char) * (strlen(fileName) + 4));
            strcpy(tmpFileName, "res/");
            strcat(tmpFileName, fileName);

            // check if the file exist
            if (!access(tmpFileName, R_OK) == 0)
            {
                // File does not exist
                free(tmpFileName);
                init_stream(&stream, FILE_DO_NOT_EXIST); // tell the client that the file do not exist
                serStreamSize = serialize_stream(&stream, serStream);
                send(communicationID, serStream, serStreamSize, 0); // send buffer to client
                break;
            }
            // File exist
            init_stream(&stream, FILE_EXIST); // tell the client that the file do not exist
            serStreamSize = serialize_stream(&stream, serStream);
            send(communicationID, serStream, serStreamSize, 0); // send buffer to client

            // Process the file, this will send it to the client
            printf("%d | Client requested existing file \"%s\" !\n", communicationID, tmpFileName);
            processFileForClient(tmpFileName, communicationID, &stream, serStream);
            printf("%d | File sent to client\n", communicationID);

            // NULL_CONTENT means that the file sending is finished
            init_stream(&stream, NULL_CONTENT);
            serStreamSize = serialize_stream(&stream, serStream);
            send(communicationID, serStream, serStreamSize, 0); // send buffer to client

            free(tmpFileName);
            break;
        default:
            break;
        }
    }

    printf("%d | Client disconnected\n", communicationID);
    destroy_stream(&stream);
}
