/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * test_ordered.c — test suite for IOTDATA_OPERATE_ORDERED
 *
 * In-order operation carries no intermediate field storage: a setter packs
 * straight into the buffer and an accessor unpacks straight out of it, with a
 * cursor that only ever moves forwards.  That buys duplicate fields -- the
 * second temperature means the second temperature slot -- and costs the
 * freedom to encode or read in any order.
 *
 * This suite covers what only this mode can do and what only this mode
 * refuses.  The shared suites (test_default, test_custom, test_complete,
 * test_failures) cover the field semantics both modes share.
 *
 * Compile:
 *   cc -DIOTDATA_VARIANT_MAPS=ordered_variants -DIOTDATA_VARIANT_MAPS_COUNT=3 \
 *      -DIOTDATA_OPERATE_ORDERED test_ordered.c iotdata.c -lm -lcjson -o test_ordered
 */

#include "iotdata.h"

#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Variant maps
 *
 * [0] duplicated  three temperature slots, to show a repeated field needs no
 *                 machinery beyond the cursor
 * [1] wide        three presence bytes but only the first slots usually set,
 *                 so the reserved presence region has to shrink again
 * [2] simple      one of each, for the plain round trip
 *
 * EVERY trailing slot is IOTDATA_FIELD_NONE. A partly written initialiser
 * leaves the rest ZERO, and zero is a real field type -- NONE is -1.
 * -----------------------------------------------------------------------*/

const iotdata_variant_def_t ordered_variants[3] = {
    [0] = {
        .name = "duplicated",
        .num_pres_bytes = 1,
        .fields = {
            { IOTDATA_FIELD_BATTERY,     "battery" },
            { IOTDATA_FIELD_TEMPERATURE, "temp_inlet" },
            { IOTDATA_FIELD_TEMPERATURE, "temp_outlet" },
            { IOTDATA_FIELD_TEMPERATURE, "temp_case" },
            { IOTDATA_FIELD_HUMIDITY,    "humidity" },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
        },
    },
    [1] = {
        .name = "wide",
        .num_pres_bytes = 3,
        .fields = {
            { IOTDATA_FIELD_BATTERY,     "battery" },
            { IOTDATA_FIELD_TEMPERATURE, "temperature" },
            { IOTDATA_FIELD_HUMIDITY,    "humidity" },
            { IOTDATA_FIELD_PRESSURE,    "pressure" },
            { IOTDATA_FIELD_CLOUDS,      "clouds" },
            { IOTDATA_FIELD_DEPTH,       "depth" },
            { IOTDATA_FIELD_FLAGS,       "flags" },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
        },
    },
    [2] = {
        .name = "simple",
        .num_pres_bytes = 1,
        .fields = {
            { IOTDATA_FIELD_BATTERY,     "battery" },
            { IOTDATA_FIELD_ENVIRONMENT, "environment" },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
            { IOTDATA_FIELD_NONE,        NULL },
        },
    },
};

#include "test_common.h"

/* =========================================================================
 * Section 1: The packet on the wire
 * ========================================================================= */

static void test_wire_identical_to_deferred(void) {
    TEST("An ordered packet is byte-for-byte what the deferred mode writes");
    /* These bytes were taken from the DEFAULT build encoding the same calls. The two modes put
       the fields down in different orders -- one as they are given, one in a pass at the end --
       and the point of the reserve-and-backfill in encode_end is that nothing on the air can
       tell. If this ever diverges, the two modes are no longer the same protocol. */
    static const uint8_t expect[] = { 0x20, 0x07, 0x00, 0x0B, 0x30, 0x8D, 0xED, 0x46, 0xC0 };
    begin(2, 7, 11);
    ASSERT_OK(iotdata_encode_battery(&enc, 55, true), "battery");
    ASSERT_OK(iotdata_encode_environment(&enc, 21.5f, 1013, 48), "environment");
    finish();
    ASSERT_EQ_U(pkt_len, sizeof(expect), "length");
    ASSERT_EQ(memcmp(pkt, expect, sizeof(expect)), 0, "bytes");
    PASS();
}

