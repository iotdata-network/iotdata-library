/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * iotdata.h - reference implementation header
 *
 * Ultra-efficient bit-packed telemetry protocol for remote sensor systems.
 * Designed for constrained devices (ESP32-C3, STM32, nRF52) with
 * battery-optimised LoRa / LPWAN transmission.
 *
 * Architecture:
 *   - Variant field (4 bits) identifies sensor type and determines which
 *     sensor fields occupy which presence flag slots.
 *   - All field encodings are universal — variant only affects slot
 *     assignment and labelling.
 *   - N presence bytes supported via extension bit chaining.
 *   - Variant 15 is reserved for future extended header format.
 *
 * Compile-time options (define BEFORE including this header):
 *
 *   IOTDATA_VARIANT_MAPS_DEFAULT   Default variant maps (weather station)
 *   IOTDATA_VARIANT_MAPS <sym>     Custom variant maps array symbol
 *   IOTDATA_VARIANT_MAPS_COUNT <n> Number of entries in custom maps
 *   IOTDATA_OPERATE_ORDERED        Operate encoding/decoding in ordered mode
 *   IOTDATA_OPERATE_ORDERED_COMPLETE ... and require all fields accounted for
 *   IOTDATA_ENABLE_SELECTIVE       Only compile explicitly enabled elements
 *   IOTDATA_ENABLE_xxx             Enable individual field types
 *   IOTDATA_ENABLE_TLV             Enable TLV
 *   IOTDATA_NO_DECODE              Exclude decoder
 *   IOTDATA_NO_ENCODE              Exclude encoder
 *   IOTDATA_NO_PRINT               Exclude Print output support
 *   IOTDATA_NO_DUMP                Exclude Dump output support
 *   IOTDATA_NO_JSON                Exclude JSON support
 *   IOTDATA_NO_TLV_SPECIFIC        Exclude TLV specific type handling
 *   IOTDATA_NO_CHECKS_STATE        Remove runtime state checks
 *   IOTDATA_NO_CHECKS_TYPES        Remove runtime type checks (e.g. temp limits)
 *   IOTDATA_NO_ERROR_STRINGS       Exclude error strings (iotdata_strerror)
 *   IOTDATA_NO_FLOATING_DOUBLES    Use float instead of double for position
 *   IOTDATA_NO_FLOATING            Integer-only mode (no float/double)
 */

#ifndef IOTDATA_H
#define IOTDATA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h> /* the inline KV builders in iotdata_fields.h use memcpy/strlen */

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_NO_FLOATING)
typedef int32_t iotdata_double_t; /* degrees * 1e7 */
typedef int32_t iotdata_float_t;  /* value * 100    */
#elif defined(IOTDATA_NO_FLOATING_DOUBLES)
typedef float iotdata_double_t;
typedef float iotdata_float_t;
#else
typedef double iotdata_double_t;
typedef float iotdata_float_t;
#endif

