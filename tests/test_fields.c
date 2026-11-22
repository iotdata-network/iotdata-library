/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * test_fields.c — one test per field, for the fields that are generated
 *
 * WHY THIS EXISTS. The other suites exercise fields incidentally: they are about
 * presence bytes, variant maps, slot order, JSON round trips, and they reach for
 * whatever field is convenient to make that point. Useful, and it leaves no way
 * to answer "is this field tested" except by reading all four. One field turned
 * out not to be tested at all -- BITS32 was never encoded anywhere.
 *
 * So this suite asks the same four questions of every field, and a field joins it
 * when it moves into specification/iotdata-fields-*.xml:
 *
 *   1. ROUND TRIP      a value survives encode and decode, within whatever its
 *                      quantisation costs
 *   2. BOUNDARIES      the lowest and highest values it admits both survive
 *   3. RANGE           every <check> in the specification actually fires
 *   4. ABSENCE         a field not encoded reads back as absent, rather than as
 *                      whatever the last packet left behind
 *
 * IT IS ALSO THE BEHAVIOURAL GUARD ON THE TEMPLATES. `make generate-check` proves
 * the generated files have not CHANGED; it cannot say whether a change is right.
 * Once a field lives only in the specification, an edit to a template moves real
 * code, and the gate will fail by design the moment someone means to edit one --
 * at which point these tests are what say whether the new output still behaves.
 * A template that compiles and reproduces nothing is caught by the gate; a
 * template that compiles and produces subtly wrong arithmetic is caught here.
 *
 * It is NOT a replacement for the other suites and does not try to be. The
 * eventual intent is to split it the way the specification is split
 * (test_fields_unitary, test_fields_weather) but not yet -- one file is easier to
 * read while the set is small.
 *
 * Compile:
 *   cc -DIOTDATA_VARIANT_MAPS=field_variants -DIOTDATA_VARIANT_MAPS_COUNT=1 \
 *      test_fields.c iotdata.c -lm -lcjson -o test_fields
 */

#include "iotdata.h"

#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * One variant carrying every generated field, each in its own slot.
 *
 * SLOT ORDER IS THE ORDER THE TESTS ENCODE IN, so that this suite builds and
 * passes under IOTDATA_OPERATE_ORDERED as well -- there the cursor only moves
 * forwards and call order is field order. Every trailing slot is explicitly
 * IOTDATA_FIELD_NONE: a partly written initialiser leaves the rest ZERO, and
 * zero is a real field type, not NONE.
 * -----------------------------------------------------------------------*/

const iotdata_variant_def_t field_variants[3] = {
    [0] = {
        .name = "fields",
        /* FOUR presence bytes = 6 + 7 + 7 + 7 = 27 slots, which is IOTDATA_MAX_DATA_FIELDS and
           so the most any variant can address. The library knows 28 fields, so they DO NOT FIT
           in one variant and IMAGE sits in a second below. That is the protocol's limit, not the
           suite's: no single packet can offer every field there is. */
        .num_pres_bytes = 4,
        .fields = {
            { IOTDATA_FIELD_BATTERY,        "battery" },
            { IOTDATA_FIELD_LINK,           "link" },
            { IOTDATA_FIELD_TEMPERATURE,    "temperature" },
            { IOTDATA_FIELD_PRESSURE,       "pressure" },
            { IOTDATA_FIELD_HUMIDITY,       "humidity" },
            { IOTDATA_FIELD_ENVIRONMENT,    "environment" },
            { IOTDATA_FIELD_WIND_SPEED,     "wind_speed" },
            { IOTDATA_FIELD_WIND_DIRECTION, "wind_direction" },
            { IOTDATA_FIELD_WIND_GUST,      "wind_gust" },
            { IOTDATA_FIELD_WIND,           "wind" },
            { IOTDATA_FIELD_RAIN_RATE,      "rain_rate" },
            { IOTDATA_FIELD_RAIN_SIZE,      "rain_size" },
            { IOTDATA_FIELD_RAIN,           "rain" },
            { IOTDATA_FIELD_SOLAR,          "solar" },
            { IOTDATA_FIELD_CLOUDS,         "clouds" },
            { IOTDATA_FIELD_AIR_QUALITY_INDEX, "air_quality_index" },
            { IOTDATA_FIELD_AIR_QUALITY_PM, "air_quality_pm" },
            { IOTDATA_FIELD_AIR_QUALITY_GAS, "air_quality_gas" },
            { IOTDATA_FIELD_AIR_QUALITY,    "air_quality" },
            { IOTDATA_FIELD_RADIATION_CPM,  "radiation_cpm" },
            { IOTDATA_FIELD_RADIATION_DOSE, "radiation_dose" },
            { IOTDATA_FIELD_RADIATION,      "radiation" },
            { IOTDATA_FIELD_DEPTH,          "depth" },
            { IOTDATA_FIELD_POSITION,       "position" },
            { IOTDATA_FIELD_DATETIME,       "datetime" },
            { IOTDATA_FIELD_FLAGS,          "flags" },
            { IOTDATA_FIELD_BITS32,         "bits32" },
        },
    },
    /* IMAGE alone, because the twenty-seven above fill every slot a variant has. */
    [1] = {
        .name = "image",
        .num_pres_bytes = 1,
        .fields = {
            { IOTDATA_FIELD_IMAGE, "image" },
            { IOTDATA_FIELD_NONE,  NULL },
        },
    },
    /* The six standalone parts of the three families that used to be bundles. Their composites
       are in variant 0; these are the halves a device can now send on their own. */
    [2] = {
        .name = "parts",
        .num_pres_bytes = 1,
        .fields = {
            { IOTDATA_FIELD_BATTERY_LEVEL,     "battery_level" },
            { IOTDATA_FIELD_BATTERY_CHARGE,    "battery_charge" },
            { IOTDATA_FIELD_LINK_RSSI,         "link_rssi" },
            { IOTDATA_FIELD_LINK_SNR,          "link_snr" },
            { IOTDATA_FIELD_SOLAR_IRRADIANCE,  "solar_irradiance" },
            { IOTDATA_FIELD_SOLAR_ULTRAVIOLET, "solar_ultraviolet" },
            { IOTDATA_FIELD_NONE,              NULL },
        },
    },
};

#include "test_common.h"

/* An empty packet, for the absence questions. The variant matters now that there are two. */
static void encode_nothing_of(uint8_t variant) {
    begin(variant, 1, 99);
    finish();
    decode_pkt();
}
static void encode_nothing(void) {
    encode_nothing_of(0);
}

/* =========================================================================
 * Device fields
 * ========================================================================= */