static void test_presence_region_shrinks(void) {
    TEST("The reserved presence region shrinks again when fewer bytes are needed");
    /* Variant 1 declares three presence bytes, so encode_begin reserves three before it knows
       which slots will be filled. Only slot 0 is, needing one -- so encode_end has to close the
       two-byte gap, or every sparse packet would ride two bytes heavy. */
    begin(1, 1, 1);
    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "battery");
    finish();
    /* 4 header + 1 presence + 6 bits of battery = 6 bytes */
    ASSERT_EQ_U(pkt_len, 6, "one presence byte, not three");
    ASSERT_EQ_U(pkt[4] & 0x80, 0, "no extension bit");
    decode_pkt();
    {
        uint8_t level_percent;
        ASSERT_OK(iotdata_decode_battery(&dec, &level_percent, NULL), "decode battery");
        ASSERT_NEAR(level_percent, 50, 4, "and it still reads back");
    }
    PASS();
}

static void test_presence_region_kept_when_needed(void) {
    TEST("The presence region keeps every byte a later slot needs");
    /* Slot 6 lives in the second presence byte, so two bytes have to survive. */
    begin(1, 1, 2);
    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "battery");
    ASSERT_OK(iotdata_encode_skip_to(&enc, 6), "skip to flags");
    ASSERT_OK(iotdata_encode_flags(&enc, 0xA5), "flags");
    finish();
    ASSERT_TRUE((pkt[4] & 0x80) != 0, "pres0 extension bit set");
    decode_pkt();
    {
        uint8_t flags;
        ASSERT_OK(iotdata_decode_flags(&dec, &flags), "decode flags");
        ASSERT_EQ_U(flags, 0xA5, "flags");
    }
    {
        uint8_t humidity_pct;
        ASSERT_ERR(iotdata_decode_humidity(&dec, &humidity_pct), IOTDATA_ERR_DECODE_FIELD_ABSENT, "skipped slot is absent");
    }
    PASS();
}

/* =========================================================================
 * Section 2: Duplicates -- the reason the mode exists
 * ========================================================================= */

static void test_duplicates_by_type(void) {
    TEST("Calling a setter again fills the next slot of that type");
    begin(0, 3, 9);
    ASSERT_OK(iotdata_encode_battery(&enc, 90, false), "battery");
    ASSERT_OK(iotdata_encode_temperature(&enc, 10.0f), "temp 1");
    ASSERT_OK(iotdata_encode_temperature(&enc, 20.0f), "temp 2");
    ASSERT_OK(iotdata_encode_temperature(&enc, 30.0f), "temp 3");
    ASSERT_OK(iotdata_encode_humidity(&enc, 44), "humidity");
    finish();
    decode_pkt();
    /* and they come back in the same order, each its own value */
    const double want[3] = { 10.0, 20.0, 30.0 };
    for (int i = 0; i < 3; i++) {
        iotdata_float_t temperature_c;
        ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "decode temp");
        ASSERT_NEAR(temperature_c, want[i], 0.3, "temp value");
    }
    PASS();
}

static void test_duplicates_exhausted(void) {
    TEST("A fourth call to a field with three slots is refused");
    begin(0, 3, 10);
    ASSERT_OK(iotdata_encode_temperature(&enc, 1.0f), "temp 1");
    ASSERT_OK(iotdata_encode_temperature(&enc, 2.0f), "temp 2");
    ASSERT_OK(iotdata_encode_temperature(&enc, 3.0f), "temp 3");
    ASSERT_ERR(iotdata_encode_temperature(&enc, 4.0f), IOTDATA_ERR_CTX_NON_EXISTENT_FIELD, "no fourth slot");
    PASS();
}

static void test_duplicates_read_exhausted(void) {
    TEST("Reading past the last occurrence says the cursor has gone by");
    begin(0, 3, 11);
    ASSERT_OK(iotdata_encode_temperature(&enc, 1.0f), "temp 1");
    ASSERT_OK(iotdata_encode_temperature(&enc, 2.0f), "temp 2");
    finish();
    decode_pkt();
    iotdata_float_t temperature_c;
    ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "read 1");
    ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "read 2");
    /* The packet DID carry temperatures, so this is not ABSENT -- it is out of reach. One mask
       test buys the difference between "no temperature here" and "you have read them all". */
    ASSERT_ERR(iotdata_decode_temperature(&dec, &temperature_c), IOTDATA_ERR_CTX_SLOT_ORDER, "all used up");
    PASS();
}

/* =========================================================================
 * Section 3: By slot -- naming the position instead of the type
 * ========================================================================= */