/* ---------------------------------------------------------------------------
 * Fields
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_VARIANT_MAPS_DEFAULT)
#define IOTDATA_ENABLE_TLV
#define IOTDATA_ENABLE_BATTERY
#define IOTDATA_ENABLE_LINK
#define IOTDATA_ENABLE_ENVIRONMENT
#define IOTDATA_ENABLE_WIND
#define IOTDATA_ENABLE_RAIN
#define IOTDATA_ENABLE_SOLAR
#define IOTDATA_ENABLE_CLOUDS
#define IOTDATA_ENABLE_AIR_QUALITY_INDEX
#define IOTDATA_ENABLE_RADIATION
#define IOTDATA_ENABLE_POSITION
#define IOTDATA_ENABLE_DATETIME
#define IOTDATA_ENABLE_FLAGS
#endif

#if !defined(IOTDATA_ENABLE_SELECTIVE)
#define IOTDATA_ENABLE_TLV
#define IOTDATA_ENABLE_BATTERY
#define IOTDATA_ENABLE_BATTERY_LEVEL
#define IOTDATA_ENABLE_BATTERY_CHARGE
#define IOTDATA_ENABLE_LINK
#define IOTDATA_ENABLE_LINK_RSSI
#define IOTDATA_ENABLE_LINK_SNR
#define IOTDATA_ENABLE_ENVIRONMENT
#define IOTDATA_ENABLE_TEMPERATURE
#define IOTDATA_ENABLE_PRESSURE
#define IOTDATA_ENABLE_HUMIDITY
#define IOTDATA_ENABLE_WIND
#define IOTDATA_ENABLE_WIND_SPEED
#define IOTDATA_ENABLE_WIND_DIRECTION
#define IOTDATA_ENABLE_WIND_GUST
#define IOTDATA_ENABLE_RAIN
#define IOTDATA_ENABLE_RAIN_RATE
#define IOTDATA_ENABLE_RAIN_SIZE
#define IOTDATA_ENABLE_SOLAR
#define IOTDATA_ENABLE_SOLAR_IRRADIANCE
#define IOTDATA_ENABLE_SOLAR_ULTRAVIOLET
#define IOTDATA_ENABLE_CLOUDS
#define IOTDATA_ENABLE_AIR_QUALITY
#define IOTDATA_ENABLE_AIR_QUALITY_INDEX
#define IOTDATA_ENABLE_AIR_QUALITY_PM
#define IOTDATA_ENABLE_AIR_QUALITY_GAS
#define IOTDATA_ENABLE_RADIATION
#define IOTDATA_ENABLE_RADIATION_CPM
#define IOTDATA_ENABLE_RADIATION_DOSE
#define IOTDATA_ENABLE_DEPTH
#define IOTDATA_ENABLE_POSITION
#define IOTDATA_ENABLE_DATETIME
#define IOTDATA_ENABLE_FLAGS
#define IOTDATA_ENABLE_BITS32
#define IOTDATA_ENABLE_IMAGE
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
enum iotdata_status_t_;
struct iotdata_encoder_t_;
struct iotdata_decoder_t_;
/* IMAGE is a field, but its code is not generated: its encoder and decoder need different
   storage, so its shape depends on the direction it travels. Same two-phase include as TLV,
   but BEFORE the generated header rather than after: the generated decoder member aggregate
   names IOTDATA_FIELDS_MEM_IMAGE_DECODE inside a struct in that very file. TLV storage is
   used a step later, in this file's own structs, so TLV can follow. */
#include "iotdata_fields_image.h"
#include "iotdata_fields.h"
/* TLV is not a field and is not generated: its types have to be here because the encoder and
   decoder carry its storage, while its bodies arrive later, where their primitives exist. */
#include "iotdata_fields_tlv.h"
#pragma GCC diagnostic pop

/* ---------------------------------------------------------------------------
 * Header: variant(4) + station(12) + sequence(16) = 32 bits
 * -------------------------------------------------------------------------*/

#define IOTDATA_VARIANT_BITS            4
#define IOTDATA_STATION_BITS            12
#define IOTDATA_SEQUENCE_BITS           16
#define IOTDATA_HEADER_BITS             (IOTDATA_VARIANT_BITS + IOTDATA_STATION_BITS + IOTDATA_SEQUENCE_BITS)

#define IOTDATA_VARIANT_MAX             14
#define IOTDATA_VARIANT_RESERVED        15
#define IOTDATA_VARIANT_MESH            IOTDATA_VARIANT_RESERVED
#define IOTDATA_STATION_MAX             4095
#define IOTDATA_SEQUENCE_MAX            65535

/* ---------------------------------------------------------------------------
 * Reserved header values: in each of the three fields, the all-ones value is
 * reserved and means something other than "an ordinary frame from a station".
 *
 *   variant  = 0xF     this is a mesh frame (iotdata_mesh.h), not telemetry
 *   station  = 0xFFF   addressed to EVERY node, not sent by one
 *   sequence = 0xFFFF  DOWN: this frame is FOR the station in the header
 *                      rather than FROM it
 *
 * Ordinarily a frame's station field says who SENT it and there is no
 * destination -- the protocol is one-way telemetry. The downstream sequence
 * inverts that one field's meaning so a node can be addressed without spending
 * header bits that do not exist: the header is exactly 32 bits, fully
 * allocated. A node accepts a downstream frame whose station is its own or the
 * broadcast id, and ignores every other one.
 *
 * Assignable station ids are therefore 1..IOTDATA_STATION_ASSIGNABLE_MAX;
 * derive them with iotdata_station_from_id() so every node agrees.
 * ------------------------------------------------------------------------ */

#define IOTDATA_STATION_BROADCAST       IOTDATA_STATION_MAX
#define IOTDATA_STATION_ASSIGNABLE_MAX  (IOTDATA_STATION_MAX - 1)
#define IOTDATA_SEQUENCE_DOWN           IOTDATA_SEQUENCE_MAX
#define IOTDATA_SEQUENCE_ASSIGNABLE_MAX (IOTDATA_SEQUENCE_MAX - 1)

