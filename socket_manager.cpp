#include "splashkit.h"
#include "socket_manager.h"
//TIME LIBS
#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

web_server setupServer(int port)
{
    return start_web_server(port);
}

void stopWebServer(web_server server)
{
    stop_web_server(server);
}

bool hasRequest(web_server server)
{
    if (has_incoming_requests(server))
    {
         return true;
    }

    return false;
}

http_request getNewRequest(web_server server)
{
    return next_web_request(server);
}

void sendRequestResponse(http_request request, string message)
{
    send_response(request, message);
}

void sendRequestResponse(http_request request, http_status_code code, string message, string contentType)
{
    send_response(request, code, message, contentType);
}

const std::string currentDateTime() {

    //Reference: https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c 

    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "[%d-%m-%Y %X]", &tstruct);

    return buf;
}

void printConsole(string message)
{
    write_line(currentDateTime() + " " + PREFIX + message);
}