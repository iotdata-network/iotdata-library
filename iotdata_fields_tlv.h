/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * iotdata_fields_tlv.h -- the TLV escape hatch, and the key-value payloads offered inside it
 *
 * TLV IS NOT A FIELD, which is why it lives here and not in the generated files.
 * The iotdata_fields_ prefix is for consistency with the other hand-written
 * piece of the fields layer, iotdata_fields_impl_image.h, and not a claim that TLV
 * is one of them: it has no identifier, no presence slot, no enum member and no
 * row in the registry.
 * It occupies no data presence slot, it carries no identifier, it is not in the
 * field enum, and nothing about it is derived from a specification -- it is the
 * way out of typed telemetry rather than a kind of it. Keeping it in
 * iotdata_fields.[ch] meant seven hundred lines of hand-written code sitting in
 * a template whose whole purpose is to hold only what is generated.
 *
 * TWO PHASES, ONE FILE. The types have to be visible to iotdata.h, because the
 * encoder and decoder structs carry TLV storage; the functions cannot be, because
 * they call bits_write, bits_read, bprintf, dump_add and CHECK_CTX_ACTIVE, which
 * iotdata.c defines further down. So this header is included twice: once for the
 * declarations, and once with IOTDATA_FIELDS_TLV_IMPLEMENTATION defined for the bodies,
 * at the point where those primitives exist.
 */

#if !defined(IOTDATA_FIELDS_TLV_H_DECLARATIONS)
#define IOTDATA_FIELDS_TLV_H_DECLARATIONS

/* ---------------------------------------------------------------------------
 * Field TLV
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_TLV)

/* The wire format first, because the in-memory shapes below are sized from it. */
#define IOTDATA_TLV_FMT_BITS     1
#define IOTDATA_TLV_TYPE_BITS    6
#define IOTDATA_TLV_TYPE_MAX     63
#define IOTDATA_TLV_MORE_BITS    1
#define IOTDATA_TLV_LENGTH_BITS  8
#define IOTDATA_TLV_HEADER_BITS  (IOTDATA_TLV_FMT_BITS + IOTDATA_TLV_TYPE_BITS + IOTDATA_TLV_MORE_BITS + IOTDATA_TLV_LENGTH_BITS)
/* What the TLV's OWN framing costs and permits, derived from the fields above rather than written
   out again. The packet budget -- how much of a frame is left for a value once the iotdata header
   and the presence map have taken their share -- is IOTDATA_TLV_VALUE_MAX in iotdata.h, because
   those constants do not exist yet at this point in the include order. */
#define IOTDATA_TLV_HEADER_BYTES ((IOTDATA_TLV_HEADER_BITS + 7u) / 8u)
#define IOTDATA_TLV_LENGTH_MAX   ((1u << IOTDATA_TLV_LENGTH_BITS) - 1u)
#define IOTDATA_TLV_FMT_RAW      0
#define IOTDATA_TLV_FMT_STRING   1

/* How many TLVs one packet may carry, and how long a DECODED string may be. The string limit is
   deliberately its own number and not IOTDATA_TLV_LENGTH_MAX: the length field counts BYTES, so
   under 6-bit packing a 255-byte value yields more characters than that, and the two would have
   to part company the day that packing is used. A raw value needs no constant of its own -- it is
   the length field, exactly. */
#define IOTDATA_TLV_MAX          8
#define IOTDATA_TLV_STR_LEN_MAX  255
#if !defined(IOTDATA_NO_ENCODE)
typedef struct {
    uint8_t format;
    uint8_t type;
    uint8_t length;
    const uint8_t *data;
    const char *str;
} iotdata_encoder_tlv_t;
typedef struct {
    union {
        uint8_t raw[IOTDATA_TLV_MAX][IOTDATA_TLV_LENGTH_MAX];
        char str[IOTDATA_TLV_MAX][IOTDATA_TLV_STR_LEN_MAX + 1];
    };
} iotdata_encode_from_json_scratch_tlv_t;
#define IOTDATA_FIELDS_DAT_TLV_ENCODE \
    uint8_t tlv_count; \
    iotdata_encoder_tlv_t tlv[IOTDATA_TLV_MAX];
