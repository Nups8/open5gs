
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "valid_time_period.h"

OpenAPI_valid_time_period_t *OpenAPI_valid_time_period_create(
    char *start_time,
    char *end_time
    )
{
    OpenAPI_valid_time_period_t *valid_time_period_local_var = OpenAPI_malloc(sizeof(OpenAPI_valid_time_period_t));
    if (!valid_time_period_local_var) {
        return NULL;
    }
    valid_time_period_local_var->start_time = start_time;
    valid_time_period_local_var->end_time = end_time;

    return valid_time_period_local_var;
}

void OpenAPI_valid_time_period_free(OpenAPI_valid_time_period_t *valid_time_period)
{
    if (NULL == valid_time_period) {
        return;
    }
    OpenAPI_lnode_t *node;
    ogs_free(valid_time_period->start_time);
    ogs_free(valid_time_period->end_time);
    ogs_free(valid_time_period);
}

cJSON *OpenAPI_valid_time_period_convertToJSON(OpenAPI_valid_time_period_t *valid_time_period)
{
    cJSON *item = NULL;

    if (valid_time_period == NULL) {
        ogs_error("OpenAPI_valid_time_period_convertToJSON() failed [ValidTimePeriod]");
        return NULL;
    }

    item = cJSON_CreateObject();
    if (valid_time_period->start_time) {
        if (cJSON_AddStringToObject(item, "startTime", valid_time_period->start_time) == NULL) {
            ogs_error("OpenAPI_valid_time_period_convertToJSON() failed [start_time]");
            goto end;
        }
    }

    if (valid_time_period->end_time) {
        if (cJSON_AddStringToObject(item, "endTime", valid_time_period->end_time) == NULL) {
            ogs_error("OpenAPI_valid_time_period_convertToJSON() failed [end_time]");
            goto end;
        }
    }

end:
    return item;
}

OpenAPI_valid_time_period_t *OpenAPI_valid_time_period_parseFromJSON(cJSON *valid_time_periodJSON)
{
    OpenAPI_valid_time_period_t *valid_time_period_local_var = NULL;
    cJSON *start_time = cJSON_GetObjectItemCaseSensitive(valid_time_periodJSON, "startTime");

    if (start_time) {
        if (!cJSON_IsString(start_time)) {
            ogs_error("OpenAPI_valid_time_period_parseFromJSON() failed [start_time]");
            goto end;
        }
    }

    cJSON *end_time = cJSON_GetObjectItemCaseSensitive(valid_time_periodJSON, "endTime");

    if (end_time) {
        if (!cJSON_IsString(end_time)) {
            ogs_error("OpenAPI_valid_time_period_parseFromJSON() failed [end_time]");
            goto end;
        }
    }

    valid_time_period_local_var = OpenAPI_valid_time_period_create (
        start_time ? ogs_strdup(start_time->valuestring) : NULL,
        end_time ? ogs_strdup(end_time->valuestring) : NULL
        );

    return valid_time_period_local_var;
end:
    return NULL;
}

