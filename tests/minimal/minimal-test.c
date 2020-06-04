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

#include "test-5gc.h"

static void test1_func(abts_case *tc, void *data)
{
    int rv;
    ogs_socknode_t *ngap;
    ogs_socknode_t *gtpu;
    ogs_pkbuf_t *gmmbuf;
    ogs_pkbuf_t *nasbuf;
    ogs_pkbuf_t *sendbuf;
    ogs_pkbuf_t *recvbuf;
    ogs_ngap_message_t message;
    int i;
    int msgindex = 0;

    ogs_nas_5gs_mobile_identity_t mobile_identity;
    ogs_nas_5gs_mobile_identity_imsi_t mobile_identity_imsi;
    test_ue_t test_ue;

    uint8_t tmp[OGS_MAX_SDU_LEN];
    const char *_ng_setup_request =
        "0015002d00000300 1b00080002f83910 0001020066001500 000000010002f839"
        "0001100801020310 0811223300154001 20";
    const char *_ng_setup_response =
        "2015003a00000400 01000e05806f7065 6e3567732d616d66 3000600008000002"
        "f839cafe00005640 01ff005000100002 f839000110080102 031008112233";
    const char *_authentication_request = 
        "000b403b00000300 000005c00100009d 000800020001001a 0025240752002008"
        "0c3818183b522614 162c07601d0d10f1 1b89a2a8de8000ad 0ccf7f55e8b20d";
    const char *_security_mode_command = 
        "000b4028"
        "00000300000005c0 0100009d00080002 0001001a00121137 f497722900075d01"
        "0005e060c04070c1";
    const char *_esm_information_request =
        "000b402000000300 000005c00100009d 000800020001001a 000a092779012320"
        "010221d9";
    const char *_initial_context_setup_request = 
        "00090080d8000006 00000005c0010000 9d00080002000100 42000a183e800000"
        "603e800000001800 8086000034008080 450009200f807f00 0002000000017127"
        "4db5d98302074202 49064000f1105ba0 00485221c1010909 08696e7465726e65"
        "7405010a2d00025e 06fefeeeee030327 2980c22304030000 0480211002000010"
        "8106080808088306 08080404000d0408 080808000d040808 0404500bf600f110"
        "0002010000000153 12172c5949640125 006b000518000c00 00004900203311c6"
        "03c6a6d67f695e5a c02bb75b381b693c 3893a6d932fd9182 3544e3e79b";
    const char *_emm_information = 
        "000b403b00000300 000005c00100009d 000800020001001a 002524271f9b491e"
        "030761430f10004f 00700065006e0035 0047005347812072 11240563490100";

    const char *_k_string = "5122250214c33e723a5dd523fc145fc0";
    uint8_t k[OGS_KEY_LEN];
    const char *_opc_string = "981d464c7c52eb6e5036234984ad0bcf";
    uint8_t opc[OGS_KEY_LEN];

    mongoc_collection_t *collection = NULL;
    bson_t *doc = NULL;
    int64_t count = 0;
    bson_error_t error;
    const char *json =
      "{"
        "\"_id\" : { \"$oid\" : \"597223158b8861d7605378c6\" }, "
        "\"imsi\" : \"2089300007487\", "
        "\"pdn\" : ["
          "{"
            "\"apn\" : \"internet\", "
            "\"_id\" : { \"$oid\" : \"597223158b8861d7605378c7\" }, "
            "\"ambr\" : {"
              "\"uplink\" : { \"$numberLong\" : \"1024000\" }, "
              "\"downlink\" : { \"$numberLong\" : \"1024000\" } "
            "},"
            "\"qos\" : { "
              "\"qci\" : 9, "
              "\"arp\" : { "
                "\"priority_level\" : 8,"
                "\"pre_emption_vulnerability\" : 1, "
                "\"pre_emption_capability\" : 1"
              "} "
            "}, "
            "\"type\" : 2"
          "}"
        "],"
        "\"ambr\" : { "
          "\"uplink\" : { \"$numberLong\" : \"1024000\" }, "
          "\"downlink\" : { \"$numberLong\" : \"1024000\" } "
        "},"
        "\"subscribed_rau_tau_timer\" : 12,"
        "\"network_access_mode\" : 2, "
        "\"subscriber_status\" : 0, "
        "\"access_restriction_data\" : 32, "
        "\"security\" : { "
          "\"k\" : \"5122250214c33e723a5dd523fc145fc0\", "
          "\"opc\" : \"981d464c7c52eb6e5036234984ad0bcf\", "
          "\"amf\" : \"8000\", "
          "\"sqn\" : { \"$numberLong\" : \"25235952177090\" }, "
          "\"rand\" : \"20080C38 18183B52 2614162C 07601D0D\" "
        "}, "
        "\"__v\" : 0 "
      "}";

    /* gNB connects to AMF */
    ngap = testgnb_ngap_client("127.0.0.1");
    ABTS_PTR_NOTNULL(tc, ngap);

#if 0
    /* eNB connects to SGW */
    gtpu = testgnb_gtpu_server("127.0.0.5");
    ABTS_PTR_NOTNULL(tc, gtpu);
#endif

    /* Send NG-Setup Reqeust */
    sendbuf = testngap_build_ng_setup_request(0x000102);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    ABTS_TRUE(tc, memcmp(sendbuf->data,
        OGS_HEX(_ng_setup_request, strlen(_ng_setup_request), tmp),
        sendbuf->len) == 0);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive NG-Setup Response */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ABTS_TRUE(tc, memcmp(recvbuf->data,
        OGS_HEX(_ng_setup_response, strlen(_ng_setup_response), tmp),
        recvbuf->len) == 0);
    ogs_pkbuf_free(recvbuf);

    /* Setup Test UE Context */
    memset(&test_ue, 0, sizeof(test_ue));

    memset(&mobile_identity_imsi, 0, sizeof(mobile_identity_imsi));
    mobile_identity.length = 12;
    mobile_identity.buffer = &mobile_identity_imsi;

    mobile_identity_imsi.h.supi_format = OGS_NAS_5GS_SUPI_FORMAT_IMSI;
    mobile_identity_imsi.h.type = OGS_NAS_5GS_MOBILE_IDENTITY_SUCI;
    ogs_nas_from_plmn_id(&mobile_identity_imsi.nas_plmn_id,
            &test_self()->tai.plmn_id);
    mobile_identity_imsi.routing_indicator1 = 0;
    mobile_identity_imsi.routing_indicator2 = 0xf;
    mobile_identity_imsi.routing_indicator3 = 0xf;
    mobile_identity_imsi.routing_indicator4 = 0xf;
    mobile_identity_imsi.protection_scheme_id = OGS_NAS_5GS_NULL_SCHEME;
    mobile_identity_imsi.home_network_pki_value = 0;
    mobile_identity_imsi.scheme_output[0] = 0;
    mobile_identity_imsi.scheme_output[1] = 0;
    mobile_identity_imsi.scheme_output[2] = 0x47;
    mobile_identity_imsi.scheme_output[3] = 0x78;

    test_ue_set_mobile_identity(&test_ue, &mobile_identity);

    test_ue.abba_len = 2;
    test_ue.nas.connection_identifier = OGS_NAS_SECURITY_BEARER_3GPP;

    OGS_HEX(_k_string, strlen(_k_string), test_ue.k);
    OGS_HEX(_opc_string, strlen(_opc_string), test_ue.opc);

    /********** Insert Subscriber in Database */
    collection = mongoc_client_get_collection(
        ogs_mongoc()->client, ogs_mongoc()->name, "subscribers");
    ABTS_PTR_NOTNULL(tc, collection);

    doc = bson_new_from_json((const uint8_t *)json, -1, &error);;
    ABTS_PTR_NOTNULL(tc, doc);
    ABTS_TRUE(tc, mongoc_collection_insert(collection,
                MONGOC_INSERT_NONE, doc, NULL, &error));
    bson_destroy(doc);

    doc = BCON_NEW("imsi", BCON_UTF8(test_ue.imsi));
    ABTS_PTR_NOTNULL(tc, doc);
    do {
        count = mongoc_collection_count (
            collection, MONGOC_QUERY_NONE, doc, 0, 0, NULL, &error);
    } while (count == 0);
    bson_destroy(doc);

    /* Send Registration request */
    gmmbuf = testgmm_build_registration_request(&test_ue, &mobile_identity);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_initial_ue_message(gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Authentication request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(&test_ue, recvbuf);
#if 0
    ABTS_TRUE(tc, memcmp(recvbuf->data, 
        OGS_HEX(_authentication_request, strlen(_authentication_request), tmp),
        recvbuf->len) == 0);
#endif

    /* Send Authentication response */
    gmmbuf = testgmm_build_authentication_response(&test_ue);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(&test_ue, gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Security mode command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(&test_ue, recvbuf);
#if 0
    ABTS_TRUE(tc, memcmp(recvbuf->data,
        OGS_HEX(_security_mode_command, strlen(_security_mode_command), tmp),
        recvbuf->len) == 0);
#endif

    /* Send Security mode complete */
    nasbuf = testgmm_build_registration_request(&test_ue, &mobile_identity);
    ABTS_PTR_NOTNULL(tc, nasbuf);
    gmmbuf = testgmm_build_security_mode_complete(&test_ue, nasbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(&test_ue, gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    ogs_pkbuf_free(nasbuf);

    ogs_msleep(300);
#if 0

    /* Send Security mode Complete */
    rv = testngap_build_security_mode_complete(&sendbuf, msgindex);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive ESM Information Request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ABTS_TRUE(tc, memcmp(recvbuf->data, 
        OGS_HEX(_esm_information_request, strlen(_security_mode_command), tmp),
        recvbuf->len) == 0);
    ogs_pkbuf_free(recvbuf);

    /* Send ESM Information Response */
    rv = testngap_build_esm_information_response(&sendbuf, msgindex);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Initial Context Setup Request + 
     * Attach Accept + 
     * Activate Default Bearer Context Request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    OGS_HEX(_initial_context_setup_request,
            strlen(_initial_context_setup_request), tmp);
    ABTS_TRUE(tc, memcmp(recvbuf->data, tmp, 62) == 0);
    ABTS_TRUE(tc, memcmp(recvbuf->data+66, tmp+66, 78) == 0);
    ABTS_TRUE(tc, memcmp(recvbuf->data+148, tmp+148, 50) == 0);
    ogs_pkbuf_free(recvbuf);

    /* Send UE Capability Info Indication */
    rv = testngap_build_ue_capability_info_indication(&sendbuf, msgindex);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    ogs_msleep(50);

    /* Send Initial Context Setup Response */
    rv = testngap_build_initial_context_setup_response(&sendbuf,
            27263233, 24, 5, 1, "127.0.0.5");
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send Attach Complete + Activate default EPS bearer cotext accept */
    rv = testngap_build_attach_complete(&sendbuf, msgindex);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    ogs_msleep(50);

    /* Receive EMM information */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    OGS_HEX(_emm_information, strlen(_emm_information), tmp);
    ABTS_TRUE(tc, memcmp(recvbuf->data, tmp, 28) == 0);
    ABTS_TRUE(tc, memcmp(recvbuf->data+32, tmp+32, 20) == 0);
    ogs_pkbuf_free(recvbuf);

    /* Send GTP-U ICMP Packet */
    rv = testgtpu_build_ping(&sendbuf, 1, "10.45.0.2", "10.45.0.1");
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    rv = testgnb_gtpu_send(gtpu, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);
#endif

    /********** Remove Subscriber in Database */
    doc = BCON_NEW("imsi", BCON_UTF8(test_ue.imsi));
    ABTS_PTR_NOTNULL(tc, doc);
    ABTS_TRUE(tc, mongoc_collection_remove(collection,
            MONGOC_REMOVE_SINGLE_REMOVE, doc, NULL, &error)) 
    bson_destroy(doc);

    mongoc_collection_destroy(collection);

    /* Clear Test UE Context */
    test_ue_remove(&test_ue);

#if 0
    /* eNB disonncect from SGW */
    testgnb_gtpu_close(gtpu);
#endif

    /* gNB disonncect from AMF */
    testgnb_ngap_close(ngap);
}

abts_suite *test_minimal(abts_suite *suite)
{
    suite = ADD_SUITE(suite)

    abts_run_test(suite, test1_func, NULL);

    return suite;
}