enum iotdata_status_t_ iotdata_encode_tlv(struct iotdata_encoder_t_ *enc, uint8_t type, const uint8_t *const data, uint8_t length);
enum iotdata_status_t_ iotdata_encode_tlv_string(struct iotdata_encoder_t_ *enc, uint8_t type, const char *const str);
#else
#define IOTDATA_FIELDS_DAT_TLV_ENCODE
#endif
#if !defined(IOTDATA_NO_DECODE)
typedef struct {
    uint8_t format;
    uint8_t type;
    uint8_t length;
    union {
        uint8_t raw[IOTDATA_TLV_LENGTH_MAX];
        char str[IOTDATA_TLV_STR_LEN_MAX + 1];
    };
} iotdata_decoder_tlv_t;
typedef struct {
    union {
        char b64[((IOTDATA_TLV_LENGTH_MAX + 2) / 3) * 4 + 1];
        char str[IOTDATA_TLV_STR_LEN_MAX + 1];
    };
} iotdata_decode_to_json_scratch_tlv_t;
#define IOTDATA_FIELDS_DAT_TLV_DECODE \
    uint8_t tlv_count; \
    iotdata_decoder_tlv_t tlv[IOTDATA_TLV_MAX];
#else
#define IOTDATA_FIELDS_DAT_TLV_DECODE
#endif
#define IOTDATA_TLV_CHAR_BITS 6
static inline bool iotdata_issixbit(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == ' ');
}
#if !defined(IOTDATA_NO_ENCODE)
#define IOTDATA_TLV_ENCODE_FROM_JSON_SCRATCH iotdata_encode_from_json_scratch_tlv_t tlv;
#else
#define IOTDATA_TLV_ENCODE_FROM_JSON_SCRATCH
#endif
#if !defined(IOTDATA_NO_DECODE)
#define IOTDATA_TLV_DECODE_TO_JSON_SCRATCH iotdata_decode_to_json_scratch_tlv_t tlv;
#else
#define IOTDATA_TLV_DECODE_TO_JSON_SCRATCH
#endif
/* TLV TAKES THE TOP BIT OF THE PRESENCE MASK, which is why this number is not 31 any more.
   The dense field ordinals grow upward from 0 and TLV sits at the top, out of their way; 31 was
   the top of a uint32_t mask, and splitting the three bundled fields took the library past 32
   fields, so iotdata_field_t became 64 bits and this had to follow it. It did not, for one
   build: IMAGE's ordinal landed on 31, IOTDATA_FIELD_VALID() then called IMAGE reserved, and
   every attempt to encode an image answered "no slot in this variant". The static assert beside
   IOTDATA_FIELD_COUNT in iotdata.h now refuses that collision rather than leaving it to be
   found. */
#define IOTDATA_FIELD_TLV_RESERVED_ID 63
#define IOTDATA_FIELDS_DEF_TLV(F)     F(TLV, IOTDATA_FIELD_TLV_RESERVED_ID)
/* IOTDATA_ERR_TLV_LEN_HIGH is raised against IOTDATA_TLV_VALUE_MAX -- what a packet can carry,
   not what the eight-bit length field can hold. See the comment at iotdata_encode_tlv.
   Three codes that sat here raising nothing have been removed (UNMATCHED, KV_MISMATCH and
   VALUE_HIGH): they described validation of the KV payloads that was never written, and an error
   a caller can switch on and never receive is worse than no error at all. */
#define IOTDATA_FIELDS_ERR_IDENTS_TLV(S) \
    S(IOTDATA_ERR_TLV_TYPE_HIGH) \
    S(IOTDATA_ERR_TLV_DATA_NULL) \
    S(IOTDATA_ERR_TLV_LEN_HIGH) \
    S(IOTDATA_ERR_TLV_FULL) \
    S(IOTDATA_ERR_TLV_STR_NULL) \
    S(IOTDATA_ERR_TLV_STR_LEN_HIGH) \
    S(IOTDATA_ERR_TLV_STR_CHAR_INVALID)
#define IOTDATA_FIELDS_ERR_STRINGS_TLV(D) \
    D(IOTDATA_ERR_TLV_TYPE_HIGH, "TLV type above maximum (63)") \
    D(IOTDATA_ERR_TLV_DATA_NULL, "TLV data pointer is NULL") \
    D(IOTDATA_ERR_TLV_LEN_HIGH, "TLV length above what a packet can carry") \
    D(IOTDATA_ERR_TLV_FULL, "TLV fields exhausted (max 8)") \
    D(IOTDATA_ERR_TLV_STR_NULL, "TLV string pointer is NULL") \
    D(IOTDATA_ERR_TLV_STR_LEN_HIGH, "TLV string too long (max 255 chars)") \
    D(IOTDATA_ERR_TLV_STR_CHAR_INVALID, "TLV string contains unencodable character")
