#include "splashkit.h"
#include "socket_manager.h"
#include "database_manager.h"
#include <string>
#include <regex>

string replaceAll(string const &text, string const &textToReplace, string const &replaceTo)
{
    return std::regex_replace(text, std::regex(textToReplace), replaceTo);
}

string stripHTML(string const &text)
{
    std::regex reg("<.*?>");
    return std::regex_replace(text, reg, "");
}

string cleanJson(string const &text)
{
    string result;
    result = text.substr(text.find("{\"", 0), text.length());
    result = result.substr(0, (result.find("\"}", 0) + 2));
    return result;
}

void handleLoginRequest(http_request request)
{
    printConsole("Login POST request: " + request_body(request));

    json responseJson = create_json(request_body(request));

    string username = json_read_string(responseJson, "username");
    string password = json_read_string(responseJson, "password");

    if (validateLogin(username, password))
    {
        sendRequestResponse(request, getToken(username));
        setStatus(getToken(username), "online");
    }
    else
    {
        sendRequestResponse(request, "invalid login");
    }
}

void handleLogoutRequest(http_request request)
{
    printConsole("Logout GET request: " + request_body(request));

    string token = request_query_parameter(request, "token", "");

    setStatus(token, "offline");
}

void handleRegisterRequest(http_request request)
{
    printConsole("Register POST request: " + request_body(request));

    json responseJson = create_json(request_body(request));

    string username = json_read_string(responseJson, "username");
    string email = json_read_string(responseJson, "email");
    string password = json_read_string(responseJson, "password");
    string confirmPassword = json_read_string(responseJson, "confirm_password");

    //Validating passwords
    if (password != confirmPassword)
    {
        sendRequestResponse(request, "Invalid Password");
        return;
    }

    //Create Account
    if (!accountExists(username))
    {
        createAccount(username, email, password);
        sendRequestResponse(request, "Success");
    }
    else
    {
        sendRequestResponse(request, "Account Exists");
    }
}

void handleGetNameRequest(http_request request)
{
    printConsole("Get Name GET request: " + request_uri(request));

    string key = request_uri_stubs(request)[2];

    printConsole("Searching username for token: " + key);

    string username = getUsername(key);

    sendRequestResponse(request, username);
}

void handleGroupsRequest(http_request request)
{
    printConsole("Groups GET request: " + request_uri(request));

    string key = request_uri_stubs(request)[2];

    printConsole("Searching groups for token: " + key);

    string groupJsonData = getUserGroups(getUsername(key));

    sendRequestResponse(request, groupJsonData);
}

void handleGetMessagesRequest(http_request request)
{
    //printConsole("Get Name GET request: " + request_uri(request));

    string groupID = request_uri_stubs(request)[2];

    //printConsole("Searching messages for group id: " + groupID);

    string messageData = getMessages(groupID, 250, 1);

    sendRequestResponse(request, messageData);
}

void handleSendMessageRequest(http_request request)
{
    printConsole("Send Message POST request: " + request_body(request));

    json responseJson = create_json(cleanJson(request_body(request)));

    string token = json_read_string(responseJson, "token");
    string message = trim(json_read_string(responseJson, "message"));
    string timestamp = json_read_string(responseJson, "timestamp");
    string groupID = request_uri_stubs(request)[2];

    if (!apiKeyExists(token))
    {
        sendRequestResponse(request, "invalid key");
        return;
    }

    if (message == "")
    {
        sendRequestResponse(request, "blank message");
        return;
    }

    if (message.length() > 2000)
    {
        sendRequestResponse(request, "limit exceded");
        return;
    }

    setStatus(token, "online");

    addMessage(token, groupID, stripHTML(message), timestamp);

    sendRequestResponse(request, "Message Sent");

    free_json(responseJson);
}

void handleGroupAccessRequest(http_request request)
{
    printConsole("Group Access GET request: " + request_uri(request));

    string groupID = request_uri_stubs(request)[2];

    string token = request_query_parameter(request, "token", "");

    if (hasGroupAccess(groupID, token))
    {
        sendRequestResponse(request, "access granted");
    }
    else
    {
        sendRequestResponse(request, "access denied");
    }
}

void handleCreateGroupRequest(http_request request)
{
    printConsole("Create Group POST request: " + request_uri(request));

    printConsole(request_body(request));

    json resultJson = create_json(cleanJson(request_body(request)));

    string token = json_read_string(resultJson, "token");
    string group_name = json_read_string(resultJson, "group_name");

    if (!apiKeyExists(token))
    {
        sendRequestResponse(request, "invalid key");
        return;
    }

    if (trim(group_name) == "")
    {
        sendRequestResponse(request, "blank name");
        return;
    }

    createGroup(getUsername(token), group_name);

    sendRequestResponse(request, group_name + " group created");
}