/* Map an arbitrary device id (a MAC-derived word, say) onto an assignable
   station id, avoiding both 0 and the broadcast id. */
static inline uint16_t iotdata_station_from_id(const uint32_t id) {
    return (uint16_t)((id % IOTDATA_STATION_ASSIGNABLE_MAX) + 1u);
}

/* For a station id that ARRIVES rather than being derived -- from a config file, a command line --
   check it before use: 0 is not a station, and the broadcast id would make the node answer every
   broadcast as though it were addressed to it alone. */
static inline bool iotdata_station_is_assignable(const uint16_t station) {
    return station != 0u && station <= IOTDATA_STATION_ASSIGNABLE_MAX;
}

/* Advance a sending node's own sequence, skipping the reserved downstream
   value so an upstream frame can never be mistaken for a command. */
static inline uint16_t iotdata_sequence_next(const uint16_t seq) {
    return (uint16_t)((seq >= IOTDATA_SEQUENCE_ASSIGNABLE_MAX) ? 0u : (seq + 1u));
}

/* ---------------------------------------------------------------------------
 * Presence byte layout (N-byte extension chain)
 *
 * Byte 0:
 *   bit 7: extension (more presence bytes follow)
 *   bit 6: TLV Data present (even if not supported in encoder)
 *   bits 5-0: data slots [0]-[5]
 *
 * Byte 1..N:
 *   bit 7: extension (more presence bytes follow)
 *   bits 6-0: data slots, 7 per byte
 *
 * Total data slots = 6 + 7*(num_pres_bytes - 1)
 *
 * This implementation limits to a total of 4 presence bytes, i.e. 48 slots,
 * but that is not a limitation in the protocol.
 * -------------------------------------------------------------------------*/

#define IOTDATA_PRES_TLV          (1U << 6)
#define IOTDATA_PRES_EXT          (1U << 7)
#define IOTDATA_PRES0_DATA_FIELDS 6
#define IOTDATA_PRESN_DATA_FIELDS 7
#define IOTDATA_PRES_MINIMUM      1
#define IOTDATA_PRES_MAXIMUM      4
#define IOTDATA_MAX_DATA_FIELDS   (IOTDATA_PRES0_DATA_FIELDS + IOTDATA_PRESN_DATA_FIELDS * (IOTDATA_PRES_MAXIMUM - 1))

/* ---------------------------------------------------------------------------
 * Packet
 * -------------------------------------------------------------------------*/

#define IOTDATA_PACKET_MINIMUM    ((IOTDATA_HEADER_BITS / 8) + IOTDATA_PRES_MINIMUM)

/* ---------------------------------------------------------------------------
 * Field types
 * -------------------------------------------------------------------------*/

typedef enum {
    IOTDATA_FIELD_NONE = -1,
#define IOTDATA_FIELDS_SELECTED_VISITOR_(name) IOTDATA_FIELD_##name,
    IOTDATA_FIELDS_SELECTED(IOTDATA_FIELDS_SELECTED_VISITOR_)
#undef IOTDATA_FIELDS_SELECTED_VISITOR_
    IOTDATA_FIELD_COUNT,
#define IOTDATA_FIELDS_RESERVED_VISITOR_(name, value) IOTDATA_FIELD_##name = value,
    IOTDATA_FIELDS_RESERVED(IOTDATA_FIELDS_RESERVED_VISITOR_)
#undef IOTDATA_FIELDS_RESERVED_VISITOR_
} iotdata_field_type_t;

/* The dense ordinals must fit the presence mask AND stay clear of the reserved ones above them.
   TLV is the only reserved field and sits at the mask's top bit, so the dense count may reach it
   but not pass it. Both halves of this are load-bearing: the first was always here, the second
   is here because the ordinals did once collide with TLV, silently, and the symptom was a field
   that could not be encoded at all. */
_Static_assert(IOTDATA_FIELD_COUNT <= 64, "fields overflow: more fields enabled than iotdata_field_t has bits");
#if defined(IOTDATA_ENABLE_TLV)
_Static_assert(IOTDATA_FIELD_COUNT <= IOTDATA_FIELD_TLV_RESERVED_ID, "field ordinals have reached TLV's reserved id -- they would be treated as reserved");
#endif

