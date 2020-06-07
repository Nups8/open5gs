/*
 * Copyright (C) 2019,2020 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "sbi-path.h"

static int server_cb(ogs_sbi_server_t *server,
        ogs_sbi_session_t *session, ogs_sbi_request_t *request)
{
    udm_event_t *e = NULL;
    int rv;

    ogs_assert(session);
    ogs_assert(request);

    e = udm_event_new(UDM_EVT_SBI_SERVER);
    ogs_assert(e);

    e->sbi.server = server;
    e->sbi.session = session;
    e->sbi.request = request;

    rv = ogs_queue_push(udm_self()->queue, e);
    if (rv != OGS_OK) {
        ogs_warn("ogs_queue_push() failed:%d", (int)rv);
        udm_event_free(e);
        return OGS_ERROR;
    }

    return OGS_OK;
}

static int client_cb(ogs_sbi_response_t *response, void *data)
{
    udm_event_t *e = NULL;
    int rv;

    ogs_assert(response);

    e = udm_event_new(UDM_EVT_SBI_CLIENT);
    ogs_assert(e);
    e->sbi.response = response;
    e->sbi.data = data;

    rv = ogs_queue_push(udm_self()->queue, e);
    if (rv != OGS_OK) {
        ogs_warn("ogs_queue_push() failed:%d", (int)rv);
        udm_event_free(e);
        return OGS_ERROR;
    }

    return OGS_OK;
}

int udm_sbi_open(void)
{
    ogs_sbi_nf_instance_t *nf_instance = NULL;

    ogs_sbi_server_start_all(server_cb);

    ogs_list_for_each(&ogs_sbi_self()->nf_instance_list, nf_instance) {
        ogs_sbi_nf_service_t *service = NULL;

        ogs_sbi_nf_instance_build_default(nf_instance, udm_self()->nf_type);

        service = ogs_sbi_nf_service_build_default(nf_instance,
                (char*)OGS_SBI_SERVICE_NAME_NUDM_UEAU);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(service, (char*)OGS_SBI_API_V1,
                (char*)OGS_SBI_API_V1_0_0, NULL);
        service = ogs_sbi_nf_service_build_default(nf_instance,
                (char*)OGS_SBI_SERVICE_NAME_NUDM_UECM);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(service, (char*)OGS_SBI_API_V1,
                (char*)OGS_SBI_API_V1_0_0, NULL);
        service = ogs_sbi_nf_service_build_default(nf_instance,
                (char*)OGS_SBI_SERVICE_NAME_NUDM_SDM);
        ogs_assert(service);
        ogs_sbi_nf_service_add_version(service, (char*)OGS_SBI_API_V2,
                (char*)OGS_SBI_API_V2_0_0, NULL);

        udm_nf_fsm_init(nf_instance);
        udm_sbi_setup_client_callback(nf_instance);
    }

    return OGS_OK;
}

void udm_sbi_close(void)
{
    ogs_sbi_server_stop_all();
}

void udm_sbi_setup_client_callback(ogs_sbi_nf_instance_t *nf_instance)
{
    ogs_sbi_client_t *client = NULL;
    ogs_sbi_nf_service_t *nf_service = NULL;
    ogs_assert(nf_instance);

    client = nf_instance->client;
    ogs_assert(client);

    client->cb = client_cb;

    ogs_list_for_each(&nf_instance->nf_service_list, nf_service) {
        client = nf_service->client;
        if (client)
            client->cb = client_cb;
    }
}

static ogs_sbi_nf_instance_t *find_or_discover_nf_instance(udm_ue_t *udm_ue)
{
    ogs_sbi_session_t *session = NULL;
    bool nrf = true;
    bool nf = true;

    ogs_assert(udm_ue);
    session = udm_ue->session;
    ogs_assert(session);
    ogs_assert(udm_ue->sbi.nf_type);

    if (!OGS_SBI_NF_INSTANCE_GET(udm_ue->nf_types, OpenAPI_nf_type_NRF))
        nrf = ogs_sbi_nf_types_associate(udm_ue->nf_types,
                OpenAPI_nf_type_NRF, udm_nf_state_registered);
    if (!OGS_SBI_NF_INSTANCE_GET(udm_ue->nf_types,
                udm_ue->sbi.nf_type))
        nf = ogs_sbi_nf_types_associate(udm_ue->nf_types,
                udm_ue->sbi.nf_type, udm_nf_state_registered);

    if (nrf == false && nf == false) {
        ogs_error("[%s] Cannot discover [%s]", udm_ue->suci,
                OpenAPI_nf_type_ToString(udm_ue->sbi.nf_type));
        ogs_sbi_server_send_error(session,
                OGS_SBI_HTTP_STATUS_SERVICE_UNAVAILABLE, NULL,
                "Cannot discover", udm_ue->suci);
        return NULL;
    }

    if (nf == false) {
        ogs_warn("[%s] Try to discover [%s]", udm_ue->suci,
                OpenAPI_nf_type_ToString(udm_ue->sbi.nf_type));
        ogs_timer_start(udm_ue->sbi.client_wait_timer,
                udm_timer_cfg(UDM_TIMER_SBI_CLIENT_WAIT)->duration);

        ogs_nnrf_disc_send_nf_discover(
            udm_ue->nf_types[OpenAPI_nf_type_NRF].nf_instance,
            udm_ue->sbi.nf_type, OpenAPI_nf_type_UDM, udm_ue);

        return NULL;
    }

    return udm_ue->nf_types[udm_ue->sbi.nf_type].nf_instance;
}

void udm_sbi_send(udm_ue_t *udm_ue, ogs_sbi_nf_instance_t *nf_instance)
{
    ogs_sbi_request_t *request = NULL;

    ogs_assert(udm_ue);
    request = udm_ue->sbi.request;
    ogs_assert(request);

    ogs_assert(nf_instance);

    ogs_timer_start(udm_ue->sbi.client_wait_timer,
            udm_timer_cfg(UDM_TIMER_SBI_CLIENT_WAIT)->duration);

    ogs_sbi_client_send_request_to_nf_instance(
            nf_instance, udm_ue->sbi.request, udm_ue);
}

void udm_sbi_discover_and_send(
        OpenAPI_nf_type_e nf_type, udm_ue_t *udm_ue, void *data,
        ogs_sbi_request_t *(*build)(udm_ue_t *udm_ue, void *data))
{
    ogs_sbi_nf_instance_t *nf_instance = NULL;

    ogs_assert(udm_ue);
    ogs_assert(nf_type);
    ogs_assert(build);

    udm_ue->sbi.nf_type = nf_type;
    if (udm_ue->sbi.request)
        ogs_sbi_request_free(udm_ue->sbi.request);
    udm_ue->sbi.request = (*build)(udm_ue, data);

    if (!nf_instance)
        nf_instance = find_or_discover_nf_instance(udm_ue);

    if (!nf_instance) return;

    return udm_sbi_send(udm_ue, nf_instance);
}
