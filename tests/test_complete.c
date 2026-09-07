/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * test_complete.c - comprehensive test suite for all field types
 *
 * Defines two custom variants to exercise every field type:
 *   Variant 0: "complete" — all bundled field types plus AQ PM/Gas,
 *              depth, and image (3 presence bytes, 16 fields)
 *   Variant 1: "standalone" — all standalone sub-field types
 *              (3 presence bytes, 15 fields)
 *
 * Tests: field round-trips, boundary values, error conditions,
 * peek, TLV typed helpers, JSON round-trip with TLV, decode
 * error paths, encode buffer overflow, and image compression.
 */

#include "test_common.h"

/* ---------------------------------------------------------------------------
 * Custom variant definitions
 * -------------------------------------------------------------------------*/

const iotdata_variant_def_t complete_variants[2] = {
    /* Variant 0: complete — bundled fields + extras not in default */
    [0] = {
        .name = "complete",
        .num_pres_bytes = 3,
        .fields = {
            /* pres0 (6 fields) */
            { IOTDATA_FIELD_BATTERY,           "battery" },
            { IOTDATA_FIELD_LINK,              "link" },
            { IOTDATA_FIELD_ENVIRONMENT,       "environment" },
            { IOTDATA_FIELD_WIND,              "wind" },
            { IOTDATA_FIELD_RAIN,              "rain" },
            { IOTDATA_FIELD_SOLAR,             "solar" },
            /* pres1 (7 fields) */
            { IOTDATA_FIELD_CLOUDS,            "clouds" },
            { IOTDATA_FIELD_AIR_QUALITY_INDEX, "air_quality" },
            { IOTDATA_FIELD_AIR_QUALITY_PM,    "air_quality_pm" },
            { IOTDATA_FIELD_AIR_QUALITY_GAS,   "air_quality_gas" },
            { IOTDATA_FIELD_RADIATION,         "radiation" },
            { IOTDATA_FIELD_DEPTH,             "depth" },
            { IOTDATA_FIELD_POSITION,          "position" },
            /* pres2 (7 fields) */
            { IOTDATA_FIELD_DATETIME,          "datetime" },
            { IOTDATA_FIELD_IMAGE,             "image" },
            { IOTDATA_FIELD_FLAGS,             "flags" },
            { IOTDATA_FIELD_NONE,              NULL },
            { IOTDATA_FIELD_NONE,              NULL },
            { IOTDATA_FIELD_NONE,              NULL },
            { IOTDATA_FIELD_NONE,              NULL },
        },
    },
    /* Variant 1: standalone — individual sub-field types */
    [1] = {
        .name = "standalone",
        .num_pres_bytes = 3,
        .fields = {
            /* pres0 (6 fields) */
            { IOTDATA_FIELD_BATTERY,           "battery" },
            { IOTDATA_FIELD_TEMPERATURE,       "temperature" },
            { IOTDATA_FIELD_PRESSURE,          "pressure" },
            { IOTDATA_FIELD_HUMIDITY,          "humidity" },
            { IOTDATA_FIELD_WIND_SPEED,        "wind_speed" },
            { IOTDATA_FIELD_WIND_DIRECTION,    "wind_direction" },
            /* pres1 (7 fields) */
            { IOTDATA_FIELD_WIND_GUST,         "wind_gust" },
            { IOTDATA_FIELD_RAIN_RATE,         "rain_rate" },
            { IOTDATA_FIELD_RAIN_SIZE,         "rain_size" },
            { IOTDATA_FIELD_RADIATION_CPM,     "radiation_cpm" },
            { IOTDATA_FIELD_RADIATION_DOSE,    "radiation_dose" },
            { IOTDATA_FIELD_DEPTH,             "depth" },
            { IOTDATA_FIELD_POSITION,          "position" },
            /* pres2 (7 fields) */
            { IOTDATA_FIELD_DATETIME,          "datetime" },
            { IOTDATA_FIELD_FLAGS,             "flags" },
            { IOTDATA_FIELD_NONE,              NULL },
            { IOTDATA_FIELD_NONE,              NULL },
            { IOTDATA_FIELD_NONE,              NULL },
            { IOTDATA_FIELD_NONE,              NULL },
            { IOTDATA_FIELD_NONE,              NULL },
        },
    },
};

/* =========================================================================
 * Section 1: Field round-trips for types not in the default variant
 * =========================================================================*/

static void test_aq_pm_round_trip(void) {
    TEST("Air quality PM round-trip");
    begin(0, 1, 1);

    uint16_t pm[4] = { 100, 250, 75, 500 };
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x0F, pm), "encode");
    finish();
    decode_pkt();

    ASSERT_EQ(!!IOTDATA_FIELD_PRESENT(dec.fields, IOTDATA_FIELD_AIR_QUALITY_PM), 1, "present");
    ASSERT_EQ_U(dec.aq_pm_present, 0x0F, "mask");
    ASSERT_EQ_U(dec.aq_pm[0], 100, "pm1.0");
    ASSERT_EQ_U(dec.aq_pm[1], 250, "pm2.5");
    ASSERT_EQ_U(dec.aq_pm[2], 75, "pm4.0");
    ASSERT_EQ_U(dec.aq_pm[3], 500, "pm10");
    PASS();
}

static void test_aq_pm_partial(void) {
    TEST("Air quality PM partial (2 channels)");
    begin(0, 1, 2);

    uint16_t pm[4] = { 50, 0, 0, 200 };
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x09, pm), "encode"); /* PM1.0 + PM10 */
    finish();
    decode_pkt();

    ASSERT_EQ_U(dec.aq_pm_present, 0x09, "mask");
    ASSERT_EQ_U(dec.aq_pm[0], 50, "pm1.0");
    ASSERT_EQ_U(dec.aq_pm[3], 200, "pm10");
    PASS();
}

static void test_aq_gas_round_trip(void) {
    TEST("Air quality gas round-trip");
    begin(0, 1, 3);

    uint16_t gas[8] = { 200, 100, 5000, 500, 250, 100, 0, 0 };
    ASSERT_OK(iotdata_encode_air_quality_gas(&enc, 0x3F, gas), "encode"); /* first 6 slots */
    finish();
    decode_pkt();

    ASSERT_EQ(!!IOTDATA_FIELD_PRESENT(dec.fields, IOTDATA_FIELD_AIR_QUALITY_GAS), 1, "present");
    ASSERT_EQ_U(dec.aq_gas_present, 0x3F, "mask");
    ASSERT_EQ_U(dec.aq_gas[0], 200, "voc");
    ASSERT_EQ_U(dec.aq_gas[1], 100, "nox");
    ASSERT_EQ_U(dec.aq_gas[2], 5000, "co2");
    ASSERT_EQ_U(dec.aq_gas[3], 500, "co");
    ASSERT_EQ_U(dec.aq_gas[4], 250, "hcho");
    ASSERT_EQ_U(dec.aq_gas[5], 100, "o3");
    PASS();
}

static void test_depth_round_trip(void) {
    TEST("Depth round-trip");
    begin(0, 1, 4);

    ASSERT_OK(iotdata_encode_depth(&enc, 500), "encode");
    finish();
    decode_pkt();

    ASSERT_EQ(!!IOTDATA_FIELD_PRESENT(dec.fields, IOTDATA_FIELD_DEPTH), 1, "present");
    ASSERT_EQ_U(dec.depth, 500, "depth");
    PASS();
}