#define IOTDATA_FIELD_RESERVED(id) ((id) == IOTDATA_FIELD_TLV)
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE) && !defined(_IOTDATA_NEED_BASE64_DECODE)
#define _IOTDATA_NEED_BASE64_DECODE
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE) && !defined(_IOTDATA_NEED_BASE64_ENCODE)
#define _IOTDATA_NEED_BASE64_ENCODE
#endif
#if !defined(IOTDATA_NO_ENCODE)
#define _IOTDATA_NEED_SIXBIT_ENCODE
#endif
#if !defined(IOTDATA_NO_DECODE)
#define _IOTDATA_NEED_SIXBIT_DECODE
#endif
#else
#define IOTDATA_FIELDS_DAT_TLV_ENCODE
#define IOTDATA_FIELDS_DAT_TLV_DECODE
#define IOTDATA_FIELDS_DEF_TLV(F)
#define IOTDATA_FIELDS_ERR_IDENTS_TLV(S)
#define IOTDATA_FIELDS_ERR_STRINGS_TLV(D)
#define IOTDATA_FIELD_RESERVED(id) 0
#define IOTDATA_TLV_ENCODE_FROM_JSON_SCRATCH
#define IOTDATA_TLV_DECODE_TO_JSON_SCRATCH
#endif

/* ---------------------------------------------------------------------------
 * TLV key-value payloads (KV) -- OFFERED encodings, not what a TLV MEANS
 *
 * THE BASE PROTOCOL PUTS NO STRUCTURE INSIDE A TLV. Its two formats say only how the bytes are
 * packed on the wire:
 *
 *   FMT_RAW     opaque binary. Whatever the type's owner puts there.
 *   FMT_STRING  6-bit packed characters, alphabet [A-Za-z0-9 ].
 *
 * The TLV mechanism is available to any implementation to carry anything at all, and nothing here
 * requires a payload to be key-value shaped.
 *
 * What follows are two OFFERED conventions for structuring those bytes -- available to anyone,
 * used by no one automatically. The NODE protocol (iotdata_node.h) standardises on them, which is
 * a decision of that layer and not of this one:
 *
 *   kvr, inside FMT_RAW      [K u8][L u8][V ...L bytes] repeated. K is a byte; V is opaque bytes
 *                            whose width and meaning belong to the key. The length makes every
 *                            pair skippable, so a reader can walk past keys it does not know --
 *                            which is what lets proprietary keys coexist with system ones, and a
 *                            mixed-vintage fleet interoperate.
 *
 *   kvs, inside FMT_STRING   "name value name value ..." space-delimited. Compact for bulk text
 *                            and readable on the wire. NOTE the 6-bit alphabet has NO PUNCTUATION,
 *                            so kvs cannot carry "1.4.2", "-5", or CSV; anything with punctuation,
 *                            a negative number, or binary must use kvr. The node protocol uses kvr
 *                            throughout for exactly that reason, and kvs is currently unused.
 *
 * Key space (kvr): bit 7 clear = SYSTEM, assigned by whoever owns the type; bit 7 set =
 * PROPRIETARY, free for a vendor or application. Type space: 0x00..0x1F = SYSTEM (assigned in
 * iotdata_node.h), 0x20..0x3F free.
 *
 * This file deliberately knows nothing about WHICH types or keys exist, only how a pair is framed.
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_TLV)

#define IOTDATA_TLV_TYPE_PROPRIETARY 0x20
#define IOTDATA_TLV_TYPE_SYSTEM_MAX  0x1F
#define IOTDATA_TLV_KEY_PROPRIETARY  0x80

#define IOTDATA_TLV_TYPE_PARTIAL     IOTDATA_TLV_TYPE_MAX

static inline bool iotdata_tlv_type_is_system(const uint8_t type) {
    return (type & IOTDATA_TLV_TYPE_PROPRIETARY) == 0;
}
static inline bool iotdata_tlv_key_is_system(const uint8_t key) {
    return (key & IOTDATA_TLV_KEY_PROPRIETARY) == 0;
}

/* --- kvr: build ---------------------------------------------------------- */

typedef struct {
    uint8_t *buf;
    size_t size;
    size_t len;
    bool overflow; /* sticky: a single check after the last add is enough */
} iotdata_kvr_t;

