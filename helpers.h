#ifndef _HELPERS_
#define _HELPERS_

#include <string>
#include <vector>

#define BUFLEN 4096 // dimensiunea maxima pentru mesaj
#define LINELEN 1000 // dimensiunea maxima pentru o linie din mesaj
#define loop while(1)

// functie care creeaza un obiect json de forma
// {”username”: String,”password”: String}
std::string authJson(std::string username, std::string password);

// functie care creeaza un obiect json de forma
// {”id”: Number,”title”: String,”author”: String,”publisher”: String,”genre”: String,”page_count”: Number}
std::string bookJson(std::string title, std::string author, std::string genre,
                         int page_count, std::string publisher);

// functie care intoarce cookie-ul dintr-un string (login)
std::string getCookie(std::string response);

// functie care intoarce JWT-ul dintr-un string (enter_library)
std::string getJWT(std::string response);

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(const char* host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

#endif
