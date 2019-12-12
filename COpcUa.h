#ifndef COPCUA_H
#define COPCUA_H

#include <sstream>

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>

#define MAX_CHAR_SIZE 1024
#define MAX_ENDPOINT_URI_SIZE 1024

static bool stopFlag = false;
static char g_endpointUri[MAX_ENDPOINT_URI_SIZE];

class COpcUa
{
public:
    COpcUa();
    int connect_server(char *url);
    std::string browse_node_list();
    std::string read_node(int iNameSpace, char *node_id);
    void write_node(int iType, int iNameSpace,char *node_id);
    void disconnect_server();
    void print_menu();

    UA_Client *client;
};

#endif // COPCUA_H
