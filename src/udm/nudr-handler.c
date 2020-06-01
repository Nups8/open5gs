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

#include "nudr-handler.h"

bool udm_nudr_dr_handle_get(
        ogs_sbi_session_t *session, ogs_sbi_message_t *recvmsg)
{
    udm_ue_t *udm_ue = NULL;

    ogs_sbi_message_t sendmsg;
    ogs_sbi_response_t *response = NULL;

    const char *tmp = "4d45b0eeb8386b629f136968837a7b0b"; /* For test */
    uint8_t k[OGS_KEY_LEN];
    uint8_t opc[OGS_KEY_LEN];
    uint8_t amf[OGS_AMF_LEN];
    uint8_t sqn[OGS_SQN_LEN];
    uint8_t rand[OGS_RAND_LEN];
    uint8_t autn[OGS_AUTN_LEN];
    uint8_t ik[OGS_KEY_LEN];
    uint8_t ck[OGS_KEY_LEN];
    uint8_t ak[OGS_AK_LEN];
    uint8_t xres[OGS_MAX_RES_LEN];
    size_t xres_len = 8;
    uint8_t xres_star[OGS_MAX_RES_LEN];
    uint8_t kausf[OGS_SHA256_DIGEST_SIZE];

    char rand_string[OGS_KEYSTRLEN(OGS_RAND_LEN)];
    char autn_string[OGS_KEYSTRLEN(OGS_AUTN_LEN)];
    char kausf_string[OGS_KEYSTRLEN(OGS_SHA256_DIGEST_SIZE)];
    char xres_star_string[OGS_KEYSTRLEN(OGS_MAX_RES_LEN)];

    OpenAPI_authentication_subscription_t *AuthenticationSubscription = NULL;
    OpenAPI_authentication_info_result_t AuthenticationInfoResult;
    OpenAPI_authentication_vector_t AuthenticationVector;

    ogs_assert(session);
    udm_ue = ogs_sbi_session_get_data(session);
    ogs_assert(udm_ue);

    ogs_assert(recvmsg);
    AuthenticationSubscription = recvmsg->AuthenticationSubscription;
    ogs_assert(AuthenticationSubscription);

    SWITCH(recvmsg->h.resource.component[2])
    CASE(OGS_SBI_RESOURCE_NAME_AUTHENTICATION_DATA)
        SWITCH(recvmsg->h.resource.component[3])
        CASE(OGS_SBI_RESOURCE_NAME_AUTHENTICATION_SUBSCRIPTION)

            if (AuthenticationSubscription->authentication_method !=
                    OpenAPI_auth_method_5G_AKA) {
                ogs_error("[%s] Not supported Auth Method [%d]",
                        udm_ue->id,
                        AuthenticationSubscription->authentication_method);
                ogs_sbi_server_send_error(session,
                        OGS_SBI_HTTP_STATUS_NOT_IMPLEMENTED,
                        recvmsg, "Not supported Auth Method", udm_ue->id);
                return false;

            }

            if (!AuthenticationSubscription->enc_permanent_key) {
                ogs_error("[%s] No encPermanentKey", udm_ue->id);
                ogs_sbi_server_send_error(session,
                        OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR,
                        recvmsg, "No encPermanentKey", udm_ue->id);
                return false;
            }
            if (!AuthenticationSubscription->enc_opc_key) {
                ogs_error("[%s] No encPermanentKey", udm_ue->id);
                ogs_sbi_server_send_error(session,
                    OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR,
                    recvmsg, "No encPermanentKey", udm_ue->id);
                return false;
            }
            if (!AuthenticationSubscription->authentication_management_field) {
                ogs_error("[%s] No authenticationManagementField", udm_ue->id);
                ogs_sbi_server_send_error(session,
                    OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR,
                    recvmsg, "No authenticationManagementField", udm_ue->id);
                return false;
            }
            if (!AuthenticationSubscription->sequence_number) {
                ogs_error("[%s] No SequenceNumber", udm_ue->id);
                ogs_sbi_server_send_error(session,
                    OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR,
                    recvmsg, "No SequenceNumber", udm_ue->id);
                return false;
            }
            if (AuthenticationSubscription->sequence_number->sqn_scheme !=
                    OpenAPI_sqn_scheme_NON_TIME_BASED) {
                ogs_error("[%s] No SequenceNumber.sqnScheme [%d]", udm_ue->id,
                    AuthenticationSubscription->sequence_number->sqn_scheme);
                ogs_sbi_server_send_error(session,
                    OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR,
                    recvmsg, "No SequenceNumber.sqnScheme", udm_ue->id);
                return false;
            }
            if (!AuthenticationSubscription->sequence_number->sqn) {
                ogs_error("[%s] No SequenceNumber.sqn", udm_ue->id);
                ogs_sbi_server_send_error(session,
                    OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR,
                    recvmsg, "No SequenceNumber.sqn", udm_ue->id);
                return false;
            }

            memset(&AuthenticationInfoResult,
                    0, sizeof(AuthenticationInfoResult));

            AuthenticationInfoResult.auth_type = OpenAPI_auth_type_5G_AKA;

            /* FIX IT! TODO! NEW! */
            ogs_random(rand, OGS_RAND_LEN);
            OGS_HEX(tmp, strlen(tmp), rand);

            ogs_ascii_to_hex(
                AuthenticationSubscription->enc_opc_key,
                strlen(AuthenticationSubscription->enc_opc_key),
                opc, sizeof(opc));
            ogs_ascii_to_hex(
                AuthenticationSubscription->authentication_management_field,
                strlen(AuthenticationSubscription->
                    authentication_management_field),
                amf, sizeof(amf));
            ogs_ascii_to_hex(
                AuthenticationSubscription->enc_permanent_key,
                strlen(AuthenticationSubscription->enc_permanent_key),
                k, sizeof(k));
            ogs_ascii_to_hex(
                AuthenticationSubscription->sequence_number->sqn,
                strlen(AuthenticationSubscription->sequence_number->sqn),
                sqn, sizeof(sqn));
            milenage_generate(opc, amf, k, sqn, rand, autn, ik, ck, ak,
                xres, &xres_len);

            ogs_assert(udm_ue->serving_network_name);

            /* TS33.501 Annex A.2 : Kausf derviation function */
            ogs_kdf_kausf(
                    ck, ik,
                    udm_ue->serving_network_name, autn,
                    kausf);

            /* TS33.501 Annex A.4 : RES* and XRES* derivation function */
            ogs_kdf_xres_star(
                    ck, ik,
                    udm_ue->serving_network_name, rand, xres, xres_len,
                    xres_star);

            memset(&AuthenticationVector, 0, sizeof(AuthenticationVector));
            AuthenticationVector.av_type = OpenAPI_av_type_5G_HE_AKA;

            ogs_hex_to_ascii(rand, sizeof(rand),
                    rand_string, sizeof(rand_string));
            AuthenticationVector.rand = rand_string;
            ogs_hex_to_ascii(xres_star, sizeof(xres_star),
                    xres_star_string, sizeof(xres_star_string));
            AuthenticationVector.xres_star = xres_star_string;
            ogs_hex_to_ascii(autn, sizeof(autn),
                    autn_string, sizeof(autn_string));
            AuthenticationVector.autn = autn_string;
            ogs_hex_to_ascii(kausf, sizeof(kausf),
                    kausf_string, sizeof(kausf_string));
            AuthenticationVector.kausf = kausf_string;

            AuthenticationInfoResult.authentication_vector =
                &AuthenticationVector;
            break;

        DEFAULT
            ogs_error("Invalid resource name [%s]",
                    recvmsg->h.resource.component[3]);
            ogs_sbi_server_send_error(session,
                    OGS_SBI_HTTP_STATUS_MEHTOD_NOT_ALLOWED,
                    recvmsg, "Unknown resource name",
                    recvmsg->h.resource.component[3]);
            return false;
        END
        break;

    DEFAULT
        ogs_error("Invalid resource name [%s]",
                recvmsg->h.resource.component[2]);
        ogs_sbi_server_send_error(session,
                OGS_SBI_HTTP_STATUS_MEHTOD_NOT_ALLOWED,
                recvmsg, "Unknown resource name",
                recvmsg->h.resource.component[2]);
    END

    memset(&sendmsg, 0, sizeof(sendmsg));

    ogs_assert(AuthenticationInfoResult.auth_type);
    sendmsg.AuthenticationInfoResult = &AuthenticationInfoResult;

    response = ogs_sbi_build_response(&sendmsg, OGS_SBI_HTTP_STATUS_OK);
    ogs_assert(response);
    ogs_sbi_server_send_response(session, response);

    return true;
}