static inline void iotdata_kvr_init(iotdata_kvr_t *const kv, uint8_t *const buf, const size_t size) {
    kv->buf = buf;
    kv->size = size;
    kv->len = 0;
    kv->overflow = (buf == NULL);
}
static inline bool iotdata_kvr_add(iotdata_kvr_t *const kv, const uint8_t key, const void *const val, const uint8_t vlen) {
    if (kv->overflow || kv->len + 2u + (size_t)vlen > kv->size) {
        kv->overflow = true;
        return false;
    }
    kv->buf[kv->len++] = key;
    kv->buf[kv->len++] = vlen;
    if (vlen > 0 && val != NULL) {
        memcpy(&kv->buf[kv->len], val, vlen);
        kv->len += vlen;
    }
    return true;
}
static inline bool iotdata_kvr_add_flag(iotdata_kvr_t *const kv, const uint8_t key) { /* present, no value */
    return iotdata_kvr_add(kv, key, NULL, 0);
}
static inline bool iotdata_kvr_add_u8(iotdata_kvr_t *const kv, const uint8_t key, const uint8_t v) {
    return iotdata_kvr_add(kv, key, &v, 1);
}
static inline bool iotdata_kvr_add_u16(iotdata_kvr_t *const kv, const uint8_t key, const uint16_t v) {
    return iotdata_kvr_add(kv, key, (const uint8_t[2]){ (uint8_t)(v >> 8), (uint8_t)v }, 2);
}
static inline bool iotdata_kvr_add_u32(iotdata_kvr_t *const kv, const uint8_t key, const uint32_t v) {
    return iotdata_kvr_add(kv, key, (const uint8_t[4]){ (uint8_t)(v >> 24), (uint8_t)(v >> 16), (uint8_t)(v >> 8), (uint8_t)v }, 4);
}
static inline bool iotdata_kvr_add_i8(iotdata_kvr_t *const kv, const uint8_t key, const int8_t v) {
    return iotdata_kvr_add_u8(kv, key, (uint8_t)v);
}
static inline bool iotdata_kvr_add_str(iotdata_kvr_t *const kv, const uint8_t key, const char *const s) {
    const size_t n = (s != NULL) ? strlen(s) : 0;
    if (n > 255) {
        kv->overflow = true;
        return false;
    }
    return iotdata_kvr_add(kv, key, s, (uint8_t)n);
}

/* --- kvr: walk ----------------------------------------------------------- */

/* Iterate the pairs in a kvr payload. Returns false at the end, or on a truncated pair (stopping
   rather than running off the buffer). Usage:
     size_t cur = 0; uint8_t k, n; const uint8_t *v;
     while (iotdata_kvr_next(buf, len, &cur, &k, &v, &n)) { ... } */
static inline bool iotdata_kvr_next(const uint8_t *const buf, const size_t len, size_t *const cursor, uint8_t *const key, const uint8_t **val, uint8_t *const vlen) {
    if (buf == NULL || cursor == NULL || *cursor + 2u > len)
        return false;
    const uint8_t k = buf[*cursor], l = buf[*cursor + 1];
    if (*cursor + 2u + (size_t)l > len)
        return false;
    if (key != NULL)
        *key = k;
    if (vlen != NULL)
        *vlen = l;
    if (val != NULL)
        *val = &buf[*cursor + 2u];
    *cursor += 2u + (size_t)l;
    return true;
}

static inline uint8_t iotdata_kvr_u8(const uint8_t *const v, const uint8_t n, const uint8_t dflt) {
    return (n == 1) ? v[0] : dflt;
}
static inline int8_t iotdata_kvr_i8(const uint8_t *const v, const uint8_t n, const int8_t dflt) {
    return (n == 1) ? (int8_t)v[0] : dflt;
}
static inline uint16_t iotdata_kvr_u16(const uint8_t *const v, const uint8_t n, const uint16_t dflt) {
    return (n == 2) ? (uint16_t)(((uint16_t)v[0] << 8) | v[1]) : dflt;
}
static inline uint32_t iotdata_kvr_u32(const uint8_t *const v, const uint8_t n, const uint32_t dflt) {
    return (n == 4) ? (((uint32_t)v[0] << 24) | ((uint32_t)v[1] << 16) | ((uint32_t)v[2] << 8) | v[3]) : dflt;
}
static inline size_t iotdata_kvr_str(const uint8_t *const v, const uint8_t n, char *const out, const size_t out_size) {
    if (out == NULL || out_size == 0)
        return 0;
    const size_t c = ((size_t)n < out_size - 1) ? (size_t)n : out_size - 1;
    if (c > 0 && v != NULL)
        memcpy(out, v, c);
    out[c] = '\0';
    return c;
}

/* --- kvs: build + walk (space-delimited text; alphanumeric values only) --- */

typedef struct {
    char *buf;
    size_t size;
    size_t len;
    bool overflow;
} iotdata_kvs_t;

static inline void iotdata_kvs_init(iotdata_kvs_t *const kv, char *const buf, const size_t size) {
    kv->buf = buf;
    kv->size = size;
    kv->len = 0;
    kv->overflow = (buf == NULL || size == 0);
    if (!kv->overflow)
        buf[0] = '\0';
}
static inline bool iotdata_kvs_add(iotdata_kvs_t *const kv, const char *const name, const char *const value) {
    if (kv->overflow || name == NULL || value == NULL) {
        kv->overflow = true;
        return false;
    }
    const size_t nl = strlen(name), vl = strlen(value);
    const size_t need = (kv->len > 0 ? 1u : 0u) + nl + 1u + vl;
    if (kv->len + need + 1u > kv->size) {
        kv->overflow = true;
        return false;
    }
    if (kv->len > 0)
        kv->buf[kv->len++] = ' ';
    memcpy(&kv->buf[kv->len], name, nl);
    kv->len += nl;
    kv->buf[kv->len++] = ' ';
    memcpy(&kv->buf[kv->len], value, vl);
    kv->len += vl;
    kv->buf[kv->len] = '\0';
    return true;
}

