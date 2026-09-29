/* iotdata_mesh.h
 *
 * Mesh relay protocol definitions for iotdata.
 *
 * Variant 15 (0x0F) is reserved for mesh control packets. This header
 * defines the control types, packet structures, and helper functions
 * for packing/unpacking mesh headers.
 *
 * See: APPENDIX_MESH.md in the iotdata repository for the full protocol
 * specification including flows, state machines, and deployment guidance.
 *
 * Include this header in both gateway and hop node firmware. Sensors do
 * not need this header — they are mesh-unaware.
 */

#ifndef IOTDATA_MESH_H
#define IOTDATA_MESH_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Constants
 * ----------------------------------------------------------------------- */

#define IOTDATA_MESH_VARIANT            0x0F

/* Control types (upper nibble of byte 4) */
#define IOTDATA_MESH_CTRL_BEACON        0x0
#define IOTDATA_MESH_CTRL_FORWARD       0x1
#define IOTDATA_MESH_CTRL_ACK           0x2
#define IOTDATA_MESH_CTRL_ROUTE_ERROR   0x3
#define IOTDATA_MESH_CTRL_NEIGHBOUR_RPT 0x4
#define IOTDATA_MESH_CTRL_DIAG          0x5
#define IOTDATA_MESH_CTRL_PING          0x6 /* v2 */
#define IOTDATA_MESH_CTRL_PONG          0x7 /* v2 */

/* Route error reasons (lower nibble of byte 4) */
#define IOTDATA_MESH_REASON_PARENT_LOST 0x0
#define IOTDATA_MESH_REASON_OVERLOADED  0x1
#define IOTDATA_MESH_REASON_SHUTDOWN    0x2

/* Beacon flags */
#define IOTDATA_MESH_FLAG_ACCEPTING     0x01 /* gateway is accepting forwards */

/* Special values */
#define IOTDATA_MESH_PARENT_NONE        0xFFF /* orphaned — no parent */
#define IOTDATA_MESH_STATION_RESERVED   0x000 /* do not assign to nodes */

/* Protocol limits */
#define IOTDATA_MESH_TTL_DEFAULT        7
#define IOTDATA_MESH_TTL_INFINITY       (IOTDATA_MESH_TTL_DEFAULT + 1)
#define IOTDATA_MESH_TTL_MAX            255
#define IOTDATA_MESH_GENERATION_HALF    2048 /* for modular comparison */
#define IOTDATA_MESH_GENERATION_MOD     4096
#define IOTDATA_MESH_MAX_NEIGHBOURS     63

/* Packet sizes */
#define IOTDATA_MESH_BEACON_SIZE        11
#define IOTDATA_MESH_FORWARD_HDR_SIZE   6 /* + inner packet bytes */
#define IOTDATA_MESH_ACK_SIZE           8
#define IOTDATA_MESH_ROUTE_ERROR_SIZE   5
#define IOTDATA_MESH_NEIGHBOUR_HDR_SIZE 10 /* + 3 per entry */
#define IOTDATA_MESH_NEIGHBOUR_ENTRY_SZ 3
#define IOTDATA_MESH_PING_SIZE          8
#define IOTDATA_MESH_PONG_SIZE          8
#define IOTDATA_MESH_DIAG_HDR_SIZE      7 /* + op-specific body */

/* Dedup ring default size */
#define IOTDATA_MESH_DEDUP_RING_SIZE    64

/* -------------------------------------------------------------------------
 * iotdata header peek — extract fields from the standard 4-byte header
 * ----------------------------------------------------------------------- */

static inline bool iotdata_mesh_peek_header(const uint8_t *buf, int len, uint8_t *variant, uint16_t *station_id, uint16_t *sequence) {
    if (len < 4)
        return false;
    *variant = (buf[0] >> 4) & 0x0F;
    *station_id = ((uint16_t)(buf[0] & 0x0F) << 8) | buf[1];
    *sequence = ((uint16_t)buf[2] << 8) | buf[3];
    return true;
}

static inline uint8_t iotdata_mesh_peek_ctrl_type(const uint8_t *buf, int len) {
    if (len < 5)
        return 0xFF;
    return (buf[4] >> 4) & 0x0F;
}

/* -------------------------------------------------------------------------
 * 4+12 bit packing helper (used throughout the mesh protocol)
 * ----------------------------------------------------------------------- */

static inline void iotdata_mesh_pack_4_12(uint8_t *dst, uint8_t hi4, uint16_t lo12) {
    dst[0] = (uint8_t)((hi4 << 4) | ((lo12 >> 8) & 0x0F));
    dst[1] = (uint8_t)(lo12 & 0xFF);
}

static inline void iotdata_mesh_unpack_4_12(const uint8_t *src, uint8_t *hi4, uint16_t *lo12) {
    *hi4 = (src[0] >> 4) & 0x0F;
    *lo12 = ((uint16_t)(src[0] & 0x0F) << 8) | src[1];
}

/* -------------------------------------------------------------------------
 * Common mesh header (bytes 0–4): pack and unpack
 * ----------------------------------------------------------------------- */

static inline void iotdata_mesh_pack_header(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq) {
    iotdata_mesh_pack_4_12(&buf[0], IOTDATA_MESH_VARIANT, sender_station);
    buf[2] = (uint8_t)(sender_seq >> 8);
    buf[3] = (uint8_t)(sender_seq & 0xFF);
    /* byte 4 left for caller to set ctrl_type + first payload nibble */
}

/* -------------------------------------------------------------------------
 * BEACON (ctrl_type 0x0) — 11 bytes
 *
 * byte 4-5:  ctrl(4) | gateway_id(12)
 * byte 6:    cost(8)
 * byte 7:    flags(4) | generation[11:8](4)
 * byte 8:    generation[7:0](8)
 * byte 9-10: interval_s(16)
 * ----------------------------------------------------------------------- */

