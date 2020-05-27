
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "trace_data.h"

OpenAPI_trace_data_t *OpenAPI_trace_data_create(
    char *trace_ref,
    OpenAPI_trace_depth_t *trace_depth,
    char *ne_type_list,
    char *event_list,
    char *collection_entity_ipv4_addr,
    char *collection_entity_ipv6_addr,
    char *interface_list
    )
{
    OpenAPI_trace_data_t *trace_data_local_var = OpenAPI_malloc(sizeof(OpenAPI_trace_data_t));
    if (!trace_data_local_var) {
        return NULL;
    }
    trace_data_local_var->trace_ref = trace_ref;
    trace_data_local_var->trace_depth = trace_depth;
    trace_data_local_var->ne_type_list = ne_type_list;
    trace_data_local_var->event_list = event_list;
    trace_data_local_var->collection_entity_ipv4_addr = collection_entity_ipv4_addr;
    trace_data_local_var->collection_entity_ipv6_addr = collection_entity_ipv6_addr;
    trace_data_local_var->interface_list = interface_list;

    return trace_data_local_var;
}

void OpenAPI_trace_data_free(OpenAPI_trace_data_t *trace_data)
{
    if (NULL == trace_data) {
        return;
    }
    OpenAPI_lnode_t *node;
    ogs_free(trace_data->trace_ref);
    OpenAPI_trace_depth_free(trace_data->trace_depth);
    ogs_free(trace_data->ne_type_list);
    ogs_free(trace_data->event_list);
    ogs_free(trace_data->collection_entity_ipv4_addr);
    ogs_free(trace_data->collection_entity_ipv6_addr);
    ogs_free(trace_data->interface_list);
    ogs_free(trace_data);
}

cJSON *OpenAPI_trace_data_convertToJSON(OpenAPI_trace_data_t *trace_data)
{
    cJSON *item = NULL;

    if (trace_data == NULL) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [TraceData]");
        return NULL;
    }

    item = cJSON_CreateObject();
    if (!trace_data->trace_ref) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [trace_ref]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "traceRef", trace_data->trace_ref) == NULL) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [trace_ref]");
        goto end;
    }

    if (!trace_data->trace_depth) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [trace_depth]");
        goto end;
    }
    cJSON *trace_depth_local_JSON = OpenAPI_trace_depth_convertToJSON(trace_data->trace_depth);
    if (trace_depth_local_JSON == NULL) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [trace_depth]");
        goto end;
    }
    cJSON_AddItemToObject(item, "traceDepth", trace_depth_local_JSON);
    if (item->child == NULL) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [trace_depth]");
        goto end;
    }

    if (!trace_data->ne_type_list) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [ne_type_list]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "neTypeList", trace_data->ne_type_list) == NULL) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [ne_type_list]");
        goto end;
    }

    if (!trace_data->event_list) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [event_list]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "eventList", trace_data->event_list) == NULL) {
        ogs_error("OpenAPI_trace_data_convertToJSON() failed [event_list]");
        goto end;
    }

    if (trace_data->collection_entity_ipv4_addr) {
        if (cJSON_AddStringToObject(item, "collectionEntityIpv4Addr", trace_data->collection_entity_ipv4_addr) == NULL) {
            ogs_error("OpenAPI_trace_data_convertToJSON() failed [collection_entity_ipv4_addr]");
            goto end;
        }
    }

    if (trace_data->collection_entity_ipv6_addr) {
        if (cJSON_AddStringToObject(item, "collectionEntityIpv6Addr", trace_data->collection_entity_ipv6_addr) == NULL) {
            ogs_error("OpenAPI_trace_data_convertToJSON() failed [collection_entity_ipv6_addr]");
            goto end;
        }
    }

    if (trace_data->interface_list) {
        if (cJSON_AddStringToObject(item, "interfaceList", trace_data->interface_list) == NULL) {
            ogs_error("OpenAPI_trace_data_convertToJSON() failed [interface_list]");
            goto end;
        }
    }

