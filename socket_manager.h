#ifndef SOCKET_MANAGER
#define SOCKET_MANAGER

#include "splashkit.h"
#include <vector>

using namespace std;

const string PREFIX = "WebServer >> ";

/**
 * Setups the webserver at the specified port 
 * 
 * @param port      The port to start the webserver on
 * 
 * @returns         The webserver on the specified port
 */
web_server setupServer(int port);

/**
 * Stops the webserver on the specified port
 * 
 * @param port      The port to stop the webserver on
 */
void stopWebServer(web_server server);

/**
 * Checks if the webserver has a http_request
 * 
 * @param server        The webserver to check for requests
 */
bool hasRequest(web_server server);

/**
 * Gets the new request from the webserver
 * 
 * @param server        The webserver to get new requests
 */
http_request getNewRequest(web_server server);

/**
 * Send request response to the http_request
 * 
 * @param request        The request to send a message to
 * @param message        The message to send to the request
 */
void sendRequestResponse(http_request request, string message);

/**
 * Send request response to the http_request with a http_status_code and contentType
 * 
 * @param request        The request to send a message to
 * @param code           The status code to send to the request
 * @param message        The message to send to the request
 * @param contentType    The contentType to send to the request
 */
void sendRequestResponse(http_request request, http_status_code code, string message, string contentType);

/**
 * Prints a message to the terminal
 * 
 * @param message        The message to send to the terminal
 */
void printConsole(string message);

#endif 