static void test_image_round_trip(void) {
    TEST("Image round-trip");
    begin(0, 1, 5);

    uint8_t img[] = { 0xFF, 0x00, 0xAA, 0x55 };
    ASSERT_OK(iotdata_encode_image(&enc, IOTDATA_IMAGE_FMT_BILEVEL, IOTDATA_IMAGE_SIZE_24x18, IOTDATA_IMAGE_COMP_RAW, IOTDATA_IMAGE_FLAG_INVERT, img, 4), "encode");
    finish();
    decode_pkt();

    ASSERT_EQ(!!IOTDATA_FIELD_PRESENT(dec.fields, IOTDATA_FIELD_IMAGE), 1, "present");
    ASSERT_EQ(dec.image_pixel_format, IOTDATA_IMAGE_FMT_BILEVEL, "fmt");
    ASSERT_EQ(dec.image_size_tier, IOTDATA_IMAGE_SIZE_24x18, "size");
    ASSERT_EQ(dec.image_compression, IOTDATA_IMAGE_COMP_RAW, "comp");
    ASSERT_EQ(dec.image_flags & IOTDATA_IMAGE_FLAG_INVERT, IOTDATA_IMAGE_FLAG_INVERT, "invert");
    ASSERT_EQ(dec.image_data_len, 4, "len");
    ASSERT_EQ(dec.image_data[0], 0xFF, "px0");
    ASSERT_EQ(dec.image_data[1], 0x00, "px1");
    ASSERT_EQ(dec.image_data[2], 0xAA, "px2");
    ASSERT_EQ(dec.image_data[3], 0x55, "px3");
    PASS();
}

/* =========================================================================
 * Section 2: Full variant tests
 * =========================================================================*/

static void test_complete_variant_all_fields(void) {
    TEST("Complete variant - all 16 fields");
    begin(0, 100, 500);

    ASSERT_OK(iotdata_encode_battery(&enc, 90, true), "bat");
    ASSERT_OK(iotdata_encode_link(&enc, -80, 0.0f), "link");
    ASSERT_OK(iotdata_encode_environment(&enc, 20.0f, 1013, 50), "env");
    ASSERT_OK(iotdata_encode_wind(&enc, 8.0f, 225, 12.0f), "wind");
    ASSERT_OK(iotdata_encode_rain(&enc, 5, 20), "rain");
    ASSERT_OK(iotdata_encode_solar(&enc, 300, 5), "solar");
    ASSERT_OK(iotdata_encode_clouds(&enc, 4), "cloud");
    ASSERT_OK(iotdata_encode_air_quality_index(&enc, 75), "aqi");
    uint16_t pm[4] = { 50, 120, 80, 200 };
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x0F, pm), "aq_pm");
    uint16_t gas[8] = { 200, 100, 5000, 500, 250, 100, 0, 0 };
    ASSERT_OK(iotdata_encode_air_quality_gas(&enc, 0x3F, gas), "aq_gas");
    ASSERT_OK(iotdata_encode_radiation(&enc, 100, 0.50f), "rad");
    ASSERT_OK(iotdata_encode_depth(&enc, 250), "depth");
    ASSERT_OK(iotdata_encode_position(&enc, 51.5, -0.1), "pos");
    ASSERT_OK(iotdata_encode_datetime(&enc, 86400), "dt");
    uint8_t img[] = { 0xDE, 0xAD };
    ASSERT_OK(iotdata_encode_image(&enc, 0, 0, 0, 0, img, 2), "img");
    ASSERT_OK(iotdata_encode_flags(&enc, 0x42), "flags");
    finish();
    decode_pkt();

    /* Spot-check a selection of fields (90% round-trips exactly with 5-bit quantisation) */
    ASSERT_EQ(dec.battery_level, 90, "bat");
    ASSERT_EQ_U(dec.aq_pm_present, 0x0F, "pm_mask");
    ASSERT_EQ_U(dec.aq_pm[1], 120, "pm25");
    ASSERT_EQ_U(dec.aq_gas_present, 0x3F, "gas_mask");
    ASSERT_EQ_U(dec.aq_gas[2], 5000, "co2");
    ASSERT_EQ_U(dec.depth, 250, "depth");
    ASSERT_EQ(dec.image_data_len, 2, "img_len");
    ASSERT_EQ(dec.image_data[0], 0xDE, "img0");
    ASSERT_EQ(dec.flags, 0x42, "flags");
    PASS();
}

static void test_standalone_variant_all_fields(void) {
    TEST("Standalone variant - all 15 fields");
    begin(1, 200, 600);

    ASSERT_OK(iotdata_encode_battery(&enc, 60, false), "bat");
    ASSERT_OK(iotdata_encode_temperature(&enc, 22.5f), "temp");
    ASSERT_OK(iotdata_encode_pressure(&enc, 1013), "pres");
    ASSERT_OK(iotdata_encode_humidity(&enc, 55), "hum");
    ASSERT_OK(iotdata_encode_wind_speed(&enc, 5.0f), "wspd");
    ASSERT_OK(iotdata_encode_wind_direction(&enc, 180), "wdir");
    ASSERT_OK(iotdata_encode_wind_gust(&enc, 8.0f), "wgust");
    ASSERT_OK(iotdata_encode_rain_rate(&enc, 10), "rrate");
    ASSERT_OK(iotdata_encode_rain_size(&enc, 20), "rsize");
    ASSERT_OK(iotdata_encode_radiation_cpm(&enc, 1500), "cpm");
    ASSERT_OK(iotdata_encode_radiation_dose(&enc, 0.75f), "dose");
    ASSERT_OK(iotdata_encode_depth(&enc, 100), "depth");
    ASSERT_OK(iotdata_encode_position(&enc, -33.8688, 151.2093), "pos");
    ASSERT_OK(iotdata_encode_datetime(&enc, 43200), "dt");
    ASSERT_OK(iotdata_encode_flags(&enc, 0xFF), "flags");
    finish();
    decode_pkt();

    /* Spot-check standalone fields */
    ASSERT_NEAR(dec.temperature, 22.5, 0.25, "temp");
    ASSERT_EQ(dec.pressure, 1013, "pres");
    ASSERT_EQ(dec.humidity, 55, "hum");
    ASSERT_NEAR(dec.wind_speed, 5.0, 0.5, "wspd");
    ASSERT_NEAR(dec.wind_direction, 180, 2.0, "wdir");
    ASSERT_NEAR(dec.wind_gust, 8.0, 0.5, "wgust");
    ASSERT_EQ(dec.rain_rate, 10, "rrate");
    ASSERT_EQ_U(dec.radiation_cpm, 1500, "cpm");
    ASSERT_NEAR(dec.radiation_dose, 0.75, 0.01, "dose");
    ASSERT_EQ_U(dec.depth, 100, "depth");
    ASSERT_NEAR(dec.position_lat, -33.8688, 0.001, "lat");
    ASSERT_NEAR(dec.position_lon, 151.2093, 0.001, "lon");
    ASSERT_EQ(dec.flags, 0xFF, "flags");
    PASS();
}

/* =========================================================================
 * Section 3: Boundary values
 * =========================================================================*/

static void test_aq_pm_boundaries(void) {
    TEST("AQ PM boundary values (min/max)");
    begin(0, 1, 10);

    /* Min: all zeros */
    uint16_t pm_min[4] = { 0, 0, 0, 0 };
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x0F, pm_min), "min");
    finish();
    decode_pkt();
    ASSERT_EQ_U(dec.aq_pm[0], 0, "min0");
    ASSERT_EQ_U(dec.aq_pm[3], 0, "min3");

    /* Max: 1275 on all channels */
    begin(0, 1, 11);
    uint16_t pm_max[4] = { 1275, 1275, 1275, 1275 };
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x0F, pm_max), "max");
    finish();
    decode_pkt();
    ASSERT_EQ_U(dec.aq_pm[0], 1275, "max0");
    ASSERT_EQ_U(dec.aq_pm[3], 1275, "max3");
    PASS();
}

static void test_aq_gas_boundaries(void) {
    TEST("AQ gas boundary values (max per slot)");
    begin(0, 1, 12);

    uint16_t gas[8] = { 510, 510, 51150, 1023, 5115, 1023, 1023, 1023 };
    ASSERT_OK(iotdata_encode_air_quality_gas(&enc, 0xFF, gas), "max all");
    finish();
    decode_pkt();

    ASSERT_EQ_U(dec.aq_gas[0], 510, "voc");
    ASSERT_EQ_U(dec.aq_gas[1], 510, "nox");
    ASSERT_EQ_U(dec.aq_gas[2], 51150, "co2");
    ASSERT_EQ_U(dec.aq_gas[3], 1023, "co");
    ASSERT_EQ_U(dec.aq_gas[4], 5115, "hcho");
    ASSERT_EQ_U(dec.aq_gas[5], 1023, "o3");
    ASSERT_EQ_U(dec.aq_gas[6], 1023, "rsvd6");
    ASSERT_EQ_U(dec.aq_gas[7], 1023, "rsvd7");
    PASS();
}