typedef struct {
    uint16_t sender_station;
    uint16_t sender_seq;
    uint16_t gateway_id;
    uint8_t cost;
    uint8_t flags;
    uint16_t generation;
    uint16_t interval_s;
} iotdata_mesh_beacon_t;

static inline int iotdata_mesh_pack_beacon(uint8_t *buf, int len, const iotdata_mesh_beacon_t *b) {
    if (len < IOTDATA_MESH_BEACON_SIZE)
        return 0;
    iotdata_mesh_pack_header(buf, b->sender_station, b->sender_seq);
    iotdata_mesh_pack_4_12(&buf[4], IOTDATA_MESH_CTRL_BEACON, b->gateway_id);
    buf[6] = b->cost;
    buf[7] = (uint8_t)(((b->flags & 0x0F) << 4) | ((b->generation >> 8) & 0x0F));
    buf[8] = (uint8_t)(b->generation & 0xFF);
    buf[9] = (uint8_t)(b->interval_s >> 8);
    buf[10] = (uint8_t)(b->interval_s & 0xFF);
    return IOTDATA_MESH_BEACON_SIZE;
}

static inline bool iotdata_mesh_unpack_beacon(const uint8_t *buf, int len, iotdata_mesh_beacon_t *b) {
    if (len < IOTDATA_MESH_BEACON_SIZE)
        return false;
    uint8_t ctrl;
    iotdata_mesh_unpack_4_12(&buf[0], &ctrl, &b->sender_station);
    b->sender_seq = ((uint16_t)buf[2] << 8) | buf[3];
    iotdata_mesh_unpack_4_12(&buf[4], &ctrl, &b->gateway_id);
    b->cost = buf[6];
    b->flags = (buf[7] >> 4) & 0x0F;
    b->generation = ((uint16_t)(buf[7] & 0x0F) << 8) | buf[8];
    b->interval_s = (uint16_t)(((uint16_t)buf[9] << 8) | buf[10]);
    return true;
}

/* -------------------------------------------------------------------------
 * FORWARD (ctrl_type 0x1) — 6 + N bytes
 *
 * byte 4:   ctrl(4) | ttl[7:4](4)
 * byte 5:   ttl[3:0](4) | pad(4)
 * byte 6+:  inner_packet (byte-aligned)
 * ----------------------------------------------------------------------- */

typedef struct {
    uint16_t sender_station;
    uint16_t sender_seq;
    uint8_t ttl;
    const uint8_t *inner_packet; /* pointer into receive buffer, not owned */
    int inner_len;
    /* extracted from inner_packet header for convenience / dedup */
    uint16_t origin_station;
    uint16_t origin_sequence;
} iotdata_mesh_forward_t;

static inline int iotdata_mesh_pack_forward(uint8_t *buf, int len, uint16_t sender_station, uint16_t sender_seq, uint8_t ttl, const uint8_t *inner, int inner_len) {
    if (len < (6 + inner_len))
        return 0;
    iotdata_mesh_pack_header(buf, sender_station, sender_seq);
    buf[4] = (uint8_t)((IOTDATA_MESH_CTRL_FORWARD << 4) | ((ttl >> 4) & 0x0F));
    buf[5] = (uint8_t)((ttl & 0x0F) << 4);
    memcpy(&buf[6], inner, (size_t)inner_len);
    return 6 + inner_len;
}

static inline bool iotdata_mesh_unpack_forward(const uint8_t *buf, int len, iotdata_mesh_forward_t *f) {
    if (len < IOTDATA_MESH_FORWARD_HDR_SIZE + 4) /* need at least inner header */
        return false;
    uint8_t ctrl;
    iotdata_mesh_unpack_4_12(&buf[0], &ctrl, &f->sender_station);
    f->sender_seq = ((uint16_t)buf[2] << 8) | buf[3];
    f->ttl = (uint8_t)(((buf[4] & 0x0F) << 4) | ((buf[5] >> 4) & 0x0F));
    f->inner_packet = &buf[6];
    f->inner_len = len - IOTDATA_MESH_FORWARD_HDR_SIZE;
    /* extract origin from inner iotdata header */
    f->origin_station = ((uint16_t)(buf[6] & 0x0F) << 8) | buf[7];
    f->origin_sequence = ((uint16_t)buf[8] << 8) | buf[9];
    return true;
}

/* -------------------------------------------------------------------------
 * ACK (ctrl_type 0x2) — 8 bytes. Confirms delivery of an inner sensor packet, keyed on its
 * ORIGIN identity (not the forwarder), so a single ACK both clears the forwarder's retry AND
 * lets any sibling suppress an about-to-send duplicate forward of the same packet.
 *
 * byte 4-5: ctrl(4) | origin_station(12)
 * byte 6-7: origin_sequence(16)
 * ----------------------------------------------------------------------- */

typedef struct {
    uint16_t sender_station; /* who sent this ACK (the receiving parent / gateway) */
    uint16_t sender_seq;
    uint16_t origin_station;  /* identity of the inner sensor packet this ACK confirms delivered */
    uint16_t origin_sequence; /* -> any relay holding this {origin_station,origin_sequence} can clear
                                 its pending forward (retry done) or suppress an about-to-send one */
} iotdata_mesh_ack_t;

static inline int iotdata_mesh_pack_ack(uint8_t *buf, int len, const iotdata_mesh_ack_t *a) {
    if (len < IOTDATA_MESH_ACK_SIZE)
        return 0;
    iotdata_mesh_pack_header(buf, a->sender_station, a->sender_seq);
    iotdata_mesh_pack_4_12(&buf[4], IOTDATA_MESH_CTRL_ACK, a->origin_station);
    buf[6] = (uint8_t)(a->origin_sequence >> 8);
    buf[7] = (uint8_t)(a->origin_sequence & 0xFF);
    return IOTDATA_MESH_ACK_SIZE;
}

