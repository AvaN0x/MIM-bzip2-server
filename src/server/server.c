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

/**
 * Function that create the socket and manage all clients connections
 * It also contains at the start, some tests for each encoder/decoder
 * @return exit status (EXIT_FAILURE || EXIT_SUCCESS)
 */
int main(int argc, char const *argv[])
{
    //#region Tests
    // if (0)
    {
        // if (0)
        {
            processFile("lorem_ipsum.txt");
            // processFile("mississippi.txt");
        }
        //----------------------//
        // Read content of file //
        //----------------------//
        if (0)
        {
            char *fileName = "mississippi.txt";

            char *tmpFileName = (char *)malloc(sizeof(char) * (strlen(fileName) + 4));
            globalCounter++;
            strcpy(tmpFileName, "res/");
            strcat(tmpFileName, fileName);

            printf("tmpFileName: %s\n", tmpFileName);
            FILE *file = fopen(tmpFileName, "r");
            free(tmpFileName);
            tmpFileName = NULL;
            globalCounter--;

            if (!file)
            {
                fprintf(stderr, "File not found\n");
                exit(0); // TODO a bit too harsh, but it's a start
            }
            char c;

            printf("File content :\n");
            while ((c = fgetc(file)) != EOF)
            {
                printf("%c", c);
            }
            printf("\n");
            fclose(file);
        }
        printf("\n");

        //----------//
        // Test BWT //
        //----------//
        if (0)
        {
            printf(FONT_YELLOW "ENCODE BWT\n" FONT_DEFAULT);

            //! Don't use char that are not in the ASCII table !!
            char *S = "MISSISSIPPI";
            int len = strlen(S);

            char L[len + 1];
            int idx = encodeBWT(S, L);

            printf("%s\n", S);
            printf("idx = %d\n", idx);

            printf("%s\n", L);

            printf(FONT_YELLOW "DECODE BWT\n" FONT_DEFAULT);
            char Sdecoded[len + 1];
            decodeBWT(L, len, idx, Sdecoded);
            printf("%s\n", Sdecoded);
            printf("\n");
        }

        //----------//
        // Test RLE //
        //----------//
        if (0)
        {
            // char *S = "mississippi";
            // char *S = "122333444455555666666777777788888888999999999";
            char *S = "misssssssssssssisssssipppi";

            int len = strlen(S);

            char *Sencoded;
            int encodedLen;

            printf("(%d) %s\n", len, S);
            printf("\033[0;33mENCODE\033[0m\n");
            encodeRLE(S, len, &Sencoded, &encodedLen);

            printf("(%d) ", encodedLen);
            for (int i = 0; i < encodedLen; i++)
                printf("%c", Sencoded[i]);
            printf("\n");

            printf("\033[0;33mDECODE\033[0m\n");
            char *Sdecoded;
            int decodedLen;

            decodeRLE(Sencoded, encodedLen, &Sdecoded, &decodedLen);
            printf("(%d) %s\n", decodedLen, Sdecoded);
        }

        //----------//
        // Test M2F //
        //----------//
        if (0)
        {
            char *S = "PSSMIPISSII";
            // char *S = "mississippi";
            int len = strlen(S);

            char *shifts = (char *)calloc(len, sizeof(char));
            encodeM2F(S, len, shifts);

            printf(FONT_YELLOW "ENCODE M2F\n" FONT_DEFAULT);
            printf("%s\n", S);
            for (int i = 0; i < len; i++)
                printf("%c (%2d) = %2d '%c'\n", S[i], i, shifts[i], shifts[i]);

            printf(FONT_YELLOW "DECODE M2F\n" FONT_DEFAULT);
            char Sdecoded[len];
            decodeM2F(shifts, len, Sdecoded);
            printf("%s\n", Sdecoded);

            free(shifts);
            printf("\n");
        }

        //--------------//
        // Test huffman //
        //--------------//
        if (0)
        {
            printf(FONT_YELLOW "ENCODE HUFFMAN\n" FONT_DEFAULT);
            // const char *S = "exemple,de,codage,de,huffman";
            // const char *S = "Bonjour je m'appelle Luc et je veux tester mes fonctions parce que sinon je ne sais pas si cela fonctionne correctment.";
            const char *S = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras sed mauris non augue accumsan venenatis. Vivamus maximus luctus erat non vestibulum. Interdum et malesuada fames ac ante ipsum primis in faucibus. In vitae arcu congue, vehicula nulla nec, porta odio. Donec tempus nisi sed massa tempor, ut convallis lectus tempor. In lectus mauris, sollicitudin sed orci ut, eleifend faucibus diam. In condimentum dolor nec mauris pharetra tempor. In faucibus scelerisque eros eget bibendum. Maecenas pretium cursus";
            int dico[128] = {};
            char *HuffmanDico[128] = {};

            // ------ GET THE FREQUENCY OF EACH CARACTER ------ //
            int len = strlen(S);
            for (int i = 0; i < len; i++)
                dico[(int)S[i]]++;

            // ------ BUILD THE ODE FOR EACH CARACTER WHERE FREQ > 0 ------ //
            buildCodeHuffman(dico, HuffmanDico);

            // memory management debug
            // printf(FONT_YELLOW "AFTER BUILD HUFFMAN CODE GLOBAL COUNT IS %d\n" FONT_DEFAULT, globalCounter);

            // Print non null codes created
            // if (0)
            {
                for (int i = 0; i < 128; i++)
                    if (HuffmanDico[i] != NULL)
                        printf("%d|%c : %s\n", i, i, HuffmanDico[i]);
            }

            // ------ ENCODE THE STRING ------ //
            unsigned char *encodedHuffman;
            int encodedHuffmanSize;
            encodedHuffman = encodeHuffman(S, len, HuffmanDico, &encodedHuffmanSize);

            // memory management debug
            // printf(FONT_YELLOW "AFTER ENCODE GLOBAL COUNT IS %d\n" FONT_DEFAULT, globalCounter);

            /*
             * If you want to see the hexa code, open powershell terminal and type:
             * format-hex path_to_file/huffmanEncoded.bin
             * file is in the res folder
             */

            // Print decimal values of encoded string
            printf(FONT_YELLOW "\nencoded is \n" FONT_DEFAULT);
            for (int i = 0; i < encodedHuffmanSize; i++)
                printf("%u ", encodedHuffman[i]);
            printf("\n");

            // ------ DECODE THE STRING ------ //
            char *decodedHuffman;
            int decodedHuffmanSize;
            decodedHuffman = decodeHuffman(encodedHuffman, encodedHuffmanSize, HuffmanDico, &decodedHuffmanSize);
            printf(FONT_YELLOW "\ndecoded is \n" FONT_DEFAULT);
            for (int i = 0; i < decodedHuffmanSize; i++)
                printf("%c", decodedHuffman[i]);
            printf("\n");

            // ------ FREE MEMORY ------ //
            free(encodedHuffman);
            globalCounter--;
            encodedHuffman = NULL;

            free(decodedHuffman);
            globalCounter--;
            decodedHuffman = NULL;

            // Free codes in HuffmanDico
            for (int i = 0; i < 128; i++)
            {
                if (HuffmanDico[i] != NULL)
                {
                    free(HuffmanDico[i]);
                    globalCounter--;
                    HuffmanDico[i] = NULL;
                }
            }

            // memory management debug
            // printf(FONT_YELLOW "AFTER DECODE GLOBAL COUNT IS %d\n" FONT_DEFAULT, globalCounter);

            // Stats
            printf(FONT_YELLOW "\nStats\n" FONT_DEFAULT);
            printf("Initial string was occupying %ld bytes\n", len * sizeof(char));
            printf("Encoded string was occupying %ld bytes\n", encodedHuffmanSize * sizeof(unsigned char));
            printf("Ratio of compression is %d%%\n", 100 * encodedHuffmanSize / len);
        }

        // Print non null codes created
        if (0)
        {
            char test[] = "eéèe";
            printf("%s (len=%d)\n", test, (int)strlen(test));
            for (int i = 0; i < strlen(test); i++)
            {
                printf("%d: %c (%d)", i, test[i], test[i]);
                if (test[i] >= 0 && test[i] <= 127)
                    printf(" and is valid");
                printf("\n");
            }
            printf("\n");
        }
        printf("\n\n");
    }

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

            if (!access(tmpFileName, R_OK) == 0)
            {
                free(tmpFileName);
                init_stream(&stream, FILE_DO_NOT_EXIST); // tell the client that the file do not exist
                serStreamSize = serialize_stream(&stream, serStream);
                send(communicationID, serStream, serStreamSize, 0); // send buffer to client
                break;
            }
            printf("%d | Client requested existing file \"%s\" !\n", communicationID, tmpFileName);

            // FIXME that's temporary
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