static void test_depth_boundaries(void) {
    TEST("Depth boundary values");
    begin(0, 1, 13);
    ASSERT_OK(iotdata_encode_depth(&enc, 0), "min");
    finish();
    decode_pkt();
    ASSERT_EQ_U(dec.depth, 0, "min");

    begin(0, 1, 14);
    ASSERT_OK(iotdata_encode_depth(&enc, 1023), "max");
    finish();
    decode_pkt();
    ASSERT_EQ_U(dec.depth, 1023, "max");
    PASS();
}

static void test_image_flags_combinations(void) {
    TEST("Image flag combinations");
    uint8_t px[] = { 0x42 };

    /* Fragment + invert */
    begin(0, 1, 15);
    ASSERT_OK(iotdata_encode_image(&enc, 0, 0, 0, IOTDATA_IMAGE_FLAG_FRAGMENT | IOTDATA_IMAGE_FLAG_INVERT, px, 1), "encode");
    finish();
    decode_pkt();
    ASSERT_EQ(dec.image_flags & IOTDATA_IMAGE_FLAG_FRAGMENT, IOTDATA_IMAGE_FLAG_FRAGMENT, "fragment");
    ASSERT_EQ(dec.image_flags & IOTDATA_IMAGE_FLAG_INVERT, IOTDATA_IMAGE_FLAG_INVERT, "invert");

    /* All formats and sizes */
    begin(0, 1, 16);
    ASSERT_OK(iotdata_encode_image(&enc, IOTDATA_IMAGE_FMT_GREY16, IOTDATA_IMAGE_SIZE_64x48, IOTDATA_IMAGE_COMP_HEATSHRINK, 0, px, 1), "grey16+64x48+hs");
    finish();
    decode_pkt();
    ASSERT_EQ(dec.image_pixel_format, IOTDATA_IMAGE_FMT_GREY16, "fmt");
    ASSERT_EQ(dec.image_size_tier, IOTDATA_IMAGE_SIZE_64x48, "sz");
    ASSERT_EQ(dec.image_compression, IOTDATA_IMAGE_COMP_HEATSHRINK, "comp");
    PASS();
}

/* =========================================================================
 * Section 4: Error conditions
 * =========================================================================*/

static void test_aq_pm_errors(void) {
    TEST("AQ PM error conditions");
    begin(0, 1, 20);

    uint16_t pm_high[4] = { 1280, 0, 0, 0 };
    ASSERT_ERR(iotdata_encode_air_quality_pm(&enc, 0x01, pm_high), IOTDATA_ERR_AIR_QUALITY_PM_VALUE_HIGH, "pm too high");
    PASS();
}

static void test_aq_gas_errors(void) {
    TEST("AQ gas error conditions");
    begin(0, 1, 21);

    uint16_t gas_high[8] = { 512, 0, 0, 0, 0, 0, 0, 0 }; /* VOC max 510 */
    ASSERT_ERR(iotdata_encode_air_quality_gas(&enc, 0x01, gas_high), IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH, "gas too high");
    PASS();
}

static void test_image_errors(void) {
    TEST("Image error conditions");
    begin(0, 1, 22);
    uint8_t px[] = { 0x42 };

    ASSERT_ERR(iotdata_encode_image(&enc, 3, 0, 0, 0, px, 1), IOTDATA_ERR_IMAGE_FORMAT_HIGH, "fmt high");
    ASSERT_ERR(iotdata_encode_image(&enc, 0, 4, 0, 0, px, 1), IOTDATA_ERR_IMAGE_SIZE_HIGH, "sz high");
    ASSERT_ERR(iotdata_encode_image(&enc, 0, 0, 3, 0, px, 1), IOTDATA_ERR_IMAGE_COMPRESSION_HIGH, "comp high");
    ASSERT_ERR(iotdata_encode_image(&enc, 0, 0, 0, 0, NULL, 1), IOTDATA_ERR_IMAGE_DATA_NULL, "data null");
    ASSERT_ERR(iotdata_encode_image(&enc, 0, 0, 0, 0, px, 255), IOTDATA_ERR_IMAGE_DATA_HIGH, "data high");
    PASS();
}

static void test_tlv_errors(void) {
    TEST("TLV error conditions");
    begin(0, 1, 23);

    /* Type too high */
    uint8_t raw[] = { 0x01 };
    ASSERT_ERR(iotdata_encode_tlv(&enc, 64, raw, 1), IOTDATA_ERR_TLV_TYPE_HIGH, "type high");

    /* Data null */
    ASSERT_ERR(iotdata_encode_tlv(&enc, 1, NULL, 1), IOTDATA_ERR_TLV_DATA_NULL, "data null");

    /* String null */
    ASSERT_ERR(iotdata_encode_tlv_string(&enc, 1, NULL), IOTDATA_ERR_TLV_STR_NULL, "str null");

    /* Invalid 6-bit char */
    ASSERT_ERR(iotdata_encode_tlv_string(&enc, 1, "hello[world"), IOTDATA_ERR_TLV_STR_CHAR_INVALID, "str char invalid");

    /* TLV full (overflow IOTDATA_TLV_MAX) */
    for (int i = 0; i < IOTDATA_TLV_MAX; i++)
        ASSERT_OK(iotdata_encode_tlv(&enc, 0x20, raw, 1), "fill");
    ASSERT_ERR(iotdata_encode_tlv(&enc, 0x20, raw, 1), IOTDATA_ERR_TLV_FULL, "full");

    PASS();
}

static void test_encode_buffer_overflow(void) {
    TEST("Encode buffer overflow");

    /* 5 bytes = header(4) + pres0(1), no room for field data */
    uint8_t small_buf[5];
    iotdata_encoder_t small_enc;
    ASSERT_OK(iotdata_encode_begin(&small_enc, small_buf, 5, 0, 1, 1), "begin ok");
    ASSERT_OK(iotdata_encode_battery(&small_enc, 50, false), "bat ok");
    size_t out_len;
    ASSERT_ERR(iotdata_encode_end(&small_enc, &out_len), IOTDATA_ERR_BUF_TOO_SMALL, "buf overflow");

    /* Buffer too small for even the header */
    ASSERT_ERR(iotdata_encode_begin(&small_enc, small_buf, 4, 0, 1, 1), IOTDATA_ERR_BUF_TOO_SMALL, "buf tiny");
    PASS();
}

/* =========================================================================
 * Section 5: Peek function
 * =========================================================================*/

static void test_peek_basic(void) {
    TEST("Peek basic");
    begin(0, 42, 1234);
    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "bat");
    finish();

    uint8_t v;
    uint16_t s, q;
    ASSERT_OK(iotdata_peek(pkt, pkt_len, &v, &s, &q), "peek");
    ASSERT_EQ(v, 0, "variant");
    ASSERT_EQ_U(s, 42, "station");
    ASSERT_EQ_U(q, 1234, "sequence");
    PASS();
}

static void test_peek_null_params(void) {
    TEST("Peek with NULL output params");
    begin(0, 10, 99);
    finish();

    /* All NULL except buf/len */
    ASSERT_OK(iotdata_peek(pkt, pkt_len, NULL, NULL, NULL), "all null");

    /* Partial NULL */
    uint8_t v;
    ASSERT_OK(iotdata_peek(pkt, pkt_len, &v, NULL, NULL), "var only");
    ASSERT_EQ(v, 0, "variant");

    uint16_t s;
    ASSERT_OK(iotdata_peek(pkt, pkt_len, NULL, &s, NULL), "station only");
    ASSERT_EQ_U(s, 10, "station");
    PASS();
}

static void test_peek_short_buffer(void) {
    TEST("Peek short buffer");

    uint8_t short_buf[] = { 0x00, 0x00, 0x00 };
    uint8_t v;
    ASSERT_ERR(iotdata_peek(short_buf, 3, &v, NULL, NULL), IOTDATA_ERR_DECODE_SHORT, "short");
    PASS();
}

