
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "deregistration_data.h"

OpenAPI_deregistration_data_t *OpenAPI_deregistration_data_create(
    OpenAPI_deregistration_reason_t *dereg_reason,
    OpenAPI_access_type_e access_type,
    int pdu_session_id,
    char *new_smf_instance_id
    )
{
    OpenAPI_deregistration_data_t *deregistration_data_local_var = OpenAPI_malloc(sizeof(OpenAPI_deregistration_data_t));
    if (!deregistration_data_local_var) {
        return NULL;
    }
    deregistration_data_local_var->dereg_reason = dereg_reason;
    deregistration_data_local_var->access_type = access_type;
    deregistration_data_local_var->pdu_session_id = pdu_session_id;
    deregistration_data_local_var->new_smf_instance_id = new_smf_instance_id;

    return deregistration_data_local_var;
}

void OpenAPI_deregistration_data_free(OpenAPI_deregistration_data_t *deregistration_data)
{
    if (NULL == deregistration_data) {
        return;
    }
    OpenAPI_lnode_t *node;
    OpenAPI_deregistration_reason_free(deregistration_data->dereg_reason);
    ogs_free(deregistration_data->new_smf_instance_id);
    ogs_free(deregistration_data);
}

cJSON *OpenAPI_deregistration_data_convertToJSON(OpenAPI_deregistration_data_t *deregistration_data)
{
    cJSON *item = NULL;

    if (deregistration_data == NULL) {
        ogs_error("OpenAPI_deregistration_data_convertToJSON() failed [DeregistrationData]");
        return NULL;
    }

    item = cJSON_CreateObject();
    if (!deregistration_data->dereg_reason) {
        ogs_error("OpenAPI_deregistration_data_convertToJSON() failed [dereg_reason]");
        goto end;
    }
    cJSON *dereg_reason_local_JSON = OpenAPI_deregistration_reason_convertToJSON(deregistration_data->dereg_reason);
    if (dereg_reason_local_JSON == NULL) {
        ogs_error("OpenAPI_deregistration_data_convertToJSON() failed [dereg_reason]");
        goto end;
    }
    cJSON_AddItemToObject(item, "deregReason", dereg_reason_local_JSON);
    if (item->child == NULL) {
        ogs_error("OpenAPI_deregistration_data_convertToJSON() failed [dereg_reason]");
        goto end;
    }

    if (!deregistration_data->access_type) {
        ogs_error("OpenAPI_deregistration_data_convertToJSON() failed [access_type]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "accessType", OpenAPI_access_type_ToString(deregistration_data->access_type)) == NULL) {
        ogs_error("OpenAPI_deregistration_data_convertToJSON() failed [access_type]");
        goto end;
    }

    if (deregistration_data->pdu_session_id) {
        if (cJSON_AddNumberToObject(item, "pduSessionId", deregistration_data->pdu_session_id) == NULL) {
            ogs_error("OpenAPI_deregistration_data_convertToJSON() failed [pdu_session_id]");
            goto end;
        }
    }

    if (deregistration_data->new_smf_instance_id) {
        if (cJSON_AddStringToObject(item, "newSmfInstanceId", deregistration_data->new_smf_instance_id) == NULL) {
            ogs_error("OpenAPI_deregistration_data_convertToJSON() failed [new_smf_instance_id]");
            goto end;
        }
    }

end:
    return item;
}

OpenAPI_deregistration_data_t *OpenAPI_deregistration_data_parseFromJSON(cJSON *deregistration_dataJSON)
{
    OpenAPI_deregistration_data_t *deregistration_data_local_var = NULL;
    cJSON *dereg_reason = cJSON_GetObjectItemCaseSensitive(deregistration_dataJSON, "deregReason");
    if (!dereg_reason) {
        ogs_error("OpenAPI_deregistration_data_parseFromJSON() failed [dereg_reason]");
        goto end;
    }

    OpenAPI_deregistration_reason_t *dereg_reason_local_nonprim = NULL;

    dereg_reason_local_nonprim = OpenAPI_deregistration_reason_parseFromJSON(dereg_reason);

    cJSON *access_type = cJSON_GetObjectItemCaseSensitive(deregistration_dataJSON, "accessType");
    if (!access_type) {
        ogs_error("OpenAPI_deregistration_data_parseFromJSON() failed [access_type]");
        goto end;
    }

    OpenAPI_access_type_e access_typeVariable;

    if (!cJSON_IsString(access_type)) {
        ogs_error("OpenAPI_deregistration_data_parseFromJSON() failed [access_type]");
        goto end;
    }
    access_typeVariable = OpenAPI_access_type_FromString(access_type->valuestring);

    cJSON *pdu_session_id = cJSON_GetObjectItemCaseSensitive(deregistration_dataJSON, "pduSessionId");

    if (pdu_session_id) {
        if (!cJSON_IsNumber(pdu_session_id)) {
            ogs_error("OpenAPI_deregistration_data_parseFromJSON() failed [pdu_session_id]");
            goto end;
        }
    }

    cJSON *new_smf_instance_id = cJSON_GetObjectItemCaseSensitive(deregistration_dataJSON, "newSmfInstanceId");

    if (new_smf_instance_id) {
        if (!cJSON_IsString(new_smf_instance_id)) {
            ogs_error("OpenAPI_deregistration_data_parseFromJSON() failed [new_smf_instance_id]");
            goto end;
        }
    }

    deregistration_data_local_var = OpenAPI_deregistration_data_create (
        dereg_reason_local_nonprim,
        access_typeVariable,
        pdu_session_id ? pdu_session_id->valuedouble : 0,
        new_smf_instance_id ? ogs_strdup(new_smf_instance_id->valuestring) : NULL
        );

    return deregistration_data_local_var;
end:
    return NULL;
}

