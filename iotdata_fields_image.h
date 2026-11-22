/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * iotdata_fields_image.h -- the IMAGE field, hand-written
 *
 * IMAGE *IS* A FIELD: it carries an identifier, a presence slot, an enum member
 * and an ops entry, and it appears in the generated registry like any other
 * field. Only its CODE is not generated. (iotdata_fields_tlv.h shares the
 * prefix for consistency, but TLV is not a field -- it has none of those.)
 *
 * It is not generated because it differs from every other field in STRUCTURE
 * rather than in spelling, five times over:
 *
 *   - the encoder and the decoder carry DIFFERENT storage. An encoder holds a
 *     const pointer to pixels it does not own; a decoder holds the 254 bytes
 *     itself. Two DAT macros, two typedefs, two MEM macros.
 *   - three scratch types, where no other field has one.
 *   - two macros feeding the library-wide JSON scratch aggregates.
 *   - eight public declarations that are not field operations at all but a
 *     compression library: run-length and heatshrink LZSS, each with its own
 *     compressor and decompressor.
 *   - decode takes a caller-provided scratch as an extra parameter.
 *
 * Generating it would have meant five new mechanisms in the template language
 * serving exactly one field. The specification still DESCRIBES it:
 * specification/iotdata-fields-image.xml carries its identifier, quantities,
 * errors and documentation, the generator emits its registry row and checks
 * its identifier against every other field's. It simply emits no C for it.
 *
 * TWO PHASES, ONE FILE, as iotdata_fields_tlv.h does and for the same reason: the
 * types have to be visible to iotdata.h, because the encoder and decoder carry
 * image storage, while the bodies call bits_write, bits_read, bprintf,
 * dump_add and CHECK_CTX_ACTIVE, which iotdata.c defines further down.
 */

#if !defined(IOTDATA_FIELDS_IMAGE_H_DECLARATIONS)
#define IOTDATA_FIELDS_IMAGE_H_DECLARATIONS

/* ---------------------------------------------------------------------------
 * Field IMAGE
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_IMAGE)
#define IOTDATA_IMAGE_DATA_MAX         254 /* max pixel data after control byte */
/* Control byte: format(2) | size(2) | compression(2) | flags(2) */
#define IOTDATA_IMAGE_FMT_BILEVEL      0 /* 1 bpp */
#define IOTDATA_IMAGE_FMT_GREY4        1 /* 2 bpp */
#define IOTDATA_IMAGE_FMT_GREY16       2 /* 4 bpp */
#define IOTDATA_IMAGE_SIZE_24x18       0
#define IOTDATA_IMAGE_SIZE_32x24       1
#define IOTDATA_IMAGE_SIZE_48x36       2
#define IOTDATA_IMAGE_SIZE_64x48       3
#define IOTDATA_IMAGE_COMP_RAW         0
#define IOTDATA_IMAGE_COMP_RLE         1
#define IOTDATA_IMAGE_COMP_HEATSHRINK  2
#define IOTDATA_IMAGE_FLAG_FRAGMENT    (1U << 1)
#define IOTDATA_IMAGE_FLAG_INVERT      (1U << 0)
/* Heatshrink fixed parameters */
#define IOTDATA_IMAGE_HS_WINDOW_SZ2    8 /* 256-byte window */
#define IOTDATA_IMAGE_HS_LOOKAHEAD_SZ2 4 /* 16-byte lookahead */
#if !defined(IOTDATA_NO_ENCODE)
#define IOTDATA_FIELDS_DAT_IMAGE_ENCODE \
    uint8_t image_pixel_format; \
    uint8_t image_size_tier; \
    uint8_t image_compression; \
    uint8_t image_flags; \
    const uint8_t *image_data; \
    uint8_t image_data_len;