static inline bool iotdata_mesh_unpack_ack(const uint8_t *buf, int len, iotdata_mesh_ack_t *a) {
    if (len < IOTDATA_MESH_ACK_SIZE)
        return false;
    uint8_t ctrl;
    iotdata_mesh_unpack_4_12(&buf[0], &ctrl, &a->sender_station);
    a->sender_seq = ((uint16_t)buf[2] << 8) | buf[3];
    iotdata_mesh_unpack_4_12(&buf[4], &ctrl, &a->origin_station);
    a->origin_sequence = ((uint16_t)buf[6] << 8) | buf[7];
    return true;
}

/* -------------------------------------------------------------------------
 * NEIGHBOUR_REPORT (ctrl_type 0x4) — 10-byte header + 3N bytes (spec G.4.7)
 *
 * A relay's periodic topology snapshot: its parent/cost/gateway + the stations it hears.
 * Extended here to carry SENSOR neighbours too (cost = 0xFF sentinel); the gateway reads
 * cost 0 = gateway, 1..254 = relay, 0xFF = sensor.
 *
 * byte 0-1: 0xF | sender_station(12)     byte 4-5: ctrl=0x4 | parent_id(12)
 * byte 2-3: sender_seq(16)               byte 6:   my_cost(8)
 * byte 7:   num_neighbours(6) | gateway_id[11:10]   byte 8: gateway_id[9:2]   byte 9: gateway_id[1:0]
 * byte 10+: entries, each { cost(8), rssi_q4(4) | station_id(12) }
 * ----------------------------------------------------------------------- */

#define IOTDATA_MESH_NBR_HDR_SIZE    10
#define IOTDATA_MESH_NBR_ENTRY_SIZE  3
#define IOTDATA_MESH_NBR_MAX         63 /* 6-bit count */
#define IOTDATA_MESH_NBR_COST_SENSOR 0xFF

typedef struct {
    uint8_t cost;     /* neighbour's advertised cost; 0xFF = sensor (no cost) */
    uint8_t rssi_q4;  /* RSSI quantised to 4 bits (5 dBm steps from -120) */
    uint16_t station; /* the heard station */
} iotdata_mesh_nbr_entry_t;

typedef struct {
    uint16_t sender_station;
    uint16_t sender_seq;
    uint16_t parent_id;
    uint8_t my_cost;
    uint8_t num_neighbours;
    uint16_t gateway_id;
} iotdata_mesh_neighbour_report_t;

static inline uint8_t iotdata_mesh_rssi_to_q4(int rssi_dbm) {
    int q = (rssi_dbm + 120) / 5;
    if (q < 0)
        q = 0;
    else if (q > 15)
        q = 15;
    return (uint8_t)q;
}
static inline int iotdata_mesh_rssi_from_q4(uint8_t q4) {
    return (int)q4 * 5 - 120;
}

/* Pack a full report from `count` entries. Returns the frame length. Clamps count to what fits. */
static inline int iotdata_mesh_pack_neighbour_report(uint8_t *buf, int len, uint16_t sender_station, uint16_t sender_seq, uint16_t parent_id, uint8_t my_cost, uint16_t gateway_id, const iotdata_mesh_nbr_entry_t *entries, int count) {
    if (count < 0)
        count = 0;
    if (count > IOTDATA_MESH_NBR_MAX)
        count = IOTDATA_MESH_NBR_MAX;
    if (len < IOTDATA_MESH_NBR_HDR_SIZE + (count * IOTDATA_MESH_NBR_ENTRY_SIZE))
        return 0;
    iotdata_mesh_pack_header(buf, sender_station, sender_seq);
    iotdata_mesh_pack_4_12(&buf[4], IOTDATA_MESH_CTRL_NEIGHBOUR_RPT, parent_id);
    buf[6] = my_cost;
    buf[7] = (uint8_t)((((uint8_t)count & 0x3Fu) << 2) | (uint8_t)((gateway_id >> 10) & 0x03u));
    buf[8] = (uint8_t)((gateway_id >> 2) & 0xFFu);
    buf[9] = (uint8_t)((gateway_id & 0x03u) << 6);
    int off = IOTDATA_MESH_NBR_HDR_SIZE;
    for (int i = 0; i < count; i++) {
        buf[off + 0] = entries[i].cost;
        buf[off + 1] = (uint8_t)(((entries[i].rssi_q4 & 0x0Fu) << 4) | (uint8_t)((entries[i].station >> 8) & 0x0Fu));
        buf[off + 2] = (uint8_t)(entries[i].station & 0xFFu);
        off += IOTDATA_MESH_NBR_ENTRY_SIZE;
    }
    return off;
}

static inline bool iotdata_mesh_unpack_neighbour_report(const uint8_t *buf, int len, iotdata_mesh_neighbour_report_t *r) {
    if (len < IOTDATA_MESH_NBR_HDR_SIZE)
        return false;
    uint8_t v, ctrl;
    iotdata_mesh_unpack_4_12(&buf[0], &v, &r->sender_station);
    r->sender_seq = (uint16_t)(((uint16_t)buf[2] << 8) | buf[3]);
    iotdata_mesh_unpack_4_12(&buf[4], &ctrl, &r->parent_id);
    r->my_cost = buf[6];
    r->num_neighbours = (uint8_t)((buf[7] >> 2) & 0x3Fu);
    r->gateway_id = (uint16_t)((((uint16_t)(buf[7] & 0x03u)) << 10) | ((uint16_t)buf[8] << 2) | ((buf[9] >> 6) & 0x03u));
    return true;
}