#define IOTDATA_FIELD_EMPTY             (0)
#define IOTDATA_FIELD_BIT(id)           ((iotdata_field_t)1 << (id))
#define IOTDATA_FIELD_PRESENT(mask, id) (((mask) >> (id)) & 1U)
#define IOTDATA_FIELD_SET(mask, id)     ((mask) |= ((iotdata_field_t)1 << (id)))
#define IOTDATA_FIELD_CLR(mask, id)     ((mask) &= ~((iotdata_field_t)1 << (id)))
#define IOTDATA_FIELD_VALID(id)         ((id) != IOTDATA_FIELD_NONE && !IOTDATA_FIELD_RESERVED(id))

/* ---------------------------------------------------------------------------
 * Presence by SLOT
 * -------------------------------------------------------------------------*/

typedef uint32_t iotdata_slots_t;

_Static_assert(IOTDATA_MAX_DATA_FIELDS <= 32, "slots overflow");

#define IOTDATA_SLOT_EMPTY             (0)
#define IOTDATA_SLOT_PRESENT(mask, si) (((mask) >> (si)) & 1U)
#define IOTDATA_SLOT_SET(mask, si)     ((mask) |= (1U << (si)))
#define IOTDATA_SLOT_CLR(mask, si)     ((mask) &= ~(1U << (si)))

#define IOTDATA_SLOT_NONE              (-1)
#define IOTDATA_SLOT_AMBIGUOUS         (-2)

/* ---------------------------------------------------------------------------
 * Variant definition — maps presence slots to field types
 *
 * Flat slot array covering all presence bytes. Entries 0..5 map to pres0,
 * entries 6..12 to pres1, entries 13..19 to pres2, etc.
 * Unused trailing slots should be IOTDATA_FIELD_NONE / NULL.
 * -------------------------------------------------------------------------*/

typedef struct {
    iotdata_field_type_t type;
    const char *label;
} iotdata_field_def_t;

typedef struct {
    const char *name;
    uint8_t num_pres_bytes;
    iotdata_field_def_t fields[IOTDATA_MAX_DATA_FIELDS];
} iotdata_variant_def_t;

const iotdata_variant_def_t *iotdata_get_variant(uint8_t variant);

#define IOTDATA_MIN_PACKET_SIZE  (IOTDATA_HEADER_BITS / 8)
#define IOTDATA_MAX_PACKET_SIZE  256

#define IOTDATA_TLV_VALUE_BUDGET (IOTDATA_MAX_PACKET_SIZE - ((IOTDATA_HEADER_BITS + 7u) / 8u) - IOTDATA_PRES_MAXIMUM - IOTDATA_TLV_HEADER_BYTES)
#define IOTDATA_TLV_VALUE_MAX    ((IOTDATA_TLV_VALUE_BUDGET < IOTDATA_TLV_LENGTH_MAX) ? IOTDATA_TLV_VALUE_BUDGET : IOTDATA_TLV_LENGTH_MAX)

/* ---------------------------------------------------------------------------
 * Status/error codes
 * -------------------------------------------------------------------------*/

typedef enum iotdata_status_t_ {
    IOTDATA_OK = 0,
#if !defined(IOTDATA_NO_ENCODE)
    IOTDATA_ERR_CTX_NULL,
    IOTDATA_ERR_CTX_NOT_BEGUN,
    IOTDATA_ERR_CTX_ALREADY_BEGUN,
    IOTDATA_ERR_CTX_ALREADY_ENDED,
    IOTDATA_ERR_CTX_DUPLICATE_FIELD,
    IOTDATA_ERR_CTX_NON_EXISTENT_FIELD,
    IOTDATA_ERR_CTX_AMBIGUOUS_FIELD,
    IOTDATA_ERR_BUF_NULL,
    IOTDATA_ERR_BUF_OVERFLOW,
    IOTDATA_ERR_BUF_TOO_SMALL,
#elif !defined(IOTDATA_NO_DUMP)
    IOTDATA_ERR_CTX_NULL,
    IOTDATA_ERR_BUF_NULL,
#endif
#if defined(IOTDATA_OPERATE_ORDERED)
    IOTDATA_ERR_CTX_SLOT_ORDER,
    IOTDATA_ERR_CTX_INCOMPLETE_FIELDS,
#endif
#if !defined(IOTDATA_NO_DECODE)
    IOTDATA_ERR_DECODE_SHORT,
    IOTDATA_ERR_DECODE_TRUNCATED,
    IOTDATA_ERR_DECODE_VARIANT,
    IOTDATA_ERR_DECODE_FIELD_ABSENT,
#elif !defined(IOTDATA_NO_DUMP)
    IOTDATA_ERR_DECODE_SHORT,
    IOTDATA_ERR_DECODE_TRUNCATED,
#endif
#if !defined(IOTDATA_NO_DUMP)
    IOTDATA_ERR_DUMP_ALLOC,
#endif
#if !defined(IOTDATA_NO_PRINT)
    IOTDATA_ERR_PRINT_ALLOC,
#endif
#if !defined(IOTDATA_NO_JSON)
    IOTDATA_ERR_JSON_PARSE,
    IOTDATA_ERR_JSON_ALLOC,
    IOTDATA_ERR_JSON_MISSING_FIELD,
#endif
    IOTDATA_ERR_HDR_VARIANT_HIGH,
    IOTDATA_ERR_HDR_VARIANT_RESERVED,
    IOTDATA_ERR_HDR_VARIANT_UNKNOWN,
    IOTDATA_ERR_HDR_STATION_HIGH,
#define IOTDATA_FIELDS_ERR_IDENTS_VISITOR_(name) name,
    IOTDATA_FIELDS_ERR_IDENTS(IOTDATA_FIELDS_ERR_IDENTS_VISITOR_)
#undef IOTDATA_FIELDS_ERR_IDENTS_VISITOR_
} iotdata_status_t;