typedef struct {
    IOTDATA_FIELDS_DAT_IMAGE_ENCODE
} iotdata_field_image_enc_t;
#define IOTDATA_FIELDS_MEM_IMAGE_ENCODE iotdata_field_image_enc_t image;
typedef struct {
    union {
        uint8_t data[IOTDATA_IMAGE_DATA_MAX];
    };
} iotdata_encode_to_json_scratch_image_t;
size_t iotdata_image_pixel_count(uint8_t size_tier);
uint8_t iotdata_image_bpp(uint8_t pixel_format);
size_t iotdata_image_bytes(uint8_t pixel_format, uint8_t size_tier);
/* RLE: returns output bytes written, 0 on error */
size_t iotdata_image_rle_compress(const uint8_t *pixels, size_t pixel_count, uint8_t bpp, uint8_t *out, size_t out_max);
size_t iotdata_image_rle_decompress(const uint8_t *compressed, size_t comp_len, uint8_t bpp, uint8_t *pixels, size_t pixel_buf_bytes);
/* Heatshrink LZSS (w=8, l=4): returns output bytes written, 0 on error */
size_t iotdata_image_hs_compress(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_max);
size_t iotdata_image_hs_decompress(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_max);
#else
#define IOTDATA_FIELDS_DAT_IMAGE_ENCODE
#define IOTDATA_FIELDS_MEM_IMAGE_ENCODE
#endif
#if !defined(IOTDATA_NO_DECODE)
#define IOTDATA_FIELDS_DAT_IMAGE_DECODE \
    uint8_t image_pixel_format; \
    uint8_t image_size_tier; \
    uint8_t image_compression; \
    uint8_t image_flags; \
    uint8_t image_data[IOTDATA_IMAGE_DATA_MAX]; \
    uint8_t image_data_len;
typedef struct {
    IOTDATA_FIELDS_DAT_IMAGE_DECODE
} iotdata_field_image_dec_t;
#define IOTDATA_FIELDS_MEM_IMAGE_DECODE iotdata_field_image_dec_t image;
/* THE CALLER'S LANDING AREA FOR DECODED PIXELS. Every other field's accessor answers in
   out-parameters, but an image is up to 254 bytes and has to be put somewhere; an ORDERED build
   has no decoder storage to put it in. So the caller provides the room and the library keeps the
   shape of it to itself -- declare one, pass its address, never look inside. It is required in
   every build, including the default one where it goes unwritten, so that a caller cannot pass
   NULL today and discover on the day it switches to ORDERED that there was nowhere to decode
   into. */
typedef union {
    iotdata_field_image_dec_t _image; /* named, not spelled out, so the size and alignment are
                                         right without the caller having any reason to look */
} iotdata_field_image_scratch_t;
typedef struct {
    union {
        char b64[((IOTDATA_IMAGE_DATA_MAX + 2) / 3) * 4 + 1];
    };
} iotdata_decode_to_json_scratch_image_t;
#else
#define IOTDATA_FIELDS_DAT_IMAGE_DECODE
#define IOTDATA_FIELDS_MEM_IMAGE_DECODE
#endif
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_image(struct iotdata_encoder_t_ *enc, uint8_t pixel_format, uint8_t size_tier, uint8_t compression, uint8_t flags, const uint8_t *data, uint8_t data_len);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_image_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t pixel_format, uint8_t size_tier, uint8_t compression, uint8_t flags, const uint8_t *data, uint8_t data_len);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_image(struct iotdata_decoder_t_ *dec, uint8_t *pixel_format, uint8_t *size_tier, uint8_t *compression, uint8_t *flags, const uint8_t **data, uint8_t *data_len, iotdata_field_image_scratch_t *scratch);
#endif
#define IOTDATA_FIELD_ID_IMAGE      0x0090u
#define IOTDATA_FIELDS_DEF_IMAGE(F) F(IMAGE)
#define IOTDATA_FIELDS_OPS_IMAGE(E) E(IMAGE, _iotdata_field_def_image)
#define IOTDATA_FIELDS_ERR_IDENTS_IMAGE(S) \
    S(IOTDATA_ERR_IMAGE_FORMAT_HIGH) \
    S(IOTDATA_ERR_IMAGE_SIZE_HIGH) \
    S(IOTDATA_ERR_IMAGE_COMPRESSION_HIGH) \
    S(IOTDATA_ERR_IMAGE_DATA_NULL) \
    S(IOTDATA_ERR_IMAGE_DATA_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_IMAGE(D) \
    D(IOTDATA_ERR_IMAGE_FORMAT_HIGH, "Image pixel format above 2") \
    D(IOTDATA_ERR_IMAGE_SIZE_HIGH, "Image size tier above 3") \
    D(IOTDATA_ERR_IMAGE_COMPRESSION_HIGH, "Image compression above 2") \
    D(IOTDATA_ERR_IMAGE_DATA_NULL, "Image data pointer is NULL") \
    D(IOTDATA_ERR_IMAGE_DATA_HIGH, "Image data exceeds 254 bytes")
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE) && !defined(_IOTDATA_NEED_BASE64_DECODE)
#define _IOTDATA_NEED_BASE64_DECODE
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE) && !defined(_IOTDATA_NEED_BASE64_ENCODE)
#define _IOTDATA_NEED_BASE64_ENCODE
#endif
#if !defined(IOTDATA_NO_ENCODE)
#define IOTDATA_IMAGE_ENCODE_FROM_JSON_SCRATCH iotdata_encode_to_json_scratch_image_t image;
#else
#define IOTDATA_IMAGE_ENCODE_FROM_JSON_SCRATCH
#endif
#if !defined(IOTDATA_NO_DECODE)
#define IOTDATA_IMAGE_DECODE_TO_JSON_SCRATCH iotdata_decode_to_json_scratch_image_t image;
#else
#define IOTDATA_IMAGE_DECODE_TO_JSON_SCRATCH
#endif
#else
#define IOTDATA_FIELDS_DAT_IMAGE_ENCODE
#define IOTDATA_FIELDS_MEM_IMAGE_ENCODE
#define IOTDATA_FIELDS_DAT_IMAGE_DECODE
#define IOTDATA_FIELDS_MEM_IMAGE_DECODE
#define IOTDATA_FIELDS_DEF_IMAGE(F)
#define IOTDATA_FIELDS_OPS_IMAGE(E)
#define IOTDATA_FIELDS_ERR_IDENTS_IMAGE(S)
#define IOTDATA_FIELDS_ERR_STRINGS_IMAGE(D)
#define IOTDATA_IMAGE_ENCODE_FROM_JSON_SCRATCH
#define IOTDATA_IMAGE_DECODE_TO_JSON_SCRATCH
#endif