/* Read the index-th neighbour entry (0-based). False if it runs past the frame. */
static inline bool iotdata_mesh_neighbour_report_entry(const uint8_t *buf, int len, int index, iotdata_mesh_nbr_entry_t *e) {
    const int off = IOTDATA_MESH_NBR_HDR_SIZE + index * IOTDATA_MESH_NBR_ENTRY_SIZE;
    if (index < 0 || off + IOTDATA_MESH_NBR_ENTRY_SIZE > len)
        return false;
    e->cost = buf[off];
    e->rssi_q4 = (uint8_t)((buf[off + 1] >> 4) & 0x0Fu);
    e->station = (uint16_t)(((uint16_t)(buf[off + 1] & 0x0Fu) << 8) | buf[off + 2]);
    return true;
}

/* -------------------------------------------------------------------------
 * ROUTE_ERROR (ctrl_type 0x3) — 5 bytes
 *
 * byte 4: ctrl(4) | reason(4)
 * ----------------------------------------------------------------------- */

typedef struct {
    uint16_t sender_station;
    uint16_t sender_seq;
    uint8_t reason;
} iotdata_mesh_route_error_t;

static inline int iotdata_mesh_pack_route_error(uint8_t *buf, int len, uint16_t sender_station, uint16_t sender_seq, uint8_t reason) {
    if (len < IOTDATA_MESH_ROUTE_ERROR_SIZE)
        return 0;
    iotdata_mesh_pack_header(buf, sender_station, sender_seq);
    buf[4] = (uint8_t)((IOTDATA_MESH_CTRL_ROUTE_ERROR << 4) | (reason & 0x0F));
    return IOTDATA_MESH_ROUTE_ERROR_SIZE;
}

static inline bool iotdata_mesh_unpack_route_error(const uint8_t *buf, int len, iotdata_mesh_route_error_t *r) {
    if (len < IOTDATA_MESH_ROUTE_ERROR_SIZE)
        return false;
    uint8_t ctrl;
    iotdata_mesh_unpack_4_12(&buf[0], &ctrl, &r->sender_station);
    r->sender_seq = ((uint16_t)buf[2] << 8) | buf[3];
    r->reason = buf[4] & 0x0F;
    return true;
}

/* -------------------------------------------------------------------------
 * DIAG (ctrl_type 0x5) — the protocol's own diagnostics.
 *
 * WHY IT LIVES HERE. "Did the mesh catch and relay that packet" is the protocol auditing the
 * contract it signed up to. A deployment using iotdata_mesh.h WITHOUT the node framework must
 * still be able to ask, so the question belongs to the protocol, not to a layer above it.
 *
 * byte 4:   ctrl(5) | kind(4)          — WHAT is being asked about (SURVEY, ...)
 * byte 5-6: op(4) | directive_id(12)   — what this frame DOES about it
 * byte 7+:  op-specific body
 *
 * THE DIRECTIVE ID is what makes a re-issue safe. A node already holding an id re-broadcasts the
 * frame — so a node that missed it the first time still gets it — but does NOT restart its own
 * window. "Send it again for whoever did not hear it" therefore cannot slide everybody else's
 * windows, and needs no targeting logic. CANCEL_ALL carries no meaningful id.
 * ----------------------------------------------------------------------- */

/* Diagnostic kinds (lower nibble of byte 4) */
#define IOTDATA_MESH_DIAG_SURVEY          0x0

/* Diagnostic ops (upper nibble of byte 5) */
#define IOTDATA_MESH_DIAG_OP_REQUEST      0x0
#define IOTDATA_MESH_DIAG_OP_RESPONSE     0x1
#define IOTDATA_MESH_DIAG_OP_ERROR        0x2
#define IOTDATA_MESH_DIAG_OP_CANCEL       0x3
#define IOTDATA_MESH_DIAG_OP_CANCEL_ALL   0x4

#define IOTDATA_MESH_DIAG_ERR_UNSUPPORTED 0x0 /* kind not implemented here */
#define IOTDATA_MESH_DIAG_ERR_FULL        0x1 /* no room for another directive */
#define IOTDATA_MESH_DIAG_ERR_MALFORMED   0x2 /* body did not parse */

#define IOTDATA_MESH_DIAG_ERROR_SIZE      (IOTDATA_MESH_DIAG_HDR_SIZE + 1)
#define IOTDATA_MESH_DIAG_CANCEL_SIZE     IOTDATA_MESH_DIAG_HDR_SIZE

typedef struct {
    uint16_t sender_station;
    uint16_t sender_seq;
    uint8_t kind;
    uint8_t op;
    uint16_t directive_id;
} iotdata_mesh_diag_t;

static inline int iotdata_mesh_pack_diag_header(uint8_t *buf, int len, uint16_t sender_station, uint16_t sender_seq, uint8_t kind, uint8_t op, uint16_t directive_id) {
    if (len < IOTDATA_MESH_DIAG_HDR_SIZE)
        return 0;
    iotdata_mesh_pack_header(buf, sender_station, sender_seq);
    buf[4] = (uint8_t)((IOTDATA_MESH_CTRL_DIAG << 4) | (kind & 0x0F));
    iotdata_mesh_pack_4_12(&buf[5], op, directive_id);
    return IOTDATA_MESH_DIAG_HDR_SIZE;
}

/* Unpacks the ENVELOPE only — kind and op say which body accessor to use next. */
static inline bool iotdata_mesh_unpack_diag(const uint8_t *buf, int len, iotdata_mesh_diag_t *d) {
    if (len < IOTDATA_MESH_DIAG_HDR_SIZE)
        return false;
    uint8_t ctrl;
    iotdata_mesh_unpack_4_12(&buf[0], &ctrl, &d->sender_station);
    d->sender_seq = (uint16_t)(((uint16_t)buf[2] << 8) | buf[3]);
    d->kind = (uint8_t)(buf[4] & 0x0F);
    iotdata_mesh_unpack_4_12(&buf[5], &d->op, &d->directive_id);
    return true;
}

