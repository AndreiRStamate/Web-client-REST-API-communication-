#include <iostream>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include <string>
#include "requests.h"
#include "json.hpp"
#include <vector>

int main(int argc, char *argv[])
{
    // mesajul transmis de client catre server
    std::string message;
    // mesajul primit de client de la server
    std::string response;
    // socket server
    int sockfd;

    // string pentru comenzile date de utilizator
    std::string command;

    // cookie de logare
    std::string cookie;

    // JWT pentru acces in biblioteca
    std::string jwtToken;

    cookie.clear();
    jwtToken.clear();
    
    // logica programului
    loop {
        std::getline(std::cin, command);

        if (!command.compare("exit")) {
            // se inchide programul
            exit(0);
        } else if (!command.compare("register")) {
            std::cout << "username=";
            std::string username;
            std::getline(std::cin, username);

            std::cout << "password=";
            std::string password;
            std::getline(std::cin, password);

            // se deschide conexiunea catre server
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            
            // se creeaza o cerere de tip POST
            message = ComputePostRequest("34.118.48.238", "/api/v1/tema/auth/register", "application/json", authJson(username, password), "");
            std::cout << message << std::endl;

            char* c_message = (char*) calloc(BUFLEN, sizeof(char));;
            strcpy(c_message, message.c_str());

            // se trimite cererea sub forma de char* (c_message)
            send_to_server(sockfd, c_message);
            response = receive_from_server(sockfd);

            // se afiseaza raspunsul server-ului
            std::cout << response << std::endl;

            // se inchide conexiunea
            close_connection(sockfd);
        } else if (!command.compare("login")) {
            // se golesc variabilele pentru cookie si JWT
            cookie.clear();
            jwtToken.clear();
            
            std::cout << "username=";
            std::string username;
            std::getline(std::cin, username);

            std::cout << "password=";
            std::string password;
            std::getline(std::cin, password);

            // se deschide conexiunea catre server
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            
            // se creeaza o cerere de tip POST
            message = ComputePostRequest("34.118.48.238", "/api/v1/tema/auth/login", "application/json", authJson(username, password), "");
            std::cout << message << std::endl;

            char* c_message = (char*) calloc(BUFLEN, sizeof(char));;
            strcpy(c_message, message.c_str());

            // se trimite cererea sub forma de char* (c_message)
            send_to_server(sockfd, c_message);
            response = receive_from_server(sockfd);

            // se afiseaza raspunsul server-ului
            std::cout << response << std::endl;

            // se incearca parsarea cookie-ului de login
            cookie = getCookie(response);

            // se inchide conexiunea
            close_connection(sockfd);
        } else if (!command.compare("enter_library")) {
            // se deschide conexiunea catre server
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

            // se creeaza o cerere de tip GET cu cookie pentru demonstrarea autentificarii
            message = ComputeGetRequest("34.118.48.238", "/api/v1/tema/library/access", cookie);
            std::cout << message << std::endl;
            
            char* c_message = (char*) calloc(BUFLEN, sizeof(char));;
            strcpy(c_message, message.c_str());

            // se trimite cererea sub forma de char* (c_message)
            send_to_server(sockfd, c_message);
            response = receive_from_server(sockfd);

            // se afiseaza raspunsul server-ului
            std::cout << response << std::endl;

            if (!cookie.empty()) {
                // se incearca parsarea jwt-ului de acces in biblioteca
                nlohmann::json jwtResponse = nlohmann::json::parse(getJWT(response));
                if (jwtResponse["token"] != nullptr) {
                    jwtToken = jwtResponse["token"];
                }
            }

            // se inchide conexiunea
            close_connection(sockfd);
        } else if (!command.compare("get_books")) {
            // se deschide conexiunea catre server
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

            // se creeaza o cerere de tip GET cu jwt pentru demonstrarea accesului la biblioteca
            message = ComputeGetAuthRequest("34.118.48.238", "/api/v1/tema/library/books", jwtToken);
            std::cout << message << std::endl;
            
            char* c_message = (char*) calloc(BUFLEN, sizeof(char));;
            strcpy(c_message, message.c_str());

            // se trimite cererea sub forma de char* (c_message)
            send_to_server(sockfd, c_message);
            response = receive_from_server(sockfd);

            // se afiseaza raspunsul server-ului
            std::cout << response << std::endl;

            // se inchide conexiunea
            close_connection(sockfd);
        } else if (!command.compare("get_book")) {
            std::string id_string;
            bool ok = 0;
            // se tot citeste un id de la tastatura pana se introduce un int
            // exceptiile sunt prinse intr-un catch
            while (!ok) {
                try {
                    std::cout << "id=";
                    std::getline(std::cin, id_string);

                    stoi(id_string);
                    ok = true;
                    std::string aux_str = std::to_string(stoi(id_string));
                    if (id_string.length() != aux_str.length()) {
                        ok = false;
                        // text de forma {X}{...} (numar urmat de un sir de caractere)
                        std::cout << "id-ul trebuie sa fie un numar si numai un numar" << std::endl;
                    }
                } catch(...){
                    std::cout << "id-ul trebuie sa fie un numar" << std::endl;
                }
            }

            // se creeaza url-ul conform cerintei
            std::string url = "/api/v1/tema/library/books/" + id_string;

            // se deschide conexiunea catre server
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

            // se creeaza o cerere de tip GET cu jwt pentru demonstrarea accesului la biblioteca
            message = ComputeGetAuthRequest("34.118.48.238", url.c_str(), jwtToken);
            std::cout << message << std::endl;
            
            char* c_message = (char*) calloc(BUFLEN, sizeof(char));;
            strcpy(c_message, message.c_str());

            // se trimite cererea sub forma de char* (c_message)
            send_to_server(sockfd, c_message);
            response = receive_from_server(sockfd);
            
            // se afiseaza raspunsul server-ului
            std::cout << response << std::endl;

            // se inchide conexiunea
            close_connection(sockfd);
        } else if (!command.compare("add_book")) {
            std::cout << "title=";
            std::string title;
            std::getline(std::cin, title);

            std::cout << "author=";
            std::string author;
            std::getline(std::cin, author);

            std::cout << "genre=";
            std::string genre;
            std::getline(std::cin, genre);

            std::cout << "publisher=";
            std::string publisher;
            std::getline(std::cin, publisher);

            int page_count;
            bool ok = 0;

            // se tot citeste un "numar de pagini" de la tastatura pana se introduce un int
            // exceptiile sunt prinse intr-un catch
            while (!ok) {
                try {
                    std::cout << "page_count=";
                    std::string page_count_string;
                    std::getline(std::cin, page_count_string);

                    page_count = stoi(page_count_string);
                    ok = true;

                    std::string aux_str = std::to_string(stoi(page_count_string));
                    if (page_count_string.length() != aux_str.length()) {
                        ok = false;
                        // text de forma {X}{...} (numar urmat de un sir de caractere)
                        std::cout << "numarul de pagini trebuie sa fie un numar si numai un numar" << std::endl;
                    }
                } catch(...){
                    std::cout << "numarul de pagini trebuie sa fie un numar" << std::endl;
                }
            }
            
            // se deschide conexiunea catre server
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            
            // se creeaza o cerere de tip POST cu jwt pentru demonstrarea accesului la biblioteca
            message = ComputePostAuthRequest("34.118.48.238", "/api/v1/tema/library/books", "application/json",
                                                bookJson(title, author, genre, page_count, publisher), jwtToken);
            std::cout << message << std::endl;

            char* c_message = (char*) calloc(BUFLEN, sizeof(char));;
            strcpy(c_message, message.c_str());

            // se trimite cererea sub forma de char* (c_message)
            send_to_server(sockfd, c_message);
            response = receive_from_server(sockfd);

            // se afiseaza raspunsul server-ului
            std::cout << response << std::endl;

            // se inchide conexiunea
            close_connection(sockfd);
        } else if (!command.compare("delete_book")) {
            std::string id_string;
            bool ok = 0;

            // se tot citeste un id de la tastatura pana se introduce un int
            // exceptiile sunt prinse intr-un catch
            while (!ok) {
                try {
                    std::cout << "id=";
                    std::getline(std::cin, id_string);

                    stoi(id_string);
                    ok = true;

                    std::string aux_str = std::to_string(stoi(id_string));
                    if (id_string.length() != aux_str.length()) {
                        ok = false;
                        // text de forma {X}{...} (numar urmat de un sir de caractere)
                        std::cout << "id-ul trebuie sa fie un numar si numai un numar" << std::endl;
                    }
                } catch(...){
                    std::cout << "id-ul trebuie sa fie un numar" << std::endl;
                }
            }
            std::string url = "/api/v1/tema/library/books/" + id_string;

            // se deschide conexiunea catre server
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

            // se creeaza o cerere de tip DELETE cu jwt pentru demonstrarea accesului la biblioteca
            message = ComputeDeleteAuthRequest("34.118.48.238", url.c_str(), jwtToken);
            std::cout << message << std::endl;
            
            char* c_message = (char*) calloc(BUFLEN, sizeof(char));;
            strcpy(c_message, message.c_str());

            // se trimite cererea sub forma de char* (c_message)
            send_to_server(sockfd, c_message);
            response = receive_from_server(sockfd);

            // se afiseaza raspunsul server-ului
            std::cout << response << std::endl;

            // se inchide conexiunea
            close_connection(sockfd);
        } else if (!command.compare("logout")) {
            // se deschide conexiunea catre server
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            
            // se creeaza o cerere de tip GET cu cookie pentru demonstrarea autentificarii
            message = ComputeGetRequest("34.118.48.238", "/api/v1/tema/auth/logout", cookie);
            std::cout << message << std::endl;

            char* c_message = (char*) calloc(BUFLEN, sizeof(char));;
            strcpy(c_message, message.c_str());

            // se trimite cererea sub forma de char* (c_message)
            send_to_server(sockfd, c_message);
            response = receive_from_server(sockfd);

            // se afiseaza raspunsul server-ului
            std::cout << response << std::endl;

            // se golesc variabilele pentru cookie si JWT
            cookie.clear();
            jwtToken.clear();

            // se inchide conexiunea
            close_connection(sockfd);
        }
    }

    return 0;
}