#endif /* IOTDATA_FIELDS_IMAGE_H_DECLARATIONS */

#if defined(IOTDATA_FIELDS_IMAGE_IMPLEMENTATION) && !defined(IOTDATA_FIELDS_IMAGE_H_DEFINITIONS)
#define IOTDATA_FIELDS_IMAGE_H_DEFINITIONS

/* =========================================================================
 * Field IMAGE
 *
 * Variable-length field: 8-bit length prefix + control byte + pixel data.
 * First variable-length data field in the protocol.
 *
 * Wire layout:
 *   [Length:8] [Control:8] [PixelData: Length-1 bytes]
 *
 * Length = number of bytes following (control + pixel data).
 * Total field size = 1 + Length bytes = (1 + Length) * 8 bits.
 *
 * Control byte:
 *   bits 7-6: pixel format (0=bilevel/1bpp, 1=grey4/2bpp, 2=grey16/4bpp)
 *   bits 5-4: size tier (0=24x18, 1=32x24, 2=48x36, 3=64x48)
 *   bits 3-2: compression (0=raw, 1=RLE, 2=heatshrink)
 *   bits 1-0: flags (bit1=fragment, bit0=invert)
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_IMAGE)
static const uint8_t _image_widths[] = { 24, 32, 48, 64 }, _image_heights[] = { 18, 24, 36, 48 }, _image_bits[] = { 1, 2, 4 };
size_t iotdata_image_pixel_count(uint8_t size_tier) {
    return size_tier < sizeof(_image_widths) / sizeof(_image_widths[0]) ? (size_t)_image_widths[size_tier] * (size_t)_image_heights[size_tier] : 0;
}
uint8_t iotdata_image_bpp(uint8_t pixel_format) {
    return pixel_format < sizeof(_image_bits) / sizeof(_image_bits[0]) ? _image_bits[pixel_format] : 0;
}
size_t iotdata_image_bytes(uint8_t pixel_format, uint8_t size_tier) {
    return (iotdata_image_pixel_count(size_tier) * (size_t)iotdata_image_bpp(pixel_format) + 7) / 8;
}
static uint8_t _pixel_get(const uint8_t *buf, size_t idx, uint8_t bpp) {
    if (bpp == 1)
        return (buf[idx / 8] >> (7 - (idx % 8))) & 1;
    else if (bpp == 2)
        return (buf[idx / 4] >> (6 - (idx % 4) * 2)) & 3;
    else if (bpp == 4)
        return (idx & 1) ? buf[idx / 2] & 0x0F : buf[idx / 2] >> 4;
    return 0;
}
static void _pixel_set(uint8_t *buf, size_t idx, uint8_t val, uint8_t bpp) {
    if (bpp == 1)
        buf[idx / 8] = (uint8_t)((buf[idx / 8] & ~(1U << (7 - (idx % 8)))) | ((val & 1) << (7 - (idx % 8))));
    else if (bpp == 2)
        buf[idx / 4] = (uint8_t)((buf[idx / 4] & ~(3U << (6 - (idx % 4) * 2))) | ((val & 3) << (6 - (idx % 4) * 2)));
    else if (bpp == 4)
        buf[idx / 2] = (uint8_t)(idx & 1 ? (buf[idx / 2] & 0xF0) | (val & 0x0F) : (buf[idx / 2] & 0x0F) | ((val & 0x0F) << 4));
}

/* -------------------------------------------------------------------------
 * RLE compression/decompression
 *
 * Bilevel (1bpp):
 *   1-byte runs: bit7 = pixel value, bits 6-0 = count-1 (1..128 pixels)
 *
 * Greyscale (2bpp, 4bpp):
 *   2-byte runs: [value:8] [count-1:8] (1..256 pixels)
 * ------------------------------------------------------------------------- */
