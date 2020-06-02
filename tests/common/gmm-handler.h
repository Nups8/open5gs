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

#ifndef TEST_GMM_HANDLE_H
#define TEST_GMM_HANDLE_H

#ifdef __cplusplus
extern "C" {
#endif

void testgmm_handle_authentication_request(test_ue_t *test_ue,
        ogs_nas_5gs_authentication_request_t *authentication_request);

#ifdef __cplusplus
}
#endif

#endif /* TEST_GMM_HANDLE_H */
