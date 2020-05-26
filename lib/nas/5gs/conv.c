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

#include "ogs-nas-5gs.h"

void ogs_nas_5gs_imsi_to_bcd(
    ogs_nas_5gs_mobile_identity_t *mobile_identity, char *imsi_bcd)
{
    ogs_nas_5gs_mobile_identity_imsi_t *mobile_identity_imsi = NULL;
    ogs_plmn_id_t plmn_id;
    char tmp[OGS_MAX_IMSI_BCD_LEN+1];
    char *p, *last;

    ogs_assert(mobile_identity);
    ogs_assert(imsi_bcd);

    p = imsi_bcd;
    last = imsi_bcd + OGS_MAX_IMSI_BCD_LEN + 1;

    mobile_identity_imsi =
        (ogs_nas_5gs_mobile_identity_imsi_t *)mobile_identity->buffer;
    ogs_assert(mobile_identity_imsi);

    ogs_nas_to_plmn_id(&plmn_id, &mobile_identity_imsi->nas_plmn_id);
    if (ogs_plmn_id_mnc_len(&plmn_id) == 2)
        p = ogs_slprintf(p, last, "%03d%02d",
                ogs_plmn_id_mcc(&plmn_id), ogs_plmn_id_mnc(&plmn_id));
    else 
        p = ogs_slprintf(p, last, "%03d%03d",
                ogs_plmn_id_mcc(&plmn_id), ogs_plmn_id_mnc(&plmn_id));

    ogs_assert(mobile_identity->length > 8);
    ogs_buffer_to_bcd(mobile_identity_imsi->scheme_output,
            mobile_identity->length - 8, tmp);
    p = ogs_slprintf(p, last, "%s", tmp);
}