size_t iotdata_image_rle_compress(const uint8_t *pixels, size_t pixel_count, uint8_t bpp, uint8_t *out, size_t out_max) {
    if (!pixels || !out || pixel_count == 0 || bpp == 0)
        return 0;
    size_t op = 0;
    if (bpp == 1) {
        uint8_t cur = _pixel_get(pixels, 0, 1);
        size_t count = 1;
        for (size_t i = 1; i < pixel_count; i++) {
            const uint8_t px = _pixel_get(pixels, i, 1);
            if (px == cur && count < (1 << 7))
                count++;
            else {
                if (op >= out_max)
                    return 0;
                out[op++] = (uint8_t)((cur << 7) | (count - 1));
                cur = px;
                count = 1;
            }
        }
        if (op >= out_max)
            return 0;
        out[op++] = (uint8_t)((cur << 7) | (count - 1));
    } else {
        uint8_t cur = _pixel_get(pixels, 0, bpp);
        size_t count = 1;
        for (size_t i = 1; i < pixel_count; i++) {
            const uint8_t px = _pixel_get(pixels, i, bpp);
            if (px == cur && count < (1 << 8))
                count++;
            else {
                if (op + 2 > out_max)
                    return 0;
                out[op++] = cur;
                out[op++] = (uint8_t)(count - 1);
                cur = px;
                count = 1;
            }
        }
        if (op + 2 > out_max)
            return 0;
        out[op++] = cur;
        out[op++] = (uint8_t)(count - 1);
    }
    return op;
}
size_t iotdata_image_rle_decompress(const uint8_t *compressed, size_t comp_len, uint8_t bpp, uint8_t *pixels, size_t pixel_buf_bytes) {
    if (!compressed || !pixels || comp_len == 0 || bpp == 0)
        return 0;
    size_t px_idx = 0;
    if (bpp == 1)
        for (size_t ip = 0; ip < comp_len; ip++) {
            const uint8_t val = (compressed[ip] >> 7) & 1;
            const size_t count = (compressed[ip] & 0x7F) + 1;
            for (size_t j = 0; j < count && px_idx < (pixel_buf_bytes * 8) / bpp; j++)
                _pixel_set(pixels, px_idx++, val, 1);
        }
    else
        for (size_t ip = 0; ip + 1 < comp_len; ip += 2) {
            const uint8_t val = compressed[ip];
            const size_t count = (size_t)compressed[ip + 1] + 1;
            for (size_t j = 0; j < count && px_idx < (pixel_buf_bytes * 8) / bpp; j++)
                _pixel_set(pixels, px_idx++, val, bpp);
        }
    const size_t used_bits = px_idx * bpp;
    if ((used_bits % 8) > 0)
        pixels[used_bits / 8] &= (uint8_t)(0xFF << (8 - (used_bits % 8)));
    return px_idx;
}

/* -------------------------------------------------------------------------
 * Heatshrink LZSS compression/decompression
 *
 * Self-contained LZSS codec.  Fixed parameters:
 *   window_sz2  = 8  (256-byte window)
 *   lookahead_sz2 = 4  (16-byte lookahead)
 *
 * Bit stream (MSB-first packing, matching protocol convention):
 *   Flag 1 → backref: [index:8] [count:4]
 *     index = distance_back - 1  (0 = 1 byte back, 255 = 256 bytes back)
 *     count = match_length - 1   (0 = 1 byte, 15 = 16 bytes)
 *     Compressor emits backrefs only for match_length >= 2.
 *   Flag 0 → literal: [byte:8]
 *
 * Decoder RAM: ~256 bytes (output serves as window).
 * Encoder: brute-force search, O(N * W * L) — fine for <=384-byte inputs.
 * ------------------------------------------------------------------------- */
