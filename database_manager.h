#ifndef DATABASE_MANAGER
#define DATABASE_MANAGER

#include "splashkit.h"

using namespace std;

/*
 * NOTE: 
 *          token is the user's api key
*/

/**
 * Loads all the database files
 * 
 */
void loadDatabase();

/**
 * Generates all the tables for the databases.
 * 
 */
void createTables();

/**
 * Runs a query on the speficed databse
 * 
 * @param databaseName    The pointer name of the database to execute the query in
 * @param query           The query to execute
 * 
 * @returns               The result of the query
 */
query_result run_query(string databaseName, string query);

/**
 * Polls the database for the speficed username
 * 
 * @param username    The username to search for
 * 
 * @returns           Whether or not the username exists in the database
 */
bool accountExists(string username);

/**
 * Polls the database for if the token is in group id group
 * 
 * @param id          The group id
 * @param token       The api key of the user
 * 
 * @returns           Whether or not the token is in apart of group id
 */
bool hasGroupAccess(string id, string token);

/**
 * Polls the database for if the token is group id owner
 * 
 * @param id          The group id
 * 
 * @returns           Whether or not the username is the owner of group id
 */
bool isGroupOwner(string id, string owner);

/**
 * Polls the database for the speficed api key
 * 
 * @param apiKey      The apikey to search for
 * 
 * @returns           Whether or not the api key exists in the database
 */
bool apiKeyExists(string apiKey);

/**
 * Creates an account with the username, email and the hashed password 
 * 
 * @param username    The new account's username
 * @param email       The new account's email adrress
 * @param password    The new account's raw password
 */
void createAccount(string username, string email, string password);

/**
 * Sets the token's online staus
 * 
 * @param token     The token to set the status for
 * @param status    The status to set the token to
 */
void setStatus(string token, string status);

/**
 * Create a group
 * 
 * @param username      The owner of the group
 * @param group_name    The group name
 */
void createGroup(string username, string group_name);

/**
 * Sets the latest read message from a group
 * 
 * @param username      The usernameof the user
 * @param group_id      The group id
 * @param timestamp     The timestamp of read message
 */
void setMessagesRead(string username, string group_id, string timestamp);

/**
 * Adds a message to a group id 
 * 
 * @param toekn       The message owner's api token
 * @param groupID     The group id to add the message to
 * @param message     The message to add to the group id
 * @param timestamp   The time the message was sent
 */
void addMessage(string token, string groupID, string message, string timestamp);

/**
 * Adds a user to the group id
 * 
 * @param groupID     The group id to add the message to
 * @param username    The username to add to the group id
 */
void addGroupMember(string username, string groupID);

/**
 * Removes a user from the group id
 * 
 * @param groupID     The group id to remove the user from
 * @param username    The username to rmove from the group id
 */
void removeGroupMember(string username, string groupID);

/**
 * Deletes a group from the database
 * 
 * @param groupID     The group id to delete
 */
void deleteGroup(string group_id);

/**
 * Query the api key from the username
 * 
 * @param username      The username to retireve the api key witb
 * 
 * @returns             The api key thats belongs to the username returns "" by default.
 */
string getToken(string username);

/**
 * Query the username from the api key
 * 
 * @param apiKey        The api key to retireve the username with
 * 
 * @returns             The username thats belongs to the api key returns "" by default.
 */
string getUsername(string apiKey);

/**
 * Query the groups from the username
 * 
 * @param apiKey        The username to retireve the groups with
 * 
 * @returns             The json result thats holds all the users chat groups
 */
string getUserGroups(string username);

/**
 * Query the groups from the username
 * 
 * @param id                The group id to retireve the groups with
 * @param messageLimit      The number of recent messages to retireve
 * @param page              The page number of messages to receieve
 * 
 * @returns         The json result thats holds messageLimit number of recent messages
 */
string getMessages(string id, int messageLimit, int page);

/**
 * Validates the login details
 * 
 * @param username    The username you want to validate
 * @param password    The password you want to validate for username
 * 
 * @returns           Whether or not the password matches @param username password in the database
 */
bool validateLogin(string username, string password);

/**
 * Gets the group name
 * 
 * @param groupID     The group id you want the name of
 * 
 * @returns           The group name of groupID
 */
string getGroupName(string groupID);

/**
 * Validates if the group id exists
 * 
 * @param username    The username you want to validate
 * 
 * @returns           Whether or not the group exists database
 */
bool groupIdExists(string id);

/**
 * Gets the group name of all unread groups
 * 
 * @param username     The username of unread groups to retireve
 * 
 * @returns            The json result of all unread group names
 */
json getUnreadGroups(string username);

/**
 * Gets all the users inside a group
 * 
 * @param username     The username of unread groups to retireve
 * 
 * @returns            The json result of all users inside the group
 */
json getGroupUsers(string group_id);

#endif 