static void test_peek_reserved_variant(void) {
    TEST("Peek reserved variant (15)");

    /* Variant 15 is reserved, and is what the mesh uses (IOTDATA_VARIANT_MESH). peek() must
       therefore SUCCEED and report it -- identifying a mesh frame is exactly what a gateway or
       relay peeks for. It is decode() that refuses to interpret it as a telemetry packet.
       Manually construct: variant=15 (0xF), station=0, sequence=0. */
    uint8_t bad[] = { 0xF0, 0x00, 0x00, 0x00, 0x00 };
    uint8_t v = 0;
    uint16_t station = 0xFFFF, sequence = 0xFFFF;
    ASSERT_OK(iotdata_peek(bad, 5, &v, &station, &sequence), "peek identifies, does not reject");
    ASSERT_EQ(v, IOTDATA_VARIANT_RESERVED, "variant reported");
    ASSERT_EQ(v, IOTDATA_VARIANT_MESH, "reserved variant is the mesh variant");
    ASSERT_EQ(station, 0, "station");

    iotdata_decoder_t d;
    ASSERT_ERR(iotdata_decode(bad, 5, &d), IOTDATA_ERR_DECODE_VARIANT, "decode refuses it");
    PASS();
}

/* =========================================================================
 * Section 6: TLV typed helpers
 * =========================================================================*/

static void test_tlv_multiple(void) {
    TEST("Multiple TLV entries in one packet");
    begin(0, 1, 37);

    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "bat");

    uint8_t kvbuf[64];
    iotdata_kvr_t kv;
    iotdata_kvr_init(&kv, kvbuf, sizeof(kvbuf));
    iotdata_kvr_add_str(&kv, 0x00, "100");
    ASSERT_OK(iotdata_encode_tlv(&enc, 0x01, kvbuf, (uint8_t)kv.len), "kvr tlv");

    uint8_t raw2[] = { 0x01, 0x02, 0x03 };
    ASSERT_OK(iotdata_encode_tlv(&enc, 0x22, raw2, 3), "raw tlv");

    ASSERT_OK(iotdata_encode_tlv_string(&enc, 0x23, "test data"), "str tlv");

    finish();
    decode_pkt();

    ASSERT_EQ(dec.tlv_count, 3, "count");
    ASSERT_EQ(dec.tlv[0].type, 0x01, "t0");
    ASSERT_EQ(dec.tlv[1].type, 0x22, "t1");
    ASSERT_EQ(dec.tlv[2].type, 0x23, "t2");
    PASS();
}

/* =========================================================================
 * Section 6b: TLV key-value payload codecs (kvr / kvs)
 * =========================================================================*/

static void test_kv_raw_round_trip(void) {
    TEST("kvr build + walk round-trip");

    uint8_t buf[128];
    iotdata_kvr_t kv;
    iotdata_kvr_init(&kv, buf, sizeof(buf));
    ASSERT_TRUE(iotdata_kvr_add_u8(&kv, 0x01, 0xAB), "add u8");
    ASSERT_TRUE(iotdata_kvr_add_u16(&kv, 0x02, 0x1234), "add u16");
    ASSERT_TRUE(iotdata_kvr_add_u32(&kv, 0x03, 0xDEADBEEFu), "add u32");
    ASSERT_TRUE(iotdata_kvr_add_i8(&kv, 0x04, -5), "add i8");
    ASSERT_TRUE(iotdata_kvr_add_str(&kv, 0x05, "1.4.2"), "add str");
    ASSERT_TRUE(iotdata_kvr_add_flag(&kv, 0x06), "add flag");
    ASSERT_TRUE(!kv.overflow, "no overflow");
    /* 1+2 + 2+2 + 4+2 + 1+2 + 5+2 + 0+2 */
    ASSERT_EQ((int)kv.len, 25, "len");

    size_t cur = 0;
    uint8_t k, n;
    const uint8_t *v;
    char sbuf[16];

    ASSERT_TRUE(iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "next 1");
    ASSERT_EQ(k, 0x01, "k1");
    ASSERT_EQ(iotdata_kvr_u8(v, n, 0), 0xAB, "v1");
    ASSERT_TRUE(iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "next 2");
    ASSERT_EQ(iotdata_kvr_u16(v, n, 0), 0x1234, "v2");
    ASSERT_TRUE(iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "next 3");
    ASSERT_TRUE(iotdata_kvr_u32(v, n, 0) == 0xDEADBEEFu, "v3");
    ASSERT_TRUE(iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "next 4");
    ASSERT_EQ(iotdata_kvr_i8(v, n, 0), -5, "v4 signed");
    ASSERT_TRUE(iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "next 5");
    ASSERT_EQ((int)iotdata_kvr_str(v, n, sbuf, sizeof(sbuf)), 5, "v5 len");
    ASSERT_EQ(strcmp(sbuf, "1.4.2"), 0, "v5 str (punctuation survives raw)");
    ASSERT_TRUE(iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "next 6");
    ASSERT_EQ(k, 0x06, "k6");
    ASSERT_EQ(n, 0, "v6 flag has no value");
    ASSERT_TRUE(!iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "end");
    PASS();
}

static void test_kv_raw_robustness(void) {
    TEST("kvr width mismatch, unknown-key skip, overflow, truncation");

    uint8_t buf[64];
    iotdata_kvr_t kv;
    iotdata_kvr_init(&kv, buf, sizeof(buf));
    iotdata_kvr_add_u16(&kv, 0x01, 0x1234);
    iotdata_kvr_add_u32(&kv, 0x80, 0x11223344u); /* a proprietary key we will skip past */
    iotdata_kvr_add_u8(&kv, 0x02, 7);

    /* reading a key at the wrong width yields the default, not garbage */
    size_t cur = 0;
    uint8_t k, n;
    const uint8_t *v;
    ASSERT_TRUE(iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "n1");
    ASSERT_EQ(iotdata_kvr_u32(v, n, 99), 99, "u32 read of a u16 -> default");
    ASSERT_EQ(iotdata_kvr_u16(v, n, 0), 0x1234, "correct width still works");

    /* an unknown/proprietary key is skippable: the walk reaches the key after it */
    ASSERT_TRUE(iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "n2");
    ASSERT_TRUE(!iotdata_tlv_key_is_system(k), "0x80 is proprietary");
    ASSERT_TRUE(iotdata_kvr_next(buf, kv.len, &cur, &k, &v, &n), "n3 (skipped past)");
    ASSERT_EQ(k, 0x02, "k3");
    ASSERT_TRUE(iotdata_tlv_key_is_system(k), "0x02 is system");

    /* a truncated payload stops rather than running off the end */
    cur = 0;
    ASSERT_TRUE(!iotdata_kvr_next(buf, 3, &cur, &k, &v, &n), "truncated pair rejected");

    /* overflow is sticky and does not write past the buffer */
    uint8_t small[6];
    iotdata_kvr_t ov;
    iotdata_kvr_init(&ov, small, sizeof(small));
    ASSERT_TRUE(iotdata_kvr_add_u32(&ov, 0x01, 1), "fits");
    ASSERT_TRUE(!iotdata_kvr_add_u32(&ov, 0x02, 2), "does not fit");
    ASSERT_TRUE(ov.overflow, "overflow sticky");
    ASSERT_EQ((int)ov.len, 6, "len unchanged after failed add");
    PASS();
}