static inline int iotdata_mesh_pack_diag_error(uint8_t *buf, int len, uint16_t sender_station, uint16_t sender_seq, uint8_t kind, uint16_t directive_id, uint8_t reason) {
    if (len < IOTDATA_MESH_DIAG_ERROR_SIZE)
        return 0;
    (void)iotdata_mesh_pack_diag_header(buf, len, sender_station, sender_seq, kind, IOTDATA_MESH_DIAG_OP_ERROR, directive_id);
    buf[7] = reason;
    return IOTDATA_MESH_DIAG_ERROR_SIZE;
}

static inline int iotdata_mesh_pack_diag_cancel(uint8_t *buf, int len, uint16_t sender_station, uint16_t sender_seq, uint8_t kind, uint16_t directive_id) {
    return iotdata_mesh_pack_diag_header(buf, len, sender_station, sender_seq, kind, IOTDATA_MESH_DIAG_OP_CANCEL, directive_id);
}

static inline int iotdata_mesh_pack_diag_cancel_all(uint8_t *buf, int len, uint16_t sender_station, uint16_t sender_seq, uint8_t kind) {
    return iotdata_mesh_pack_diag_header(buf, len, sender_station, sender_seq, kind, IOTDATA_MESH_DIAG_OP_CANCEL_ALL, 0u);
}

/* -------------------------------------------------------------------------
 * DIAG / SURVEY (kind 0x0) — who hears a station, and how well.
 *
 * REQUEST  (op 0x0): byte 7-8 duration_m(16), byte 9-10 batch_s(16), byte 11 count(8),
 *                    byte 12+ station(16) x count
 * RESPONSE (op 0x1): byte 7 count(8), byte 8+ entry(6) x count
 *   entry: station(16) | sequence(16) | rssi(8, offset) | snr_q4(int8)
 *
 * ONLY THE FIRST HOP IS THE MEASUREMENT. A node records a reception only when it hears the
 * station DIRECTLY — a bare packet, not one arriving inside a FORWARD. An RSSI taken off a
 * relayed copy is the relay-to-relay link, which the protocol already reports elsewhere.
 *
 * DURATION IS TIME, NOT A PACKET COUNT. "The next N packets from station X" never expires if X
 * is dead, and the whole point of a timebox is that a forgotten directive stops by itself.
 * Minutes, so a directive can outlive a working day; batch in seconds, so a walked survey can
 * report every few seconds while a background watch reports every half hour.
 *
 * AN EMPTY RESPONSE IS THE POINT, NOT AN OMISSION. count == 0 says "this directive is live here
 * and I heard nothing". Without it, a lost REQUEST and a genuine coverage hole look identical —
 * and inventing coverage holes is precisely the failure a survey must not have.
 *
 * NOT rssi_q4 HERE. The 4-bit quantisation floors at -120 dBm, which is fine for choosing a
 * route and useless for a survey: -120 to -137 is where the interesting edge is, and all of it
 * would read as zero. Full 1 dBm steps, offset-encoded. SNR earns its byte too — below the noise
 * floor RSSI flattens out while SNR keeps falling, so it is the better edge discriminator; it is
 * carried in the radio's own quarter-dB units.
 * ----------------------------------------------------------------------- */

#define IOTDATA_MESH_DIAG_SURVEY_RSSI_OFF 160 /* dBm + 160, so -159..-1 lands in 1..159 */

/* THREE STATES, NOT ONE NUMBER. A receiver can give a reading, say it has none, or give one that
   ran off the top of its scale -- and a survey that renders the last two AS numbers invents
   measurements. Both ends of the byte are free to say so: no LoRa receiver reads -160 dBm and none
   reads +95, so neither sentinel can collide with a real value.
   In dBm space the convention is the RADIO DRIVER'S OWN (<= 0 is a reading, 0 means saturated, a
   positive value is not a dBm at all), so a caller hands the driver's value straight in and has
   nothing to translate -- which is exactly where it was got wrong. */
#define IOTDATA_MESH_DIAG_RSSI_WIRE_NONE  0x00u
#define IOTDATA_MESH_DIAG_RSSI_WIRE_SAT   0xFFu
#define IOTDATA_MESH_DIAG_RSSI_NONE       1 /* positive: not a dBm value -- no reading available */
#define IOTDATA_MESH_DIAG_RSSI_SAT        0 /* at least this strong; the reading wrapped */

/* Not every radio reports SNR -- a UART LoRa module typically appends RSSI and nothing else. The
   field is still carried, because the radios that DO report it give the better reading at the
   edge; a receiver that cannot must say so rather than send a plausible-looking 0 dB. */
#define IOTDATA_MESH_DIAG_SNR_NONE        (-128)

/* 1 dBm steps across the whole range a LoRa receiver can report, unlike the 4-bit routing
   quantisation above which floors at -120 dBm. Real readings are clamped INSIDE the sentinels, so
   an extreme measurement can never be mistaken for an absent one. */
static inline uint8_t iotdata_mesh_rssi_dbm_encode(int rssi_dbm) {
    if (rssi_dbm > 0)
        return IOTDATA_MESH_DIAG_RSSI_WIRE_NONE;
    if (rssi_dbm == 0)
        return IOTDATA_MESH_DIAG_RSSI_WIRE_SAT;
    const int v = rssi_dbm + IOTDATA_MESH_DIAG_SURVEY_RSSI_OFF;
    return (uint8_t)(v < 1 ? 1 : (v > 254 ? 254 : v));
}

static inline int iotdata_mesh_rssi_dbm_decode(uint8_t e) {
    if (e == IOTDATA_MESH_DIAG_RSSI_WIRE_NONE)
        return IOTDATA_MESH_DIAG_RSSI_NONE;
    if (e == IOTDATA_MESH_DIAG_RSSI_WIRE_SAT)
        return IOTDATA_MESH_DIAG_RSSI_SAT;
    return (int)e - IOTDATA_MESH_DIAG_SURVEY_RSSI_OFF;
}

/* The only test a consumer should apply before treating it as a number. */
static inline bool iotdata_mesh_rssi_is_reading(int rssi_dbm) {
    return rssi_dbm < 0;
}