#define _HS_W      (1 << IOTDATA_IMAGE_HS_WINDOW_SZ2)    /* 256 */
#define _HS_L      (1 << IOTDATA_IMAGE_HS_LOOKAHEAD_SZ2) /* 16 */
#define _HS_W_BITS IOTDATA_IMAGE_HS_WINDOW_SZ2           /* 8 */
#define _HS_L_BITS IOTDATA_IMAGE_HS_LOOKAHEAD_SZ2        /* 4 */
typedef struct {
    uint8_t *buf;
    size_t max;
    size_t byte_idx;
    uint8_t bit_idx; /* 7 = MSB of current byte, 0 = LSB */
    bool overflow;
} _hs_bw_t;
static void _hs_bw_init(_hs_bw_t *bw, uint8_t *buf, size_t max) {
    bw->buf = buf;
    bw->max = max;
    bw->byte_idx = 0;
    bw->bit_idx = 7;
    bw->overflow = false;
    if (max > 0)
        buf[0] = 0;
}
static void _hs_bw_put(_hs_bw_t *bw, uint32_t value, uint8_t nbits) {
    for (int i = nbits - 1; i >= 0; i--) {
        if (bw->byte_idx >= bw->max) {
            bw->overflow = true;
            break;
        }
        if (value & (1U << i))
            bw->buf[bw->byte_idx] |= (1U << bw->bit_idx);
        if (bw->bit_idx == 0) {
            bw->bit_idx = 7;
            bw->byte_idx++;
            if (bw->byte_idx < bw->max)
                bw->buf[bw->byte_idx] = 0;
        } else {
            bw->bit_idx--;
        }
    }
}
static size_t _hs_bw_bytes(const _hs_bw_t *bw) {
    return bw->bit_idx == 7 ? bw->byte_idx : bw->byte_idx + 1;
}
typedef struct {
    const uint8_t *buf;
    size_t len;
    size_t byte_idx;
    uint8_t bit_idx;
} _hs_br_t;
static void _hs_br_init(_hs_br_t *br, const uint8_t *buf, size_t len) {
    br->buf = buf;
    br->len = len;
    br->byte_idx = 0;
    br->bit_idx = 7;
}
static int _hs_br_get(_hs_br_t *br, uint8_t nbits) {
    uint32_t val = 0;
    for (int i = nbits - 1; i >= 0; i--) {
        if (br->byte_idx >= br->len)
            return -1;
        if (br->buf[br->byte_idx] & (1U << br->bit_idx))
            val |= (1U << i);
        if (br->bit_idx == 0) {
            br->bit_idx = 7;
            br->byte_idx++;
        } else {
            br->bit_idx--;
        }
    }
    return (int)val;
}
static bool _hs_br_done(const _hs_br_t *br) {
    return br->byte_idx >= br->len;
}
size_t iotdata_image_hs_compress(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_max) {
    if (!in || !out || in_len == 0 || out_max == 0)
        return 0;
    _hs_bw_t bw;
    _hs_bw_init(&bw, out, out_max);
    size_t ip = 0;
    while (ip < in_len && !bw.overflow) {
        /* Search for longest match in window */
        size_t best_len = 0, best_off = 0;
        const size_t max_match = (in_len - ip) < _HS_L ? (in_len - ip) : _HS_L;
        for (size_t off = ip > _HS_W ? ip - _HS_W : 0; off < ip; off++) {
            size_t ml = 0;
            while (ml < max_match && in[off + ml] == in[ip + ml])
                ml++;
            if (ml > best_len) {
                best_len = ml;
                best_off = ip - off;
                if (ml == max_match)
                    break;
            }
        }
        if (best_len >= 2) {
            /* Backref: flag(1) + index(W_BITS) + count(L_BITS) */
            _hs_bw_put(&bw, 1, 1);
            _hs_bw_put(&bw, (uint32_t)(best_off - 1), _HS_W_BITS);
            _hs_bw_put(&bw, (uint32_t)(best_len - 1), _HS_L_BITS);
            ip += best_len;
        } else {
            /* Literal: flag(0) + byte(8) */
            _hs_bw_put(&bw, 0, 1);
            _hs_bw_put(&bw, in[ip], 8);
            ip++;
        }
    }
    if (bw.overflow)
        return 0;
    return _hs_bw_bytes(&bw);
}
size_t iotdata_image_hs_decompress(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_max) {
    if (!in || !out || in_len == 0 || out_max == 0)
        return 0;
    _hs_br_t br;
    _hs_br_init(&br, in, in_len);
    size_t op = 0;
    while (!_hs_br_done(&br) && op < out_max) {
        const int flag = _hs_br_get(&br, 1);
        if (flag < 0)
            break;
        if (flag == 0) {
            const int byte = _hs_br_get(&br, 8);
            if (byte < 0)
                break;
            out[op++] = (uint8_t)byte;
        } else {
            const int index = _hs_br_get(&br, _HS_W_BITS);
            if (index < 0)
                break;
            const int count = _hs_br_get(&br, _HS_L_BITS);
            if (count < 0)
                break;
            if (((size_t)index + 1) > op)
                break; /* invalid: references before start */
            for (size_t j = 0; j < ((size_t)count + 1) && op < out_max; j++, op++)
                out[op] = out[op - ((size_t)index + 1)];
        }
    }
    return op;
}