static void test_kv_raw_in_tlv(void) {
    TEST("kvr payload carried in a TLV, decoded and walked");
    begin(0, 1, 40);

    uint8_t kvbuf[64];
    iotdata_kvr_t kv;
    iotdata_kvr_init(&kv, kvbuf, sizeof(kvbuf));
    iotdata_kvr_add_u32(&kv, 0x00, 123456);
    iotdata_kvr_add_u16(&kv, 0x02, 42);
    ASSERT_OK(iotdata_encode_tlv(&enc, 0x02, kvbuf, (uint8_t)kv.len), "encode tlv");
    finish();
    decode_pkt();

    ASSERT_EQ(dec.tlv_count, 1, "count");
    ASSERT_EQ(dec.tlv[0].format, IOTDATA_TLV_FMT_RAW, "kvr rides in a RAW tlv");
    ASSERT_TRUE(iotdata_tlv_type_is_system(dec.tlv[0].type), "0x02 is a system type");

    size_t cur = 0;
    uint8_t k, n;
    const uint8_t *v;
    ASSERT_TRUE(iotdata_kvr_next(dec.tlv[0].raw, dec.tlv[0].length, &cur, &k, &v, &n), "walk 1");
    ASSERT_EQ(k, 0x00, "k1");
    ASSERT_TRUE(iotdata_kvr_u32(v, n, 0) == 123456u, "v1 survived the packet");
    ASSERT_TRUE(iotdata_kvr_next(dec.tlv[0].raw, dec.tlv[0].length, &cur, &k, &v, &n), "walk 2");
    ASSERT_EQ(iotdata_kvr_u16(v, n, 0), 42, "v2");
    ASSERT_TRUE(!iotdata_kvr_next(dec.tlv[0].raw, dec.tlv[0].length, &cur, &k, &v, &n), "end");
    PASS();
}

static void test_kv_string_round_trip(void) {
    TEST("kvs build + walk round-trip");

    char buf[128];
    iotdata_kvs_t kv;
    iotdata_kvs_init(&kv, buf, sizeof(buf));
    ASSERT_TRUE(iotdata_kvs_add(&kv, "firmware", "142"), "add 1");
    ASSERT_TRUE(iotdata_kvs_add(&kv, "hardware", "3"), "add 2");
    ASSERT_TRUE(!kv.overflow, "no overflow");
    ASSERT_EQ(strcmp(buf, "firmware 142 hardware 3"), 0, "wire form");

    size_t cur = 0, nl, vl;
    const char *nm, *val;
    ASSERT_TRUE(iotdata_kvs_next(buf, &cur, &nm, &nl, &val, &vl), "next 1");
    ASSERT_EQ((int)nl, 8, "name len");
    ASSERT_EQ(strncmp(nm, "firmware", nl), 0, "name");
    ASSERT_EQ(strncmp(val, "142", vl), 0, "value");
    ASSERT_TRUE(iotdata_kvs_next(buf, &cur, &nm, &nl, &val, &vl), "next 2");
    ASSERT_EQ(strncmp(nm, "hardware", nl), 0, "name 2");
    ASSERT_TRUE(!iotdata_kvs_next(buf, &cur, &nm, &nl, &val, &vl), "end");

    /* a dangling key with no value is malformed and must stop the walk, not read past */
    char odd[] = "a 1 b";
    cur = 0;
    ASSERT_TRUE(iotdata_kvs_next(odd, &cur, &nm, &nl, &val, &vl), "odd: first pair ok");
    ASSERT_TRUE(!iotdata_kvs_next(odd, &cur, &nm, &nl, &val, &vl), "odd: dangling key stops");

    /* kvs values must be 6-bit representable; punctuation is NOT (that is what kvr is for) */
    ASSERT_TRUE(!iotdata_issixbit('.'), "'.' not encodable in a string TLV");
    ASSERT_TRUE(!iotdata_issixbit('-'), "'-' not encodable in a string TLV");
    ASSERT_TRUE(iotdata_issixbit('A') && iotdata_issixbit('9') && iotdata_issixbit(' '), "alnum + space are");
    PASS();
}

/* =========================================================================
 * Section 6c: node system TLVs (iotdata_node.h)
 *
 * Deliberately minimal for now -- enough to pin the definitions down and give later work a
 * place to grow into.
 * =========================================================================*/

static void test_node_definitions(void) {
    TEST("Node system TLV definitions");

    const uint8_t types[] = { IOTDATA_NODE_TLV_VERSION, IOTDATA_NODE_TLV_VARIANT, IOTDATA_NODE_TLV_CONTROL,
                              IOTDATA_NODE_TLV_STATUS,  IOTDATA_NODE_TLV_CONFIG,  IOTDATA_NODE_TLV_DIAGNOSTICS,
                              IOTDATA_NODE_TLV_CONTENT };
    for (size_t i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
        ASSERT_TRUE(iotdata_tlv_type_is_system(types[i]), "type is system");
        ASSERT_TRUE(iotdata_node_tlv_name(types[i]) != NULL, "type is named");
        size_t n = 0;
        const iotdata_node_keydef_t *tbl = iotdata_node_tlv_keys(types[i], &n);
        ASSERT_TRUE(tbl != NULL && n > 0, "type has keys");
        for (size_t k = 0; k < n; k++)
            ASSERT_TRUE(iotdata_tlv_key_is_system(tbl[k].key), "system keys have bit7 clear");
        /* asking for a type is the same identifier as the type itself */
        ASSERT_EQ(iotdata_node_tlv_control_key(types[i]), types[i], "request key == type");
        ASSERT_EQ(iotdata_node_tlv_control_type(types[i]), types[i], "and back again");
    }
    /* proprietary space is neither system nor named -- the split is on the type field's top bit,
       which is 0x20 because the field is 6 bits wide (the key field is 8, so its bit is 0x80) */
    ASSERT_TRUE(iotdata_tlv_type_is_system(0x1F), "0x1F is the last system type");
    ASSERT_TRUE(!iotdata_tlv_type_is_system(0x20), "0x20 is proprietary");
    ASSERT_TRUE(iotdata_node_tlv_name(0x20) == NULL, "proprietary type unnamed");
    ASSERT_TRUE(!iotdata_tlv_key_is_system(0x80), "0x80 is a proprietary key");
    ASSERT_TRUE(iotdata_node_tlv_key_name(IOTDATA_NODE_TLV_STATUS, 0x80) == NULL, "proprietary key unnamed");
    /* a non-request control command does not map to a type. the sentinel must not be 0, because
       0x00 is RECEIVE -- a real type */
    ASSERT_EQ(iotdata_node_tlv_control_type(IOTDATA_NODE_CONTROL_REBOOT), IOTDATA_NODE_TLV_NONE, "reboot is not a request");
    ASSERT_TRUE(IOTDATA_NODE_TLV_NONE != IOTDATA_NODE_TLV_RECEIVE, "sentinel is not a valid type");

    /* RECEIVE is a system type with keys, but it is volunteered, never requested */
    ASSERT_TRUE(iotdata_tlv_type_is_system(IOTDATA_NODE_TLV_RECEIVE), "receive is a system type");
    ASSERT_TRUE(iotdata_node_tlv_name(IOTDATA_NODE_TLV_RECEIVE) != NULL, "receive is named");
    ASSERT_EQ(iotdata_node_tlv_control_key(IOTDATA_NODE_TLV_RECEIVE), IOTDATA_NODE_TLV_NONE, "receive cannot be requested");
    PASS();
}

static void test_node_status_tlv(void) {
    TEST("Node STATUS TLV encode/decode via kvr");
    begin(0, 1, 48);

    uint8_t kvbuf[64];
    iotdata_kvr_t kv;
    iotdata_kvr_init(&kv, kvbuf, sizeof(kvbuf));
    iotdata_kvr_add_u32(&kv, IOTDATA_NODE_STATUS_UPTIME, 3600);
    iotdata_kvr_add_u16(&kv, IOTDATA_NODE_STATUS_RESTARTS, 7);
    iotdata_kvr_add_u8(&kv, IOTDATA_NODE_STATUS_REASON, IOTDATA_NODE_REASON_WATCHDOG);
    iotdata_kvr_add_i8(&kv, IOTDATA_NODE_STATUS_TEMPERATURE, -5);
    ASSERT_TRUE(!kv.overflow, "built");
    ASSERT_OK(iotdata_encode_tlv(&enc, IOTDATA_NODE_TLV_STATUS, kvbuf, (uint8_t)kv.len), "encode");
    finish();
    decode_pkt();

    ASSERT_EQ(dec.tlv_count, 1, "one tlv");
    ASSERT_EQ(dec.tlv[0].type, IOTDATA_NODE_TLV_STATUS, "type");

    size_t cur = 0;
    uint8_t k, n;
    const uint8_t *v;
    int seen = 0;
    while (iotdata_kvr_next(dec.tlv[0].raw, dec.tlv[0].length, &cur, &k, &v, &n)) {
        /* every key is known, and arrived at the width its definition claims */
        ASSERT_TRUE(iotdata_node_tlv_key_name(IOTDATA_NODE_TLV_STATUS, k) != NULL, "key named");
        ASSERT_EQ(n, iotdata_node_tlv_key_width(IOTDATA_NODE_TLV_STATUS, k), "width matches definition");
        switch (k) {
        case IOTDATA_NODE_STATUS_UPTIME:
            ASSERT_TRUE(iotdata_kvr_u32(v, n, 0) == 3600u, "uptime");
            break;
        case IOTDATA_NODE_STATUS_RESTARTS:
            ASSERT_EQ(iotdata_kvr_u16(v, n, 0), 7, "restarts");
            break;
        case IOTDATA_NODE_STATUS_REASON:
            ASSERT_EQ(strcmp(iotdata_node_tlv_status_reason_str(iotdata_kvr_u8(v, n, 0)), "watchdog"), 0, "reason");
            break;
        case IOTDATA_NODE_STATUS_TEMPERATURE:
            ASSERT_EQ(iotdata_kvr_i8(v, n, 0), -5, "temperature is signed");
            break;
        default:
            break;
        }
        seen++;
    }
    ASSERT_EQ(seen, 4, "all four keys walked");
    PASS();
}

