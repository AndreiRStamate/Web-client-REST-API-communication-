#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <iostream>
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include <string>
#include "requests.h"

// computes and returns a GET request string + cookie
std::string ComputeGetRequest(const char* host, const char* url, std::string cookie) {
    // char* care contine toata cererea
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    // char* cu care se construieste mesajul "linie cu linie"
    char *line = (char*) calloc(LINELEN, sizeof(char));

    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (!cookie.empty()) {
        sprintf(line, "Cookie: %s", cookie.c_str());
        compute_message(message, line);
    }

    compute_message(message, "");

    free(line);
    return message;
}

// computes and returns a GET request string + JWT
std::string ComputeGetAuthRequest(const char* host, const char* url, std::string JWT) {
    // char* care contine toata cererea
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    // char* cu care se construieste mesajul "linie cu linie"
    char *line = (char*) calloc(LINELEN, sizeof(char));

    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (!JWT.empty()) {
        sprintf(line, "Authorization: Bearer %s", JWT.c_str());
        compute_message(message, line);
    }

    compute_message(message, "");

    free(line);
    return message;
}

// computes and returns a POST request string (cookie can be NULL if not needed)
std::string ComputePostRequest(const char* host, const char* url, const char* contentType,
                                 std::string data, std::string cookie) {
    // char* care contine toata cererea
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    // char* cu care se construieste mesajul "linie cu linie"
    char *line = (char*) calloc(LINELEN, sizeof(char));
    // char* in care se copiaza continutul json-ului dat ca string in data
    char *body_data_buffer = (char*) calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (!data.empty()) {
        sprintf(line, "Content-Type: %s", contentType);
        compute_message(message, line);

        strcat(body_data_buffer, data.c_str());

        int len = data.length();

        sprintf(line, "Content-Length: %d", len);
        compute_message(message, line);
    }
    if (!cookie.empty()) {
        sprintf(line, "Cookie: %s", cookie.c_str());
        compute_message(message, line);
    }
    compute_message(message, "");
    
    compute_message(message, body_data_buffer);

    free(body_data_buffer);
    free(line);
    return message;
}

// computes and returns a POST request string (JWT can be NULL if not needed)
std::string ComputePostAuthRequest(const char* host, const char* url, const char* contentType,
                                 std::string data, std::string JWT) {
    // char* care contine toata cererea
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    // char* cu care se construieste mesajul "linie cu linie"
    char *line = (char*) calloc(LINELEN, sizeof(char));
    // char* in care se copiaza continutul json-ului dat ca string in data
    char *body_data_buffer = (char*) calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", contentType);
    compute_message(message, line);

    strcat(body_data_buffer, data.c_str());

    int len = data.length();

    sprintf(line, "Content-Length: %d", len);
    compute_message(message, line);

    if (!JWT.empty()) {
        sprintf(line, "Authorization: Bearer %s", JWT.c_str());
        compute_message(message, line);
    }
    compute_message(message, "");
    
    compute_message(message, body_data_buffer);

    free(body_data_buffer);
    free(line);
    return message;
}

// computes and returns a DELETE request string (JWT can be NULL if not needed)
std::string ComputeDeleteAuthRequest(const char* host, const char* url, std::string JWT) {
    // char* care contine toata cererea
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    // char* cu care se construieste mesajul "linie cu linie"
    char *line = (char*) calloc(LINELEN, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (!JWT.empty()) {
        sprintf(line, "Authorization: Bearer %s", JWT.c_str());
        compute_message(message, line);
    }
    compute_message(message, "");

    free(line);
    return message;

}