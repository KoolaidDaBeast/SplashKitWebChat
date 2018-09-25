#include "splashkit.h"
#include "database_manager.h"
#include "socket_manager.h"
#include <chrono>
#include <cmath>

using namespace std;
using namespace std::chrono;

int hash_string(const string &str)
{
    return std::hash<string>()(str);
}

void loadDatabase()
{
    open_database("account", "account.sqlite");
    open_database("message", "message.sqlite");
    open_database("group", "group.sqlite");

    // //Attach account database to user_group
    // query_result result;
    // result = run_query("message", "END TRANSACTION;");
    // result = run_query("message", "ATTACH DATABASE '" + path_to_resource("account.sqlite", DATABASE_RESOURCE) +  "' AS accDB;");
    // result = run_query("message", "BEGIN TRANSACTION;");

    // if (query_success(result))
    //     printConsole("Attached database successfully!");
    // else
    //     printConsole("Failed to attach database!");
}

query_result run_query(string databaseName, string query)
{
    query_result result = run_sql(databaseName, query);

    return result;
}

void createTables()
{
    query_result accountTable = run_query("account", "CREATE TABLE IF NOT EXISTS accounts (username CHAR(20), email text, password text, api_key CHAR(40), status text, logged_at text);");
    query_result userGroupsTable = run_query("account", "CREATE TABLE IF NOT EXISTS user_groups (username CHAR(20), group_id text);");
    query_result groupsTable = run_query("group", "CREATE TABLE IF NOT EXISTS groups (group_id text, group_name text, owner text);");
    query_result groupMessageTable = run_query("message", "CREATE TABLE IF NOT EXISTS user_messages (group_id text, username text, message text, timestamp text);");
    query_result notificationTable = run_query("message", "CREATE TABLE IF NOT EXISTS read_messages (username text, group_id text, timestamp text);");
}

bool accountExists(string username)
{
    query_result result = run_query("account", "SELECT username FROM accounts WHERE username LIKE '" + username + "';");

    if (has_row(result))
    {

        return true;
    }

    return false;
}

bool groupIdExists(string id)
{

    query_result result = run_query("group", "SELECT group_id FROM groups WHERE group_id = '" + id + "';");

    if (has_row(result))
    {
        return true;
    }

    return false;
}

bool apiKeyExists(string apiKey)
{
    query_result result = run_query("account", "SELECT api_key FROM accounts WHERE api_key = '" + apiKey + "';");

    if (has_row(result))
    {

        return true;
    }

    return false;
}

bool hasGroupAccess(string id, string token)
{
    query_result result = run_query("account", "SELECT group_id FROM user_groups WHERE group_id = '" + id + "' AND username LIKE '" + getUsername(token) + "';");

    if (has_row(result))
    {
        return true;
    }

    return false;
}

bool isGroupOwner(string id, string owner)
{
    query_result result = run_query("group", "SELECT group_id FROM groups WHERE group_id = '" + id + "' AND owner LIKE '" + owner + "';");

    if (has_row(result))
    {
        return true;
    }

    return false;
}

bool hasGroupNotification(string username, string group_id)
{
    query_result result = run_query("message", "SELECT group_id FROM read_messages WHERE group_id = '" + group_id + "' AND username LIKE '" + username + "';");

    if (has_row(result))
    {
        return true;
    }

    return false;
}

string buildApiKey()
{
    string characterArray = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890";

    string key = "";

    int size = (rnd() * 30) + 10;

    for (int i = 0; i < size; i++)
    {
        int index = rnd((characterArray.length() - 1));
        key = key + characterArray.substr(index, 1);
    }

    return key;
}

string buildGroupId()
{

    string characterArray = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";

    string key = "";

    int size = (rnd() * 10) + 5;

    for (int i = 0; i < size; i++)
    {
        int index = rnd((characterArray.length() - 1));
        key = key + characterArray.substr(index, 1);
    }

    return key + "." + to_string(rnd(999999));
}