end:
    return item;
}

OpenAPI_trace_data_t *OpenAPI_trace_data_parseFromJSON(cJSON *trace_dataJSON)
{
    OpenAPI_trace_data_t *trace_data_local_var = NULL;
    cJSON *trace_ref = cJSON_GetObjectItemCaseSensitive(trace_dataJSON, "traceRef");
    if (!trace_ref) {
        ogs_error("OpenAPI_trace_data_parseFromJSON() failed [trace_ref]");
        goto end;
    }


    if (!cJSON_IsString(trace_ref)) {
        ogs_error("OpenAPI_trace_data_parseFromJSON() failed [trace_ref]");
        goto end;
    }

    cJSON *trace_depth = cJSON_GetObjectItemCaseSensitive(trace_dataJSON, "traceDepth");
    if (!trace_depth) {
        ogs_error("OpenAPI_trace_data_parseFromJSON() failed [trace_depth]");
        goto end;
    }

    OpenAPI_trace_depth_t *trace_depth_local_nonprim = NULL;

    trace_depth_local_nonprim = OpenAPI_trace_depth_parseFromJSON(trace_depth);

    cJSON *ne_type_list = cJSON_GetObjectItemCaseSensitive(trace_dataJSON, "neTypeList");
    if (!ne_type_list) {
        ogs_error("OpenAPI_trace_data_parseFromJSON() failed [ne_type_list]");
        goto end;
    }


    if (!cJSON_IsString(ne_type_list)) {
        ogs_error("OpenAPI_trace_data_parseFromJSON() failed [ne_type_list]");
        goto end;
    }

    cJSON *event_list = cJSON_GetObjectItemCaseSensitive(trace_dataJSON, "eventList");
    if (!event_list) {
        ogs_error("OpenAPI_trace_data_parseFromJSON() failed [event_list]");
        goto end;
    }


    if (!cJSON_IsString(event_list)) {
        ogs_error("OpenAPI_trace_data_parseFromJSON() failed [event_list]");
        goto end;
    }

    cJSON *collection_entity_ipv4_addr = cJSON_GetObjectItemCaseSensitive(trace_dataJSON, "collectionEntityIpv4Addr");

    if (collection_entity_ipv4_addr) {
        if (!cJSON_IsString(collection_entity_ipv4_addr)) {
            ogs_error("OpenAPI_trace_data_parseFromJSON() failed [collection_entity_ipv4_addr]");
            goto end;
        }
    }

    cJSON *collection_entity_ipv6_addr = cJSON_GetObjectItemCaseSensitive(trace_dataJSON, "collectionEntityIpv6Addr");

    if (collection_entity_ipv6_addr) {
        if (!cJSON_IsString(collection_entity_ipv6_addr)) {
            ogs_error("OpenAPI_trace_data_parseFromJSON() failed [collection_entity_ipv6_addr]");
            goto end;
        }
    }

    cJSON *interface_list = cJSON_GetObjectItemCaseSensitive(trace_dataJSON, "interfaceList");

    if (interface_list) {
        if (!cJSON_IsString(interface_list)) {
            ogs_error("OpenAPI_trace_data_parseFromJSON() failed [interface_list]");
            goto end;
        }
    }

    trace_data_local_var = OpenAPI_trace_data_create (
        ogs_strdup(trace_ref->valuestring),
        trace_depth_local_nonprim,
        ogs_strdup(ne_type_list->valuestring),
        ogs_strdup(event_list->valuestring),
        collection_entity_ipv4_addr ? ogs_strdup(collection_entity_ipv4_addr->valuestring) : NULL,
        collection_entity_ipv6_addr ? ogs_strdup(collection_entity_ipv6_addr->valuestring) : NULL,
        interface_list ? ogs_strdup(interface_list->valuestring) : NULL
        );

    return trace_data_local_var;
end:
    return NULL;
}

