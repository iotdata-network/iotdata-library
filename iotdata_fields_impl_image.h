/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * iotdata_fields_impl_image.h -- IMAGE's compression, which is not field code at all
 *
 * THE _impl_ IN THE NAME IS A CONVENTION: a field whose structures and signatures are generated
 * like any other's, but whose implementation overflows into a file of its own. Anything else that
 * grows an implementation the template cannot carry belongs in iotdata_fields_impl_<field>.h for
 * the same reason and under the same rule -- the generated file keeps the SHAPES, so a change to
 * the template still reaches them, and only the bodies live out here.
 *
 * iotdata_fields_tlv.h is NOT one of these. TLV is not a field at all: no identifier, no presence
 * slot, no enum member, no registry row, and nothing of it is generated.
 *
 * WHAT IS LEFT HERE is a compression library that happens to be used by a field: run-length
 * encoding, and heatshrink LZSS with an eight-bit window and a four-bit lookahead. Seven
 * functions and one table of geometries. They take no encoder, no decoder and no field -- just
 * bytes in and bytes out -- which is why they were never going to be generated from a field
 * specification, and why everything else that was in this file now is.
 *
 * Their prototypes are declared from the specification, so they sit beside the field that needs
 * them: see the decl_encode block in specifiers/iotdata-fields-image.xml.
 *
 * ONE PHASE, not two: there are no types here any more, so nothing needs to be visible to
 * iotdata.h. The guard is the ordinary kind.
 */

#if !defined(IOTDATA_FIELDS_IMPL_IMAGE_IMPLEMENTATION_DONE)
#define IOTDATA_FIELDS_IMPL_IMAGE_IMPLEMENTATION_DONE

#if defined(IOTDATA_ENABLE_IMAGE) && !defined(IOTDATA_NO_ENCODE)

/* -------------------------------------------------------------------------
 * pixels
 * ------------------------------------------------------------------------- */

static uint8_t _pixel_get(const uint8_t *const buf, const size_t idx, const uint8_t bpp) {
    if (bpp == 1)
        return (buf[idx / 8] >> (7 - (idx % 8))) & 1;
    else if (bpp == 2)
        return (buf[idx / 4] >> (6 - (idx % 4) * 2)) & 3;
    else if (bpp == 4)
        return (idx & 1) ? buf[idx / 2] & 0x0F : buf[idx / 2] >> 4;
    return 0;
}

static void _pixel_set(uint8_t *const buf, const size_t idx, const uint8_t val, const uint8_t bpp) {
    if (bpp == 1)
        buf[idx / 8] = (uint8_t)((buf[idx / 8] & ~(1U << (7 - (idx % 8)))) | ((val & 1) << (7 - (idx % 8))));
    else if (bpp == 2)
        buf[idx / 4] = (uint8_t)((buf[idx / 4] & ~(3U << (6 - (idx % 4) * 2))) | ((val & 3) << (6 - (idx % 4) * 2)));
    else if (bpp == 4)
        buf[idx / 2] = (uint8_t)(idx & 1 ? (buf[idx / 2] & 0xF0) | (val & 0x0F) : (buf[idx / 2] & 0x0F) | ((val & 0x0F) << 4));
}

/* -------------------------------------------------------------------------
 * generic
 * ------------------------------------------------------------------------- */

/* the four size tiers, and the bits per pixel of the three formats */
static const uint8_t _image_widths[] = { 24, 32, 48, 64 }, _image_heights[] = { 18, 24, 36, 48 }, _image_bits[] = { 1, 2, 4 };

size_t iotdata_image_pixel_count(const uint8_t size_tier) {
    return size_tier < sizeof(_image_widths) / sizeof(_image_widths[0]) ? (size_t)_image_widths[size_tier] * (size_t)_image_heights[size_tier] : 0;
}

uint8_t iotdata_image_bpp(const uint8_t pixel_format) {
    return pixel_format < sizeof(_image_bits) / sizeof(_image_bits[0]) ? _image_bits[pixel_format] : 0;
}

size_t iotdata_image_bytes(const uint8_t pixel_format, const uint8_t size_tier) {
    return (iotdata_image_pixel_count(size_tier) * (size_t)iotdata_image_bpp(pixel_format) + 7) / 8;
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

static void _hs_bw_init(_hs_bw_t *const bw, uint8_t *const buf, const size_t max) {
    bw->buf = buf;
    bw->max = max;
    bw->byte_idx = 0;
    bw->bit_idx = 7;
    bw->overflow = false;
    if (max > 0)
        buf[0] = 0;
}

static void _hs_bw_put(_hs_bw_t *const bw, const uint32_t value, const uint8_t nbits) {
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

static size_t _hs_bw_bytes(const _hs_bw_t *const bw) {
    return bw->bit_idx == 7 ? bw->byte_idx : bw->byte_idx + 1;
}

typedef struct {
    const uint8_t *buf;
    size_t len;
    size_t byte_idx;
    uint8_t bit_idx;
} _hs_br_t;

static void _hs_br_init(_hs_br_t *const br, const uint8_t *const buf, const size_t len) {
    br->buf = buf;
    br->len = len;
    br->byte_idx = 0;
    br->bit_idx = 7;
}

static int _hs_br_get(_hs_br_t *const br, const uint8_t nbits) {
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

static bool _hs_br_done(const _hs_br_t *const br) {
    return br->byte_idx >= br->len;
}

size_t iotdata_image_hs_compress(const uint8_t *const in, const size_t in_len, uint8_t *const out, const size_t out_max) {
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

size_t iotdata_image_hs_decompress(const uint8_t *const in, const size_t in_len, uint8_t *const out, const size_t out_max) {
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

/* -------------------------------------------------------------------------
 * RLE compression/decompression
 *
 * Bilevel (1bpp):
 *   1-byte runs: bit7 = pixel value, bits 6-0 = count-1 (1..128 pixels)
 *
 * Greyscale (2bpp, 4bpp):
 *   2-byte runs: [value:8] [count-1:8] (1..256 pixels)
 * ------------------------------------------------------------------------- */

size_t iotdata_image_rle_compress(const uint8_t *const pixels, const size_t pixel_count, const uint8_t bpp, uint8_t *const out, const size_t out_max) {
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

size_t iotdata_image_rle_decompress(const uint8_t *const compressed, const size_t comp_len, const uint8_t bpp, uint8_t *const pixels, const size_t pixel_buf_bytes) {
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

#endif /* IOTDATA_ENABLE_IMAGE && !IOTDATA_NO_ENCODE */

#endif /* IOTDATA_FIELDS_IMPL_IMAGE_IMPLEMENTATION_DONE */