/* Iterate "name value" pairs in a kvs payload. `name`/`value` point into `str` and are NOT
   NUL-terminated -- use the paired lengths. Returns false at the end or on an odd trailing token. */
static inline bool iotdata_kvs_next(const char *const str, size_t *const cursor, const char **name, size_t *const name_len, const char **value, size_t *const value_len) {
    if (str == NULL || cursor == NULL)
        return false;
    size_t i = *cursor;
    while (str[i] == ' ')
        i++;
    if (str[i] == '\0')
        return false;
    const size_t ns = i;
    while (str[i] != '\0' && str[i] != ' ')
        i++;
    const size_t ne = i;
    while (str[i] == ' ')
        i++;
    if (str[i] == '\0')
        return false; /* key without a value: malformed, stop */
    const size_t vs = i;
    while (str[i] != '\0' && str[i] != ' ')
        i++;
    if (name != NULL)
        *name = &str[ns];
    if (name_len != NULL)
        *name_len = ne - ns;
    if (value != NULL)
        *value = &str[vs];
    if (value_len != NULL)
        *value_len = i - vs;
    *cursor = i;
    return true;
}

#endif /* IOTDATA_ENABLE_TLV */

#endif /* IOTDATA_FIELDS_TLV_H_DECLARATIONS */

#if defined(IOTDATA_FIELDS_TLV_IMPLEMENTATION) && !defined(IOTDATA_FIELDS_TLV_H_DEFINITIONS)
#define IOTDATA_FIELDS_TLV_H_DEFINITIONS

/* =========================================================================
 * Field TLV
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_TLV)
#if !defined(IOTDATA_NO_ENCODE)
iotdata_status_t iotdata_encode_tlv(iotdata_encoder_t *const enc, const uint8_t type, const uint8_t *const data, const uint8_t length) {
    CHECK_CTX_ACTIVE(enc);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (type > IOTDATA_TLV_TYPE_MAX)
        return IOTDATA_ERR_TLV_TYPE_HIGH;
    /* a zero-length TLV is a legitimate message -- an empty RECEIVE says "listening, for anything"
       -- and the packer never dereferences data when length is 0, so NULL is allowed there */
    if (!data && length > 0)
        return IOTDATA_ERR_TLV_DATA_NULL;
    /* THE BOUND THAT BINDS IS THE FRAME'S, NOT THE LENGTH FIELD'S. A length of 255 fits the
       eight-bit field, so comparing against IOTDATA_TLV_LENGTH_MAX can never fail -- which is
       why this check read as unreachable and was left out. What a packet can actually carry is
       IOTDATA_TLV_VALUE_MAX: the compiled maximum packet size less the header, the presence
       bytes and this TLV's own framing. That came to 246, and the whole system already sizes
       its buffers by it -- the node variant TLV, the runtime key-value buffer, the control
       payload, the TSA encoder -- while the library itself never checked. A caller handing over
       247 was accepted here and failed later as a buffer error naming nothing.
       It is deliberately a STATIC bound rather than the space actually left: it depends only on
       the TLV and the build, so the same call answers the same way wherever it appears in a
       sequence. Whether a particular packet fits a particular buffer is a different question,
       answered at encode_end, and answered there as BUF_TOO_SMALL. */
    if (length > IOTDATA_TLV_VALUE_MAX)
        return IOTDATA_ERR_TLV_LEN_HIGH;