// -----------------------------------------------------------------------------------------------------------------------------------------

#define IOTDATA_MESH_DIAG_SURVEY_REQ_SIZE  (IOTDATA_MESH_DIAG_HDR_SIZE + 4) /* + 2 per station */
#define IOTDATA_MESH_DIAG_SURVEY_REQ_ENTRY 2
#define IOTDATA_MESH_DIAG_SURVEY_RSP_SIZE  IOTDATA_MESH_DIAG_HDR_SIZE /* + records, to the end */
#define IOTDATA_MESH_DIAG_SURVEY_RSP_ENTRY 6
#define IOTDATA_MESH_DIAG_SURVEY_POS_SIZE  12  /* an extension record: two entry slots */
#define IOTDATA_MESH_DIAG_SURVEY_MAX       128 /* a packer's sanity limit; the frame runs out first */

/* THE EXTENSION RECORD. A response body is a run of 6-byte observations, except that station
   0x000 -- IOTDATA_MESH_STATION_RESERVED, already "do not assign to nodes" -- is not a station at
   all. It marks a record that is something else, and a reader that meets one consumes that record's
   own length instead of six.
   Today there is one: the OBSERVER'S POSITION, which is why a relay that knows where it stands can
   turn a column of numbers into a map, and why a surveying node can report what IT heard from
   somewhere identifiable. It appears at most once, because there is one observer.
   It is a door rather than a hole: an antenna height, a timestamp, whatever the next question
   wants, arrives the same way and needs no flag, no count and no new op. */
typedef struct {
    int32_t latitude_e7;  /* degrees x 1e-7; +-900000000 inside a +-2.1e9 range */
    int32_t longitude_e7; /* degrees x 1e-7; +-1800000000 */
    int16_t altitude_m;
} iotdata_mesh_diag_position_t;

static inline void _iotdata_mesh_wr_i32(uint8_t *const b, const int32_t v) {
    const uint32_t u = (uint32_t)v;
    b[0] = (uint8_t)(u >> 24);
    b[1] = (uint8_t)(u >> 16);
    b[2] = (uint8_t)(u >> 8);
    b[3] = (uint8_t)(u);
}