typedef enum {
    IOTDATA_STATE_IDLE = 0,
    IOTDATA_STATE_BEGUN,
    IOTDATA_STATE_ENDED,
} iotdata_state_t;

/* THE PRESENCE BOOKKEEPING MASK, one bit per enabled field, and so the thing that sets how many
   fields a single build may enable. It was uint32_t and the library had 28 fields; splitting the
   three bundles took that to 34, which is why it is 64 bits now. The cost is four bytes in the
   encoder and four in the decoder, against a decoder that was 2536 bytes before ORDERED mode.

   This is NOT a wire quantity -- it never leaves the context, and the packet's own presence bytes
   are a separate, variable-length thing. Nothing about the protocol changes with it. */
typedef uint64_t iotdata_field_t;

/* ---------------------------------------------------------------------------
 * Encoder context
 * -------------------------------------------------------------------------*/

#if !defined(IOTDATA_NO_ENCODE)
typedef struct iotdata_encoder_t_ {
    uint8_t *buf;
    size_t buf_size;
    iotdata_state_t state;

    size_t packed_bits;
    size_t packed_bytes;

    uint8_t variant;
    uint16_t station;
    uint16_t sequence;
    iotdata_field_t fields;
    iotdata_slots_t slots;

#if defined(IOTDATA_OPERATE_ORDERED)
    const iotdata_variant_def_t *vdef;
    size_t bp;
    size_t pres_bit;
    int8_t pres_reserved;
    int8_t slot_count;
    int8_t cur_slot;
#else
    int8_t slot_of_type[IOTDATA_FIELD_COUNT];
#endif

    IOTDATA_FIELDS_MEM_ENCODER

} iotdata_encoder_t;
#endif /* !IOTDATA_NO_ENCODE */

/* ---------------------------------------------------------------------------
 * Decoded content
 * -------------------------------------------------------------------------*/

#if !defined(IOTDATA_NO_DECODE)
typedef struct iotdata_decoder_t_ {
    size_t packed_bits;
    size_t packed_bytes;

    uint8_t variant;
    uint16_t station;
    uint16_t sequence;
    iotdata_field_t fields;
    iotdata_slots_t slots;

#if defined(IOTDATA_OPERATE_ORDERED)
    const iotdata_variant_def_t *vdef;
    const uint8_t *buf;
    size_t bb;
    size_t bp;
    size_t data_bit;
    int8_t slot_count;
    int8_t cur_slot;
#endif

    IOTDATA_FIELDS_MEM_DECODER

} iotdata_decoder_t;
#endif /* !IOTDATA_NO_DECODE */

/* ---------------------------------------------------------------------------
 * Encode/Decode
 * -------------------------------------------------------------------------*/

#if !defined(IOTDATA_NO_ENCODE)
iotdata_status_t iotdata_encode_begin(iotdata_encoder_t *enc, uint8_t *buf, size_t buf_size, uint8_t variant, uint16_t station, uint16_t sequence);
iotdata_status_t iotdata_encode_end(iotdata_encoder_t *enc, size_t *out_bytes);
#endif /* !IOTDATA_NO_ENCODE */

