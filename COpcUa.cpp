#include "COpcUa.h"

COpcUa::COpcUa()
{

}

int COpcUa::connect_server(char *url)
{
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    UA_StatusCode retval = UA_Client_connect(client, url);
    if(retval != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return (int)retval;
    }
    return 1;
}

std::string COpcUa::browse_node_list()
{
    std::ostringstream stringStream;

    /* Browse some objects */
    printf("Browsing nodes in objects folder:\n");
    UA_BrowseRequest bReq;
    UA_BrowseRequest_init(&bReq);
    bReq.requestedMaxReferencesPerNode = 0;
    bReq.nodesToBrowse = UA_BrowseDescription_new();
    bReq.nodesToBrowseSize = 1;
    bReq.nodesToBrowse[0].nodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER); /* browse objects folder */
    bReq.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL; /* return everything */
    UA_BrowseResponse bResp = UA_Client_Service_browse(client, bReq);
    printf("%-9s %-16s %-16s %-16s\n", "NAMESPACE", "NODEID", "BROWSE NAME", "DISPLAY NAME");
    for(size_t i = 0; i < bResp.resultsSize; ++i)
    {
        for(size_t j = 0; j < bResp.results[i].referencesSize; ++j)
        {
            UA_ReferenceDescription *ref = &(bResp.results[i].references[j]);
            if(ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_NUMERIC)
            {
                printf("%-9d %-16d %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
                       ref->nodeId.nodeId.identifier.numeric, (int)ref->browseName.name.length,
                       ref->browseName.name.data, (int)ref->displayName.text.length,
                       ref->displayName.text.data);
                stringStream << "NamesapceIndex :" << ref->nodeId.nodeId.namespaceIndex
                             << " numeric :" << ref->nodeId.nodeId.identifier.numeric
                             << " length : " << (int)ref->browseName.name.length
                             << " name.data : " << ref->browseName.name.data
                             << " text.lenght : " << (int)ref->displayName.text.length
                             << " text.data : " << ref->displayName.text.data;
            } else if(ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_STRING) {
                printf("%-9d %-16.*s %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
                       (int)ref->nodeId.nodeId.identifier.string.length,
                       ref->nodeId.nodeId.identifier.string.data,
                       (int)ref->browseName.name.length, ref->browseName.name.data,
                       (int)ref->displayName.text.length, ref->displayName.text.data);
                stringStream << "NamesapceIndex : " << ref->nodeId.nodeId.namespaceIndex
                             << " string.lenght : " << (int)ref->nodeId.nodeId.identifier.string.length
                             << " string.data : " << ref->nodeId.nodeId.identifier.string.data
                             << " name.lenght : " << (int)ref->browseName.name.length
                             << " name.data : " << ref->browseName.name.data
                             << " text.lenght : " << (int)ref->displayName.text.length
                             << " test.data : " << ref->displayName.text.data;
            }
            stringStream << "\n";
        }
    }
    UA_BrowseRequest_clear(&bReq);
    UA_BrowseResponse_clear(&bResp);
    return stringStream.str();
}

std::string COpcUa::read_node(int iNameSpace, char *node_id)
{
    std::ostringstream stringStream;
    UA_StatusCode retval;
    printf("\nReading the value of node (%d, %s):\n",iNameSpace,node_id);

    stringStream << "Reading the value of node (" << iNameSpace << ", " << node_id << "):\n";
    UA_Variant *val = UA_Variant_new();
    retval = UA_Client_readValueAttribute(client, UA_NODEID_STRING(iNameSpace, node_id), val);

    if(retval == UA_STATUSCODE_GOOD && UA_Variant_isScalar(val))
    {
        if(val->type == &UA_TYPES[UA_TYPES_BOOLEAN])
        {
            UA_Boolean value = *(UA_Boolean*)val->data;
            printf("the value is: %d\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_SBYTE])
        {
            UA_SByte value = *(UA_SByte*)val->data;
            printf("the value is: %i\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_BYTE])
        {
            UA_Byte value = *(UA_Byte*)val->data;
            printf("the value is: %i\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_INT16])
        {
            UA_Int16 value = *(UA_Int16*)val->data;
            printf("the value is: %i\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_UINT16])
        {
            UA_UInt16 value = *(UA_UInt16*)val->data;
            printf("the value is: %i\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_INT32])
        {
            UA_Int32 value = *(UA_Int32*)val->data;
            printf("the value is: %i\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_UINT32])
        {
            UA_UInt32 value = *(UA_UInt32*)val->data;
            printf("the value is: %i\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_INT64])
        {
            UA_Int64 value = *(UA_Int64*)val->data;
            printf("the value is: %lli\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_UINT64])
        {
            UA_UInt64 value = *(UA_UInt64*)val->data;
            printf("the value is: %llu\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_FLOAT])
        {
            UA_Float value = *(UA_Float*)val->data;
            printf("the value is: %f\n", value);
            stringStream << "the value is: " << value << "\n";
        }
        else if(val->type == &UA_TYPES[UA_TYPES_DOUBLE])
        {
            UA_Double value = *(UA_Double*)val->data;
            printf("the value is: %lf\n", value);
            stringStream << "the value is: " << value << "\n";
        }
    }
    else
    {
        if(val->type == &UA_TYPES[UA_TYPES_DOUBLE])
        {
            printf("the value is [");
            stringStream << "the value is [";
            for(int i=0;i<val->arrayLength;i++)
            {
                printf("%lf ",(UA_Double *)val[i].data);
                stringStream << (UA_Double *)val[i].data << " ";
            }
            printf("]\n");
            stringStream << "]\n";
        }
    }

    UA_Variant_delete(val);

    return stringStream.str();
}

void COpcUa::write_node(int iType, int iNameSpace, char *node_id)
{
    /* Write node attribute */
    UA_Int32 value = 3;
    printf("\nWriting a value of node (3, \"test_int\"):\n");

    UA_Variant *myVariant = UA_Variant_new();
    UA_Variant_setScalarCopy(myVariant, &value, &UA_TYPES[UA_TYPES_INT32]);
    UA_Client_writeValueAttribute(client, UA_NODEID_STRING(1, "test_int"), myVariant);
    UA_Variant_delete(myVariant);
}

void COpcUa::disconnect_server()
{
    UA_Client_disconnect(client);
    UA_Client_delete(client);
}

void COpcUa::print_menu()
{
    printf("\n=============== OPC UA munu =======================\n\n");
    printf("start : Get endpoints and start opcua client \n");
    printf("browse : browse all nodes\n");
    printf("read : read value attribute of node \n");
    printf("write : write value to node \n");
    printf("quit : terminate/stop opcua server/client and then quit\n");
    printf("help : print menu\n");
    printf("\n=============== OPC UA munu =======================\n\n");
}

