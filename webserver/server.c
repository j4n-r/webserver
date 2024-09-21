#include "server.h"

void* handle_connection(void* client_socket);

int main(int argc, char** argv) {
    int server_socket = 0, client_socket = 0, addr_size = 0;
    SA_IN server_addr = {}, client_addr = {};

    // make a new socket with ipv4 and tcp
    checkErr((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create socket");

    // zero out struct and fill declare family as ipv4, serverport INADDR_ANY ??
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    // bind socketto port
    checkErr(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Bind failed");
    // list on socket and declare how many connections can be in the queue
    checkErr(listen(server_socket, SERVER_BACKLOG), "Listen failed");

    // loop for new connections
    while (true) {
        printf("Waiting for connections ...\n");

        // get socklen for accept method
        addr_size = sizeof(SA_IN);
        // accept new connections and get back a new socket which with the new connection,
        // client_addr stays for new connections
        client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size);
        checkErr(client_socket, "accept failed");
        printf("Connected!\n");

        pthread_t newThread;
        int* pClientSocket = malloc(sizeof(int));
        *pClientSocket = client_socket;
        pthread_create(&newThread, NULL, handle_connection, pClientSocket);
        pthread_detach(newThread);
    }
}

void* handle_connection(void* pClientSocket) {
    int client_socket = *((int*)pClientSocket);
    free(pClientSocket);
    char responseBuffer[BUFSIZE + HEADERBUFSIZE] = {};
    char contentType[] = "text/html; charset=utf-8";
    char contentBuffer[BUFSIZE] = {};
    size_t contentLength = 0;
    char requestBuffer[BUFSIZE] = {};
    char pathBuffer[PATHBUFSIZE] = {};

    printRequest(client_socket, requestBuffer);

    //  get file path from request (url)
    getPathFromRequest(requestBuffer, pathBuffer);

    //  get content from file
    contentLength = readFile(contentBuffer, pathBuffer);

    parseContentTypeFromPath(pathBuffer, contentType);
    // construct http headers and get length of it
    size_t headerLength = constructHttpHeaders(responseBuffer, contentLength, contentType);

    // copy contenbuffer directly after the requestBuffer in memory
    memcpy(responseBuffer + headerLength, contentBuffer, contentLength);

    // send the responseBuffer to the socket
    send(client_socket, responseBuffer, headerLength + contentLength, 0);
    /*printf("*********************** Response *****************************\n %s\n********************************************\n", responseBuffer);*/
    // close everything
    close(client_socket);
    printf("Connection close\n");
    return NULL;
}

void parseContentTypeFromPath(const char* path, char* contentType) {
    char extension[10] = {};
    char js[] = "text/javascript; charset=utf-8";
    char html[] = "text/html; charset=utf-8";
    char css[] = "text/css; charset=utf-8";

    for (int i = 0, j = 0; (size_t)i < strlen(path); i++) {
        if (path[i] == '.') {
            while (path[i] != '\0' && j < 9) {
                extension[j++] = path[++i];
            }
            extension[j] = '\0';
            break;
        }
    }
    if (!strcmp(extension, "js")) {
        strncpy(contentType, js, strlen(js));

    } else if (!strcmp(extension, "css")) {
        strncpy(contentType, css, strlen(css));

    } else if (!strcmp(extension, "html")) {
        strncpy(contentType, html, strlen(html));
    }
    printf("Extension: %s\n", extension);
    printf("Content type: %s\n", contentType);
}

void getPathFromRequest(const char* request, char* pathBuffer) {
    char relativPathBuffer[PATHBUFSIZE] = {};
    char* relativPath = "frontend/";
    memcpy(relativPathBuffer, relativPath, strlen(relativPath));

    for (int i = 0, j = strlen(relativPathBuffer); i < PATHBUFSIZE; i++) {
        if (request[i] == '/') {
            while (request[i] != ' ' && j < BUFSIZE) {
                relativPathBuffer[j++] = request[i++];
            }
            break;
        }
    }
    if (realpath(relativPathBuffer, pathBuffer) == NULL) {
        perror("error resolving path");
        exit(1);
    }
    // TODO add checking for wrong direcotry
    printf("Relativ path: %s\n", relativPathBuffer);
    printf("Full path: %s\n", pathBuffer);
}

size_t constructHttpHeaders(char* contentBuffer, const size_t contentLength, const char* contentType) {
    time_t now = time(NULL);
    char date[128];
    if (strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now)) == 0)
        exit(1);
    snprintf(contentBuffer, HEADERBUFSIZE, "HTTP/1.1 200 OK\r\n"
                                           "Date: %s\r\n"
                                           "Content-Length: %zu\r\n"
                                           "Content-Type: %s\r\n"
                                           "Connection: keep-alive\r\n"
                                           "\r\n",
             date, contentLength, contentType);

    return strlen(contentBuffer);
}