/* An empty RECEIVE is the whole message "listening, for anything" -- and because its type is 0x00
   and its length 0, every bit of it is zero. Worth pinning that this survives a round trip, since
   it is the one system TLV indistinguishable from a run of zero bits. */
static void test_node_receive_tlv(void) {
    TEST("Node RECEIVE TLV, empty and populated");

    begin(0, 1, 49);
    ASSERT_OK(iotdata_encode_tlv(&enc, IOTDATA_NODE_TLV_RECEIVE, NULL, 0), "encode empty");
    finish();
    decode_pkt();
    ASSERT_EQ(dec.tlv_count, 1, "one tlv");
    ASSERT_EQ(dec.tlv[0].type, IOTDATA_NODE_TLV_RECEIVE, "type 0x00 survives");
    ASSERT_EQ(dec.tlv[0].length, 0, "zero length");
    ASSERT_EQ(dec.tlv[0].format, IOTDATA_TLV_FMT_RAW, "raw");
    size_t cur = 0;
    uint8_t k, n;
    const uint8_t *v;
    ASSERT_TRUE(!iotdata_kvr_next(dec.tlv[0].raw, dec.tlv[0].length, &cur, &k, &v, &n), "no keys to walk");

    /* the same type carrying both its optional keys */
    begin(0, 1, 50);
    uint8_t kvbuf[32];
    iotdata_kvr_t kv;
    iotdata_kvr_init(&kv, kvbuf, sizeof(kvbuf));
    iotdata_kvr_add_u16(&kv, IOTDATA_NODE_RECEIVE_DURATION, 500);
    iotdata_kvr_add_u32(&kv, IOTDATA_NODE_RECEIVE_TYPES,
                        (1u << IOTDATA_NODE_TLV_CONTROL) | (1u << IOTDATA_NODE_TLV_CONFIG));
    ASSERT_TRUE(!kv.overflow, "built");
    ASSERT_OK(iotdata_encode_tlv(&enc, IOTDATA_NODE_TLV_RECEIVE, kvbuf, (uint8_t)kv.len), "encode");
    finish();
    decode_pkt();

    ASSERT_EQ(dec.tlv[0].type, IOTDATA_NODE_TLV_RECEIVE, "type");
    cur = 0;
    int seen = 0;
    uint32_t types = 0;
    while (iotdata_kvr_next(dec.tlv[0].raw, dec.tlv[0].length, &cur, &k, &v, &n)) {
        ASSERT_TRUE(iotdata_node_tlv_key_name(IOTDATA_NODE_TLV_RECEIVE, k) != NULL, "key named");
        ASSERT_EQ(n, iotdata_node_tlv_key_width(IOTDATA_NODE_TLV_RECEIVE, k), "width matches definition");
        if (k == IOTDATA_NODE_RECEIVE_DURATION)
            ASSERT_EQ(iotdata_kvr_u16(v, n, 0), 500, "duration");
        if (k == IOTDATA_NODE_RECEIVE_TYPES)
            types = iotdata_kvr_u32(v, n, 0);
        seen++;
    }
    ASSERT_EQ(seen, 2, "both keys walked");
    ASSERT_TRUE((types & (1u << IOTDATA_NODE_TLV_CONFIG)) != 0, "config accepted");
    ASSERT_TRUE((types & (1u << IOTDATA_NODE_TLV_CONTENT)) == 0, "content declined");
    PASS();
}

/* VARIANT carries a whole variant suite: key = variant number, value = name plus one 12-bit field
   id per presence slot. Round-trip the suite this build was compiled against. */
static void test_node_variant_tlv(void) {
    TEST("Node VARIANT TLV, suite encode/decode");

    const iotdata_variant_def_t *const def = iotdata_get_variant(0);
    ASSERT_TRUE(def != NULL, "variant 0 defined");

    const size_t slots = iotdata_node_variant_slots(def->num_pres_bytes);
    ASSERT_TRUE(slots > 0 && slots <= IOTDATA_MAX_DATA_FIELDS, "slot count derived from presence bytes");

    uint8_t val[128];
    const size_t vlen = iotdata_node_variant_encode(def, val, sizeof(val));
    ASSERT_TRUE(vlen > 0, "encoded");
    ASSERT_EQ(vlen, 1u + strlen(def->name) + iotdata_node_variant_field_bytes(slots), "length is name + packed fields");

    const char *name = NULL;
    const uint8_t *fields = NULL;
    size_t namelen = 0;
    const size_t count = iotdata_node_variant_decode(val, vlen, &name, &namelen, &fields);
    ASSERT_EQ(namelen, strlen(def->name), "name length");
    ASSERT_EQ(memcmp(name, def->name, namelen), 0, "name bytes");
    ASSERT_TRUE(count >= slots, "every slot survives (the last byte may hold a spare nibble)");

    /* every slot comes back as the field it went in as, in the same position */
    for (size_t i = 0; i < slots; i++)
        ASSERT_EQ(iotdata_node_variant_field_get(fields, i), iotdata_node_variant_field_id(def->fields[i].type),
                  "field id at its slot");

    /* the packing straddles bytes, so prove neighbours do not bleed into each other */
    uint8_t packed[8];
    memset(packed, 0, sizeof(packed));
    const uint16_t probe[] = { 0x000u, 0xFFFu, 0xABCu, 0x123u, 0xFFFu };
    for (size_t i = 0; i < 5; i++)
        iotdata_node_variant_field_set(packed, i, probe[i]);
    for (size_t i = 0; i < 5; i++)
        ASSERT_EQ(iotdata_node_variant_field_get(packed, i), probe[i], "adjacent 12-bit ids stay distinct");

    /* a value too small to hold the name it claims is rejected rather than read past */
    const uint8_t bad[] = { 0x40, 'x' };
    ASSERT_EQ(iotdata_node_variant_decode(bad, sizeof(bad), NULL, NULL, NULL), 0, "overlong name rejected");
    ASSERT_EQ(iotdata_node_variant_encode(def, val, 4), 0, "no room, no encode");

    /* the manifest names exactly the variants that exist, so a receiver collecting chunks knows
       when it has them all -- and never claims the mesh variant */
    const uint16_t manifest = iotdata_node_variant_manifest();
    ASSERT_TRUE(iotdata_node_variant_manifest_has(manifest, 0), "variant 0 is in the manifest");
    ASSERT_TRUE(!iotdata_node_variant_manifest_has(manifest, IOTDATA_VARIANT_RESERVED), "mesh variant never named");
    ASSERT_EQ(manifest & (1u << IOTDATA_VARIANT_RESERVED), 0, "bit 15 clear");
    uint8_t defined = 0;
    for (uint8_t v = 0; v <= IOTDATA_VARIANT_MAX; v++) {
        ASSERT_EQ(iotdata_node_variant_manifest_has(manifest, v), iotdata_get_variant(v) != NULL, "bit tracks definition");
        if (iotdata_get_variant(v) != NULL)
            defined++;
    }
    ASSERT_EQ(iotdata_node_variant_manifest_count(manifest), defined, "count is the population count");
    PASS();
}