#endif
    if (enc->tlv_count >= IOTDATA_TLV_MAX)
        return IOTDATA_ERR_TLV_FULL;
    const int idx = enc->tlv_count++;
    enc->tlv[idx].format = IOTDATA_TLV_FMT_RAW;
    enc->tlv[idx].type = type;
    enc->tlv[idx].length = length;
    enc->tlv[idx].data = data;
    IOTDATA_FIELD_SET(enc->fields, IOTDATA_FIELD_TLV);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_tlv_string(iotdata_encoder_t *const enc, const uint8_t type, const char *const str) {
    CHECK_CTX_ACTIVE(enc);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (type > IOTDATA_TLV_TYPE_MAX)
        return IOTDATA_ERR_TLV_TYPE_HIGH;
    if (!str)
        return IOTDATA_ERR_TLV_STR_NULL;
    const size_t slen = strlen(str);
    if (slen > IOTDATA_TLV_STR_LEN_MAX)
        return IOTDATA_ERR_TLV_STR_LEN_HIGH;
    for (size_t i = 0; i < slen; i++)
        if (char_to_sixbit(str[i]) < 0)
            return IOTDATA_ERR_TLV_STR_CHAR_INVALID;
#else
    size_t slen = strlen(str);
    if (slen > IOTDATA_TLV_STR_LEN_MAX)
        slen = IOTDATA_TLV_STR_LEN_MAX;
#endif
    if (enc->tlv_count >= IOTDATA_TLV_MAX)
        return IOTDATA_ERR_TLV_FULL;
    const int idx = enc->tlv_count++;
    enc->tlv[idx].format = IOTDATA_TLV_FMT_STRING;
    enc->tlv[idx].type = type;
    enc->tlv[idx].length = (uint8_t)slen;
    enc->tlv[idx].str = str;
    IOTDATA_FIELD_SET(enc->fields, IOTDATA_FIELD_TLV);
    return IOTDATA_OK;
}
static bool pack_tlv(uint8_t *const buf, const size_t bb, size_t *const bp, const iotdata_encoder_t *const enc) {
    for (int i = 0; i < enc->tlv_count; i++) {
        if (!bits_write(buf, bb, bp, enc->tlv[i].format, IOTDATA_TLV_FMT_BITS))
            return false;
        if (!bits_write(buf, bb, bp, enc->tlv[i].type, IOTDATA_TLV_TYPE_BITS))
            return false;
        if (!bits_write(buf, bb, bp, (i < enc->tlv_count - 1) ? 1 : 0, IOTDATA_TLV_MORE_BITS))
            return false;
        if (!bits_write(buf, bb, bp, enc->tlv[i].length, IOTDATA_TLV_LENGTH_BITS))
            return false;
        for (int j = 0, l = enc->tlv[i].format == IOTDATA_TLV_FMT_STRING ? IOTDATA_TLV_CHAR_BITS : 8; j < enc->tlv[i].length; j++)
            if (!bits_write(buf, bb, bp, enc->tlv[i].format == IOTDATA_TLV_FMT_STRING ? (uint32_t)char_to_sixbit(enc->tlv[i].str[j]) : enc->tlv[i].data[j], (uint8_t)l))
                return false;
    }
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_tlv(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_decoder_t *const dec) {
    bool more = true;
    while (more) {
        if (*bp + IOTDATA_TLV_HEADER_BITS > bb)
            return false;
        const uint8_t format = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_FMT_BITS);
        const uint8_t type = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_TYPE_BITS);
        more = bits_read(buf, bb, bp, IOTDATA_TLV_MORE_BITS) != 0;
        const uint8_t length = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_LENGTH_BITS);
        const uint8_t bpv = format == IOTDATA_TLV_FMT_STRING ? IOTDATA_TLV_CHAR_BITS : 8;
        if (*bp + (size_t)bpv * length > bb)
            return false;
        if (dec->tlv_count >= IOTDATA_TLV_MAX) {
            *bp += (size_t)bpv * length;
        } else {
            const int idx = dec->tlv_count++;
            dec->tlv[idx].format = format;
            dec->tlv[idx].type = type;
            dec->tlv[idx].length = length;
            if (format == IOTDATA_TLV_FMT_STRING) {
                for (int j = 0; j < length; j++)
                    dec->tlv[idx].str[j] = sixbit_to_char((uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_CHAR_BITS));
                dec->tlv[idx].str[length] = '\0';
            } else {
                for (int j = 0; j < length; j++)
                    dec->tlv[idx].raw[j] = (uint8_t)bits_read(buf, bb, bp, 8);
            }
        }
    }
    return true;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void _json_set_tlv_data(cJSON *obj, const iotdata_decoder_tlv_t *t, iotdata_decode_to_json_scratch_t *const scratch) {
    cJSON_AddStringToObject(obj, "format", t->format == IOTDATA_TLV_FMT_STRING ? "string" : "raw");
    if (t->format == IOTDATA_TLV_FMT_STRING)
        cJSON_AddStringToObject(obj, "data", t->str);
    else
        cJSON_AddStringToObject(obj, "data", _b64_encode(t->raw, t->length, scratch->tlv.b64));
}
static void json_set_tlv(cJSON *root, const iotdata_decoder_t *const dec, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    cJSON *arr = cJSON_AddArrayToObject(root, label);
    for (int i = 0; i < dec->tlv_count; i++) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "type", dec->tlv[i].type);
        _json_set_tlv_data(obj, &dec->tlv[i], scratch);
        cJSON_AddItemToArray(arr, obj);
    }
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t _json_get_tlv_generic(cJSON *item, iotdata_encoder_t *const enc, const int tidx, const uint8_t type, iotdata_encode_from_json_scratch_tlv_t *const scratch) {
    cJSON *format = cJSON_GetObjectItem(item, "format");
    cJSON *data = cJSON_GetObjectItem(item, "data");
    if (!data || !cJSON_IsString(data))
        return IOTDATA_OK; /* skip malformed */
    if (strcmp(format ? format->valuestring : "raw", "string") == 0) {
        snprintf(scratch->str[tidx], IOTDATA_TLV_STR_LEN_MAX + 1, "%s", data->valuestring);
        return iotdata_encode_tlv_string(enc, type, scratch->str[tidx]);
    } else
        return iotdata_encode_tlv(enc, type, scratch->raw[tidx], (uint8_t)_b64_decode(data->valuestring, scratch->raw[tidx], IOTDATA_TLV_LENGTH_MAX));
}
static iotdata_status_t json_get_tlv(cJSON *root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_tlv_t *const scratch) {
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j || !cJSON_IsArray(j))
        return IOTDATA_OK;
    cJSON *item;
    int tidx = 0;
    cJSON_ArrayForEach(item, j) {
        if (tidx >= IOTDATA_TLV_MAX)
            break;
        cJSON *j_type = cJSON_GetObjectItem(item, "type");
        if (j_type) {
            const uint8_t type = (uint8_t)j_type->valueint;
            iotdata_status_t rc;
            rc = _json_get_tlv_generic(item, enc, tidx, type, scratch);
            if (rc != IOTDATA_OK)
                return rc;
            tidx++;
        }
    }
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int _dump_tlv_data(size_t *const bp, iotdata_dump_t *const dump, int n, const uint8_t format, const uint8_t length, const int tlv_idx, const char *const name) {
    const size_t data_bits = (format == IOTDATA_TLV_FMT_STRING) ? (size_t)length * IOTDATA_TLV_CHAR_BITS : (size_t)length * 8;
    snprintf(dump->_name_buf, sizeof(dump->_name_buf), "tlv[%d].%s", tlv_idx, name);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "(%" PRIu32 " bits)", (uint32_t)data_bits); // XXX string
    n = dump_add(dump, n, *bp, data_bits, 0, dump->_dec_buf, format == IOTDATA_TLV_FMT_STRING ? "6-bit chars" : "raw bytes", dump->_name_buf);
    *bp += data_bits;
    return n;
}
static int dump_tlv(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    bool more = true;
    int tlv_idx = 0;
    while (more && *bp + IOTDATA_TLV_HEADER_BITS <= bb) {
        size_t s = *bp;
        const uint8_t format = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_FMT_BITS);
        const uint8_t type = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_TYPE_BITS);
        more = bits_read(buf, bb, bp, IOTDATA_TLV_MORE_BITS) != 0;
        snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%s type=0x%02" PRIX8 " more=%d", format == IOTDATA_TLV_FMT_STRING ? "str" : "raw", type, more ? 1 : 0);
        snprintf(dump->_name_buf, sizeof(dump->_name_buf), "tlv[%d].hdr", tlv_idx);
        n = dump_add(dump, n, s, IOTDATA_TLV_FMT_BITS + IOTDATA_TLV_TYPE_BITS + IOTDATA_TLV_MORE_BITS, 0, dump->_dec_buf, "format+type+more", dump->_name_buf);
        s = *bp;
        const uint8_t length = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_LENGTH_BITS);
        snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8, length);
        snprintf(dump->_name_buf, sizeof(dump->_name_buf), "tlv[%d].len", tlv_idx);
        n = dump_add(dump, n, s, IOTDATA_TLV_LENGTH_BITS, length, dump->_dec_buf, "0..255", dump->_name_buf);
        if (length > 0) {
            n = _dump_tlv_data(bp, dump, n, format, length, tlv_idx, "data");
        }
        tlv_idx++;
    }
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void _print_tlv_data(const iotdata_decoder_tlv_t *t, iotdata_buf_t *const bp, const int i) {
    /* Application-defined TLVs (0x20+) */
    if (t->format == IOTDATA_TLV_FMT_STRING) {
        bprintf(bp, "    [%d] type=%" PRIu8 " str(%" PRIu8 ")=\"%s\"\n", i, t->type, t->length, t->str);
    } else {
        bprintf(bp, "    [%d] type=%" PRIu8 " raw(%" PRIu8 ")=", i, t->type, t->length);
        for (int j = 0; j < t->length && j < 16; j++)
            bprintf(bp, "%02" PRIX8, t->raw[j]);
        if (t->length > 16)
            bprintf(bp, "...");
        bprintf(bp, "\n");
    }
}
static void print_tlv(const iotdata_decoder_t *const dec, iotdata_buf_t *const bp, const char *const label) {
    bprintf(bp, "  %s: %" PRIu8 " TLV entries\n", label, dec->tlv_count);
    for (int i = 0; i < dec->tlv_count; i++)
        _print_tlv_data(&dec->tlv[i], bp, i);
}
#endif

