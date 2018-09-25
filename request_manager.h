#ifndef REQUEST_MANAGER
#define REQUEST_MANAGER

#include "splashkit.h"
#include <vector>

using namespace std;

/**
 * Replaces a text in a string
 * 
 * @param text              The string that should be replaced
 * @param textToReplace     The text you want to replace
 * @param replaceTo         The replacement text for textToReplace
 * 
 * @returns                 The replaced string
 */
string replaceAll(string const &text, string const &textToReplace, string const &replaceTo);

/**
 * Strips HTML tags from a string
 * 
 * @param text              The string that should be stripped of html tags
 * 
 * @returns                 The stripped string
 */
string stripHTML(string const &text);

/**
 * Handles the login request
 * 
 * @param request    The http request for the POST request
 */
void handleLoginRequest(http_request request);

/**
 * Handles the logout request
 * 
 * @param request    The http request for the GET request
 */
void handleLogoutRequest(http_request request);

/**
 * Handles the register request
 * 
 * @param request    The http request for the POST request
 */
void handleRegisterRequest(http_request request);

/**
 * Handles the get name request
 * 
 * @param request    The http request for the GET request
 */
void handleGetNameRequest(http_request request);

/**
 * Handles the get group request
 * 
 * @param request    The http request for the GET request
 */
void handleGroupsRequest(http_request request);

/**
 * Handles the get messages request
 * 
 * @param request    The http request for the GET request
 */
void handleGetMessagesRequest(http_request request);

/**
 * Handles the send message request
 * 
 * @param request    The http request for the POST request
 */
void handleSendMessageRequest(http_request request);

/**
 * Handles the group access request
 * 
 * @param request    The http request for the GET request
 */
void handleGroupAccessRequest(http_request request);

/**
 * Handles the create group request
 * 
 * @param request    The http request for the POST request
 */
void handleCreateGroupRequest(http_request request);

/**
 * Handles the add user request
 * 
 * @param request    The http request for the POST request
 */
void handleAddUserRequest(http_request request);

/**
 * Handles the remove user request
 * 
 * @param request    The http request for the POST request
 */
void handleRemoveUserRequest(http_request request);

/**
 * Handles the notification request
 * 
 * @param request    The http request for the GET request
 */
void handleNotificationRequest(http_request request);

/**
 * Handles the update notification request
 * 
 * @param request    The http request for the POST request
 */
void handleUpdateNotificationRequest(http_request request);

/**
 * Handles the delete group request
 * 
 * @param request    The http request for the POST request
 */
void handleDeleteGroupRequest(http_request request);

/**
 * Handles the group users request
 * 
 * @param request    The http request for the GET request
 */
void handleGroupUsersRequest(http_request request);

/**
 * Sends html file to the http_request
 * 
 * @param request    The http request to send the html file to
 * @param fileName   The name of the file to send to the request
 */
void send_html_file(http_request request, string fileName);

/**
 * Sends html file to the http_request
 * 
 * @param request    The http request to send the html file to
 * @param fileName   The name of the file to send to the request
 * @param replace    The array of string you want to replace
 * @param replaceTo  The array of value to replace the replace param with
 */
void send_html_file(http_request request, string fileName, vector<string> replace, vector<string> replaceTo);

#endif 