/* The three header fields each reserve their all-ones value, so the helpers that produce a station
   id or advance a sequence must never land on one. */
static void test_node_reserved_header_values(void) {
    TEST("Reserved header values");

    ASSERT_EQ(IOTDATA_STATION_BROADCAST, IOTDATA_STATION_MAX, "broadcast is all-ones station");
    ASSERT_EQ(IOTDATA_SEQUENCE_DOWN, IOTDATA_SEQUENCE_MAX, "down is all-ones sequence");
    ASSERT_EQ(IOTDATA_VARIANT_MESH, IOTDATA_VARIANT_RESERVED, "mesh is all-ones variant");

    /* no device id maps onto 0 or the broadcast id */
    for (uint32_t id = 0; id < 9000; id++) {
        const uint16_t s = iotdata_station_from_id(id);
        ASSERT_TRUE(s != 0 && s != IOTDATA_STATION_BROADCAST, "station id is assignable");
    }
    ASSERT_EQ(iotdata_station_from_id(IOTDATA_STATION_ASSIGNABLE_MAX - 1), IOTDATA_STATION_ASSIGNABLE_MAX, "top assignable id reachable");

    /* an id that arrives rather than being derived is checked, not trusted */
    ASSERT_TRUE(!iotdata_station_is_assignable(0), "0 is not a station");
    ASSERT_TRUE(!iotdata_station_is_assignable(IOTDATA_STATION_BROADCAST), "broadcast is not assignable");
    ASSERT_TRUE(iotdata_station_is_assignable(1), "1 is");
    ASSERT_TRUE(iotdata_station_is_assignable(IOTDATA_STATION_ASSIGNABLE_MAX), "and so is the top one");

    /* the sequence wraps past the reserved value rather than onto it */
    ASSERT_EQ(iotdata_sequence_next(0), 1, "ordinary increment");
    ASSERT_EQ(iotdata_sequence_next(IOTDATA_SEQUENCE_ASSIGNABLE_MAX - 1), IOTDATA_SEQUENCE_ASSIGNABLE_MAX, "up to the last usable");
    ASSERT_EQ(iotdata_sequence_next(IOTDATA_SEQUENCE_ASSIGNABLE_MAX), 0, "then wraps, skipping down");
    ASSERT_EQ(iotdata_sequence_next(IOTDATA_SEQUENCE_DOWN), 0, "and recovers if it ever holds one");
    PASS();
}

/* =========================================================================
 * Section 7: JSON round-trip
 * =========================================================================*/

static void test_json_round_trip_complete(void) {
    TEST("JSON round-trip (complete variant)");
    begin(0, 10, 999);

    ASSERT_OK(iotdata_encode_battery(&enc, 80, true), "bat");
    ASSERT_OK(iotdata_encode_link(&enc, -80, 0.0f), "link");
    ASSERT_OK(iotdata_encode_environment(&enc, 20.0f, 1013, 50), "env");
    ASSERT_OK(iotdata_encode_wind(&enc, 8.0f, 225, 12.0f), "wind");
    ASSERT_OK(iotdata_encode_rain(&enc, 5, 20), "rain");
    ASSERT_OK(iotdata_encode_solar(&enc, 300, 5), "solar");
    ASSERT_OK(iotdata_encode_clouds(&enc, 4), "cloud");
    ASSERT_OK(iotdata_encode_air_quality_index(&enc, 75), "aqi");
    uint16_t pm[4] = { 50, 120, 80, 200 };
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x0F, pm), "pm");
    uint16_t gas[8] = { 200, 100, 5000, 500, 250, 100, 0, 0 };
    ASSERT_OK(iotdata_encode_air_quality_gas(&enc, 0x3F, gas), "gas");
    ASSERT_OK(iotdata_encode_radiation(&enc, 100, 0.50f), "rad");
    ASSERT_OK(iotdata_encode_depth(&enc, 250), "depth");
    ASSERT_OK(iotdata_encode_position(&enc, 51.5, -0.1), "pos");
    ASSERT_OK(iotdata_encode_datetime(&enc, 86400), "dt");
    ASSERT_OK(iotdata_encode_flags(&enc, 0x42), "flags");
    finish();

    /* Binary → JSON */
    char *json = NULL;
    iotdata_decode_to_json_scratch_t dec_scratch;
    ASSERT_OK(iotdata_decode_to_json(pkt, pkt_len, &json, &dec_scratch), "to_json");

    /* JSON → binary */
    uint8_t pkt2[256];
    size_t len2;
    iotdata_encode_from_json_scratch_t enc_scratch;
    ASSERT_OK(iotdata_encode_from_json(json, pkt2, sizeof(pkt2), &len2, &enc_scratch), "from_json");
    free(json);

    ASSERT_EQ(pkt_len, len2, "len match");
    ASSERT_EQ(memcmp(pkt, pkt2, pkt_len), 0, "bytes match");
    PASS();
}

static void test_json_round_trip_with_tlv(void) {
    TEST("JSON round-trip with TLV");
    begin(0, 5, 100);

    ASSERT_OK(iotdata_encode_battery(&enc, 60, false), "bat");

    /* Add a raw TLV and a string TLV */
    uint8_t raw[] = { 0xDE, 0xAD };
    ASSERT_OK(iotdata_encode_tlv(&enc, 0x20, raw, 2), "tlv raw");
    ASSERT_OK(iotdata_encode_tlv_string(&enc, 0x21, "hello world"), "tlv str");

    /* and a third, to exercise the multi-TLV JSON path */
    ASSERT_OK(iotdata_encode_tlv_string(&enc, 0x22, "test note"), "tlv str2");

    finish();

    /* Binary → JSON */
    char *json = NULL;
    iotdata_decode_to_json_scratch_t dec_scratch;
    ASSERT_OK(iotdata_decode_to_json(pkt, pkt_len, &json, &dec_scratch), "to_json");

    /* JSON → binary (zero scratch: iotdata_encode_begin does not reset tlv_count) */
    uint8_t pkt2[256];
    size_t len2;
    iotdata_encode_from_json_scratch_t enc_scratch;
    ASSERT_OK(iotdata_encode_from_json(json, pkt2, sizeof(pkt2), &len2, &enc_scratch), "from_json");
    free(json);

    /* Decode round-tripped packet and verify TLVs */
    ASSERT_OK(iotdata_decode(pkt2, len2, &dec), "decode2");
    ASSERT_EQ(dec.tlv_count, 3, "tlv count");
    ASSERT_EQ(dec.tlv[0].type, 0x20, "t0 type");
    ASSERT_EQ(dec.tlv[0].format, IOTDATA_TLV_FMT_RAW, "t0 fmt");
    ASSERT_EQ(dec.tlv[1].type, 0x21, "t1 type");
    ASSERT_EQ(dec.tlv[1].format, IOTDATA_TLV_FMT_STRING, "t1 fmt");
    ASSERT_EQ(strcmp(dec.tlv[1].str, "hello world"), 0, "t1 str");
    ASSERT_EQ(dec.tlv[2].type, 0x22, "t2 type");
    PASS();
}