#if !defined(IOTDATA_NO_ENCODE)
static bool pack_image(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_image_enc_t *const v = vv;
    /* Length = 1 (control byte) + pixel data bytes */
    if (!bits_write(buf, bb, bp, (uint8_t)(1 + v->image_data_len), 8))
        return false;
    /* Control byte: format(2) | size(2) | compression(2) | flags(2) */
    if (!bits_write(buf, bb, bp, (uint8_t)((v->image_pixel_format << 6) | (v->image_size_tier << 4) | (v->image_compression << 2) | (v->image_flags & 0x03)), 8))
        return false;
    /* Pixel data (compressed or raw, as provided by caller) */
    for (int i = 0; i < v->image_data_len; i++)
        if (!bits_write(buf, bb, bp, v->image_data[i], 8))
            return false;
    return true;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_image_by_slot(iotdata_encoder_t *enc, int slot, uint8_t pixel_format, uint8_t size_tier, uint8_t compression, uint8_t flags, const uint8_t *data, uint8_t data_len) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_IMAGE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (pixel_format > 2)
        return IOTDATA_ERR_IMAGE_FORMAT_HIGH;
    if (size_tier > 3)
        return IOTDATA_ERR_IMAGE_SIZE_HIGH;
    if (compression > 2)
        return IOTDATA_ERR_IMAGE_COMPRESSION_HIGH;
    if (!data && data_len > 0)
        return IOTDATA_ERR_IMAGE_DATA_NULL;
    if (data_len > IOTDATA_IMAGE_DATA_MAX)
        return IOTDATA_ERR_IMAGE_DATA_HIGH;
#endif
    iotdata_field_image_enc_t v;
    v.image_pixel_format = pixel_format;
    v.image_size_tier = size_tier;
    v.image_compression = compression;
    v.image_flags = flags & 0x03;
    v.image_data = data;
    v.image_data_len = data_len;
    PACK_DIRECT(enc, pack_image, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_IMAGE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_image(iotdata_encoder_t *enc, uint8_t pixel_format, uint8_t size_tier, uint8_t compression, uint8_t flags, const uint8_t *data, uint8_t data_len) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_IMAGE, si);
    return iotdata_encode_image_by_slot(enc, si, pixel_format, size_tier, compression, flags, data, data_len);
}
#else
iotdata_status_t iotdata_encode_image(iotdata_encoder_t *enc, uint8_t pixel_format, uint8_t size_tier, uint8_t compression, uint8_t flags, const uint8_t *data, uint8_t data_len) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_IMAGE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (pixel_format > 2)
        return IOTDATA_ERR_IMAGE_FORMAT_HIGH;
    if (size_tier > 3)
        return IOTDATA_ERR_IMAGE_SIZE_HIGH;
    if (compression > 2)
        return IOTDATA_ERR_IMAGE_COMPRESSION_HIGH;
    if (!data && data_len > 0)
        return IOTDATA_ERR_IMAGE_DATA_NULL;
    if (data_len > IOTDATA_IMAGE_DATA_MAX)
        return IOTDATA_ERR_IMAGE_DATA_HIGH;
