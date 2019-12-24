#ifndef COPCUA_H
#define COPCUA_H

#include <sstream>

#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
#include <pthread.h>


#define MAX_CHAR_SIZE 1024
#define MAX_ENDPOINT_URI_SIZE 1024

static bool stopFlag = false;
static char g_endpointUri[MAX_ENDPOINT_URI_SIZE];
static volatile UA_Boolean running = true;
static UA_Boolean b_running;

class OneBody;

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
    //server
    static void *server_loop(void *ptr);
    int ua_server_run();
    void ua_server_stop();
    static void ua_create_node_int32(const char * node, int value);
    static void ua_create_node_double(const char * node, double value);
    static void ua_create_node_str(const char * node, const char * value);
    static void ua_create_node_int32_array(int id_type, const char * node, int value[], int size);
    static void ua_update_node_int32(const char * node, int value);
    static void ua_update_node_double(const char * node, double value);
    static void ua_update_node_int32_array(const char * node, int value[], int size);
    static UA_Int32 ua_read_node_int32(const char * node);
    static int* ua_read_node_int32_array(const char * node);
    void serverSequnece(UA_ServerCallback listner);

    static UA_Server *server;
    UA_Client *client;
    static UA_ServerConfig *config;
    static pthread_t m_serverThread;
    static bool m_bTrigger;

};

#endif // COPCUA_H