string generateValidApiKey()
{
    string key = buildApiKey();

    while (apiKeyExists(key))
    {
        key = buildApiKey();
    }

    return key;
}

string generateValidGroupId()
{
    string key = buildGroupId();

    while (groupIdExists(key))
    {
        key = buildGroupId();
    }

    return key;
}

void setTimestamp(string username, bool old)
{
    json jsonFile; 

    if (old)
    {
        //Timestamp of when last data was set
        jsonFile = json_from_file("online_users_timestamp_old.json");

        double milliTime = std::ceil(high_resolution_clock::now().time_since_epoch().count() / 1000000);

        json_set_number(jsonFile, to_lowercase(username), milliTime);

        json_to_file(jsonFile, "online_users_timestamp_old.json");
    }
    else
    {
        //Timestamp of when last data was set
        jsonFile = json_from_file("online_users_timestamp.json");

        double milliTime = std::ceil(high_resolution_clock::now().time_since_epoch().count() / 1000000);

        json_set_number(jsonFile, to_lowercase(username), milliTime);

        json_to_file(jsonFile, "online_users_timestamp.json");
    }
}

double getTimestamp(string username, bool old)
{
       json jsonFile; 

    if (old)
    {
        //Timestamp of when last data was set
        jsonFile = json_from_file("online_users_timestamp_old.json");

        return json_read_number_as_double(jsonFile, username);
    }
    else
    {
        //Timestamp of when last data was set
        jsonFile = json_from_file("online_users_timestamp.json");

        return json_read_number_as_double(jsonFile, username);
    } 
}

void setLocalUserStatus(string username, string status)
{
    json jsonFile = json_from_file("online_users.json");

    json_set_string(jsonFile, to_lowercase(username), status);

    json_to_file(jsonFile, "online_users.json");

    setTimestamp(username, false);
    setTimestamp(username, true);
}

string getLocalUserStatus(string username)
{
    json jsonFile = json_from_file("online_users.json");

    return json_read_string(jsonFile, to_lowercase(username));
}

void removeLocalUserStatus(string username)
{
    json jsonFile = json_from_file("online_users.json");

    json_set_string(jsonFile, to_lowercase(username), "offline");

    json_to_file(jsonFile, "online_users.json");
}

void createAccount(string username, string email, string password)
{
    string api_key = generateValidApiKey();

    printConsole(username + " generated Api Key: " + api_key);

    string date = "2018";

    password = to_string(hash_string(password));

    printConsole(username + " hashed password: " + password);

    if (accountExists(username))
    {
        printConsole(email + " account already exists.");
        return;
    }

    query_result result = run_query("account",
                                    "INSERT INTO accounts VALUES ('" + username + "', '" + email + "', '" + password + "', '" + api_key + "', '" + "offline" + "', '" + date + "');");

    if (query_success(result))
    {
        printConsole(email + " account created.");

        setLocalUserStatus(username, "offline");
    }
    else
    {
        printConsole(email + " failed to create account.");
    }
}

void addMessage(string token, string groupID, string message, string timestamp)
{
    if (!apiKeyExists(token))
    {
        return;
    }

    query_result result = run_query("message",
                                    "INSERT INTO user_messages VALUES ('" + groupID + "', '" + getUsername(token) + "', '" + message + "', '" + timestamp + "');");

    if (query_success(result))
    {
        printConsole("Sent '" + message + "' with token: " + token);
    }
    else
    {
        printConsole("Failed to send '" + message + "' with token: " + token);
    }
}

void setStatus(string token, string status)
{
    if (!accountExists(getUsername(token)))
    {
        return;
    }

    setLocalUserStatus(getUsername(token), status);

    query_result result = run_query("account", "UPDATE accounts SET status = '" + status + "' WHERE api_key = '" + token + "';");

    if (query_success(result))
    {
        printConsole("Token -> " + token + " status has been set to: " + status);
    }
    else
    {
        printConsole("Failed to set token -> " + token + " status");
    }
}