#endif
    enc->image.image_pixel_format = pixel_format;
    enc->image.image_size_tier = size_tier;
    enc->image.image_compression = compression;
    enc->image.image_flags = flags & 0x03;
    enc->image.image_data = data;
    enc->image.image_data_len = data_len;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_IMAGE);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_image(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_image_dec_t *const v = vv;
    if (*bp + 16 > bb)
        return false; /* need at least length + control */
    const uint8_t length = (uint8_t)bits_read(buf, bb, bp, 8);
    if (length < 1)
        return false; /* control byte required */
    if (*bp + (length * 8) > bb)
        return false; /* short data */
    const uint8_t control = (uint8_t)bits_read(buf, bb, bp, 8);
    v->image_pixel_format = (control >> 6) & 0x03;
    v->image_size_tier = (control >> 4) & 0x03;
    v->image_compression = (control >> 2) & 0x03;
    v->image_flags = control & 0x03;
    v->image_data_len = 0;
    for (int i = 0; i < (length - 1); i++) {
        const uint8_t pixel = (uint8_t)bits_read(buf, bb, bp, 8);
        if (i < IOTDATA_IMAGE_DATA_MAX)
            v->image_data[v->image_data_len++] = pixel;
    }
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_image(iotdata_decoder_t *dec, uint8_t *pixel_format, uint8_t *size_tier, uint8_t *compression, uint8_t *flags, const uint8_t **data, uint8_t *data_len, iotdata_field_image_scratch_t *scratch) {
    if (!scratch)
        return IOTDATA_ERR_BUF_NULL;
#if defined(IOTDATA_OPERATE_ORDERED)
    /* THE PIXELS LAND IN THE CALLER'S SCRATCH, which is the whole reason it is a parameter: there
       is no decoder storage here, and a 254-byte local would die on return with *data still
       pointing into it. */
    iotdata_field_image_dec_t *const v = &scratch->_image;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_IMAGE, unpack_image, v);
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_IMAGE);
    /* The pixels are already in the decoder, so the scratch goes unwritten and *data is the
       decoder's own buffer. It is still demanded, so that a caller cannot pass NULL in this mode
       and discover on the day it switches to ORDERED that there was nowhere to decode into. */
    const iotdata_field_image_dec_t *const v = &dec->image;