static void test_by_slot_exact(void) {
    TEST("A by-slot setter fills the slot it names and leaves the rest absent");
    begin(0, 3, 12);
    ASSERT_OK(iotdata_encode_temperature_by_slot(&enc, 2, 77.0f), "temp into slot 2");
    finish();
    decode_pkt();
    ASSERT_TRUE(!IOTDATA_SLOT_PRESENT(dec.slots, 0), "slot 0 absent");
    ASSERT_TRUE(!IOTDATA_SLOT_PRESENT(dec.slots, 1), "slot 1 absent");
    ASSERT_TRUE(IOTDATA_SLOT_PRESENT(dec.slots, 2), "slot 2 present");
    {
        iotdata_float_t temperature_c;
        ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "decode temp");
        ASSERT_NEAR(temperature_c, 77.0, 0.3, "the slot 2 value");
    }
    PASS();
}

static void test_by_slot_backwards_refused(void) {
    TEST("A by-slot setter cannot go back behind the cursor");
    begin(0, 3, 13);
    ASSERT_OK(iotdata_encode_temperature_by_slot(&enc, 2, 77.0f), "slot 2");
    ASSERT_ERR(iotdata_encode_temperature_by_slot(&enc, 1, 5.0f), IOTDATA_ERR_CTX_SLOT_ORDER, "slot 1 is behind");
    PASS();
}

static void test_by_slot_type_must_match(void) {
    TEST("A by-slot setter refuses a slot that holds another type");
    begin(0, 3, 14);
    /* slot 1 is a temperature, not a battery */
    ASSERT_ERR(iotdata_encode_battery_by_slot(&enc, 1, 50, false), IOTDATA_ERR_CTX_NON_EXISTENT_FIELD, "wrong type for slot");
    /* and nothing was consumed by the refusal */
    ASSERT_OK(iotdata_encode_battery_by_slot(&enc, 0, 50, false), "slot 0 still available");
    PASS();
}

static void test_by_slot_past_the_end(void) {
    TEST("A by-slot setter refuses a slot the variant does not have");
    begin(0, 3, 15);
    ASSERT_ERR(iotdata_encode_temperature_by_slot(&enc, 99, 1.0f), IOTDATA_ERR_CTX_NON_EXISTENT_FIELD, "slot 99");
    PASS();
}

/* =========================================================================
 * Section 4: Skipping, and the out-of-sync tripwire
 * ========================================================================= */

static void test_skip_costs_nothing(void) {
    TEST("A skipped slot costs no bits, only a clear presence bit");
    begin(0, 3, 16);
    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "battery");
    ASSERT_OK(iotdata_encode_skip(&enc), "skip temp_inlet");
    ASSERT_OK(iotdata_encode_skip(&enc), "skip temp_outlet");
    ASSERT_OK(iotdata_encode_temperature(&enc, 33.0f), "temp_case");
    finish();
    decode_pkt();
    ASSERT_TRUE(!IOTDATA_SLOT_PRESENT(dec.slots, 1), "slot 1 absent");
    ASSERT_TRUE(!IOTDATA_SLOT_PRESENT(dec.slots, 2), "slot 2 absent");
    ASSERT_TRUE(IOTDATA_SLOT_PRESENT(dec.slots, 3), "slot 3 present");
    {
        iotdata_float_t temperature_c;
        ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "decode temp");
        ASSERT_NEAR(temperature_c, 33.0, 0.3, "the third slot's value");
    }
    PASS();
}

static void test_skip_to_and_remaining(void) {
    TEST("skip_to jumps the cursor, and slots_remaining says what is left");
    begin(0, 3, 17);
    ASSERT_EQ(iotdata_encode_slots_remaining(&enc), 5, "five fields to start");
    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "battery");
    ASSERT_EQ(iotdata_encode_slots_remaining(&enc), 4, "four left");
    ASSERT_OK(iotdata_encode_skip_to(&enc, 4), "skip the three temperatures");
    ASSERT_EQ(iotdata_encode_slots_remaining(&enc), 1, "humidity left");
    ASSERT_OK(iotdata_encode_humidity(&enc, 60), "humidity");
    ASSERT_EQ(iotdata_encode_slots_remaining(&enc), 0, "nothing left");
    /* the padding slots are NOT counted: the variant has five fields, not twenty-seven */
    finish();
    PASS();
}

