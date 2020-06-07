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

#ifndef AMF_SBI_PATH_H
#define AMF_SBI_PATH_H

#include "nausf-build.h"
#include "nudm-build.h"

#ifdef __cplusplus
extern "C" {
#endif

int amf_sbi_open(void);
void amf_sbi_close(void);

void amf_sbi_setup_client_callback(ogs_sbi_nf_instance_t *nf_instance);

int amf_sbi_discover_and_send(
        amf_ue_t *amf_ue, OpenAPI_nf_type_e nf_type,
        int (*discover_handler)(
            amf_ue_t *amf_ue, ogs_sbi_nf_instance_t *nf_instance));
void amf_sbi_discover_and_send2(
        amf_ue_t *amf_ue, OpenAPI_nf_type_e nf_type,
        ogs_sbi_request_t *(*build)(amf_ue_t *amf_ue));
void amf_sbi_send(amf_ue_t *amf_ue, ogs_sbi_nf_instance_t *nf_instance);

int amf_nausf_auth_send_authenticate(
        amf_ue_t *amf_ue, ogs_sbi_nf_instance_t *nf_instance);
int amf_nausf_auth_send_authenticate_confirmation(
        amf_ue_t *amf_ue, ogs_sbi_nf_instance_t *nf_instance);
int amf_nudm_uecm_send_registration(
        amf_ue_t *amf_ue, ogs_sbi_nf_instance_t *nf_instance);
int amf_nudm_sdm_send_get(amf_ue_t *amf_ue, ogs_sbi_nf_instance_t *nf_instance);

#ifdef __cplusplus
}
#endif

#endif /* AMF_SBI_PATH_H */