static void test_field_battery(void) {
    TEST("BATTERY: round trip, boundaries, range, absence");

    /* Round trip. The level is held in five bits across 0..100, so it comes back
       to within a couple of percent; the charging flag is one bit and exact. */
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_battery(&enc, 50, true), "encode 50% charging");
    finish();
    decode_pkt();
    {
        uint8_t level_percent;
        bool charging;
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery(&dec, &level_percent, &charging), "decode");
        ASSERT_NEAR(level_percent, 50, 4, "level");
        ASSERT_EQ(charging, true, "charging");
    }

    /* Both boundaries. */
    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_battery(&enc, 0, false), "encode 0%");
    finish();
    decode_pkt();
    {
        uint8_t level_percent;
        bool charging;
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery(&dec, &level_percent, &charging), "decode 0%");
        ASSERT_EQ_U(level_percent, 0, "0 is exact");
        ASSERT_EQ(charging, false, "not charging");
    }
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_battery(&enc, 100, true), "encode 100%");
    finish();
    decode_pkt();
    {
        uint8_t level_percent;
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery(&dec, &level_percent, NULL), "decode 100%");
        ASSERT_EQ_U(level_percent, 100, "100 is exact");
    }

    /* Range. */
    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_battery(&enc, 101, false), IOTDATA_ERR_BATTERY_LEVEL_HIGH, "101% refused");

    /* Sweep. Every admissible level, not a sample of them: a width or promotion
       fault shows up at one end of a range and nowhere near the middle, which is
       exactly what a single mid-range value cannot see. */
    for (uint8_t v = 0; v <= IOTDATA_BATTERY_LEVEL_MAX; v++) {
        begin(0, 1, (uint16_t)(100 + v));
        ASSERT_OK(iotdata_encode_battery(&enc, v, (v & 1) != 0), "sweep encode");
        finish();
        decode_pkt();
        uint8_t level_percent;
        bool charging;
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery(&dec, &level_percent, &charging), "sweep decode");
        ASSERT_NEAR(level_percent, v, 4, "sweep level within quantisation");
        ASSERT_EQ(charging, (v & 1) != 0, "sweep charging exact");
    }

    /* Absence. */
    encode_nothing();
    {
        uint8_t level_percent;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_battery(&dec, &level_percent, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_link(void) {
    TEST("LINK: round trip, boundaries, range, absence");

    /* RSSI steps in 4 dBm from -120, so -88 lands exactly on a step. SNR has two
       bits across -20..+10 in steps of 10, which is as coarse as the protocol
       gets -- the tolerance below is the step, not slack. */
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_link(&enc, -88, 0), "encode -88 dBm, 0 dB");
    finish();
    decode_pkt();
    {
        int8_t rssi_dbm;
        iotdata_float_t snr_db;
        rewind_dec();
        ASSERT_OK(iotdata_decode_link(&dec, &rssi_dbm, &snr_db), "decode");
        ASSERT_EQ(rssi_dbm, -88, "rssi on a step is exact");
        ASSERT_NEAR(snr_db, 0, IOTDATA_LINK_SNR_STEP, "snr within one step");
    }

    /* Both boundaries. */
    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_link(&enc, IOTDATA_LINK_RSSI_MIN, IOTDATA_LINK_SNR_MIN), "encode minimums");
    finish();
    decode_pkt();
    {
        int8_t rssi_dbm;
        iotdata_float_t snr_db;
        rewind_dec();
        ASSERT_OK(iotdata_decode_link(&dec, &rssi_dbm, &snr_db), "decode minimums");
        ASSERT_EQ(rssi_dbm, IOTDATA_LINK_RSSI_MIN, "rssi min");
        ASSERT_NEAR(snr_db, IOTDATA_LINK_SNR_MIN, IOTDATA_LINK_SNR_STEP, "snr min");
    }
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_link(&enc, IOTDATA_LINK_RSSI_MAX, IOTDATA_LINK_SNR_MAX), "encode maximums");
    finish();
    decode_pkt();
    {
        int8_t rssi_dbm;
        iotdata_float_t snr_db;
        rewind_dec();
        ASSERT_OK(iotdata_decode_link(&dec, &rssi_dbm, &snr_db), "decode maximums");
        ASSERT_EQ(rssi_dbm, IOTDATA_LINK_RSSI_MAX, "rssi max");
        ASSERT_NEAR(snr_db, IOTDATA_LINK_SNR_MAX, IOTDATA_LINK_SNR_STEP, "snr max");
    }

    /* Range -- all four of this field's checks. */
    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_link(&enc, IOTDATA_LINK_RSSI_MIN - 1, 0), IOTDATA_ERR_LINK_RSSI_LOW, "rssi below");
    ASSERT_ERR(iotdata_encode_link(&enc, IOTDATA_LINK_RSSI_MAX + 1, 0), IOTDATA_ERR_LINK_RSSI_HIGH, "rssi above");
    ASSERT_ERR(iotdata_encode_link(&enc, -88, IOTDATA_LINK_SNR_MIN - 1), IOTDATA_ERR_LINK_SNR_LOW, "snr below");
    ASSERT_ERR(iotdata_encode_link(&enc, -88, IOTDATA_LINK_SNR_MAX + 1), IOTDATA_ERR_LINK_SNR_HIGH, "snr above");

    /* Sweep, stepping the way the wire does so every value lands on a step. The
       loop counts in steps rather than in units so that it reads the same in a
       floating build and an integer one. */
    for (int k = 0; IOTDATA_LINK_RSSI_MIN + k * IOTDATA_LINK_RSSI_STEP <= IOTDATA_LINK_RSSI_MAX; k++) {
        const int8_t want = (int8_t)(IOTDATA_LINK_RSSI_MIN + k * IOTDATA_LINK_RSSI_STEP);
        begin(0, 1, (uint16_t)(200 + k));
        ASSERT_OK(iotdata_encode_link(&enc, want, 0), "sweep encode rssi");
        finish();
        decode_pkt();
        int8_t rssi_dbm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_link(&dec, &rssi_dbm, NULL), "sweep decode rssi");
        ASSERT_EQ(rssi_dbm, want, "every rssi step is exact");
    }
    for (int k = 0; IOTDATA_LINK_SNR_MIN + (iotdata_float_t)k * IOTDATA_LINK_SNR_STEP <= IOTDATA_LINK_SNR_MAX; k++) {
        const iotdata_float_t want = IOTDATA_LINK_SNR_MIN + (iotdata_float_t)k * IOTDATA_LINK_SNR_STEP;
        begin(0, 1, (uint16_t)(300 + k));
        ASSERT_OK(iotdata_encode_link(&enc, -88, want), "sweep encode snr");
        finish();
        decode_pkt();
        iotdata_float_t snr_db;
        rewind_dec();
        ASSERT_OK(iotdata_decode_link(&dec, NULL, &snr_db), "sweep decode snr");
        ASSERT_NEAR(snr_db, want, IOTDATA_LINK_SNR_STEP, "every snr step within one step");
    }

    /* Absence. */
    encode_nothing();
    {
        int8_t rssi_dbm;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_link(&dec, &rssi_dbm, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

/* =========================================================================
 * Weather fields
 * ========================================================================= */

static void test_field_solar(void) {
    TEST("SOLAR: round trip, boundaries, range, absence");

    /* Neither quantity is quantised, so both are exact. */
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_solar(&enc, 500, 7), "encode 500 W/m2, UV 7");
    finish();
    decode_pkt();
    {
        uint16_t irradiance_wm2;
        uint8_t ultraviolet_index;
        rewind_dec();
        ASSERT_OK(iotdata_decode_solar(&dec, &irradiance_wm2, &ultraviolet_index), "decode");
        ASSERT_EQ_U(irradiance_wm2, 500, "irradiance");
        ASSERT_EQ_U(ultraviolet_index, 7, "ultraviolet");
    }

    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_solar(&enc, 0, 0), "encode minimums");
    finish();
    decode_pkt();
    {
        uint16_t irradiance_wm2;
        uint8_t ultraviolet_index;
        rewind_dec();
        ASSERT_OK(iotdata_decode_solar(&dec, &irradiance_wm2, &ultraviolet_index), "decode minimums");
        ASSERT_EQ_U(irradiance_wm2, 0, "irradiance 0");
        ASSERT_EQ_U(ultraviolet_index, 0, "ultraviolet 0");
    }
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_solar(&enc, IOTDATA_SOLAR_IRRADIANCE_MAX, IOTDATA_SOLAR_ULTRAVIOLET_MAX), "encode maximums");
    finish();
    decode_pkt();
    {
        uint16_t irradiance_wm2;
        uint8_t ultraviolet_index;
        rewind_dec();
        ASSERT_OK(iotdata_decode_solar(&dec, &irradiance_wm2, &ultraviolet_index), "decode maximums");
        ASSERT_EQ_U(irradiance_wm2, IOTDATA_SOLAR_IRRADIANCE_MAX, "irradiance max");
        ASSERT_EQ_U(ultraviolet_index, IOTDATA_SOLAR_ULTRAVIOLET_MAX, "ultraviolet max");
    }

    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_solar(&enc, IOTDATA_SOLAR_IRRADIANCE_MAX + 1, 0), IOTDATA_ERR_SOLAR_IRRADIANCE_HIGH, "irradiance above");
    ASSERT_ERR(iotdata_encode_solar(&enc, 0, IOTDATA_SOLAR_ULTRAVIOLET_MAX + 1), IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH, "ultraviolet above");

    /* Sweep each quantity across its whole range, the other held at zero. Ten bits
       of irradiance is where a 16-bit int would show itself if anything promoted
       wrongly. */
    for (uint16_t v = 0; v <= IOTDATA_SOLAR_IRRADIANCE_MAX; v++) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_solar(&enc, v, 0), "sweep encode irradiance");
        finish();
        decode_pkt();
        uint16_t irradiance_wm2;
        rewind_dec();
        ASSERT_OK(iotdata_decode_solar(&dec, &irradiance_wm2, NULL), "sweep decode irradiance");
        ASSERT_EQ_U(irradiance_wm2, v, "every irradiance is exact");
    }
    for (uint8_t v = 0; v <= IOTDATA_SOLAR_ULTRAVIOLET_MAX; v++) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_solar(&enc, 0, v), "sweep encode ultraviolet");
        finish();
        decode_pkt();
        uint8_t ultraviolet_index;
        rewind_dec();
        ASSERT_OK(iotdata_decode_solar(&dec, NULL, &ultraviolet_index), "sweep decode ultraviolet");
        ASSERT_EQ_U(ultraviolet_index, v, "every ultraviolet is exact");
    }

    encode_nothing();
    {
        uint16_t irradiance_wm2;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_solar(&dec, &irradiance_wm2, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_clouds(void) {
    TEST("CLOUDS: round trip, boundaries, range, absence");

    /* Four bits hold 0..8 exactly, so nothing is lost. */
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_clouds(&enc, 4), "encode 4 okta");
    finish();
    decode_pkt();
    {
        uint8_t okta;
        rewind_dec();
        ASSERT_OK(iotdata_decode_clouds(&dec, &okta), "decode");
        ASSERT_EQ_U(okta, 4, "okta");
    }

    for (uint8_t v = 0; v <= IOTDATA_CLOUDS_MAX; v++) {
        begin(0, 1, (uint16_t)(10 + v));
        ASSERT_OK(iotdata_encode_clouds(&enc, v), "encode every admissible okta");
        finish();
        decode_pkt();
        uint8_t okta;
        rewind_dec();
        ASSERT_OK(iotdata_decode_clouds(&dec, &okta), "decode every admissible okta");
        ASSERT_EQ_U(okta, v, "exact at every step");
    }

    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_clouds(&enc, IOTDATA_CLOUDS_MAX + 1), IOTDATA_ERR_CLOUDS_HIGH, "9 okta refused");

    encode_nothing();
    {
        uint8_t okta;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_clouds(&dec, &okta), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_depth(void) {
    TEST("DEPTH: round trip, boundaries, range, absence");

    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_depth(&enc, 300), "encode 300 cm");
    finish();
    decode_pkt();
    {
        uint16_t depth_cm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_depth(&dec, &depth_cm), "decode");
        ASSERT_EQ_U(depth_cm, 300, "depth");
    }

    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_depth(&enc, 0), "encode 0 cm");
    finish();
    decode_pkt();
    {
        uint16_t depth_cm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_depth(&dec, &depth_cm), "decode 0");
        ASSERT_EQ_U(depth_cm, 0, "0 cm");
    }
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_depth(&enc, IOTDATA_DEPTH_MAX), "encode maximum");
    finish();
    decode_pkt();
    {
        uint16_t depth_cm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_depth(&dec, &depth_cm), "decode maximum");
        ASSERT_EQ_U(depth_cm, IOTDATA_DEPTH_MAX, "max cm");
    }

    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_depth(&enc, IOTDATA_DEPTH_MAX + 1), IOTDATA_ERR_DEPTH_HIGH, "above maximum");

    /* Sweep the whole ten-bit range exhaustively. */
    for (uint16_t v = 0; v <= IOTDATA_DEPTH_MAX; v++) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_depth(&enc, v), "sweep encode");
        finish();
        decode_pkt();
        uint16_t depth_cm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_depth(&dec, &depth_cm), "sweep decode");
        ASSERT_EQ_U(depth_cm, v, "every centimetre is exact");
    }

    encode_nothing();
    {
        uint16_t depth_cm;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_depth(&dec, &depth_cm), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

/* =========================================================================
 * Context fields
 * ========================================================================= */

static void test_field_datetime(void) {
    TEST("DATETIME: round trip, boundaries, range, absence");

    /* Five-second resolution, so a value on a five-second boundary is exact and
       anything else is rounded down by the division. */
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_datetime(&enc, 86400), "encode one day in");
    finish();
    decode_pkt();
    {
        uint32_t seconds_from_year_start;
        rewind_dec();
        ASSERT_OK(iotdata_decode_datetime(&dec, &seconds_from_year_start), "decode");
        ASSERT_EQ_U(seconds_from_year_start, 86400, "a multiple of the resolution is exact");
    }
    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_datetime(&enc, 7), "encode 7 seconds");
    finish();
    decode_pkt();
    {
        uint32_t seconds_from_year_start;
        rewind_dec();
        ASSERT_OK(iotdata_decode_datetime(&dec, &seconds_from_year_start), "decode 7 seconds");
        ASSERT_EQ_U(seconds_from_year_start, 5, "7s falls to the 5s tick below it");
    }

    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_datetime(&enc, 0), "encode the start of the year");
    finish();
    decode_pkt();
    {
        uint32_t seconds_from_year_start;
        rewind_dec();
        ASSERT_OK(iotdata_decode_datetime(&dec, &seconds_from_year_start), "decode 0");
        ASSERT_EQ_U(seconds_from_year_start, 0, "0 seconds");
    }
    begin(0, 1, 4);
    ASSERT_OK(iotdata_encode_datetime(&enc, IOTDATA_DATETIME_MAX * IOTDATA_DATETIME_RES), "encode the last tick");
    finish();
    decode_pkt();
    {
        uint32_t seconds_from_year_start;
        rewind_dec();
        ASSERT_OK(iotdata_decode_datetime(&dec, &seconds_from_year_start), "decode the last tick");
        ASSERT_EQ_U(seconds_from_year_start, IOTDATA_DATETIME_MAX * IOTDATA_DATETIME_RES, "last tick");
    }

    begin(0, 1, 5);
    ASSERT_ERR(iotdata_encode_datetime(&enc, (IOTDATA_DATETIME_MAX + 1) * IOTDATA_DATETIME_RES), IOTDATA_ERR_DATETIME_HIGH, "one tick too far");

    /* Sweep across the twenty-four bit tick space, at every power of two and at a
       stride that is not one. Twenty-four bits is past what a 16-bit int holds, so
       this is the field where a width assumption bites -- and IOTDATA_DATETIME_MAX
       is itself built from a shift, which is why it is written as a 32-bit one. */
    for (unsigned b = 0; b < IOTDATA_DATETIME_BITS; b++) {
        const uint32_t tick = (uint32_t)1 << b;
        begin(0, 1, (uint16_t)(400 + b));
        ASSERT_OK(iotdata_encode_datetime(&enc, tick * IOTDATA_DATETIME_RES), "sweep encode power of two");
        finish();
        decode_pkt();
        uint32_t seconds_from_year_start;
        rewind_dec();
        ASSERT_OK(iotdata_decode_datetime(&dec, &seconds_from_year_start), "sweep decode power of two");
        ASSERT_EQ_U(seconds_from_year_start, tick * IOTDATA_DATETIME_RES, "every bit position survives");
    }
    for (uint32_t tick = 0; tick <= IOTDATA_DATETIME_MAX; tick += 65521u) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_datetime(&enc, tick * IOTDATA_DATETIME_RES), "sweep encode stride");
        finish();
        decode_pkt();
        uint32_t seconds_from_year_start;
        rewind_dec();
        ASSERT_OK(iotdata_decode_datetime(&dec, &seconds_from_year_start), "sweep decode stride");
        ASSERT_EQ_U(seconds_from_year_start, tick * IOTDATA_DATETIME_RES, "every stride point survives");
    }

    encode_nothing();
    {
        uint32_t seconds_from_year_start;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_datetime(&dec, &seconds_from_year_start), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

/* =========================================================================
 * Unitary fields
 * ========================================================================= */

static void test_field_flags(void) {
    TEST("FLAGS: round trip, every bit, absence");

    /* No quantisation and no range: every one of the 256 values must survive
       exactly, which is cheap enough to check exhaustively. */
    for (unsigned v = 0; v <= 0xFF; v++) {
        begin(0, 1, (uint16_t)v);
        ASSERT_OK(iotdata_encode_flags(&enc, (uint8_t)v), "encode");
        finish();
        decode_pkt();
        uint8_t flags;
        rewind_dec();
        ASSERT_OK(iotdata_decode_flags(&dec, &flags), "decode");
        ASSERT_EQ_U(flags, v, "every bit pattern survives");
    }

    encode_nothing();
    {
        uint8_t flags;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_flags(&dec, &flags), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_bits32(void) {
    TEST("BITS32: round trip, the full width, absence");

    /* THIS FIELD WAS NOT TESTED ANYWHERE before this suite -- no other test
       encodes it. Thirty-two bits is the widest a single slot carries, so the
       ends and the alternating patterns are the ones worth naming. */
    static const uint32_t patterns[] = { 0u, 1u, 0xFFFFFFFFu, 0xAAAAAAAAu, 0x55555555u, 0x80000000u, 0x7FFFFFFFu, 0x12345678u };
    for (size_t i = 0; i < sizeof(patterns) / sizeof(patterns[0]); i++) {
        begin(0, 1, (uint16_t)i);
        ASSERT_OK(iotdata_encode_bits32(&enc, patterns[i]), "encode");
        finish();
        decode_pkt();
        uint32_t bits32;
        rewind_dec();
        ASSERT_OK(iotdata_decode_bits32(&dec, &bits32), "decode");
        ASSERT_EQ_U(bits32, patterns[i], "the whole word survives");
    }

    /* A SINGLE BIT WALKED THROUGH ALL THIRTY-TWO POSITIONS. The widest quantity in
       the protocol, and the one place where an int narrower than the field would
       drop the top half without saying anything. Also the complement of each, so
       a stuck bit shows either way round. */
    for (unsigned b = 0; b < IOTDATA_BITS32_BITS; b++) {
        const uint32_t one = (uint32_t)1 << b;
        begin(0, 1, (uint16_t)(500 + b));
        ASSERT_OK(iotdata_encode_bits32(&enc, one), "walk encode");
        finish();
        decode_pkt();
        uint32_t bits32;
        rewind_dec();
        ASSERT_OK(iotdata_decode_bits32(&dec, &bits32), "walk decode");
        ASSERT_EQ_U(bits32, one, "bit survives at its own position");

        begin(0, 1, (uint16_t)(600 + b));
        ASSERT_OK(iotdata_encode_bits32(&enc, ~one), "walk encode complement");
        finish();
        decode_pkt();
        rewind_dec();
        ASSERT_OK(iotdata_decode_bits32(&dec, &bits32), "walk decode complement");
        ASSERT_EQ_U(bits32, (uint32_t)~one, "and its complement");
    }

    encode_nothing();
    {
        uint32_t bits32;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_bits32(&dec, &bits32), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_temperature(void) {
    TEST("TEMPERATURE: round trip, boundaries, range, absence");

    /* Quarter-degree resolution over -40..+80, which nine bits hold exactly, so a value ON a
       quarter-degree step survives unchanged. Everything here is written in terms of the field's
       own constants so that it reads the same in a floating build and an integer one, where the
       same quantity is held in hundredths. */
    const iotdata_float_t mid = (iotdata_float_t)(IOTDATA_TEMPERATURE_MIN + 90 * IOTDATA_TEMPERATURE_RES);
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_temperature(&enc, mid), "encode a mid value");
    finish();
    decode_pkt();
    {
        iotdata_float_t temperature_c;
        rewind_dec();
        ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "decode");
        ASSERT_NEAR(temperature_c, mid, IOTDATA_TEMPERATURE_RES, "within one step");
    }

    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_temperature(&enc, IOTDATA_TEMPERATURE_MIN), "encode the minimum");
    finish();
    decode_pkt();
    {
        iotdata_float_t temperature_c;
        rewind_dec();
        ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "decode the minimum");
        ASSERT_NEAR(temperature_c, IOTDATA_TEMPERATURE_MIN, IOTDATA_TEMPERATURE_RES, "minimum");
    }
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_temperature(&enc, IOTDATA_TEMPERATURE_MAX), "encode the maximum");
    finish();
    decode_pkt();
    {
        iotdata_float_t temperature_c;
        rewind_dec();
        ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "decode the maximum");
        ASSERT_NEAR(temperature_c, IOTDATA_TEMPERATURE_MAX, IOTDATA_TEMPERATURE_RES, "maximum");
    }

    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_temperature(&enc, (iotdata_float_t)(IOTDATA_TEMPERATURE_MIN - IOTDATA_TEMPERATURE_RES)), IOTDATA_ERR_TEMPERATURE_LOW, "below");
    ASSERT_ERR(iotdata_encode_temperature(&enc, (iotdata_float_t)(IOTDATA_TEMPERATURE_MAX + IOTDATA_TEMPERATURE_RES)), IOTDATA_ERR_TEMPERATURE_HIGH, "above");

    /* Sweep every representable step across the whole span. */
    for (int k = 0; IOTDATA_TEMPERATURE_MIN + (iotdata_float_t)k * IOTDATA_TEMPERATURE_RES <= IOTDATA_TEMPERATURE_MAX; k++) {
        const iotdata_float_t want = IOTDATA_TEMPERATURE_MIN + (iotdata_float_t)k * IOTDATA_TEMPERATURE_RES;
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_temperature(&enc, want), "sweep encode");
        finish();
        decode_pkt();
        iotdata_float_t temperature_c;
        rewind_dec();
        ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "sweep decode");
        ASSERT_NEAR(temperature_c, want, IOTDATA_TEMPERATURE_RES, "every step survives");
    }

    encode_nothing();
    {
        iotdata_float_t temperature_c;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_temperature(&dec, &temperature_c), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_pressure(void) {
    TEST("PRESSURE: round trip, boundaries, range, absence");

    /* Quantised by offset alone: eight bits carry the 255 hPa span above the minimum, so every
       admissible value is exact and the sweep can be exhaustive. */
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_pressure(&enc, 1013), "encode 1013 hPa");
    finish();
    decode_pkt();
    {
        uint16_t pressure_hpa;
        rewind_dec();
        ASSERT_OK(iotdata_decode_pressure(&dec, &pressure_hpa), "decode");
        ASSERT_EQ_U(pressure_hpa, 1013, "exact");
    }

    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_pressure(&enc, IOTDATA_PRESSURE_MIN - 1), IOTDATA_ERR_PRESSURE_LOW, "below");
    ASSERT_ERR(iotdata_encode_pressure(&enc, IOTDATA_PRESSURE_MAX + 1), IOTDATA_ERR_PRESSURE_HIGH, "above");

    for (uint16_t v = IOTDATA_PRESSURE_MIN; v <= IOTDATA_PRESSURE_MAX; v++) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_pressure(&enc, v), "sweep encode");
        finish();
        decode_pkt();
        uint16_t pressure_hpa;
        rewind_dec();
        ASSERT_OK(iotdata_decode_pressure(&dec, &pressure_hpa), "sweep decode");
        ASSERT_EQ_U(pressure_hpa, v, "every hectopascal is exact");
    }

    encode_nothing();
    {
        uint16_t pressure_hpa;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_pressure(&dec, &pressure_hpa), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_humidity(void) {
    TEST("HUMIDITY: round trip, boundaries, range, absence");

    /* Seven bits hold 0..100 with room to spare and no quantisation, so exhaustive and exact. */
    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_humidity(&enc, IOTDATA_HUMIDITY_MAX + 1), IOTDATA_ERR_HUMIDITY_HIGH, "above");

    for (uint8_t v = 0; v <= IOTDATA_HUMIDITY_MAX; v++) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_humidity(&enc, v), "sweep encode");
        finish();
        decode_pkt();
        uint8_t humidity_pct;
        rewind_dec();
        ASSERT_OK(iotdata_decode_humidity(&dec, &humidity_pct), "sweep decode");
        ASSERT_EQ_U(humidity_pct, v, "every percent is exact");
    }

    encode_nothing();
    {
        uint8_t humidity_pct;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_humidity(&dec, &humidity_pct), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_environment(void) {
    TEST("ENVIRONMENT: round trip, range, absence, and independence from its parts");

    const iotdata_float_t t = (iotdata_float_t)(IOTDATA_TEMPERATURE_MIN + 100 * IOTDATA_TEMPERATURE_RES);
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_environment(&enc, t, 1013, 48), "encode");
    finish();
    decode_pkt();
    {
        iotdata_float_t temp_c;
        uint16_t pressure_hpa;
        uint8_t humidity_pct;
        rewind_dec();
        ASSERT_OK(iotdata_decode_environment(&dec, &temp_c, &pressure_hpa, &humidity_pct), "decode");
        ASSERT_NEAR(temp_c, t, IOTDATA_TEMPERATURE_RES, "temperature");
        ASSERT_EQ_U(pressure_hpa, 1013, "pressure");
        ASSERT_EQ_U(humidity_pct, 48, "humidity");
    }

    /* A COMPOSITE RAISES ITS PARTS' ERRORS, not errors of its own -- it has no error codes. */
    begin(0, 1, 2);
    ASSERT_ERR(iotdata_encode_environment(&enc, (iotdata_float_t)(IOTDATA_TEMPERATURE_MAX + IOTDATA_TEMPERATURE_RES), 1013, 48), IOTDATA_ERR_TEMPERATURE_HIGH, "temperature range");
    ASSERT_ERR(iotdata_encode_environment(&enc, t, IOTDATA_PRESSURE_MAX + 1, 48), IOTDATA_ERR_PRESSURE_HIGH, "pressure range");
    ASSERT_ERR(iotdata_encode_environment(&enc, t, 1013, IOTDATA_HUMIDITY_MAX + 1), IOTDATA_ERR_HUMIDITY_HIGH, "humidity range");

    /* ITS STORAGE IS ITS OWN. A variant carrying both the composite and a standalone part must
       keep two separate values -- the bug that per-field structs were introduced to fix. */
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_humidity(&enc, 20), "standalone humidity");
    ASSERT_OK(iotdata_encode_environment(&enc, t, 1000, 80), "environment humidity");
    finish();
    decode_pkt();
    {
        uint8_t humidity_pct;
        rewind_dec();
        ASSERT_OK(iotdata_decode_humidity(&dec, &humidity_pct), "decode standalone");
        ASSERT_EQ_U(humidity_pct, 20, "standalone kept its own");
        rewind_dec();
        ASSERT_OK(iotdata_decode_environment(&dec, NULL, NULL, &humidity_pct), "decode composite");
        ASSERT_EQ_U(humidity_pct, 80, "composite kept its own");
    }

    encode_nothing();
    {
        iotdata_float_t temp_c;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_environment(&dec, &temp_c, NULL, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_wind_speed(void) {
    TEST("WIND_SPEED: round trip, boundaries, range, absence");

    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_wind_speed(&enc, (iotdata_float_t)(IOTDATA_WIND_SPEED_MAX + IOTDATA_WIND_SPEED_RES)), IOTDATA_ERR_WIND_SPEED_HIGH, "above");

    for (int k = 0; (iotdata_float_t)k * IOTDATA_WIND_SPEED_RES <= IOTDATA_WIND_SPEED_MAX; k++) {
        const iotdata_float_t want = (iotdata_float_t)k * IOTDATA_WIND_SPEED_RES;
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_wind_speed(&enc, want), "sweep encode");
        finish();
        decode_pkt();
        iotdata_float_t speed_ms;
        rewind_dec();
        ASSERT_OK(iotdata_decode_wind_speed(&dec, &speed_ms), "sweep decode");
        ASSERT_NEAR(speed_ms, want, IOTDATA_WIND_SPEED_RES, "every step survives");
    }

    encode_nothing();
    {
        iotdata_float_t speed_ms;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_wind_speed(&dec, &speed_ms), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_wind_direction(void) {
    TEST("WIND_DIRECTION: round trip, boundaries, range, absence");

    /* 0..359 degrees in eight bits, so this field is NOT exact: 360 bearings do not fit 256
       codes. Two degrees is the worst the round trip can cost. It is also the field whose two
       build forms are different formulas rather than one at two widths, so a sweep across the
       whole circle is the thing that would catch one of them being wrong. */
    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_wind_direction(&enc, IOTDATA_WIND_DIRECTION_MAX + 1), IOTDATA_ERR_WIND_DIRECTION_HIGH, "above");

    for (uint16_t v = 0; v <= IOTDATA_WIND_DIRECTION_MAX; v++) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_wind_direction(&enc, v), "sweep encode");
        finish();
        decode_pkt();
        uint16_t direction_deg;
        rewind_dec();
        ASSERT_OK(iotdata_decode_wind_direction(&dec, &direction_deg), "sweep decode");
        ASSERT_NEAR(direction_deg, v, 2.0, "every bearing within two degrees");
        ASSERT_TRUE(direction_deg <= IOTDATA_WIND_DIRECTION_MAX, "and never off the compass");
    }

    encode_nothing();
    {
        uint16_t direction_deg;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_wind_direction(&dec, &direction_deg), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_wind_gust(void) {
    TEST("WIND_GUST: round trip, boundaries, range, absence");

    /* Quantises exactly as the speed does, and since the normalisation it says so in its OWN
       functions and constants rather than calling into WIND_SPEED's. */
    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_wind_gust(&enc, (iotdata_float_t)(IOTDATA_WIND_GUST_MAX + IOTDATA_WIND_GUST_RES)), IOTDATA_ERR_WIND_GUST_HIGH, "above");

    for (int k = 0; (iotdata_float_t)k * IOTDATA_WIND_GUST_RES <= IOTDATA_WIND_GUST_MAX; k++) {
        const iotdata_float_t want = (iotdata_float_t)k * IOTDATA_WIND_GUST_RES;
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_wind_gust(&enc, want), "sweep encode");
        finish();
        decode_pkt();
        iotdata_float_t gust_ms;
        rewind_dec();
        ASSERT_OK(iotdata_decode_wind_gust(&dec, &gust_ms), "sweep decode");
        ASSERT_NEAR(gust_ms, want, IOTDATA_WIND_GUST_RES, "every step survives");
    }

    encode_nothing();
    {
        iotdata_float_t gust_ms;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_wind_gust(&dec, &gust_ms), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_wind(void) {
    TEST("WIND: round trip, range, absence, and independence from its parts");

    const iotdata_float_t sp = (iotdata_float_t)(20 * IOTDATA_WIND_SPEED_RES);
    const iotdata_float_t gu = (iotdata_float_t)(60 * IOTDATA_WIND_GUST_RES);
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_wind(&enc, sp, 135, gu), "encode");
    finish();
    decode_pkt();
    {
        iotdata_float_t speed_ms, gust_ms;
        uint16_t direction_deg;
        rewind_dec();
        ASSERT_OK(iotdata_decode_wind(&dec, &speed_ms, &direction_deg, &gust_ms), "decode");
        ASSERT_NEAR(speed_ms, sp, IOTDATA_WIND_SPEED_RES, "speed");
        ASSERT_NEAR(direction_deg, 135, 2.0, "direction");
        ASSERT_NEAR(gust_ms, gu, IOTDATA_WIND_GUST_RES, "gust");
    }

    begin(0, 1, 2);
    ASSERT_ERR(iotdata_encode_wind(&enc, (iotdata_float_t)(IOTDATA_WIND_SPEED_MAX + IOTDATA_WIND_SPEED_RES), 135, gu), IOTDATA_ERR_WIND_SPEED_HIGH, "speed range");
    ASSERT_ERR(iotdata_encode_wind(&enc, sp, IOTDATA_WIND_DIRECTION_MAX + 1, gu), IOTDATA_ERR_WIND_DIRECTION_HIGH, "direction range");
    ASSERT_ERR(iotdata_encode_wind(&enc, sp, 135, (iotdata_float_t)(IOTDATA_WIND_GUST_MAX + IOTDATA_WIND_GUST_RES)), IOTDATA_ERR_WIND_GUST_HIGH, "gust range");

    /* The composite and a standalone part keep separate storage. */
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_wind_speed(&enc, (iotdata_float_t)(4 * IOTDATA_WIND_SPEED_RES)), "standalone speed");
    ASSERT_OK(iotdata_encode_wind(&enc, sp, 135, gu), "composite speed");
    finish();
    decode_pkt();
    {
        iotdata_float_t speed_ms;
        rewind_dec();
        ASSERT_OK(iotdata_decode_wind_speed(&dec, &speed_ms), "decode standalone");
        ASSERT_NEAR(speed_ms, (iotdata_float_t)(4 * IOTDATA_WIND_SPEED_RES), IOTDATA_WIND_SPEED_RES, "standalone kept its own");
        rewind_dec();
        ASSERT_OK(iotdata_decode_wind(&dec, &speed_ms, NULL, NULL), "decode composite");
        ASSERT_NEAR(speed_ms, sp, IOTDATA_WIND_SPEED_RES, "composite kept its own");
    }

    encode_nothing();
    {
        iotdata_float_t speed_ms;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_wind(&dec, &speed_ms, NULL, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_rain_rate(void) {
    TEST("RAIN_RATE: round trip, the whole range, absence");

    /* THE ONE FIELD WITH NOTHING TO REJECT. Eight bits carry a uint8_t exactly, so every value
       the parameter can hold is admissible and the sweep is the entire domain. The dead
       IOTDATA_ERR_RAIN_RATE_HIGH was dropped when this field was migrated -- it was in the enum
       with a message and no way to raise it, since a uint8_t cannot exceed 255. */
    for (unsigned v = 0; v <= IOTDATA_RAIN_RATE_MAX; v++) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_rain_rate(&enc, (uint8_t)v), "sweep encode");
        finish();
        decode_pkt();
        uint8_t rate_mmhr;
        rewind_dec();
        ASSERT_OK(iotdata_decode_rain_rate(&dec, &rate_mmhr), "sweep decode");
        ASSERT_EQ_U(rate_mmhr, v, "every millimetre per hour is exact");
    }

    encode_nothing();
    {
        uint8_t rate_mmhr;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_rain_rate(&dec, &rate_mmhr), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_rain_size(void) {
    TEST("RAIN_SIZE: round trip, boundaries, range, absence");

    /* The member carries TENTHS of a millimetre and the wire carries quarters of that, so the
       admissible span is MAX * SCALE and a value lands exactly when it is a multiple of SCALE. */
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_rain_size(&enc, 20), "encode 2.0 mm/d");
    finish();
    decode_pkt();
    {
        uint8_t size10_mmd;
        rewind_dec();
        ASSERT_OK(iotdata_decode_rain_size(&dec, &size10_mmd), "decode");
        ASSERT_NEAR(size10_mmd, 20, IOTDATA_RAIN_SIZE_SCALE, "within one step");
    }

    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_rain_size(&enc, IOTDATA_RAIN_SIZE_MAX * IOTDATA_RAIN_SIZE_SCALE + 1), IOTDATA_ERR_RAIN_SIZE_HIGH, "above");

    for (unsigned v = 0; v <= IOTDATA_RAIN_SIZE_MAX * IOTDATA_RAIN_SIZE_SCALE; v++) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_rain_size(&enc, (uint8_t)v), "sweep encode");
        finish();
        decode_pkt();
        uint8_t size10_mmd;
        rewind_dec();
        ASSERT_OK(iotdata_decode_rain_size(&dec, &size10_mmd), "sweep decode");
        ASSERT_NEAR(size10_mmd, v, IOTDATA_RAIN_SIZE_SCALE, "every step survives");
    }

    encode_nothing();
    {
        uint8_t size10_mmd;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_rain_size(&dec, &size10_mmd), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_rain(void) {
    TEST("RAIN: round trip, range, absence, and independence from its parts");

    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_rain(&enc, 12, 20), "encode");
    finish();
    decode_pkt();
    {
        uint8_t rate_mmhr, size10_mmd;
        rewind_dec();
        ASSERT_OK(iotdata_decode_rain(&dec, &rate_mmhr, &size10_mmd), "decode");
        ASSERT_EQ_U(rate_mmhr, 12, "rate");
        ASSERT_NEAR(size10_mmd, 20, IOTDATA_RAIN_SIZE_SCALE, "size");
    }

    /* Only the size has a range to violate; the rate cannot be out of range. */
    begin(0, 1, 2);
    ASSERT_ERR(iotdata_encode_rain(&enc, 12, IOTDATA_RAIN_SIZE_MAX * IOTDATA_RAIN_SIZE_SCALE + 1), IOTDATA_ERR_RAIN_SIZE_HIGH, "size range");

    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_rain_rate(&enc, 3), "standalone rate");
    ASSERT_OK(iotdata_encode_rain(&enc, 40, 20), "composite rate");
    finish();
    decode_pkt();
    {
        uint8_t rate_mmhr;
        rewind_dec();
        ASSERT_OK(iotdata_decode_rain_rate(&dec, &rate_mmhr), "decode standalone");
        ASSERT_EQ_U(rate_mmhr, 3, "standalone kept its own");
        rewind_dec();
        ASSERT_OK(iotdata_decode_rain(&dec, &rate_mmhr, NULL), "decode composite");
        ASSERT_EQ_U(rate_mmhr, 40, "composite kept its own");
    }

    encode_nothing();
    {
        uint8_t rate_mmhr;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_rain(&dec, &rate_mmhr, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_radiation_cpm(void) {
    TEST("RADIATION_CPM: round trip, boundaries, range, absence");

    /* Fourteen bits, so the ceiling is 16383 and NOT the 65535 its error message used to claim.
       The parameter is a uint16_t, which means a caller CAN hand over a value the wire cannot
       hold -- the check is the only thing between that and a silently truncated count. */
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_radiation_cpm(&enc, 1234), "encode");
    finish();
    decode_pkt();
    {
        uint16_t cpm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_radiation_cpm(&dec, &cpm), "decode");
        ASSERT_EQ_U(cpm, 1234, "exact");
    }

    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_radiation_cpm(&enc, IOTDATA_RADIATION_CPM_MAX), "encode the maximum");
    finish();
    decode_pkt();
    {
        uint16_t cpm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_radiation_cpm(&dec, &cpm), "decode the maximum");
        ASSERT_EQ_U(cpm, IOTDATA_RADIATION_CPM_MAX, "maximum");
    }

    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_radiation_cpm(&enc, IOTDATA_RADIATION_CPM_MAX + 1), IOTDATA_ERR_RADIATION_CPM_HIGH, "above");
    ASSERT_ERR(iotdata_encode_radiation_cpm(&enc, 65535), IOTDATA_ERR_RADIATION_CPM_HIGH, "and the old message's ceiling is rejected too");

    for (uint16_t v = 0; v <= IOTDATA_RADIATION_CPM_MAX; v = (uint16_t)(v + 37)) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_radiation_cpm(&enc, v), "sweep encode");
        finish();
        decode_pkt();
        uint16_t cpm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_radiation_cpm(&dec, &cpm), "sweep decode");
        ASSERT_EQ_U(cpm, v, "every count is exact");
    }

    encode_nothing();
    {
        uint16_t cpm;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_radiation_cpm(&dec, &cpm), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_radiation_dose(void) {
    TEST("RADIATION_DOSE: round trip, boundaries, range, absence");

    const iotdata_float_t mid = (iotdata_float_t)(500 * IOTDATA_RADIATION_DOSE_RES);
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_radiation_dose(&enc, mid), "encode");
    finish();
    decode_pkt();
    {
        iotdata_float_t usvh;
        rewind_dec();
        ASSERT_OK(iotdata_decode_radiation_dose(&dec, &usvh), "decode");
        ASSERT_NEAR(usvh, mid, IOTDATA_RADIATION_DOSE_RES, "within one step");
    }

    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_radiation_dose(&enc, IOTDATA_RADIATION_DOSE_MAX), "encode the maximum");
    finish();
    decode_pkt();
    {
        iotdata_float_t usvh;
        rewind_dec();
        ASSERT_OK(iotdata_decode_radiation_dose(&dec, &usvh), "decode the maximum");
        ASSERT_NEAR(usvh, IOTDATA_RADIATION_DOSE_MAX, IOTDATA_RADIATION_DOSE_RES, "maximum");
    }

    /* The only quantity here that rejects a NEGATIVE value, so both arms of the check are worth
       firing -- a dose rate below zero is not a reading, it is a broken sensor. */
    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_radiation_dose(&enc, (iotdata_float_t)(-IOTDATA_RADIATION_DOSE_RES)), IOTDATA_ERR_RADIATION_DOSE_HIGH, "below zero");
    ASSERT_ERR(iotdata_encode_radiation_dose(&enc, (iotdata_float_t)(IOTDATA_RADIATION_DOSE_MAX + IOTDATA_RADIATION_DOSE_RES)), IOTDATA_ERR_RADIATION_DOSE_HIGH, "above");

    for (int k = 0; (iotdata_float_t)k * IOTDATA_RADIATION_DOSE_RES <= IOTDATA_RADIATION_DOSE_MAX; k += 29) {
        const iotdata_float_t want = (iotdata_float_t)k * IOTDATA_RADIATION_DOSE_RES;
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_radiation_dose(&enc, want), "sweep encode");
        finish();
        decode_pkt();
        iotdata_float_t usvh;
        rewind_dec();
        ASSERT_OK(iotdata_decode_radiation_dose(&dec, &usvh), "sweep decode");
        ASSERT_NEAR(usvh, want, IOTDATA_RADIATION_DOSE_RES, "every step survives");
    }

    encode_nothing();
    {
        iotdata_float_t usvh;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_radiation_dose(&dec, &usvh), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_radiation(void) {
    TEST("RADIATION: round trip, range, absence, and independence from its parts");

    const iotdata_float_t dose = (iotdata_float_t)(250 * IOTDATA_RADIATION_DOSE_RES);
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_radiation(&enc, 900, dose), "encode");
    finish();
    decode_pkt();
    {
        uint16_t cpm;
        iotdata_float_t usvh;
        rewind_dec();
        ASSERT_OK(iotdata_decode_radiation(&dec, &cpm, &usvh), "decode");
        ASSERT_EQ_U(cpm, 900, "cpm");
        ASSERT_NEAR(usvh, dose, IOTDATA_RADIATION_DOSE_RES, "dose");
    }

    /* THE CHECK THIS COMPOSITE DID NOT HAVE. Before RADIATION was migrated it validated the dose
       and not the count, so a cpm above the fourteen-bit ceiling went onto the wire truncated:
       20000 arrived as 3616, silently, while the standalone setter rejected the same value. The
       specification gives a composite its parts' checks, which closed the hole. */
    begin(0, 1, 2);
    ASSERT_ERR(iotdata_encode_radiation(&enc, IOTDATA_RADIATION_CPM_MAX + 1, dose), IOTDATA_ERR_RADIATION_CPM_HIGH, "cpm range");
    ASSERT_ERR(iotdata_encode_radiation(&enc, 20000, dose), IOTDATA_ERR_RADIATION_CPM_HIGH, "the value that used to truncate to 3616");
    ASSERT_ERR(iotdata_encode_radiation(&enc, 900, (iotdata_float_t)(IOTDATA_RADIATION_DOSE_MAX + IOTDATA_RADIATION_DOSE_RES)), IOTDATA_ERR_RADIATION_DOSE_HIGH, "dose range");

    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_radiation_cpm(&enc, 7), "standalone cpm");
    ASSERT_OK(iotdata_encode_radiation(&enc, 900, dose), "composite cpm");
    finish();
    decode_pkt();
    {
        uint16_t cpm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_radiation_cpm(&dec, &cpm), "decode standalone");
        ASSERT_EQ_U(cpm, 7, "standalone kept its own");
        rewind_dec();
        ASSERT_OK(iotdata_decode_radiation(&dec, &cpm, NULL), "decode composite");
        ASSERT_EQ_U(cpm, 900, "composite kept its own");
    }

    encode_nothing();
    {
        uint16_t cpm;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_radiation(&dec, &cpm, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_air_quality_index(void) {
    TEST("AIR_QUALITY_INDEX: round trip, boundaries, range, absence");

    begin(0, 1, 4);
    ASSERT_ERR(iotdata_encode_air_quality_index(&enc, IOTDATA_AIR_QUALITY_INDEX_MAX + 1), IOTDATA_ERR_AIR_QUALITY_INDEX_HIGH, "above");

    /* Nine bits hold 0..500 with room to spare and nothing is quantised, so the sweep is the
       whole domain and every value is exact. */
    for (uint16_t v = 0; v <= IOTDATA_AIR_QUALITY_INDEX_MAX; v++) {
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_air_quality_index(&enc, v), "sweep encode");
        finish();
        decode_pkt();
        uint16_t aq_index;
        rewind_dec();
        ASSERT_OK(iotdata_decode_air_quality_index(&dec, &aq_index), "sweep decode");
        ASSERT_EQ_U(aq_index, v, "every index is exact");
    }

    encode_nothing();
    {
        uint16_t aq_index;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_air_quality_index(&dec, &aq_index), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_air_quality_pm(void) {
    TEST("AIR_QUALITY_PM: presence mask, round trip, range, absence");

    /* THE FIRST FIELD WHOSE SHAPE IS A MASK AND A VECTOR, so the questions are different: which
       channels came back, what the absent ones read as, and whether a mask bit for a channel
       that does not exist can get onto the wire. */
    const uint16_t pm_in[IOTDATA_AIR_QUALITY_PM_COUNT] = { 10, 25, 40, 100 };
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x0F, pm_in), "encode all four channels");
    finish();
    decode_pkt();
    {
        uint8_t present;
        uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT];
        rewind_dec();
        ASSERT_OK(iotdata_decode_air_quality_pm(&dec, &present, pm), "decode");
        ASSERT_EQ_U(present, 0x0F, "all four present");
        for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
            ASSERT_NEAR(pm[i], pm_in[i], IOTDATA_AIR_QUALITY_PM_VALUE_RES, "each channel within one step");
    }

    /* A PARTIAL MASK: the absent channels must read as zero, not as whatever was in the
       encoder, and the present ones must still land in their own positions. */
    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x05, pm_in), "encode channels 0 and 2");
    finish();
    decode_pkt();
    {
        uint8_t present;
        uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT];
        rewind_dec();
        ASSERT_OK(iotdata_decode_air_quality_pm(&dec, &present, pm), "decode");
        ASSERT_EQ_U(present, 0x05, "only 0 and 2 present");
        ASSERT_NEAR(pm[0], pm_in[0], IOTDATA_AIR_QUALITY_PM_VALUE_RES, "channel 0 survived");
        ASSERT_EQ_U(pm[1], 0, "channel 1 absent reads zero");
        ASSERT_NEAR(pm[2], pm_in[2], IOTDATA_AIR_QUALITY_PM_VALUE_RES, "channel 2 survived");
        ASSERT_EQ_U(pm[3], 0, "channel 3 absent reads zero");
    }

    /* THE MASK IS MASKED. Only four channels exist, so a fifth bit must not reach the wire --
       the setter ands it away rather than rejecting it. */
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0xFF, pm_in), "a mask with bits for channels that do not exist");
    finish();
    decode_pkt();
    {
        uint8_t present;
        rewind_dec();
        ASSERT_OK(iotdata_decode_air_quality_pm(&dec, &present, NULL), "decode");
        ASSERT_EQ_U(present, 0x0F, "the spare bits never made it onto the wire");
    }

    /* A value is only checked where its channel is PRESENT: an out-of-range reading in a
       channel nobody claimed is not an error, because it never goes on the wire. */
    {
        uint16_t bad[IOTDATA_AIR_QUALITY_PM_COUNT] = { 0, 0, 0, 0 };
        bad[2] = IOTDATA_AIR_QUALITY_PM_VALUE_MAX + 1;
        begin(0, 1, 4);
        ASSERT_ERR(iotdata_encode_air_quality_pm(&enc, 0x04, bad), IOTDATA_ERR_AIR_QUALITY_PM_VALUE_HIGH, "present and out of range");
        ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x03, bad), "absent and out of range is not an error");
    }

    /* Sweep the value range on one channel. */
    for (unsigned v = 0; v <= IOTDATA_AIR_QUALITY_PM_VALUE_MAX; v += 7) {
        uint16_t one[IOTDATA_AIR_QUALITY_PM_COUNT] = { 0, 0, 0, 0 };
        one[1] = (uint16_t)v;
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x02, one), "sweep encode");
        finish();
        decode_pkt();
        uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT];
        rewind_dec();
        ASSERT_OK(iotdata_decode_air_quality_pm(&dec, NULL, pm), "sweep decode");
        ASSERT_NEAR(pm[1], v, IOTDATA_AIR_QUALITY_PM_VALUE_RES, "every step survives");
    }

    encode_nothing();
    {
        uint8_t present;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_air_quality_pm(&dec, &present, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_air_quality_gas(void) {
    TEST("AIR_QUALITY_GAS: per-slot widths, presence mask, range, absence");

    /* EIGHT SLOTS, EACH WITH ITS OWN WIDTH, RESOLUTION AND CEILING -- the only field in the
       library like it. A value that fits CO2 would overflow VOC, so the ceilings have to be
       checked per slot, which is what makes the table rather than a constant load-bearing. */
    const uint16_t gas_in[IOTDATA_AIR_QUALITY_GAS_COUNT] = {
        100,  /* VOC   res 2,  max 510   */
        200,  /* NOX   res 2,  max 510   */
        1000, /* CO2   res 50, max 51150 */
        50,   /* CO    res 1,  max 1023  */
        500,  /* HCHO  res 5,  max 5115  */
        300,  /* O3    res 1,  max 1023  */
        0,    0,
    };
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_air_quality_gas(&enc, 0x3F, gas_in), "encode the six real slots");
    finish();
    decode_pkt();
    {
        uint8_t present;
        uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT];
        rewind_dec();
        ASSERT_OK(iotdata_decode_air_quality_gas(&dec, &present, gas), "decode");
        ASSERT_EQ_U(present, 0x3F, "six present");
        ASSERT_NEAR(gas[IOTDATA_AIR_QUALITY_GAS_INDEX_VOC], gas_in[0], IOTDATA_AIR_QUALITY_GAS_RES_VOC, "VOC");
        ASSERT_NEAR(gas[IOTDATA_AIR_QUALITY_GAS_INDEX_CO2], gas_in[2], IOTDATA_AIR_QUALITY_GAS_RES_CO2, "CO2");
        ASSERT_NEAR(gas[IOTDATA_AIR_QUALITY_GAS_INDEX_CO], gas_in[3], IOTDATA_AIR_QUALITY_GAS_RES_CO, "CO");
        ASSERT_NEAR(gas[IOTDATA_AIR_QUALITY_GAS_INDEX_HCHO], gas_in[4], IOTDATA_AIR_QUALITY_GAS_RES_HCHO, "HCHO");
        ASSERT_EQ_U(gas[IOTDATA_AIR_QUALITY_GAS_INDEX_RSVD6], 0, "an absent slot reads zero");
    }

    /* EVERY SLOT'S OWN CEILING, one at a time: the value just past each slot's maximum must be
       rejected FOR THAT SLOT, which a single shared ceiling could not express. */
    {
        const uint16_t maxima[IOTDATA_AIR_QUALITY_GAS_COUNT] = {
            IOTDATA_AIR_QUALITY_GAS_MAX_VOC,  IOTDATA_AIR_QUALITY_GAS_MAX_NOX, IOTDATA_AIR_QUALITY_GAS_MAX_CO2,   IOTDATA_AIR_QUALITY_GAS_MAX_CO,
            IOTDATA_AIR_QUALITY_GAS_MAX_HCHO, IOTDATA_AIR_QUALITY_GAS_MAX_O3,  IOTDATA_AIR_QUALITY_GAS_MAX_RSVD6, IOTDATA_AIR_QUALITY_GAS_MAX_RSVD7,
        };
        for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++) {
            uint16_t g[IOTDATA_AIR_QUALITY_GAS_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0 };
            /* a fresh packet for each: an accepted encode FILLS the slot, so a second one in
               the same packet fails for having been added rather than for its value */
            g[i] = maxima[i];
            begin(0, 1, 4);
            ASSERT_OK(iotdata_encode_air_quality_gas(&enc, (uint8_t)(1U << i), g), "its own maximum is allowed");
            g[i] = (uint16_t)(maxima[i] + 1);
            begin(0, 1, 4);
            ASSERT_ERR(iotdata_encode_air_quality_gas(&enc, (uint8_t)(1U << i), g), IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH, "one past it is not");
        }
    }

    /* And the point of per-slot ceilings: a value legal in one slot is illegal in another. */
    {
        uint16_t g[IOTDATA_AIR_QUALITY_GAS_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        g[IOTDATA_AIR_QUALITY_GAS_INDEX_CO2] = 20000;
        begin(0, 1, 4);
        ASSERT_OK(iotdata_encode_air_quality_gas(&enc, (uint8_t)(1U << IOTDATA_AIR_QUALITY_GAS_INDEX_CO2), g), "20000 fits CO2");
        g[IOTDATA_AIR_QUALITY_GAS_INDEX_VOC] = 20000;
        begin(0, 1, 4);
        ASSERT_ERR(iotdata_encode_air_quality_gas(&enc, (uint8_t)(1U << IOTDATA_AIR_QUALITY_GAS_INDEX_VOC), g), IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH, "and not VOC");
    }

    encode_nothing();
    {
        uint8_t present;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_air_quality_gas(&dec, &present, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_air_quality(void) {
    TEST("AIR_QUALITY: round trip, range, absence, and independence from its parts");

    const uint16_t pm_in[IOTDATA_AIR_QUALITY_PM_COUNT] = { 10, 25, 40, 100 };
    const uint16_t gas_in[IOTDATA_AIR_QUALITY_GAS_COUNT] = { 100, 200, 1000, 50, 500, 300, 0, 0 };
    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_air_quality(&enc, 150, 0x0F, pm_in, 0x3F, gas_in), "encode");
    finish();
    decode_pkt();
    {
        uint16_t aq_index;
        uint8_t pm_present, gas_present;
        uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], gas[IOTDATA_AIR_QUALITY_GAS_COUNT];
        rewind_dec();
        ASSERT_OK(iotdata_decode_air_quality(&dec, &aq_index, &pm_present, pm, &gas_present, gas), "decode");
        ASSERT_EQ_U(aq_index, 150, "index");
        ASSERT_EQ_U(pm_present, 0x0F, "pm mask");
        ASSERT_EQ_U(gas_present, 0x3F, "gas mask");
        ASSERT_NEAR(pm[0], pm_in[0], IOTDATA_AIR_QUALITY_PM_VALUE_RES, "a pm channel");
        ASSERT_NEAR(gas[IOTDATA_AIR_QUALITY_GAS_INDEX_CO2], gas_in[2], IOTDATA_AIR_QUALITY_GAS_RES_CO2, "a gas slot");
    }

    /* THE COMPOSITE RAISES ITS PARTS' ERRORS, all three of them. */
    begin(0, 1, 2);
    ASSERT_ERR(iotdata_encode_air_quality(&enc, IOTDATA_AIR_QUALITY_INDEX_MAX + 1, 0x0F, pm_in, 0x3F, gas_in), IOTDATA_ERR_AIR_QUALITY_INDEX_HIGH, "index range");
    {
        uint16_t bad_pm[IOTDATA_AIR_QUALITY_PM_COUNT] = { 0, 0, 0, 0 };
        bad_pm[0] = IOTDATA_AIR_QUALITY_PM_VALUE_MAX + 1;
        ASSERT_ERR(iotdata_encode_air_quality(&enc, 150, 0x01, bad_pm, 0x3F, gas_in), IOTDATA_ERR_AIR_QUALITY_PM_VALUE_HIGH, "pm range");
        uint16_t bad_gas[IOTDATA_AIR_QUALITY_GAS_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        bad_gas[IOTDATA_AIR_QUALITY_GAS_INDEX_VOC] = IOTDATA_AIR_QUALITY_GAS_MAX_VOC + 1;
        ASSERT_ERR(iotdata_encode_air_quality(&enc, 150, 0x0F, pm_in, 0x01, bad_gas), IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH, "gas range");
    }

    /* Its storage is its own, as for every composite. */
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_air_quality_index(&enc, 7), "standalone index");
    ASSERT_OK(iotdata_encode_air_quality(&enc, 150, 0x0F, pm_in, 0x3F, gas_in), "composite index");
    finish();
    decode_pkt();
    {
        uint16_t aq_index;
        rewind_dec();
        ASSERT_OK(iotdata_decode_air_quality_index(&dec, &aq_index), "decode standalone");
        ASSERT_EQ_U(aq_index, 7, "standalone kept its own");
        rewind_dec();
        ASSERT_OK(iotdata_decode_air_quality(&dec, &aq_index, NULL, NULL, NULL, NULL), "decode composite");
        ASSERT_EQ_U(aq_index, 150, "composite kept its own");
    }

    encode_nothing();
    {
        uint16_t aq_index;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_air_quality(&dec, &aq_index, NULL, NULL, NULL, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_position(void) {
    TEST("POSITION: round trip, boundaries, range, absence");

    /* THE ONLY FIELD IN DOUBLES, and the only one whose two build forms are different FORMULAS
       rather than one at two widths: degrees scaled across a 24-bit span in a floating build,
       ten-millionths of a degree through int64_t arithmetic in an integer one. Everything here
       is written in the field's own constants so it reads the same either way.

       24 bits over 180 degrees is about 10.7 microdegrees per step, so the tolerance is one
       step of the field's own scale rather than a round number. */
#if defined(IOTDATA_NO_FLOATING)
    /* THE QUANTITY IS TEN-MILLIONTHS OF A DEGREE HERE, so a step is a whole number of them and
       the RANGE constants to divide are the _I ones -- dividing the degree-valued 180.0 in
       integer arithmetic gives zero, which would quietly turn every tolerance below into a
       demand for exactness. */
    const iotdata_double_t lat_step = (iotdata_double_t)(IOTDATA_POSITION_LAT_RANGE_I / IOTDATA_POSITION_SCALE + 1);
    const iotdata_double_t lon_step = (iotdata_double_t)(IOTDATA_POSITION_LON_RANGE_I / IOTDATA_POSITION_SCALE + 1);
#else
    const iotdata_double_t lat_step = (iotdata_double_t)IOTDATA_POSITION_LAT_RANGE / (iotdata_double_t)IOTDATA_POSITION_SCALE;
    const iotdata_double_t lon_step = (iotdata_double_t)IOTDATA_POSITION_LON_RANGE / (iotdata_double_t)IOTDATA_POSITION_SCALE;
#endif

    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_position(&enc, (iotdata_double_t)IOTDATA_POSITION_LAT_LOW / 2, (iotdata_double_t)IOTDATA_POSITION_LON_HIGH / 4), "encode");
    finish();
    decode_pkt();
    {
        iotdata_double_t lat, lon;
        rewind_dec();
        ASSERT_OK(iotdata_decode_position(&dec, &lat, &lon), "decode");
        ASSERT_NEAR(lat, (iotdata_double_t)IOTDATA_POSITION_LAT_LOW / 2, lat_step, "latitude within one step");
        ASSERT_NEAR(lon, (iotdata_double_t)IOTDATA_POSITION_LON_HIGH / 4, lon_step, "longitude within one step");
    }

    /* BOTH POLES AND BOTH MERIDIANS, which is where an off-by-one in the scaling shows. */
    begin(0, 1, 2);
    ASSERT_OK(iotdata_encode_position(&enc, (iotdata_double_t)IOTDATA_POSITION_LAT_LOW, (iotdata_double_t)IOTDATA_POSITION_LON_LOW), "encode the low corner");
    finish();
    decode_pkt();
    {
        iotdata_double_t lat, lon;
        rewind_dec();
        ASSERT_OK(iotdata_decode_position(&dec, &lat, &lon), "decode the low corner");
        ASSERT_NEAR(lat, (iotdata_double_t)IOTDATA_POSITION_LAT_LOW, lat_step, "south pole");
        ASSERT_NEAR(lon, (iotdata_double_t)IOTDATA_POSITION_LON_LOW, lon_step, "antimeridian west");
    }
    begin(0, 1, 3);
    ASSERT_OK(iotdata_encode_position(&enc, (iotdata_double_t)IOTDATA_POSITION_LAT_HIGH, (iotdata_double_t)IOTDATA_POSITION_LON_HIGH), "encode the high corner");
    finish();
    decode_pkt();
    {
        iotdata_double_t lat, lon;
        rewind_dec();
        ASSERT_OK(iotdata_decode_position(&dec, &lat, &lon), "decode the high corner");
        ASSERT_NEAR(lat, (iotdata_double_t)IOTDATA_POSITION_LAT_HIGH, lat_step, "north pole");
        ASSERT_NEAR(lon, (iotdata_double_t)IOTDATA_POSITION_LON_HIGH, lon_step, "antimeridian east");
    }

    /* The null island, which is the one coordinate a sign error cannot hide in. */
    begin(0, 1, 4);
    ASSERT_OK(iotdata_encode_position(&enc, 0, 0), "encode zero, zero");
    finish();
    decode_pkt();
    {
        iotdata_double_t lat, lon;
        rewind_dec();
        ASSERT_OK(iotdata_decode_position(&dec, &lat, &lon), "decode zero, zero");
        ASSERT_NEAR(lat, 0, lat_step, "latitude zero");
        ASSERT_NEAR(lon, 0, lon_step, "longitude zero");
    }

    /* ALL FOUR CHECKS, which is every error this field declares. */
    begin(0, 1, 1);
    ASSERT_ERR(iotdata_encode_position(&enc, (iotdata_double_t)IOTDATA_POSITION_LAT_LOW - 1, 0), IOTDATA_ERR_POSITION_LAT_LOW, "below the south pole");
    ASSERT_ERR(iotdata_encode_position(&enc, (iotdata_double_t)IOTDATA_POSITION_LAT_HIGH + 1, 0), IOTDATA_ERR_POSITION_LAT_HIGH, "above the north pole");
    ASSERT_ERR(iotdata_encode_position(&enc, 0, (iotdata_double_t)IOTDATA_POSITION_LON_LOW - 1), IOTDATA_ERR_POSITION_LON_LOW, "west of the antimeridian");
    ASSERT_ERR(iotdata_encode_position(&enc, 0, (iotdata_double_t)IOTDATA_POSITION_LON_HIGH + 1), IOTDATA_ERR_POSITION_LON_HIGH, "east of the antimeridian");

    /* A SWEEP ACROSS BOTH SPANS. The two quantities differ only in offset and range, so a sweep
       that walks them together would miss a lat/lon mix-up; these walk latitude and longitude in
       OPPOSITE directions so that swapping them breaks the test. */
    for (int k = 0; k <= 64; k++) {
#if defined(IOTDATA_NO_FLOATING)
        /* THE ARITHMETIC MUST BE WIDER THAN THE QUANTITY. Longitude's range is 3.6e9 in
           ten-millionths of a degree, so range*k overflows an int32 at the very first step and
           the sweep walks off the planet. The _I constants are long long, which keeps the
           product and the division in a type that holds them; only the result is narrowed. */
        const iotdata_double_t lat_want = (iotdata_double_t)(IOTDATA_POSITION_LAT_LOW + IOTDATA_POSITION_LAT_RANGE_I * k / 64);
        const iotdata_double_t lon_want = (iotdata_double_t)(IOTDATA_POSITION_LON_HIGH - IOTDATA_POSITION_LON_RANGE_I * k / 64);
#else
        /* k cast explicitly: where IOTDATA_NO_FLOATING_DOUBLES narrows this type to a float,
           an implicit int-to-float conversion is a -Wconversion error */
        const iotdata_double_t lat_want = (iotdata_double_t)IOTDATA_POSITION_LAT_LOW + (iotdata_double_t)IOTDATA_POSITION_LAT_RANGE * (iotdata_double_t)k / 64;
        const iotdata_double_t lon_want = (iotdata_double_t)IOTDATA_POSITION_LON_HIGH - (iotdata_double_t)IOTDATA_POSITION_LON_RANGE * (iotdata_double_t)k / 64;
#endif
        begin(0, 1, 1);
        ASSERT_OK(iotdata_encode_position(&enc, lat_want, lon_want), "sweep encode");
        finish();
        decode_pkt();
        iotdata_double_t lat, lon;
        rewind_dec();
        ASSERT_OK(iotdata_decode_position(&dec, &lat, &lon), "sweep decode");
        ASSERT_NEAR(lat, lat_want, lat_step, "latitude survives");
        ASSERT_NEAR(lon, lon_want, lon_step, "longitude survives");
    }

    encode_nothing();
    {
        iotdata_double_t lat;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_position(&dec, &lat, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_image(void) {
    TEST("IMAGE: round trip, every check, absence");

    /* THE ONE FIELD WHOSE CODE IS NOT GENERATED, and therefore the one whose specification entry
       nothing checks -- iotdata-fields-image.xml carries its identifier, quantities and errors
       for the registry, and a compiler never reads it. THIS TEST IS WHAT KEEPS IT HONEST: every
       error name and every constant below comes from that document, so a name that drifts does
       not compile. The first draft of that entry had all five error names wrong.

       It is also the first variable-length field, and the only one whose decoder needs scratch
       from the caller: the decoded pixels have to live somewhere, and the decoder does not own
       254 bytes on behalf of a field that may be absent. */
    static const uint8_t pixels[8] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x23, 0x45, 0x67 };
    iotdata_field_image_scratch_t scratch;

    begin(1, 1, 1);
    ASSERT_OK(iotdata_encode_image(&enc, IOTDATA_IMAGE_FMT_GREY4, IOTDATA_IMAGE_SIZE_32x24, IOTDATA_IMAGE_COMP_RAW, IOTDATA_IMAGE_FLAG_INVERT, pixels, sizeof(pixels)), "encode");
    finish();
    decode_pkt();
    {
        uint8_t fmt, tier, comp, flags, len;
        const uint8_t *data;
        rewind_dec();
        ASSERT_OK(iotdata_decode_image(&dec, &fmt, &tier, &comp, &flags, &data, &len, &scratch), "decode");
        ASSERT_EQ_U(fmt, IOTDATA_IMAGE_FMT_GREY4, "pixel format");
        ASSERT_EQ_U(tier, IOTDATA_IMAGE_SIZE_32x24, "size tier");
        ASSERT_EQ_U(comp, IOTDATA_IMAGE_COMP_RAW, "compression");
        ASSERT_EQ_U(flags, IOTDATA_IMAGE_FLAG_INVERT, "flags");
        ASSERT_EQ_U(len, sizeof(pixels), "length");
        ASSERT_TRUE(data != NULL, "and the pixels came back");
        if (data)
            for (unsigned i = 0; i < sizeof(pixels); i++)
                ASSERT_EQ_U(data[i], pixels[i], "byte for byte");
    }

    /* A ZERO-LENGTH IMAGE is legitimate -- it says "this is what I have, which is nothing" --
       and the packer must not dereference the pointer to discover that. */
    begin(1, 1, 2);
    ASSERT_OK(iotdata_encode_image(&enc, IOTDATA_IMAGE_FMT_BILEVEL, IOTDATA_IMAGE_SIZE_24x18, IOTDATA_IMAGE_COMP_RAW, 0, pixels, 0), "encode an empty image");
    finish();
    decode_pkt();
    {
        uint8_t len = 0xFF;
        rewind_dec();
        ASSERT_OK(iotdata_decode_image(&dec, NULL, NULL, NULL, NULL, NULL, &len, &scratch), "decode it");
        ASSERT_EQ_U(len, 0, "zero bytes of pixels");
    }

    /* THE FULL 254 BYTES, which is the whole point of the length prefix. */
    {
        static uint8_t full[IOTDATA_IMAGE_DATA_MAX];
        for (unsigned i = 0; i < sizeof(full); i++)
            full[i] = (uint8_t)(i * 7 + 1);
        begin(1, 1, 3);
        ASSERT_OK(iotdata_encode_image(&enc, IOTDATA_IMAGE_FMT_GREY16, IOTDATA_IMAGE_SIZE_64x48, IOTDATA_IMAGE_COMP_HEATSHRINK, IOTDATA_IMAGE_FLAG_FRAGMENT, full, (uint8_t)sizeof(full)), "encode the maximum");
        finish();
        decode_pkt();
        uint8_t len;
        const uint8_t *data;
        rewind_dec();
        ASSERT_OK(iotdata_decode_image(&dec, NULL, NULL, NULL, NULL, &data, &len, &scratch), "decode the maximum");
        ASSERT_EQ_U(len, IOTDATA_IMAGE_DATA_MAX, "all 254 bytes");
        if (data)
            ASSERT_EQ_U(data[IOTDATA_IMAGE_DATA_MAX - 1], full[IOTDATA_IMAGE_DATA_MAX - 1], "including the last one");
    }

    /* EVERY ERROR THE FIELD DECLARES -- the names the specification document claims. */
    begin(1, 1, 4);
    ASSERT_ERR(iotdata_encode_image(&enc, IOTDATA_IMAGE_FMT_GREY16 + 1, 0, 0, 0, pixels, 1), IOTDATA_ERR_IMAGE_FORMAT_HIGH, "pixel format above the last format");
    ASSERT_ERR(iotdata_encode_image(&enc, 0, IOTDATA_IMAGE_SIZE_64x48 + 1, 0, 0, pixels, 1), IOTDATA_ERR_IMAGE_SIZE_HIGH, "size tier above the last tier");
    ASSERT_ERR(iotdata_encode_image(&enc, 0, 0, IOTDATA_IMAGE_COMP_HEATSHRINK + 1, 0, pixels, 1), IOTDATA_ERR_IMAGE_COMPRESSION_HIGH, "compression above the last scheme");
    ASSERT_ERR(iotdata_encode_image(&enc, 0, 0, 0, 0, NULL, 1), IOTDATA_ERR_IMAGE_DATA_NULL, "a null pointer with a non-zero length");
    ASSERT_OK(iotdata_encode_image(&enc, 0, 0, 0, 0, NULL, 0), "but a null pointer with no length is fine");

    /* Each format, tier and compression scheme the field names, accepted in turn. */
    for (uint8_t fmt = IOTDATA_IMAGE_FMT_BILEVEL; fmt <= IOTDATA_IMAGE_FMT_GREY16; fmt++)
        for (uint8_t tier = IOTDATA_IMAGE_SIZE_24x18; tier <= IOTDATA_IMAGE_SIZE_64x48; tier++) {
            begin(1, 1, 1);
            ASSERT_OK(iotdata_encode_image(&enc, fmt, tier, IOTDATA_IMAGE_COMP_RAW, 0, pixels, sizeof(pixels)), "sweep encode");
            finish();
            decode_pkt();
            uint8_t gf, gt;
            rewind_dec();
            ASSERT_OK(iotdata_decode_image(&dec, &gf, &gt, NULL, NULL, NULL, NULL, &scratch), "sweep decode");
            ASSERT_EQ_U(gf, fmt, "format survives");
            ASSERT_EQ_U(gt, tier, "tier survives");
        }

    encode_nothing_of(1);
    {
        uint8_t len;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_image(&dec, NULL, NULL, NULL, NULL, NULL, &len, &scratch), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

/* =========================================================================
 * The standalone parts of the three families that used to be bundles
 *
 * Their composites keep the wire format they always had; these are the halves a
 * device can now send alone. Each has six operation bodies that are NEW code --
 * the only code in the generated header with no byte-exact predecessor to diff
 * against -- so this is the only thing standing behind them.
 * ========================================================================= */

static void test_field_battery_level(void) {
    TEST("BATTERY_LEVEL: round trip, the whole range, the check, absence");

    for (unsigned v = 0; v <= IOTDATA_BATTERY_LEVEL_MAX; v++) {
        begin(2, 1, 1);
        ASSERT_OK(iotdata_encode_battery_level(&enc, (uint8_t)v), "sweep encode");
        finish();
        decode_pkt();
        uint8_t level;
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery_level(&dec, &level), "sweep decode");
        /* five bits over 0..100 is about 3.2 per step */
        ASSERT_NEAR(level, v, 4, "every percent within one step");
    }
    begin(2, 1, 2);
    ASSERT_ERR(iotdata_encode_battery_level(&enc, IOTDATA_BATTERY_LEVEL_MAX + 1), IOTDATA_ERR_BATTERY_LEVEL_HIGH, "above 100%");
    encode_nothing_of(2);
    {
        uint8_t level;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_battery_level(&dec, &level), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_battery_charge(void) {
    TEST("BATTERY_CHARGE: both states, absence");

    /* ONE BIT, so the whole domain is two values -- and the reason the family was split: before
       it, saying "charging" meant inventing a state of charge to say it with. */
    for (int k = 0; k < 2; k++) {
        begin(2, 1, 1);
        ASSERT_OK(iotdata_encode_battery_charge(&enc, k != 0), "encode");
        finish();
        decode_pkt();
        bool charging = !(k != 0);
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery_charge(&dec, &charging), "decode");
        ASSERT_EQ_U(charging, (k != 0), "the state survives");
    }
    encode_nothing_of(2);
    {
        bool charging;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_battery_charge(&dec, &charging), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_link_rssi(void) {
    TEST("LINK_RSSI: round trip, both bounds, the range, absence");

    for (int v = IOTDATA_LINK_RSSI_MIN; v <= IOTDATA_LINK_RSSI_MAX; v++) {
        begin(2, 1, 1);
        ASSERT_OK(iotdata_encode_link_rssi(&enc, (int8_t)v), "sweep encode");
        finish();
        decode_pkt();
        int8_t rssi;
        rewind_dec();
        ASSERT_OK(iotdata_decode_link_rssi(&dec, &rssi), "sweep decode");
        ASSERT_NEAR(rssi, v, IOTDATA_LINK_RSSI_STEP, "every dBm within one step");
    }
    begin(2, 1, 2);
    ASSERT_ERR(iotdata_encode_link_rssi(&enc, IOTDATA_LINK_RSSI_MIN - 1), IOTDATA_ERR_LINK_RSSI_LOW, "below the floor");
    ASSERT_ERR(iotdata_encode_link_rssi(&enc, IOTDATA_LINK_RSSI_MAX + 1), IOTDATA_ERR_LINK_RSSI_HIGH, "above the ceiling");
    encode_nothing_of(2);
    {
        int8_t rssi;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_link_rssi(&dec, &rssi), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_link_snr(void) {
    TEST("LINK_SNR: round trip, both bounds, absence");

    /* TWO BITS for a thirty-decibel span: the coarsest quantity in the library, so the
       tolerance is a whole step and the test says so rather than pretending otherwise. */
    const iotdata_float_t step = IOTDATA_LINK_SNR_STEP;
    for (int k = 0; k <= 3; k++) {
        const iotdata_float_t want = (iotdata_float_t)(IOTDATA_LINK_SNR_MIN + step * (iotdata_float_t)k);
        if (want > IOTDATA_LINK_SNR_MAX)
            break;
        begin(2, 1, 1);
        ASSERT_OK(iotdata_encode_link_snr(&enc, want), "sweep encode");
        finish();
        decode_pkt();
        iotdata_float_t snr;
        rewind_dec();
        ASSERT_OK(iotdata_decode_link_snr(&dec, &snr), "sweep decode");
        ASSERT_NEAR(snr, want, step, "every step survives");
    }
    begin(2, 1, 2);
    ASSERT_ERR(iotdata_encode_link_snr(&enc, (iotdata_float_t)(IOTDATA_LINK_SNR_MIN - step)), IOTDATA_ERR_LINK_SNR_LOW, "below the floor");
    ASSERT_ERR(iotdata_encode_link_snr(&enc, (iotdata_float_t)(IOTDATA_LINK_SNR_MAX + step)), IOTDATA_ERR_LINK_SNR_HIGH, "above the ceiling");
    encode_nothing_of(2);
    {
        iotdata_float_t snr;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_link_snr(&dec, &snr), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_solar_irradiance(void) {
    TEST("SOLAR_IRRADIANCE: round trip, the whole range, the check, absence");

    /* Ten bits hold 0..1023 exactly, so nothing is lost and the sweep is the whole domain. */
    for (unsigned v = 0; v <= IOTDATA_SOLAR_IRRADIANCE_MAX; v++) {
        begin(2, 1, 1);
        ASSERT_OK(iotdata_encode_solar_irradiance(&enc, (uint16_t)v), "sweep encode");
        finish();
        decode_pkt();
        uint16_t wm2;
        rewind_dec();
        ASSERT_OK(iotdata_decode_solar_irradiance(&dec, &wm2), "sweep decode");
        ASSERT_EQ_U(wm2, v, "every W/m2 is exact");
    }
    begin(2, 1, 2);
    ASSERT_ERR(iotdata_encode_solar_irradiance(&enc, IOTDATA_SOLAR_IRRADIANCE_MAX + 1), IOTDATA_ERR_SOLAR_IRRADIANCE_HIGH, "above the ceiling");
    encode_nothing_of(2);
    {
        uint16_t wm2;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_solar_irradiance(&dec, &wm2), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

static void test_field_solar_ultraviolet(void) {
    TEST("SOLAR_ULTRAVIOLET: round trip, the whole range, the check, absence");

    for (unsigned v = 0; v <= IOTDATA_SOLAR_ULTRAVIOLET_MAX; v++) {
        begin(2, 1, 1);
        ASSERT_OK(iotdata_encode_solar_ultraviolet(&enc, (uint8_t)v), "sweep encode");
        finish();
        decode_pkt();
        uint8_t uv;
        rewind_dec();
        ASSERT_OK(iotdata_decode_solar_ultraviolet(&dec, &uv), "sweep decode");
        ASSERT_EQ_U(uv, v, "every index is exact");
    }
    begin(2, 1, 2);
    ASSERT_ERR(iotdata_encode_solar_ultraviolet(&enc, IOTDATA_SOLAR_ULTRAVIOLET_MAX + 1), IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH, "above the ceiling");
    encode_nothing_of(2);
    {
        uint8_t uv;
        rewind_dec();
        ASSERT_ERR(iotdata_decode_solar_ultraviolet(&dec, &uv), IOTDATA_ERR_DECODE_FIELD_ABSENT, "absent");
    }
    PASS();
}

/* A SPLIT FAMILY MUST NOT SHARE STORAGE WITH ITS COMPOSITE, exactly as the older families do
   not: a packet may carry both, and each must keep its own value. */
static void test_split_families_independent(void) {
    TEST("A split family's parts and its composite keep separate storage");

    begin(0, 1, 1);
    ASSERT_OK(iotdata_encode_battery(&enc, 80, true), "composite battery");
    ASSERT_OK(iotdata_encode_link(&enc, -92, 0), "composite link");
    ASSERT_OK(iotdata_encode_solar(&enc, 800, 11), "composite solar");
    finish();
    decode_pkt();
    {
        uint8_t level;
        bool charging;
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery(&dec, &level, &charging), "composite decodes");
        ASSERT_NEAR(level, 80, 4, "its level");
        ASSERT_EQ_U(charging, true, "its state");
    }
    begin(2, 1, 1);
    ASSERT_OK(iotdata_encode_battery_level(&enc, 20), "standalone level");
    ASSERT_OK(iotdata_encode_battery_charge(&enc, false), "standalone charge");
    finish();
    decode_pkt();
    {
        uint8_t level;
        bool charging = true;
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery_level(&dec, &level), "standalone level decodes");
        ASSERT_NEAR(level, 20, 4, "its own level, not the composite's");
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery_charge(&dec, &charging), "standalone charge decodes");
        ASSERT_EQ_U(charging, false, "its own state");
    }
    PASS();
}

/* =========================================================================
 * All of them together
 * ========================================================================= */

static void test_fields_all_at_once(void) {
    TEST("Every field, across the two variants it takes");

    /* In slot order, so this passes under IOTDATA_OPERATE_ORDERED too. */
    begin(0, 7, 11);
    ASSERT_OK(iotdata_encode_battery(&enc, 75, false), "battery");
    ASSERT_OK(iotdata_encode_link(&enc, -92, 0), "link");
    ASSERT_OK(iotdata_encode_temperature(&enc, (iotdata_float_t)(IOTDATA_TEMPERATURE_MIN + 80 * IOTDATA_TEMPERATURE_RES)), "temperature");
    ASSERT_OK(iotdata_encode_pressure(&enc, 1013), "pressure");
    ASSERT_OK(iotdata_encode_humidity(&enc, 55), "humidity");
    ASSERT_OK(iotdata_encode_environment(&enc, (iotdata_float_t)(IOTDATA_TEMPERATURE_MIN + 120 * IOTDATA_TEMPERATURE_RES), 1000, 60), "environment");
    ASSERT_OK(iotdata_encode_wind_speed(&enc, (iotdata_float_t)(20 * IOTDATA_WIND_SPEED_RES)), "wind_speed");
    ASSERT_OK(iotdata_encode_wind_direction(&enc, 180), "wind_direction");
    ASSERT_OK(iotdata_encode_wind_gust(&enc, (iotdata_float_t)(40 * IOTDATA_WIND_GUST_RES)), "wind_gust");
    ASSERT_OK(iotdata_encode_wind(&enc, (iotdata_float_t)(10 * IOTDATA_WIND_SPEED_RES), 270, (iotdata_float_t)(30 * IOTDATA_WIND_GUST_RES)), "wind");
    ASSERT_OK(iotdata_encode_rain_rate(&enc, 8), "rain_rate");
    ASSERT_OK(iotdata_encode_rain_size(&enc, 20), "rain_size");
    ASSERT_OK(iotdata_encode_rain(&enc, 15, 28), "rain");
    ASSERT_OK(iotdata_encode_solar(&enc, 800, 11), "solar");
    ASSERT_OK(iotdata_encode_clouds(&enc, 6), "clouds");
    {
        const uint16_t pm_all[IOTDATA_AIR_QUALITY_PM_COUNT] = { 10, 25, 40, 100 };
        const uint16_t gas_all[IOTDATA_AIR_QUALITY_GAS_COUNT] = { 100, 200, 1000, 50, 500, 300, 0, 0 };
        ASSERT_OK(iotdata_encode_air_quality_index(&enc, 90), "air_quality_index");
        ASSERT_OK(iotdata_encode_air_quality_pm(&enc, 0x0F, pm_all), "air_quality_pm");
        ASSERT_OK(iotdata_encode_air_quality_gas(&enc, 0x3F, gas_all), "air_quality_gas");
        ASSERT_OK(iotdata_encode_air_quality(&enc, 150, 0x0F, pm_all, 0x3F, gas_all), "air_quality");
    }
    ASSERT_OK(iotdata_encode_radiation_cpm(&enc, 450), "radiation_cpm");
    ASSERT_OK(iotdata_encode_radiation_dose(&enc, (iotdata_float_t)(120 * IOTDATA_RADIATION_DOSE_RES)), "radiation_dose");
    ASSERT_OK(iotdata_encode_radiation(&enc, 900, (iotdata_float_t)(250 * IOTDATA_RADIATION_DOSE_RES)), "radiation");
    ASSERT_OK(iotdata_encode_depth(&enc, 150), "depth");
    ASSERT_OK(iotdata_encode_position(&enc, (iotdata_double_t)IOTDATA_POSITION_LAT_HIGH / 2, (iotdata_double_t)IOTDATA_POSITION_LON_LOW / 4), "position");
    ASSERT_OK(iotdata_encode_datetime(&enc, 3600), "datetime");
    ASSERT_OK(iotdata_encode_flags(&enc, 0x5A), "flags");
    ASSERT_OK(iotdata_encode_bits32(&enc, 0xDEADBEEFu), "bits32");
    finish();
    decode_pkt();

    /* All eight present at once, and each still its own value -- the question a
       per-field test cannot ask. */
    {
        uint8_t level_percent;
        rewind_dec();
        ASSERT_OK(iotdata_decode_battery(&dec, &level_percent, NULL), "battery back");
        ASSERT_NEAR(level_percent, 75, 4, "battery value");
    }
    {
        int8_t rssi_dbm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_link(&dec, &rssi_dbm, NULL), "link back");
        ASSERT_EQ(rssi_dbm, -92, "link value");
    }
    {
        uint16_t irradiance_wm2;
        uint8_t ultraviolet_index;
        rewind_dec();
        ASSERT_OK(iotdata_decode_solar(&dec, &irradiance_wm2, &ultraviolet_index), "solar back");
        ASSERT_EQ_U(irradiance_wm2, 800, "solar irradiance");
        ASSERT_EQ_U(ultraviolet_index, 11, "solar ultraviolet");
    }
    {
        uint8_t okta;
        rewind_dec();
        ASSERT_OK(iotdata_decode_clouds(&dec, &okta), "clouds back");
        ASSERT_EQ_U(okta, 6, "clouds value");
    }
    {
        uint16_t depth_cm;
        rewind_dec();
        ASSERT_OK(iotdata_decode_depth(&dec, &depth_cm), "depth back");
        ASSERT_EQ_U(depth_cm, 150, "depth value");
    }
    {
        uint32_t seconds_from_year_start;
        rewind_dec();
        ASSERT_OK(iotdata_decode_datetime(&dec, &seconds_from_year_start), "datetime back");
        ASSERT_EQ_U(seconds_from_year_start, 3600, "datetime value");
    }
    {
        uint8_t flags;
        rewind_dec();
        ASSERT_OK(iotdata_decode_flags(&dec, &flags), "flags back");
        ASSERT_EQ_U(flags, 0x5A, "flags value");
    }
    {
        uint32_t bits32;
        rewind_dec();
        ASSERT_OK(iotdata_decode_bits32(&dec, &bits32), "bits32 back");
        ASSERT_EQ_U(bits32, 0xDEADBEEFu, "bits32 value");
    }
    /* AND IMAGE, in a packet of its own: there is no variant that can hold all 28, so
       "every field" takes two. */
    begin(1, 1, 12);
    {
        static const uint8_t px[4] = { 1, 2, 3, 4 };
        ASSERT_OK(iotdata_encode_image(&enc, IOTDATA_IMAGE_FMT_BILEVEL, IOTDATA_IMAGE_SIZE_24x18, IOTDATA_IMAGE_COMP_RAW, 0, px, sizeof(px)), "image");
    }
    finish();
    decode_pkt();
    {
        uint8_t len;
        iotdata_field_image_scratch_t sc;
        rewind_dec();
        ASSERT_OK(iotdata_decode_image(&dec, NULL, NULL, NULL, NULL, NULL, &len, &sc), "image decodes");
        ASSERT_EQ_U(len, 4, "with its four bytes");
    }

    PASS();
}

/* =========================================================================
 * Main
 * ========================================================================= */

int main(void) {
    printf("\n=== test_fields: one test per generated field ===\n");

    printf("\n--- Split families: the standalone parts ---\n");
    test_field_battery_level();
    test_field_battery_charge();
    test_field_link_rssi();
    test_field_link_snr();
    test_field_solar_irradiance();
    test_field_solar_ultraviolet();
    test_split_families_independent();

    printf("\n--- Device ---\n");
    test_field_battery();
    test_field_link();

    printf("\n--- Weather ---\n");
    test_field_temperature();
    test_field_pressure();
    test_field_humidity();
    test_field_environment();
    test_field_wind_speed();
    test_field_wind_direction();
    test_field_wind_gust();
    test_field_wind();
    test_field_rain_rate();
    test_field_rain_size();
    test_field_rain();
    test_field_solar();
    test_field_clouds();
    test_field_depth();

    printf("\n--- Air quality ---\n");
    test_field_air_quality_index();
    test_field_air_quality_pm();
    test_field_air_quality_gas();
    test_field_air_quality();

    printf("\n--- Radiation ---\n");
    test_field_radiation_cpm();
    test_field_radiation_dose();
    test_field_radiation();
    printf("\n--- Position ---\n");
    test_field_position();

    printf("\n--- Context ---\n");
    test_field_datetime();

    printf("\n--- Image ---\n");
    test_field_image();

    printf("\n--- Unitary ---\n");
    test_field_flags();
    test_field_bits32();

    printf("\n--- Together ---\n");
    test_fields_all_at_once();

    printf("\n--- Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed > 0)
        printf(", %d FAILED", tests_failed);
    printf(" ---\n\n");

    return tests_failed > 0 ? 1 : 0;
}