static void test_skip_to_backwards_refused(void) {
    TEST("skip_to cannot move the cursor backwards");
    begin(0, 3, 18);
    ASSERT_OK(iotdata_encode_skip_to(&enc, 3), "forward");
    ASSERT_ERR(iotdata_encode_skip_to(&enc, 1), IOTDATA_ERR_CTX_SLOT_ORDER, "backward");
    PASS();
}

static void test_skip_past_the_end_refused(void) {
    TEST("skip cannot step past the last slot");
    begin(0, 3, 19);
    ASSERT_OK(iotdata_encode_skip_to(&enc, enc.slot_count), "to the end");
    ASSERT_ERR(iotdata_encode_skip(&enc), IOTDATA_ERR_CTX_NON_EXISTENT_FIELD, "one more");
    PASS();
}

/* =========================================================================
 * Section 5: Reading out of order, and getting a second look
 * ========================================================================= */

static void test_read_backwards_refused(void) {
    TEST("Reading an earlier slot after a later one is refused");
    begin(2, 1, 20);
    ASSERT_OK(iotdata_encode_battery(&enc, 60, false), "battery");
    ASSERT_OK(iotdata_encode_environment(&enc, 18.0f, 1000, 55), "environment");
    finish();
    decode_pkt();
    {
        uint8_t level_percent;
        iotdata_float_t temp_c;
        ASSERT_OK(iotdata_decode_battery(&dec, &level_percent, NULL), "battery");
        ASSERT_OK(iotdata_decode_environment(&dec, &temp_c, NULL, NULL), "environment");
        ASSERT_ERR(iotdata_decode_battery(&dec, &level_percent, NULL), IOTDATA_ERR_CTX_SLOT_ORDER, "battery again");
    }
    PASS();
}

static void test_reset_gives_a_second_look(void) {
    TEST("reset rewinds the cursor so the packet can be read again");
    begin(2, 1, 21);
    ASSERT_OK(iotdata_encode_battery(&enc, 60, false), "battery");
    ASSERT_OK(iotdata_encode_environment(&enc, 18.0f, 1000, 55), "environment");
    finish();
    decode_pkt();
    uint8_t first = 0, second = 0;
    ASSERT_OK(iotdata_decode_battery(&dec, &first, NULL), "first pass");
    {
        iotdata_float_t temp_c;
        ASSERT_OK(iotdata_decode_environment(&dec, &temp_c, NULL, NULL), "environment");
        ASSERT_NEAR(temp_c, 18.0, 0.3, "temp");
    }
    ASSERT_OK(iotdata_decode_reset(&dec), "reset");
    ASSERT_OK(iotdata_decode_battery(&dec, &second, NULL), "second pass");
    ASSERT_EQ_U(first, second, "same value both times");
    PASS();
}

static void test_decode_skip_steps_over(void) {
    TEST("decode_skip steps over a present slot without delivering it");
    begin(2, 1, 22);
    ASSERT_OK(iotdata_encode_battery(&enc, 60, false), "battery");
    ASSERT_OK(iotdata_encode_environment(&enc, 18.0f, 1000, 55), "environment");
    finish();
    decode_pkt();
    ASSERT_OK(iotdata_decode_skip(&dec), "skip battery");
    {
        iotdata_float_t temp_c;
        uint16_t pressure_hpa;
        uint8_t humidity_pct;
        ASSERT_OK(iotdata_decode_environment(&dec, &temp_c, &pressure_hpa, &humidity_pct), "environment");
        ASSERT_NEAR(temp_c, 18.0, 0.3, "temp");
        ASSERT_EQ_U(pressure_hpa, 1000, "pressure");
        ASSERT_EQ_U(humidity_pct, 55, "humidity");
    }
    PASS();
}

static void test_wind_to_slot(void) {
    TEST("wind_to_slot reaches a later slot, unpacking what it passes");
    begin(0, 3, 23);
    ASSERT_OK(iotdata_encode_temperature(&enc, 1.0f), "temp 1");
    ASSERT_OK(iotdata_encode_temperature(&enc, 2.0f), "temp 2");
    ASSERT_OK(iotdata_encode_temperature(&enc, 3.0f), "temp 3");
    finish();
    decode_pkt();
    ASSERT_OK(iotdata_decode_wind_to_slot(&dec, 3), "wind to the third temperature");
    {
        iotdata_float_t temperature_c;
        ASSERT_OK(iotdata_decode_temperature(&dec, &temperature_c), "decode temp");
        ASSERT_NEAR(temperature_c, 3.0, 0.3, "the third value, not the first");
    }
    ASSERT_ERR(iotdata_decode_wind_to_slot(&dec, 1), IOTDATA_ERR_CTX_SLOT_ORDER, "and not backwards");
    PASS();
}