/* TLV wrappers matching generic field-op signatures (close as practical) so
 * that the generic encode/decode/dump/print/json paths in iotdata.c stay
 * field-agnostic. When IOTDATA_ENABLE_TLV is unset the #else branch below
 * provides empty stubs with the same signatures, so call sites compile
 * unconditionally. */

#if !defined(IOTDATA_NO_ENCODE)
static void iotdata_encode_tlv_begin(iotdata_encoder_t *const enc) {
    enc->tlv_count = 0;
}
static void iotdata_encode_tlv_pres(const iotdata_encoder_t *const enc, uint8_t *const pres) {
    if (IOTDATA_FIELD_PRESENT(enc->fields, IOTDATA_FIELD_TLV))
        pres[0] |= IOTDATA_PRES_TLV;
}
static bool iotdata_encode_tlv_data(uint8_t *const buf, const size_t bb, size_t *const bp, const iotdata_encoder_t *const enc) {
    if (IOTDATA_FIELD_PRESENT(enc->fields, IOTDATA_FIELD_TLV))
        return pack_tlv(buf, bb, bp, enc);
    return true;
}
#endif

#if !defined(IOTDATA_NO_DECODE)
static bool iotdata_decode_tlv_data(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_decoder_t *const dec, const uint8_t pres0) {
    dec->tlv_count = 0;
    if ((pres0 & IOTDATA_PRES_TLV) != 0) {
        IOTDATA_FIELD_SET(dec->fields, IOTDATA_FIELD_TLV);
        return unpack_tlv(buf, bb, bp, dec);
    }
    return true;
}
#endif