void addGroupMember(string username, string groupID)
{
    if (!accountExists(username))
    {
        return;
    }

    query_result result = run_query("account", "INSERT INTO user_groups VALUES('" + username + "', '" + groupID + "');");

    if (query_success(result))
    {
        printConsole("User -> " + username + " has been added to group " + groupID);
    }
    else
    {
        printConsole("Failed to add user -> " + username + " to group " + groupID);
    }
}

void removeGroupMember(string username, string groupID)
{
    if (!accountExists(username))
    {
        return;
    }

    query_result result = run_query("account", "DELETE FROM user_groups WHERE username LIKE '" + username + "' AND group_id = '" + groupID + "';");

    if (query_success(result))
    {
        printConsole("User -> " + username + " has been removed from group " + groupID);
    }
    else
    {
        printConsole("Failed to remove user -> " + username + " from group " + groupID);
    }
}

void createGroup(string username, string group_name)
{
    string groupId = generateValidGroupId();

    query_result result = run_query("group", "INSERT INTO groups VALUES('" + groupId + "', '" + group_name + "', '" + username + "');");

    if (query_success(result))
    {
        printConsole("User -> " + username + " has created group " + group_name);

        addGroupMember(username, groupId);
    }
    else
    {
        printConsole("Failed to create group " + group_name + " for user " + username);
    }
}

void setMessagesRead(string username, string group_id, string timestamp)
{
    query_result result;

    if (hasGroupNotification(username, group_id))
    {
        result = run_query("message", "UPDATE read_messages SET timestamp = '" + timestamp + "' WHERE username LIKE '" + username + "' AND group_id = '" + group_id + "';");
    }
    else
    {
        result = run_query("message", "INSERT INTO read_messages VALUES ('" + username + "', '" + group_id + "', '" + timestamp + "');");
    }

    if (query_success(result))
    {
        printConsole("User -> " + username + " notifications have been updated. ");
    }
    else
    {
        printConsole("Failed to update notifications for user " + username);
    }
}

void deleteGroup(string group_id)
{
    if (!groupIdExists(group_id))
    {
        printConsole("Group " + group_id + " doesn't exist!");
        return;
    }

    query_result result;

    result = run_query("group", "DELETE FROM groups WHERE group_id = '" + group_id + "';");
    result = run_query("account", "DELETE FROM user_groups WHERE group_id = '" + group_id + "';");
    result = run_query("account", "DELETE FROM user_groups WHERE group_id = '" + group_id + "';");
    result = run_query("message", "DELETE FROM user_messages WHERE group_id = '" + group_id + "';");

    if (query_success(result))
    {
        printConsole("Group -> " + group_id + " data has been deleted.");
    }
    else
    {
        printConsole("Failed to delete group -> " + group_id + ".");
    }
}

string getToken(string username)
{
    query_result result = run_query("account", "SELECT api_key FROM accounts WHERE username LIKE '" + username + "';");

    if (has_row(result))
    {
        string token = query_column_for_string(result, 0);

        return token;
    }

    return "";
}

string getUsername(string apiKey)
{
    query_result result = run_query("account", "SELECT username FROM accounts WHERE api_key = '" + apiKey + "';");

    if (has_row(result))
    {
        string username = query_column_for_string(result, 0);

        return username;
    }

    return "";
}

string getPassword(string username)
{
    query_result result = run_query("account", "SELECT password FROM accounts WHERE username LIKE '" + username + "' OR email = '" + username + "';");

    if (has_row(result))
    {
        string username = query_column_for_string(result, 0);

        return username;
    }

    return "";
}

bool validateLogin(string username, string password)
{
    if (!accountExists(username))
    {
        return false;
    }

    int hashedPassword = hash_string(password);
    int actualHashedPassword = convert_to_integer(getPassword(username));

    //printConsole(username + "'s HP = " + to_string(hashedPassword) + " and AHP = " + to_string(actualHashedPassword));

    if (hashedPassword == actualHashedPassword)
    {
        return true;
    }

    return false;
}