/* =========================================================================
 * Section 6: What this mode gives up
 * ========================================================================= */

static void test_truncation_found_at_the_accessor(void) {
    TEST("Truncation surfaces at the accessor, not at decode");
    /* THE PROPERTY THIS MODE GIVES UP. There is no CRC at this layer, and iotdata_decode does not
       traverse the fields -- that is the whole point -- so a short packet is accepted and the
       field that runs off the end is the one that reports it. The deferred mode is all-or-nothing;
       this one is not, and a caller that needs the old guarantee has to check every accessor. */
    begin(0, 3, 24);
    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "battery");
    ASSERT_OK(iotdata_encode_temperature(&enc, 20.0f), "temperature");
    finish();
    ASSERT_TRUE(pkt_len > 5, "packet long enough to cut");
    /* cut the last byte off */
    ASSERT_OK(iotdata_decode(pkt, pkt_len - 1, &dec), "decode accepts a short packet");
    {
        uint8_t level_percent;
        ASSERT_OK(iotdata_decode_battery(&dec, &level_percent, NULL), "the first field is fine");
    }
    {
        iotdata_float_t temperature_c;
        ASSERT_ERR(iotdata_decode_temperature(&dec, &temperature_c), IOTDATA_ERR_DECODE_TRUNCATED, "the second runs off the end");
    }
    PASS();
}

static void test_failed_setter_does_not_advance(void) {
    TEST("A setter that fails its value checks leaves the cursor alone");
    /* The cursor is found before the value is checked and committed only after the bits are down,
       so a rejected value does not consume the slot it was meant for. */
    begin(0, 3, 25);
    ASSERT_ERR(iotdata_encode_battery(&enc, 200, false), IOTDATA_ERR_BATTERY_LEVEL_HIGH, "101% refused");
    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "slot 0 still there");
    finish();
    decode_pkt();
    {
        uint8_t level_percent;
        ASSERT_OK(iotdata_decode_battery(&dec, &level_percent, NULL), "battery");
        ASSERT_NEAR(level_percent, 50, 4, "the good value");
    }
    PASS();
}

static void test_failed_accessor_does_not_advance(void) {
    TEST("An accessor that finds nothing leaves the cursor alone");
    /* What the composite-or-atomic idiom rests on:
           if (decode_environment(...) == OK) ... else if (decode_temperature(...) == OK) ...
       If the failed first call had eaten the temperature slot, the second could never find it. */
    begin(0, 3, 26);
    ASSERT_OK(iotdata_encode_temperature(&enc, 12.0f), "temperature");
    finish();
    decode_pkt();
    {
        iotdata_float_t temp_c;
        uint8_t humidity_pct;
        /* this variant has no ENVIRONMENT at all */
        ASSERT_ERR(iotdata_decode_environment(&dec, &temp_c, NULL, NULL), IOTDATA_ERR_DECODE_FIELD_ABSENT, "no environment");
        ASSERT_ERR(iotdata_decode_humidity(&dec, &humidity_pct), IOTDATA_ERR_DECODE_FIELD_ABSENT, "no humidity either");
        /* and the temperature is still reachable */
        ASSERT_OK(iotdata_decode_temperature(&dec, &temp_c), "temperature survived both");
        ASSERT_NEAR(temp_c, 12.0, 0.3, "value");
    }
    PASS();
}

/* =========================================================================
 * Section 7: The generic walkers, which hold no storage either
 * ========================================================================= */

#if !defined(IOTDATA_NO_PRINT)
static void test_print_walks_the_wire(void) {
    TEST("print reads the fields off the wire, including duplicates");
    char out[1024];
    begin(0, 3, 27);
    ASSERT_OK(iotdata_encode_temperature(&enc, 11.0f), "temp 1");
    ASSERT_OK(iotdata_encode_temperature(&enc, 22.0f), "temp 2");
    finish();
    iotdata_print_scratch_t scratch;
    ASSERT_OK(iotdata_print_to_string(pkt, pkt_len, out, sizeof(out), &scratch), "print");
    /* both duplicate slots appear, under their own labels */
    ASSERT_TRUE(strstr(out, "temp_inlet") != NULL, "first label");
    ASSERT_TRUE(strstr(out, "temp_outlet") != NULL, "second label");
    ASSERT_TRUE(strstr(out, "11.0") != NULL, "first value");
    ASSERT_TRUE(strstr(out, "22.0") != NULL, "second value");
    PASS();
}
#endif

