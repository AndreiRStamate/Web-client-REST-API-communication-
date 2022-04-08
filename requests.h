#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string + cookie
std::string ComputeGetRequest(const char* host, const char* url, std::string cookie);

// computes and returns a GET request string + JWT
std::string ComputeGetAuthRequest(const char* host, const char* url, std::string JWT);

// computes and returns a POST request string (cookie can be NULL if not needed)
std::string ComputePostRequest(const char* host, const char* url, const char* contentType,
								 std::string data, std::string cookie);

// computes and returns a POST request string (JWT can be NULL if not needed)
std::string ComputePostAuthRequest(const char* host, const char* url, const char* contentType,
                                 std::string data, std::string JWT);

// computes and returns a DELETE request string (JWT can be NULL if not needed)
std::string ComputeDeleteAuthRequest(const char* host, const char* url, std::string JWT);

#endif
