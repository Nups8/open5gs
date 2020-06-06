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

#ifndef UDM_SBI_PATH_H
#define UDM_SBI_PATH_H

#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "nudr-build.h"

int udm_sbi_open(void);
void udm_sbi_close(void);

void udm_sbi_setup_client_callback(ogs_sbi_nf_instance_t *nf_instance);

void udm_sbi_discover_and_send(
        udm_ue_t *udm_ue, OpenAPI_nf_type_e nf_type,
        void (*discover_handler)(
            udm_ue_t *udm_ue, ogs_sbi_nf_instance_t *nf_instance));

void udm_nudr_dr_send_query_authentication(
        udm_ue_t *udm_ue, ogs_sbi_nf_instance_t *nf_instance);
void udm_nudr_dr_send_update_authentication(
        udm_ue_t *udm_ue, ogs_sbi_nf_instance_t *nf_instance);
void udm_nudr_dr_send_update_context(
        udm_ue_t *udm_ue, ogs_sbi_nf_instance_t *nf_instance);

#ifdef __cplusplus
}
#endif

#endif /* UDM_SBI_PATH_H */