static void test_json_round_trip_standalone(void) {
    TEST("JSON round-trip (standalone variant)");
    begin(1, 20, 400);

    ASSERT_OK(iotdata_encode_battery(&enc, 90, true), "bat");
    ASSERT_OK(iotdata_encode_temperature(&enc, -10.0f), "temp");
    ASSERT_OK(iotdata_encode_pressure(&enc, 950), "pres");
    ASSERT_OK(iotdata_encode_humidity(&enc, 80), "hum");
    ASSERT_OK(iotdata_encode_wind_speed(&enc, 15.0f), "wspd");
    ASSERT_OK(iotdata_encode_wind_direction(&enc, 270), "wdir");
    ASSERT_OK(iotdata_encode_wind_gust(&enc, 25.0f), "wgust");
    ASSERT_OK(iotdata_encode_rain_rate(&enc, 20), "rrate");
    ASSERT_OK(iotdata_encode_rain_size(&enc, 8), "rsize");
    ASSERT_OK(iotdata_encode_radiation_cpm(&enc, 200), "cpm");
    ASSERT_OK(iotdata_encode_radiation_dose(&enc, 1.50f), "dose");
    ASSERT_OK(iotdata_encode_depth(&enc, 500), "depth");
    ASSERT_OK(iotdata_encode_position(&enc, 35.6762, 139.6503), "pos");
    ASSERT_OK(iotdata_encode_datetime(&enc, 43200), "dt");
    ASSERT_OK(iotdata_encode_flags(&enc, 0xAA), "flags");
    finish();

    char *json = NULL;
    iotdata_decode_to_json_scratch_t dec_scratch;
    ASSERT_OK(iotdata_decode_to_json(pkt, pkt_len, &json, &dec_scratch), "to_json");

    uint8_t pkt2[256];
    size_t len2;
    iotdata_encode_from_json_scratch_t enc_scratch;
    ASSERT_OK(iotdata_encode_from_json(json, pkt2, sizeof(pkt2), &len2, &enc_scratch), "from_json");
    free(json);

    ASSERT_EQ(pkt_len, len2, "len match");
    ASSERT_EQ(memcmp(pkt, pkt2, pkt_len), 0, "bytes match");
    PASS();
}

/* =========================================================================
 * Section 8: Decode error paths
 * =========================================================================*/

static void test_decode_short(void) {
    TEST("Decode short buffer");

    uint8_t short_buf[] = { 0x00, 0x00, 0x00 };
    ASSERT_ERR(iotdata_decode(short_buf, 3, &dec), IOTDATA_ERR_DECODE_SHORT, "short");
    PASS();
}

static void test_decode_truncated(void) {
    TEST("Decode truncated (field data missing)");

    /* Encode a packet with battery, then truncate to header+pres0 only */
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "bat");
    finish();

    /* Full packet is 6 bytes; truncate to 5 (header + pres0, no field data) */
    ASSERT_ERR(iotdata_decode(pkt, 5, &dec), IOTDATA_ERR_DECODE_TRUNCATED, "truncated");
    PASS();
}

static void test_decode_reserved_variant(void) {
    TEST("Decode reserved variant (15)");

    /* Manually construct: variant=15, station=0, seq=0, pres0=0 */
    uint8_t bad[] = { 0xF0, 0x00, 0x00, 0x00, 0x00 };
    ASSERT_ERR(iotdata_decode(bad, 5, &dec), IOTDATA_ERR_DECODE_VARIANT, "reserved");
    PASS();
}

/* =========================================================================
 * Section 9: Dump and print
 * =========================================================================*/

static void test_dump_complete_variant(void) {
    TEST("Dump complete variant");
    begin(0, 5, 42);

    ASSERT_OK(iotdata_encode_battery(&enc, 90, false), "bat");
    ASSERT_OK(iotdata_encode_depth(&enc, 300), "depth");
    uint16_t pm[4] = { 100, 200, 150, 300 };
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x0F, pm), "pm");
    finish();

    char str[8192];
    iotdata_dump_t dump;
    ASSERT_OK(iotdata_dump_to_string(&dump, pkt, pkt_len, str, sizeof(str), true), "dump");
    if (!strstr(str, "variant")) {
        FAIL("missing variant");
        return;
    }
    if (!strstr(str, "battery")) {
        FAIL("missing battery");
        return;
    }
    PASS();
}

static void test_print_complete_variant(void) {
    TEST("Print complete variant");
    begin(0, 7, 100);

    ASSERT_OK(iotdata_encode_battery(&enc, 60, true), "bat");
    ASSERT_OK(iotdata_encode_environment(&enc, 15.0f, 1000, 70), "env");
    ASSERT_OK(iotdata_encode_depth(&enc, 200), "depth");
    finish();

    char str[8192];
    iotdata_print_scratch_t print_scratch;
    ASSERT_OK(iotdata_print_to_string(pkt, pkt_len, str, sizeof(str), &print_scratch), "print");
    if (!strstr(str, "complete")) {
        FAIL("missing variant name");
        return;
    }
    PASS();
}

/* =========================================================================
 * Section 10: Image compression utilities
 * =========================================================================*/

static void test_image_rle_round_trip(void) {
    TEST("Image RLE compress/decompress");

    /* 128 bilevel pixels: 64 white + 64 black */
    uint8_t pixels[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    uint8_t compressed[64];
    size_t comp_len = iotdata_image_rle_compress(pixels, 128, 1, compressed, sizeof(compressed));
    if (comp_len == 0) {
        FAIL("compress failed");
        return;
    }

    uint8_t decompressed[16] = { 0x42 };
    size_t decomp_len = iotdata_image_rle_decompress(compressed, comp_len, 1, decompressed, sizeof(decompressed));
    if (decomp_len == 0) {
        FAIL("decompress failed");
        return;
    }

    ASSERT_EQ(memcmp(pixels, decompressed, 16), 0, "round-trip");
    PASS();
}

static void test_image_heatshrink_round_trip(void) {
    TEST("Image heatshrink compress/decompress");

    /* Repetitive pattern: 64 bytes of repeating 0,1,2,3 */
    uint8_t raw[64];
    for (int i = 0; i < 64; i++)
        raw[i] = (uint8_t)(i & 3);

    uint8_t compressed[128];
    size_t comp_len = iotdata_image_hs_compress(raw, 64, compressed, sizeof(compressed));
    if (comp_len == 0) {
        FAIL("compress failed");
        return;
    }

    uint8_t decompressed[64] = { 0xFF };
    size_t decomp_len = iotdata_image_hs_decompress(compressed, comp_len, decompressed, sizeof(decompressed));
    if (decomp_len == 0) {
        FAIL("decompress failed");
        return;
    }

    ASSERT_EQ(memcmp(raw, decompressed, 64), 0, "round-trip");
    PASS();
}

/* =========================================================================
 * Main
 * =========================================================================*/

int main(void) {
    printf("\n=== iotdata — comprehensive test suite ===\n\n");

    printf("--- Section 1: Field round-trips (new types) ---\n");
    test_aq_pm_round_trip();
    test_aq_pm_partial();
    test_aq_gas_round_trip();
    test_depth_round_trip();
    test_image_round_trip();

    printf("\n--- Section 2: Full variant tests ---\n");
    test_complete_variant_all_fields();
    test_standalone_variant_all_fields();

    printf("\n--- Section 3: Boundary values ---\n");
    test_aq_pm_boundaries();
    test_aq_gas_boundaries();
    test_depth_boundaries();
    test_image_flags_combinations();

    printf("\n--- Section 4: Error conditions ---\n");
    test_aq_pm_errors();
    test_aq_gas_errors();
    test_image_errors();
    test_tlv_errors();
    test_encode_buffer_overflow();

    printf("\n--- Section 5: Peek ---\n");
    test_peek_basic();
    test_peek_null_params();
    test_peek_short_buffer();
    test_peek_reserved_variant();

    printf("\n--- Section 6: TLV typed helpers ---\n");
    test_tlv_multiple();
    test_kv_raw_round_trip();
    test_kv_raw_robustness();
    test_kv_raw_in_tlv();
    test_kv_string_round_trip();
    test_node_definitions();
    test_node_status_tlv();
    test_node_receive_tlv();
    test_node_variant_tlv();
    test_node_reserved_header_values();

    printf("\n--- Section 7: JSON round-trip ---\n");
    test_json_round_trip_complete();
    test_json_round_trip_with_tlv();
    test_json_round_trip_standalone();

    printf("\n--- Section 8: Decode error paths ---\n");
    test_decode_short();
    test_decode_truncated();
    test_decode_reserved_variant();

    printf("\n--- Section 9: Dump and print ---\n");
    test_dump_complete_variant();
    test_print_complete_variant();

    printf("\n--- Section 10: Image compression ---\n");
    test_image_rle_round_trip();
    test_image_heatshrink_round_trip();

    printf("\n--- Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed > 0)
        printf(", %d FAILED", tests_failed);
    printf(" ---\n\n");

    return tests_failed > 0 ? 1 : 0;
}
