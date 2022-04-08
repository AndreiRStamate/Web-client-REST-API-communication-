#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"
#include <string>
#include <nlohmann/json.hpp>
#include <sstream>

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

// functie care creeaza un obiect json de forma
// {”username”: String,”password”: String}
std::string authJson(std::string username, std::string password) {
    nlohmann::json res;
    res["username"] = username;
    res["password"] = password;

    return res.dump();
}

// functie care creeaza un obiect json de forma
// {”id”: Number,”title”: String,”author”: String,”publisher”: String,”genre”: String,”page_count”: Number}
std::string bookJson(std::string title, std::string author, std::string genre,
                         int page_count, std::string publisher) {
    nlohmann::json res;
    res["title"] = title;
    res["author"] = author;
    res["genre"] = genre;
    res["page_count"] = page_count;
    res["publisher"] = publisher;

    return res.dump();
}

// functie care intoarce cookie-ul dintr-un string
std::string getCookie(std::string response) {
    std::stringstream ss;

    size_t cookiePos = response.find("Set-Cookie: ");

    // nu s-a primit cookie
    if (cookiePos == std::string::npos) {
        return "";
    }
    
    cookiePos += 12; // strlen("Set-Cookie: ") = 12
    size_t cookieEndPos = response.find(";", cookiePos);
    if (cookiePos != std::string::npos) {
        ss << response.substr(cookiePos, cookieEndPos-cookiePos);
    }

    // se returneaza tot cookie-ul (incepe de la cookiePos+12 si se termina la intalnirea ";")
    return ss.str();
}

// functie care intoarce JWT-ul dintr-un string
std::string getJWT(std::string response) {
    size_t jwtPos = response.find("{");

    // nu s-a primit JWT
    if (jwtPos == std::string::npos) {
        return "";
    }

    // se returneaza tot mesajul incepand cu "{"
    return response.substr(jwtPos);
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(const char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);

    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}