#if defined(IOTDATA_OPERATE_ORDERED) && !defined(IOTDATA_NO_ENCODE)
iotdata_status_t iotdata_encode_skip(iotdata_encoder_t *enc);
iotdata_status_t iotdata_encode_skip_to(iotdata_encoder_t *enc, int slot);
int iotdata_encode_slots_remaining(const iotdata_encoder_t *enc);
#endif

#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_peek(const uint8_t *buf, size_t len, uint8_t *variant, uint16_t *station, uint16_t *sequence);
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_decode_reset(iotdata_decoder_t *dec);
iotdata_status_t iotdata_decode_skip(iotdata_decoder_t *dec);
iotdata_status_t iotdata_decode_wind_to_slot(iotdata_decoder_t *dec, int slot);
#endif
iotdata_status_t iotdata_decode(const uint8_t *buf, size_t len, iotdata_decoder_t *out);
#endif /* !IOTDATA_NO_DECODE */

/* ---------------------------------------------------------------------------
 * Dump, Print (requires decoder), JSON (requires encoder/decoder)
 * -------------------------------------------------------------------------*/

#if !defined(IOTDATA_NO_DUMP)

#define IOTDATA_DUMP_RANGE_STR_STATIC

#define IOTDATA_DUMP_FIELD_NAME_MAX  32
#define IOTDATA_DUMP_DECODED_STR_MAX 32
#define IOTDATA_DUMP_RANGE_STR_MAX   32

typedef struct {
    size_t bit_offset;
    size_t bit_length;
    char field_name[IOTDATA_DUMP_FIELD_NAME_MAX];
    uint32_t raw_value;
    char decoded_str[IOTDATA_DUMP_DECODED_STR_MAX];
#ifdef IOTDATA_DUMP_RANGE_STR_STATIC
    const char *range_str;
#else
    char range_str[IOTDATA_DUMP_RANGE_STR_MAX];
#endif
} iotdata_dump_entry_t;

#define IOTDATA_MAX_DUMP_ENTRIES 48

typedef struct {
    char _name_buf[IOTDATA_DUMP_FIELD_NAME_MAX];
    char _dec_buf[IOTDATA_DUMP_DECODED_STR_MAX];
    iotdata_dump_entry_t entries[IOTDATA_MAX_DUMP_ENTRIES];
    size_t count;
    size_t packed_bits;
    size_t packed_bytes;
} iotdata_dump_t;

iotdata_status_t iotdata_dump_to_string(iotdata_dump_t *dump, const uint8_t *buf, size_t len, char *out, size_t out_size, bool verbose);
#endif /* !IOTDATA_NO_DUMP */

#if !defined(IOTDATA_NO_PRINT)
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_print_decoder_to_string(const iotdata_decoder_t *dec, char *out, size_t out_size);
typedef struct {
    iotdata_decoder_t dec;
} iotdata_print_scratch_t;
iotdata_status_t iotdata_print_to_string(const uint8_t *buf, size_t len, char *out, size_t out_size, iotdata_print_scratch_t *scratch);
#endif
#endif /* !IOTDATA_NO_PRINT */

#if !defined(IOTDATA_NO_JSON)
#if !defined(IOTDATA_NO_DECODE)
typedef struct {
    iotdata_decoder_t dec;
    union {
        bool _dummy;
        IOTDATA_FIELDS_DECODE_TO_JSON_SCRATCH
    };
} iotdata_decode_to_json_scratch_t;
iotdata_status_t iotdata_decode_to_json(const uint8_t *buf, size_t len, char **json_out, iotdata_decode_to_json_scratch_t *scratch);
#endif /* !IOTDATA_NO_DECODE */
#if !defined(IOTDATA_NO_ENCODE)
typedef struct {
    iotdata_encoder_t enc;
    union {
        bool _dummy;
        IOTDATA_FIELDS_ENCODE_FROM_JSON_SCRATCH
    };
} iotdata_encode_from_json_scratch_t;
iotdata_status_t iotdata_encode_from_json(const char *json, uint8_t *buf, size_t buf_size, size_t *out_bytes, iotdata_encode_from_json_scratch_t *scratch);
#endif /* !IOTDATA_NO_ENCODE */
#endif /* !IOTDATA_NO_JSON */

/* ---------------------------------------------------------------------------
 * Ancillary
 * -------------------------------------------------------------------------*/

#if !defined(IOTDATA_NO_ERROR_STRINGS)
const char *iotdata_strerror(iotdata_status_t status);
#endif

#ifdef __cplusplus
}
#endif

#endif /* IOTDATA_H */