#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void iotdata_decode_tlv_json_set(cJSON *const root, const iotdata_decoder_t *const dec, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    if (IOTDATA_FIELD_PRESENT(dec->fields, IOTDATA_FIELD_TLV))
        json_set_tlv(root, dec, label, scratch);
}
#endif

#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t iotdata_encode_tlv_json_get(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    return json_get_tlv(root, enc, label, &scratch->tlv);
}
#endif

#if !defined(IOTDATA_NO_DUMP)
static int iotdata_dump_tlv(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label, const uint8_t pres0) {
    if ((pres0 & IOTDATA_PRES_TLV) != 0)
        return dump_tlv(buf, bb, bp, dump, n, label);
    return n;
}
#endif

#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void iotdata_print_tlv(const iotdata_decoder_t *const dec, iotdata_buf_t *const bp, const char *const label) {
    if (IOTDATA_FIELD_PRESENT(dec->fields, IOTDATA_FIELD_TLV))
        print_tlv(dec, bp, label);
}
#endif

#else /* !IOTDATA_ENABLE_TLV: empty stubs with matching signatures */

#if !defined(IOTDATA_NO_ENCODE)
static inline void iotdata_encode_tlv_begin(iotdata_encoder_t *const enc) {
    (void)enc;
}
static inline void iotdata_encode_tlv_pres(const iotdata_encoder_t *const enc, uint8_t *const pres) {
    (void)enc;
    (void)pres;
}
static inline bool iotdata_encode_tlv_data(uint8_t *const buf, const size_t bb, size_t *const bp, const iotdata_encoder_t *const enc) {
    (void)buf;
    (void)bb;
    (void)bp;
    (void)enc;
    return true;
}
#endif

#if !defined(IOTDATA_NO_DECODE)
static inline bool iotdata_decode_tlv_data(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_decoder_t *const dec, const uint8_t pres0) {
    (void)buf;
    (void)bb;
    (void)bp;
    (void)dec;
    (void)pres0;
    return true;
}
#endif

#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static inline void iotdata_decode_tlv_json_set(cJSON *const root, const iotdata_decoder_t *const dec, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    (void)root;
    (void)dec;
    (void)label;
    (void)scratch;
}
#endif

#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static inline iotdata_status_t iotdata_encode_tlv_json_get(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)root;
    (void)enc;
    (void)label;
    (void)scratch;
    return IOTDATA_OK;
}
#endif

#if !defined(IOTDATA_NO_DUMP)
static inline int iotdata_dump_tlv(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label, const uint8_t pres0) {
    (void)buf;
    (void)bb;
    (void)bp;
    (void)dump;
    (void)label;
    (void)pres0;
    return n;
}
#endif

#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static inline void iotdata_print_tlv(const iotdata_decoder_t *const dec, iotdata_buf_t *const bp, const char *const label) {
    (void)dec;
    (void)bp;
    (void)label;
}
#endif

#endif

#endif /* IOTDATA_FIELDS_TLV_IMPLEMENTATION */