string getGroupName(string groupID)
{
    query_result result = run_query("group", "SELECT group_name FROM groups WHERE group_id = '" + groupID + "';");

    if (has_row(result))
    {
        string group_name = query_column_for_string(result, 0);

        return group_name;
    }

    return "";
}

string getUserGroups(string username)
{
    if (!accountExists(username))
    {
        return json_to_string(create_json());
    }

    query_result result = run_query("account", "SELECT group_id FROM user_groups WHERE username LIKE '" + username + "';");

    //Acts as JSON Aray
    json resultJson = create_json();

    int i = 0;

    if (!has_row(result))
    {
        return json_to_string(resultJson);
    }

    do
    {
        //JSON object that will be store each result row data
        json innerJson = create_json();

        string groupID = query_column_for_string(result, 0);

        json_set_string(innerJson, "group_id", groupID);
        json_set_string(innerJson, "group_name", getGroupName(groupID));

        //printConsole("Group ID: " + groupID + " index " + to_string(i) + " data: " + json_to_string(innerJson));

        //Sets innerJson to resultJson under the index number i.
        json_set_object(resultJson, to_string(i), innerJson);
        i++;
    } while (get_next_row(result));

    return json_to_string(resultJson);
}

string getMessages(string id, int messageLimit, int page)
{
    if (!groupIdExists(id))
    {
        printConsole("Group " + id + " doesnt exist.");
        return json_to_string(create_json());
    }

    query_result result = run_query("message", "SELECT usr_msg.username, usr_msg.message, usr_msg.timestamp FROM user_messages usr_msg WHERE usr_msg.group_id = '" + id + "' ORDER BY usr_msg.timestamp ASC LIMIT " + to_string(messageLimit) + ";");

    if (!has_row(result))
    {
        printConsole("Group " + id + " has no messages.");
        return json_to_string(create_json());
    }
    //Acts as JSON Aray
    json resultJson = create_json();

    int i = 0;

    do
    {
        //JSON object that will be store each result row data
        json innerJson = create_json();

        json_set_string(innerJson, "username", query_column_for_string(result, 0));
        json_set_string(innerJson, "message", query_column_for_string(result, 1));
        json_set_string(innerJson, "timestamp", query_column_for_string(result, 2));
        json_set_string(innerJson, "status", getLocalUserStatus(query_column_for_string(result, 0)));

        //printConsole("Group ID: " + groupID + " index " + to_string(i) + " data: " + json_to_string(innerJson));

        //Sets innerJson to resultJson under the index number i.
        json_set_object(resultJson, to_string(i), innerJson);
        i++;
    } while (get_next_row(result));

    return json_to_string(resultJson);
}

json getUnreadGroups(string username)
{
    json groupNames = create_json();

    query_result result = run_query("message",
                                    "SELECT DISTINCT um.group_id FROM user_messages um, read_messages rm WHERE rm.username = '" + username + "' AND um.timestamp > rm.timestamp AND um.group_id = rm.group_id;");

    if (has_row(result))
    {
        int i = 0;

        do
        {
            json innerJson = create_json();
            json_set_string(innerJson, "group_id", query_column_for_string(result, 0));
            json_set_string(innerJson, "group_name", getGroupName(query_column_for_string(result, 0)));

            json_set_object(groupNames, to_string(i), innerJson);

            i++;
        } while (get_next_row(result));
    }

    return groupNames;
}

json getGroupUsers(string group_id)
{
    json groupNames = create_json();

    query_result result = run_query("account",
                                    "SELECT acc.username, acc.status FROM accounts acc, user_groups usr WHERE usr.group_id = '" + group_id + "';");

    if (has_row(result))
    {
        int i = 0;

        do
        {
            json innerJson = create_json();
            json_set_string(innerJson, "username", query_column_for_string(result, 0));
            json_set_string(innerJson, "status", query_column_for_string(result, 1));

            json_set_object(groupNames, to_string(i), innerJson);

            i++;
        } while (get_next_row(result));
    }

    return groupNames;
}

void forceLogoutUsrs()
{
}