void handleAddUserRequest(http_request request)
{
    printConsole("Add User POST request: " + request_uri(request));

    printConsole(request_body(request));

    json resultJson = create_json(cleanJson(request_body(request)));

    string token = json_read_string(resultJson, "token");
    string username = json_read_string(resultJson, "username");
    string groupId = json_read_string(resultJson, "group_id");

    if (!apiKeyExists(token))
    {
        sendRequestResponse(request, "invalid key");
        return;
    }

    if (!accountExists(username))
    {
        sendRequestResponse(request, "invalid username");
        return;
    }

    if (hasGroupAccess(groupId, getToken(username)))
    {
        sendRequestResponse(request, "already exists");
        return;
    }

    addGroupMember(username, groupId);

    sendRequestResponse(request, "success");
}

void handleRemoveUserRequest(http_request request)
{
    printConsole("Remove User POST request: " + request_uri(request));

    printConsole(request_body(request));

    json resultJson = create_json(cleanJson(request_body(request)));

    string token = json_read_string(resultJson, "token");
    string username = json_read_string(resultJson, "username");
    string groupId = json_read_string(resultJson, "group_id");

    if (!apiKeyExists(token))
    {
        sendRequestResponse(request, "invalid key");
        return;
    }

    if (!accountExists(username))
    {
        sendRequestResponse(request, "invalid username");
        return;
    }

    if (!hasGroupAccess(groupId, getToken(username)))
    {
        sendRequestResponse(request, "no access");
        return;
    }

    if (isGroupOwner(groupId, username) && to_lowercase(getUsername(token)) != to_lowercase(username))
    {
        sendRequestResponse(request, "no permission");
        return;
    }

    removeGroupMember(username, groupId);

    sendRequestResponse(request, "success");
}

void handleNotificationRequest(http_request request)
{
    //printConsole("Notification POST request: " + request_uri(request));

    string token = request_query_parameter(request, "token", "");
    string groupId = request_uri_stubs(request)[2];

    if (!apiKeyExists(token))
    {
        sendRequestResponse(request, "invalid key");
        return;
    }

    sendRequestResponse(request, json_to_string(getUnreadGroups(getUsername(token))));
}

void handleUpdateNotificationRequest(http_request request)
{
    printConsole("Update Notification POST request: " + request_uri(request));

    json resultJson = create_json(cleanJson(request_body(request)));

    string token = json_read_string(resultJson, "token");
    string groupId = json_read_string(resultJson, "group_id");
    string timestamp = json_read_string(resultJson, "timestamp");

    if (!apiKeyExists(token))
    {
        sendRequestResponse(request, "invalid key");
        return;
    }

    string username = getUsername(token);

    if (!groupIdExists(groupId))
    {
        sendRequestResponse(request, "invalid group");
        return;
    }

    setMessagesRead(username, groupId, timestamp);

    sendRequestResponse(request, "success");
}

void handleDeleteGroupRequest(http_request request)
{
    printConsole("Delete Group POST request: " + request_uri(request));

    json resultJson = create_json(cleanJson(request_body(request)));

    string token = json_read_string(resultJson, "token");
    string groupId = json_read_string(resultJson, "group_id");

    if (!apiKeyExists(token))
    {
        sendRequestResponse(request, "invalid key");
        return;
    }

    if (!groupIdExists(groupId))
    {
        sendRequestResponse(request, "invalid group");
        return;
    }

    deleteGroup(groupId);

    sendRequestResponse(request, "success");
}

void handleGroupUsersRequest(http_request request)
{
    //printConsole("Group Users GET request: " + request_uri(request));

    string token = request_query_parameter(request, "token", "");
    string group_id = request_uri_stubs(request)[2];

    if (!apiKeyExists(token))
    {
        sendRequestResponse(request, "invalid key");
        return;
    }

    if (!groupIdExists(group_id))
    {
        sendRequestResponse(request, "invalid group");
        return;
    }

    sendRequestResponse(request, json_to_string(getGroupUsers(group_id)));
}

void send_html_file(http_request request, string fileName)
{
    string htmlCode = file_as_string(fileName, OTHER_RESOURCE);

    //Repalce Username
    string apiKey = request_query_parameter(request, "key", "Guest");

    htmlCode = replaceAll(htmlCode, "#USERNAME#", getUsername(apiKey));

    sendRequestResponse(request, HTTP_STATUS_OK, htmlCode, "text/html");
}

void send_html_file(http_request request, string fileName, vector<string> replace, vector<string> replaceTo)
{
    string htmlCode = file_as_string(fileName, OTHER_RESOURCE);

    for (int i = 0; i < replace.size(); i++)
    {
        htmlCode = replaceAll(htmlCode, replace[i], replaceTo[i]);
    }

    sendRequestResponse(request, HTTP_STATUS_OK, htmlCode, "text/html");
}