#endif
    if (pixel_format)
        *pixel_format = v->image_pixel_format;
    if (size_tier)
        *size_tier = v->image_size_tier;
    if (compression)
        *compression = v->image_compression;
    if (flags)
        *flags = v->image_flags;
    /* Good until the next decode on this decoder, or until the scratch dies -- whichever first. */
    if (data)
        *data = v->image_data;
    if (data_len)
        *data_len = v->image_data_len;
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) || !defined(IOTDATA_NO_DUMP) || !defined(IOTDATA_NO_PRINT)
static const char *_image_fmt_names[] = { "bilevel", "grey4", "grey16", "reserved" };
static const char *_image_size_names[] = { "24x18", "32x24", "48x36", "64x48" };
static const char *_image_comp_names[] = { "raw", "rle", "heatshrink", "reserved" };
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_image(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_image_dec_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    cJSON_AddStringToObject(obj, "format", _image_fmt_names[v->image_pixel_format & 0x03]);
    cJSON_AddStringToObject(obj, "size", _image_size_names[v->image_size_tier & 0x03]);
    cJSON_AddStringToObject(obj, "compression", _image_comp_names[v->image_compression & 0x03]);
    cJSON_AddBoolToObject(obj, "fragment", (v->image_flags & IOTDATA_IMAGE_FLAG_FRAGMENT) != 0);
    cJSON_AddBoolToObject(obj, "invert", (v->image_flags & IOTDATA_IMAGE_FLAG_INVERT) != 0);
    if (v->image_data_len > 0)
        cJSON_AddStringToObject(obj, "pixels", _b64_encode(v->image_data, v->image_data_len, scratch->image.b64));
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_image(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    /* Parse pixel_format */
    uint8_t fmt = 0;
    cJSON *jf = cJSON_GetObjectItem(j, "format");
    if (jf && jf->valuestring)
        for (int i = 0; i < (int)(sizeof(_image_fmt_names) / sizeof(_image_fmt_names[0])); i++)
            if (strcmp(jf->valuestring, _image_fmt_names[i]) == 0) {
                fmt = (uint8_t)i;
                break;
            }
    /* Parse size */
    uint8_t sz = 0;
    cJSON *js = cJSON_GetObjectItem(j, "size");
    if (js && js->valuestring)
        for (int i = 0; i < (int)(sizeof(_image_size_names) / sizeof(_image_size_names[0])); i++)
            if (strcmp(js->valuestring, _image_size_names[i]) == 0) {
                sz = (uint8_t)i;
                break;
            }
    /* Parse compression */
    uint8_t comp = 0;
    cJSON *jc = cJSON_GetObjectItem(j, "compression");
    if (jc && jc->valuestring)
        for (int i = 0; i < (int)(sizeof(_image_comp_names) / sizeof(_image_comp_names[0])); i++)
            if (strcmp(jc->valuestring, _image_comp_names[i]) == 0) {
                comp = (uint8_t)i;
                break;
            }
    /* Parse flags */
    uint8_t flags = 0;
    if (cJSON_IsTrue(cJSON_GetObjectItem(j, "fragment")))
        flags |= IOTDATA_IMAGE_FLAG_FRAGMENT;
    if (cJSON_IsTrue(cJSON_GetObjectItem(j, "invert")))
        flags |= IOTDATA_IMAGE_FLAG_INVERT;
    /* Parse pixels (base64 → raw bytes) */
    cJSON *jp = cJSON_GetObjectItem(j, "pixels");
    return iotdata_encode_image(enc, fmt, sz, comp, flags, scratch->image.data, jp && jp->valuestring ? (uint8_t)_b64_decode(jp->valuestring, scratch->image.data, sizeof(scratch->image.data)) : 0);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_image(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    if (*bp + 16 > bb)
        return n;
    /* Length byte */
    size_t s = *bp;
    const uint8_t length = (uint8_t)bits_read(buf, bb, bp, 8);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 " (%" PRIu8 " total)", length, 1 + length);
    n = dump_add(dump, n, s, 8, length, dump->_dec_buf, "1..255", "image_length");
    /* Control byte */
    s = *bp;
    const uint8_t control = (uint8_t)bits_read(buf, bb, bp, 8);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%s %s %s%s%s", _image_fmt_names[(control >> 6) & 3], _image_size_names[(control >> 4) & 3], _image_comp_names[(control >> 2) & 3], (control & IOTDATA_IMAGE_FLAG_FRAGMENT) ? " frag" : "",
             (control & IOTDATA_IMAGE_FLAG_INVERT) ? " inv" : "");
    n = dump_add(dump, n, s, 8, control, dump->_dec_buf, "fmt|sz|comp|flg", "image_control");
    /* Pixel data (show as single span) */
    const uint8_t data_len = (length > 1) ? (uint8_t)(length - 1) : 0;
    if (data_len > 0) {
        s = *bp;
        const size_t data_bits = (size_t)data_len * 8;
        if (*bp + data_bits <= bb)
            *bp += data_bits;
        else
            *bp = bb;
        snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 " bytes", data_len);
        n = dump_add(dump, n, s, data_bits, 0, dump->_dec_buf, "pixel data", "image_pixels");
    }
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_image(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_image_dec_t *const v = vv;
    bprintf(bp, "  %s:%s %s %s %s, %" PRIu8 " bytes%s%s\n", label, _padd(label), _image_fmt_names[v->image_pixel_format & 3], _image_size_names[v->image_size_tier & 3], _image_comp_names[v->image_compression & 3], v->image_data_len,
            (v->image_flags & IOTDATA_IMAGE_FLAG_FRAGMENT) ? " [fragment]" : "", (v->image_flags & IOTDATA_IMAGE_FLAG_INVERT) ? " [inverted]" : "");
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_image = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, image))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, image))
    _IOTDATA_OP_NAME("image")
    _IOTDATA_OP_BITS(0) /* variable length */
    _IOTDATA_OP_PACK(pack_image)
    _IOTDATA_OP_UNPACK(unpack_image)
    _IOTDATA_OP_DUMP(dump_image)
    _IOTDATA_OP_PRINT(print_image)
    _IOTDATA_OP_JSON_SET(json_set_image)
    _IOTDATA_OP_JSON_GET(json_get_image)
};
#endif

#endif /* IOTDATA_FIELDS_IMAGE_IMPLEMENTATION */
