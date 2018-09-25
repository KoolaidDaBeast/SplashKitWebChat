#include "splashkit.h"
#include "socket_manager.h"
#include "request_manager.h"
#include "database_manager.h"

const int PORT = 80;

void to_lowercase_array(vector<string> &data)
{
    for (int i = 0; i < data.size(); i++)
    {
        /*
        * Don't convert to lowercase if data[0] is == chatroom, notification, update-notification.
        * Because of case senstive group id's.
        */
        if (to_lowercase(data[i]) == "chatroom" || to_lowercase(data[i]) == "notification" || to_lowercase(data[i]) == "update-notification" || to_lowercase(data[i]) == "users")
        {
            break;
        }

        data[i] = trim(to_lowercase(data[i]));
    }
}

int main()
{
    //Starting web server
    printConsole("Starting web server at port " + to_string(PORT) + "!");
    web_server server = setupServer(PORT);
    printConsole("Started web server at port " + to_string(PORT) + "!");

    //Open database
    loadDatabase();

    createTables();

    //Start timer to logout users who are inactive
    create_timer("STATUS_TIMER");
    start_timer("STATUS_TIMER");

    do
    {
        if (hasRequest(server))
        {
            http_request conn = getNewRequest(server);

            //Reterieve uri stubs
            vector<string> uriStubs = request_uri_stubs(conn);

            to_lowercase_array(uriStubs);

            /*
            * HTML VIEW PAGES
            */
            //Homepage
            if (uriStubs[0] == "")
            {
                printConsole("Client has connected to: " + request_uri(conn));

                send_html_file(conn, "homepage.html");
            }
            //Groups Page
            else if (uriStubs[0] == "groups")
            {
                printConsole("Client has connected to: " + request_uri(conn));

                send_html_file(conn, "groups.html");
            }
            //Register Page
            else if (uriStubs[0] == "register")
            {
                printConsole("Client has connected to: " + request_uri(conn));

                send_html_file(conn, "register.html");
            }
            //Chatroom Page
            else if (uriStubs[0] == "chatroom" && uriStubs.size() > 1)
            {
                printConsole("Client has connected to: " + request_uri(conn));

                if (groupIdExists(uriStubs[1]))
                {
                    string groupName = getGroupName(uriStubs[1]);

                    if (groupName.length() > 35)
                    {
                        groupName = groupName.substr(0, 32) + "...";
                    }

                    vector<string> replace = {"FETCH_GROUP_ID", "FETCH_GROUP_NAME"};
                    vector<string> replaceTo = {uriStubs[1], groupName};

                    send_html_file(conn, "chatroom.html", replace, replaceTo);
                }
                else
                {
                    sendRequestResponse(conn, "Invalid Chatroom ID.");
                }
            }
            /*
            * POST,GET REQUESTS
            */
            else if (uriStubs[0] == "request" && uriStubs[1] == "login")
            {
                handleLoginRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "logout")
            {
                handleLogoutRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "register")
            {
                handleRegisterRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "username")
            {
                handleGetNameRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "groups")
            {
                handleGroupsRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "messages")
            {
                handleGetMessagesRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "add-message" && uriStubs[2] != "")
            {
                handleSendMessageRequest(conn);
            }
            else if (uriStubs[0] == "request-access" && uriStubs[1] == "group")
            {
                handleGroupAccessRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "create-group")
            {
                handleCreateGroupRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "add-user")
            {
                handleAddUserRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "remove-user")
            {
                handleRemoveUserRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "notification")
            {
                handleNotificationRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "update-notification" && groupIdExists(uriStubs[2]))
            {
                handleUpdateNotificationRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "delete-group")
            {
                handleDeleteGroupRequest(conn);
            }
            else if (uriStubs[0] == "request" && uriStubs[1] == "users" && groupIdExists(uriStubs[2]))
            {
                handleGroupUsersRequest(conn);
            }
            else
            {
                sendRequestResponse(conn, "Error 400: Invalid Page");
            }
        }
    } while (!quit_requested());

    stopWebServer(server);
    printConsole("Stopped web server at port " + to_string(PORT) + "!");

    return 0;
}