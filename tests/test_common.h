/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * test_common.h - shared test framework macros, variables, and helpers
 */

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "iotdata.h"
#include "iotdata_node.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * Test framework
 * -------------------------------------------------------------------------*/

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    do { \
        tests_run++; \
        printf("  %-58s ", name); \
        fflush(stdout); \
    } while (0)

#define PASS() \
    do { \
        tests_passed++; \
        printf("PASS\n"); \
    } while (0)

#define FAIL(msg) \
    do { \
        tests_failed++; \
        printf("FAIL: %s\n", msg); \
    } while (0)

#define ASSERT_EQ(a, b, msg) \
    do { \
        if ((a) != (b)) { \
            printf("FAIL: %s (got %d, expected %d)\n", msg, (int)(a), (int)(b)); \
            tests_failed++; \
            return; \
        } \
    } while (0)

#define ASSERT_EQ_U(a, b, msg) \
    do { \
        if ((a) != (b)) { \
            printf("FAIL: %s (got %u, expected %u)\n", msg, (unsigned)(a), (unsigned)(b)); \
            tests_failed++; \
            return; \
        } \
    } while (0)

#define ASSERT_NEAR(a, b, tol, msg) \
    do { \
        if (fabs((double)(a) - (double)(b)) > (tol)) { \
            printf("FAIL: %s (got %.6f, expected %.6f, tol %.6f)\n", msg, (double)(a), (double)(b), (double)(tol)); \
            tests_failed++; \
            return; \
        } \
    } while (0)

#define ASSERT_OK(rc, msg) \
    do { \
        iotdata_status_t _rc = (rc); \
        if (_rc != IOTDATA_OK) { \
            printf("FAIL: %s (%s)\n", msg, iotdata_strerror(_rc)); \
            tests_failed++; \
            return; \
        } \
    } while (0)

#define ASSERT_ERR(rc, expected, msg) \
    do { \
        iotdata_status_t _rc = (rc); \
        if (_rc != (expected)) { \
            printf("FAIL: %s (got %s, expected %s)\n", msg, iotdata_strerror(_rc), iotdata_strerror(expected)); \
            tests_failed++; \
            return; \
        } \
    } while (0)

#define ASSERT_TRUE(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("FAIL: %s\n", msg); \
            tests_failed++; \
            return; \
        } \
    } while (0)

/* ---------------------------------------------------------------------------
 * Shared helpers
 * -------------------------------------------------------------------------*/

static iotdata_encoder_t enc;
static uint8_t pkt[256];
static size_t pkt_len;
static iotdata_decoder_t dec;

static void begin(uint8_t variant, uint16_t station, uint16_t seq) {
    assert(iotdata_encode_begin(&enc, pkt, sizeof(pkt), variant, station, seq) == IOTDATA_OK);
}

static void finish(void) {
    assert(iotdata_encode_end(&enc, &pkt_len) == IOTDATA_OK);
}

static void decode_pkt(void) {
    assert(iotdata_decode(pkt, pkt_len, &dec) == IOTDATA_OK);
}

/* THE SECOND ATTEMPT AT A SINGLE-SLOT FIELD. Both modes refuse it; they reach the refusal by
 * different routes, so the suites name the outcome rather than the route.
 *   deferred: the slot is already marked present            -> DUPLICATE_FIELD
 *   ordered:  the cursor is past the only slot of that type -> NON_EXISTENT_FIELD
 * What matters either way is that a packet cannot carry two of a field that has room for one. */
#if defined(IOTDATA_OPERATE_ORDERED)
#define ERR_REFILL IOTDATA_ERR_CTX_NON_EXISTENT_FIELD
#else
#define ERR_REFILL IOTDATA_ERR_CTX_DUPLICATE_FIELD
#endif

/* READ A FIELD WITHOUT CARING WHAT WAS READ BEFORE IT.
 *
 * Nothing in the deferred build: every field has its own storage and an accessor is a plain read.
 * Under IOTDATA_OPERATE_ORDERED a read advances a cursor that only moves forwards, so asking for
 * an earlier slot after a later one is refused -- correctly, and that refusal is what
 * tests/test_ordered.c exists to check.
 *
 * These suites are not about read ORDER, they are about what each field carries, so they rewind
 * first and stay the same tests in both modes. The cost is re-unpacking the slots in front, which
 * matters to nobody here. */
static inline void rewind_dec(void) {
#if defined(IOTDATA_OPERATE_ORDERED)
    assert(iotdata_decode_reset(&dec) == IOTDATA_OK);
#endif
}

#endif /* TEST_COMMON_H */