static inline int32_t _iotdata_mesh_rd_i32(const uint8_t *const b) {
    return (int32_t)(((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3]);
}

// -----------------------------------------------------------------------------------------------------------------------------------------

typedef struct {
    uint16_t station;  /* the station that was heard */
    uint16_t sequence; /* its sequence -- the key the gateway joins the payload on */
    int rssi_dbm;      /* < 0 a reading; 0 saturated; > 0 none -- iotdata_mesh_rssi_is_reading() */
    int8_t snr_q4;     /* quarter-dB, as the radio reports it */
} iotdata_mesh_diag_survey_entry_t;

typedef struct {
    uint16_t duration_m; /* how long the directive lives */
    uint16_t batch_s;    /* how often a response goes out, empty or not */
    int count;           /* stations named -- DERIVED from the frame, not carried in it */
} iotdata_mesh_diag_survey_request_t;

// -----------------------------------------------------------------------------------------------------------------------------------------

static inline int iotdata_mesh_pack_diag_survey_request(uint8_t *buf, int len, uint16_t sender_station, uint16_t sender_seq, uint16_t directive_id, uint16_t duration_m, uint16_t batch_s, const uint16_t *stations, int count) {
    if (count < 0 || count > IOTDATA_MESH_DIAG_SURVEY_MAX || (count > 0 && stations == NULL))
        return 0;
    const int need = IOTDATA_MESH_DIAG_SURVEY_REQ_SIZE + count * IOTDATA_MESH_DIAG_SURVEY_REQ_ENTRY;
    if (len < need)
        return 0;
    (void)iotdata_mesh_pack_diag_header(buf, len, sender_station, sender_seq, IOTDATA_MESH_DIAG_SURVEY, IOTDATA_MESH_DIAG_OP_REQUEST, directive_id);
    buf[7] = (uint8_t)(duration_m >> 8);
    buf[8] = (uint8_t)(duration_m & 0xFFu);
    buf[9] = (uint8_t)(batch_s >> 8);
    buf[10] = (uint8_t)(batch_s & 0xFFu);
    int off = IOTDATA_MESH_DIAG_SURVEY_REQ_SIZE;
    for (int i = 0; i < count; i++) {
        buf[off] = (uint8_t)(stations[i] >> 8);
        buf[off + 1] = (uint8_t)(stations[i] & 0xFFu);
        off += IOTDATA_MESH_DIAG_SURVEY_REQ_ENTRY;
    }
    return need;
}

/* count comes out of the LENGTH. A body that is not a whole number of stations is refused. */
static inline bool iotdata_mesh_unpack_diag_survey_request(const uint8_t *buf, int len, iotdata_mesh_diag_survey_request_t *r) {
    if (len < IOTDATA_MESH_DIAG_SURVEY_REQ_SIZE)
        return false;
    const int body = len - IOTDATA_MESH_DIAG_SURVEY_REQ_SIZE;
    if ((body % IOTDATA_MESH_DIAG_SURVEY_REQ_ENTRY) != 0)
        return false;
    r->duration_m = (uint16_t)(((uint16_t)buf[7] << 8) | buf[8]);
    r->batch_s = (uint16_t)(((uint16_t)buf[9] << 8) | buf[10]);
    r->count = body / IOTDATA_MESH_DIAG_SURVEY_REQ_ENTRY;
    return true;
}

static inline bool iotdata_mesh_diag_survey_request_station(const uint8_t *buf, int len, int index, uint16_t *station) {
    const int off = IOTDATA_MESH_DIAG_SURVEY_REQ_SIZE + index * IOTDATA_MESH_DIAG_SURVEY_REQ_ENTRY;
    if (index < 0 || off + IOTDATA_MESH_DIAG_SURVEY_REQ_ENTRY > len)
        return false;
    *station = (uint16_t)(((uint16_t)buf[off] << 8) | buf[off + 1]);
    return true;
}

// -----------------------------------------------------------------------------------------------------------------------------------------

/* An empty body is a legitimate, and REQUIRED, response: it states that the directive is live here
   and that nothing was heard in the window. Without it, a lost REQUEST and a genuine hole in
   coverage are indistinguishable, and a survey that invents coverage holes is worse than none.
   `position` is optional: NULL from a node that does not know where it stands. */
static inline int iotdata_mesh_pack_diag_survey_response(uint8_t *buf, int len, uint16_t sender_station, uint16_t sender_seq, uint16_t directive_id, const iotdata_mesh_diag_survey_entry_t *entries, int count,
                                                         const iotdata_mesh_diag_position_t *position) {
    if (count < 0 || count > IOTDATA_MESH_DIAG_SURVEY_MAX || (count > 0 && entries == NULL))
        return 0;
    const int need = IOTDATA_MESH_DIAG_SURVEY_RSP_SIZE + count * IOTDATA_MESH_DIAG_SURVEY_RSP_ENTRY + (position != NULL ? IOTDATA_MESH_DIAG_SURVEY_POS_SIZE : 0);
    if (len < need)
        return 0;
    (void)iotdata_mesh_pack_diag_header(buf, len, sender_station, sender_seq, IOTDATA_MESH_DIAG_SURVEY, IOTDATA_MESH_DIAG_OP_RESPONSE, directive_id);
    int off = IOTDATA_MESH_DIAG_SURVEY_RSP_SIZE;
    /* the position FIRST, so a reader that only wants it stops after one record */
    if (position != NULL) {
        buf[off] = 0u;
        buf[off + 1] = 0u; /* IOTDATA_MESH_STATION_RESERVED: not an observation */
        _iotdata_mesh_wr_i32(&buf[off + 2], position->latitude_e7);
        _iotdata_mesh_wr_i32(&buf[off + 6], position->longitude_e7);
        buf[off + 10] = (uint8_t)((uint16_t)position->altitude_m >> 8);
        buf[off + 11] = (uint8_t)((uint16_t)position->altitude_m & 0xFFu);
        off += IOTDATA_MESH_DIAG_SURVEY_POS_SIZE;
    }
    for (int i = 0; i < count; i++) {
        buf[off] = (uint8_t)(entries[i].station >> 8);
        buf[off + 1] = (uint8_t)(entries[i].station & 0xFFu);
        buf[off + 2] = (uint8_t)(entries[i].sequence >> 8);
        buf[off + 3] = (uint8_t)(entries[i].sequence & 0xFFu);
        buf[off + 4] = iotdata_mesh_rssi_dbm_encode(entries[i].rssi_dbm);
        buf[off + 5] = (uint8_t)entries[i].snr_q4;
        off += IOTDATA_MESH_DIAG_SURVEY_RSP_ENTRY;
    }
    return need;
}

/* Walks the body one record at a time, skipping extensions. Start with *off at
   IOTDATA_MESH_DIAG_SURVEY_RSP_SIZE; returns false at the end, or on a tail that is not a whole
   record -- which is the structural check the absent count would never have given. */
static inline bool iotdata_mesh_diag_survey_entry_next(const uint8_t *buf, int len, int *off, iotdata_mesh_diag_survey_entry_t *e) {
    while (*off + IOTDATA_MESH_DIAG_SURVEY_RSP_ENTRY <= len) {
        const int at = *off;
        const uint16_t station = (uint16_t)(((uint16_t)buf[at] << 8) | buf[at + 1]);
        if (station == IOTDATA_MESH_STATION_RESERVED) { /* an extension: step over it */
            if (at + IOTDATA_MESH_DIAG_SURVEY_POS_SIZE > len)
                return false; /* truncated extension: stop rather than read past it */
            *off = at + IOTDATA_MESH_DIAG_SURVEY_POS_SIZE;
        } else {
            *off = at + IOTDATA_MESH_DIAG_SURVEY_RSP_ENTRY;
            e->station = station;
            e->sequence = (uint16_t)(((uint16_t)buf[at + 2] << 8) | buf[at + 3]);
            e->rssi_dbm = iotdata_mesh_rssi_dbm_decode(buf[at + 4]);
            e->snr_q4 = (int8_t)buf[at + 5];
            return true;
        }
    }
    return false;
}

/* The observer's position, if it gave one. At most one: there is one observer. */
static inline bool iotdata_mesh_diag_survey_position(const uint8_t *buf, int len, iotdata_mesh_diag_position_t *p) {
    for (int at = IOTDATA_MESH_DIAG_SURVEY_RSP_SIZE; at + IOTDATA_MESH_DIAG_SURVEY_RSP_ENTRY <= len;) {
        const uint16_t station = (uint16_t)(((uint16_t)buf[at] << 8) | buf[at + 1]);
        if (station == IOTDATA_MESH_STATION_RESERVED) {
            if (at + IOTDATA_MESH_DIAG_SURVEY_POS_SIZE > len)
                return false;
            p->latitude_e7 = _iotdata_mesh_rd_i32(&buf[at + 2]);
            p->longitude_e7 = _iotdata_mesh_rd_i32(&buf[at + 6]);
            p->altitude_m = (int16_t)(((uint16_t)buf[at + 10] << 8) | buf[at + 11]);
            return true;
        } else {
            at += IOTDATA_MESH_DIAG_SURVEY_RSP_ENTRY;
        }
    }
    return false;
}

/* How many observations the body holds. For logging and for tests; a consumer should walk. */
static inline int iotdata_mesh_diag_survey_entry_count(const uint8_t *buf, int len) {
    int n = 0, off = IOTDATA_MESH_DIAG_SURVEY_RSP_SIZE;
    iotdata_mesh_diag_survey_entry_t e;
    while (iotdata_mesh_diag_survey_entry_next(buf, len, &off, &e))
        n++;
    return n;
}

/* True when the body is a whole number of records with nothing left over. */
static inline bool iotdata_mesh_diag_survey_response_ok(const uint8_t *buf, int len) {
    if (len < IOTDATA_MESH_DIAG_SURVEY_RSP_SIZE)
        return false;
    int off = IOTDATA_MESH_DIAG_SURVEY_RSP_SIZE;
    while (off + IOTDATA_MESH_DIAG_SURVEY_RSP_ENTRY <= len) {
        const uint16_t station = (uint16_t)(((uint16_t)buf[off] << 8) | buf[off + 1]);
        const int step = (station == (uint16_t)IOTDATA_MESH_STATION_RESERVED) ? IOTDATA_MESH_DIAG_SURVEY_POS_SIZE : IOTDATA_MESH_DIAG_SURVEY_RSP_ENTRY;
        if (off + step > len)
            return false;
        off += step;
    }
    return off == len;
}

/* -------------------------------------------------------------------------
 * Duplicate suppression ring buffer
 * ----------------------------------------------------------------------- */

typedef struct {
    uint16_t station_id;
    uint16_t sequence;
} iotdata_mesh_dedup_entry_t;

typedef struct {
    iotdata_mesh_dedup_entry_t entries[IOTDATA_MESH_DEDUP_RING_SIZE];
    int head, size;
} iotdata_mesh_dedup_ring_t;

static inline void iotdata_mesh_dedup_init(iotdata_mesh_dedup_ring_t *ring) {
    memset(ring, 0, sizeof(*ring));
}

static inline bool iotdata_mesh_dedup_insert(iotdata_mesh_dedup_ring_t *ring, uint16_t station_id, uint16_t sequence) {
    /* scan for duplicate */
    for (int i = 0; i < (ring->size < IOTDATA_MESH_DEDUP_RING_SIZE ? ring->size : IOTDATA_MESH_DEDUP_RING_SIZE); i++)
        if (ring->entries[i].station_id == station_id && ring->entries[i].sequence == sequence)
            return false; /* duplicate */
    /* new — add to ring */
    ring->entries[ring->head].station_id = station_id;
    ring->entries[ring->head].sequence = sequence;
    ring->head = (ring->head + 1) % IOTDATA_MESH_DEDUP_RING_SIZE;
    if (ring->size < IOTDATA_MESH_DEDUP_RING_SIZE)
        ring->size++;
    return true; /* new */
}

/* -------------------------------------------------------------------------
 * Generation comparison (modular, 12-bit)
 *
 * Returns true if gen_a is newer than gen_b.
 * ----------------------------------------------------------------------- */

static inline bool iotdata_mesh_generation_newer(uint16_t gen_a, uint16_t gen_b) {
    const uint16_t diff = (gen_a - gen_b) & (IOTDATA_MESH_GENERATION_MOD - 1);
    return diff > 0 && diff < IOTDATA_MESH_GENERATION_HALF;
}

/* -------------------------------------------------------------------------
 * RSSI quantisation (4-bit, 5 dBm steps from -120 dBm floor)
 * ----------------------------------------------------------------------- */

static inline uint8_t iotdata_mesh_rssi_encode(int rssi_dbm) {
    const int q = (rssi_dbm + 120) / 5;
    return (uint8_t)(q < 0 ? 0 : (q > 15 ? 15 : q));
}

static inline int iotdata_mesh_rssi_decode(uint8_t q) {
    return (int)q * 5 - 120;
}

/* -------------------------------------------------------------------------
 * Control type name (for logging)
 * ----------------------------------------------------------------------- */

static inline const char *iotdata_mesh_ctrl_name(uint8_t ctrl_type) {
    switch (ctrl_type) {
    case IOTDATA_MESH_CTRL_BEACON:
        return "BEACON";
    case IOTDATA_MESH_CTRL_FORWARD:
        return "FORWARD";
    case IOTDATA_MESH_CTRL_ACK:
        return "ACK";
    case IOTDATA_MESH_CTRL_ROUTE_ERROR:
        return "ROUTE_ERROR";
    case IOTDATA_MESH_CTRL_NEIGHBOUR_RPT:
        return "NEIGHBOUR_RPT";
    case IOTDATA_MESH_CTRL_PING:
        return "PING";
    case IOTDATA_MESH_CTRL_PONG:
        return "PONG";
    case IOTDATA_MESH_CTRL_DIAG:
        return "DIAG";
    default:
        return "UNKNOWN";
    }
}

static inline const char *iotdata_mesh_diag_kind_name(uint8_t kind) {
    switch (kind) {
    case IOTDATA_MESH_DIAG_SURVEY:
        return "survey";
    default:
        return "unknown";
    }
}

static inline const char *iotdata_mesh_diag_op_name(uint8_t op) {
    switch (op) {
    case IOTDATA_MESH_DIAG_OP_REQUEST:
        return "request";
    case IOTDATA_MESH_DIAG_OP_RESPONSE:
        return "response";
    case IOTDATA_MESH_DIAG_OP_ERROR:
        return "error";
    case IOTDATA_MESH_DIAG_OP_CANCEL:
        return "cancel";
    case IOTDATA_MESH_DIAG_OP_CANCEL_ALL:
        return "cancel_all";
    default:
        return "unknown";
    }
}

static inline const char *iotdata_mesh_reason_name(uint8_t reason) {
    switch (reason) {
    case IOTDATA_MESH_REASON_PARENT_LOST:
        return "parent_lost";
    case IOTDATA_MESH_REASON_OVERLOADED:
        return "overloaded";
    case IOTDATA_MESH_REASON_SHUTDOWN:
        return "shutdown";
    default:
        return "unknown";
    }
}

#endif /* IOTDATA_MESH_H */