#if !defined(IOTDATA_NO_JSON)
static void test_json_walks_the_wire(void) {
    TEST("decode_to_json reads the fields off the wire, including duplicates");
    char *json = NULL;
    begin(0, 3, 28);
    ASSERT_OK(iotdata_encode_temperature(&enc, 11.0f), "temp 1");
    ASSERT_OK(iotdata_encode_temperature(&enc, 22.0f), "temp 2");
    finish();
    iotdata_decode_to_json_scratch_t scratch;
    ASSERT_OK(iotdata_decode_to_json(pkt, pkt_len, &json, &scratch), "to json");
    ASSERT_TRUE(json != NULL, "json produced");
    ASSERT_TRUE(strstr(json, "temp_inlet") != NULL, "first label");
    ASSERT_TRUE(strstr(json, "temp_outlet") != NULL, "second label");
    free(json);
    PASS();
}
#endif

#if !defined(IOTDATA_NO_DUMP)
static void test_dump_walks_the_wire(void) {
    TEST("dump is unchanged by the mode, having never held storage");
    char out[4096];
    begin(0, 3, 29);
    ASSERT_OK(iotdata_encode_battery(&enc, 50, false), "battery");
    ASSERT_OK(iotdata_encode_temperature(&enc, 11.0f), "temp");
    finish();
    iotdata_dump_t dump;
    ASSERT_OK(iotdata_dump_to_string(&dump, pkt, pkt_len, out, sizeof(out), false), "dump");
    /* DUMP NAMES FIELDS CANONICALLY, not by the variant's label -- every dump_ op takes the label
       and discards it. That is deliberate: a dump is a wire-level view, and the names are the
       protocol's rather than this variant's. It does mean two duplicate slots dump under one
       name; the bit offsets in the verbose form are what tell them apart. */
    ASSERT_TRUE(strstr(out, "battery_level") != NULL, "battery in the dump");
    ASSERT_TRUE(strstr(out, "temperature=") != NULL, "temperature in the dump");
    PASS();
}
#endif

/* =========================================================================
 * Main
 * ========================================================================= */

int main(void) {
    printf("\n=== test_ordered: IOTDATA_OPERATE_ORDERED ===\n");

    printf("\n--- Section 1: The packet on the wire ---\n");
    test_wire_identical_to_deferred();
    test_presence_region_shrinks();
    test_presence_region_kept_when_needed();

    printf("\n--- Section 2: Duplicates ---\n");
    test_duplicates_by_type();
    test_duplicates_exhausted();
    test_duplicates_read_exhausted();

    printf("\n--- Section 3: By slot ---\n");
    test_by_slot_exact();
    test_by_slot_backwards_refused();
    test_by_slot_type_must_match();
    test_by_slot_past_the_end();

    printf("\n--- Section 4: Skipping ---\n");
    test_skip_costs_nothing();
    test_skip_to_and_remaining();
    test_skip_to_backwards_refused();
    test_skip_past_the_end_refused();

    printf("\n--- Section 5: Reading out of order ---\n");
    test_read_backwards_refused();
    test_reset_gives_a_second_look();
    test_decode_skip_steps_over();
    test_wind_to_slot();

    printf("\n--- Section 6: What this mode gives up ---\n");
    test_truncation_found_at_the_accessor();
    test_failed_setter_does_not_advance();
    test_failed_accessor_does_not_advance();

    printf("\n--- Section 7: The generic walkers ---\n");
#if !defined(IOTDATA_NO_PRINT)
    test_print_walks_the_wire();
#endif
#if !defined(IOTDATA_NO_JSON)
    test_json_walks_the_wire();
#endif
#if !defined(IOTDATA_NO_DUMP)
    test_dump_walks_the_wire();
#endif

    printf("\n--- Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed > 0)
        printf(", %d FAILED", tests_failed);
    printf(" ---\n\n");

    return tests_failed > 0 ? 1 : 0;
}
