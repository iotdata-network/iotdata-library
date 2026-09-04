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

#define IOTDATA_MESH_VARIANT                             0x0F

/* Control types (upper nibble of byte 4) */
#define IOTDATA_MESH_CTRL_BEACON                         0x0
#define IOTDATA_MESH_CTRL_FORWARD                        0x1
#define IOTDATA_MESH_CTRL_ACK                            0x2
#define IOTDATA_MESH_CTRL_ROUTE_ERROR                    0x3
#define IOTDATA_MESH_CTRL_NEIGHBOUR_RPT                  0x4
#define IOTDATA_MESH_CTRL_PING                           0x5 /* v2 */
#define IOTDATA_MESH_CTRL_PONG                           0x6 /* v2 */
#define IOTDATA_MESH_CTRL_MANAGE                         0x7 /* node management / diagnostics (request + response, see command byte) */

/* Management (ctrl 0x7) commands — byte 6; command-specific args follow at byte 8 */
/* Command byte (MANAGE frame, byte 6). The high bit (IOTDATA_MESH_MANAGE_RESPONSE) is the
 * DIRECTION: clear = a REQUEST to a node, set = a RESPONSE from a node (routed up). So a node
 * or an in-path analyser classifies request vs reply from the command byte alone — never by
 * inspecting the args (a request may carry scope ids; a reply carries the report). Report-type
 * commands come as explicit REQUEST/RESPONSE pairs; action commands are request-only for now.
 * The RESPONSE opcodes are RESERVED — the uplink response mechanism is future work (reports go
 * to the node console for now); broadcast responses will be spread via the timed TX queue. */
#define IOTDATA_MESH_MANAGE_RESPONSE                     0x80 /* command-byte flag: set = response, clear = request */

#define IOTDATA_MESH_MANAGE_CMD_STATUS_REQUEST           0x01 /* dump status + peers + filter (no args) */
#define IOTDATA_MESH_MANAGE_CMD_STATUS_RESPONSE          (IOTDATA_MESH_MANAGE_CMD_STATUS_REQUEST | IOTDATA_MESH_MANAGE_RESPONSE)
#define IOTDATA_MESH_MANAGE_CMD_STATIONS_REPORT_REQUEST  0x02 /* dump the "stations heard" table (mesh + sensor, no args) */
#define IOTDATA_MESH_MANAGE_CMD_STATIONS_REPORT_RESPONSE (IOTDATA_MESH_MANAGE_CMD_STATIONS_REPORT_REQUEST | IOTDATA_MESH_MANAGE_RESPONSE)
#define IOTDATA_MESH_MANAGE_CMD_PEERS_REPORT_REQUEST     0x03 /* dump the neighbour/peer table (no args) */
#define IOTDATA_MESH_MANAGE_CMD_PEERS_REPORT_RESPONSE    (IOTDATA_MESH_MANAGE_CMD_PEERS_REPORT_REQUEST | IOTDATA_MESH_MANAGE_RESPONSE)
#define IOTDATA_MESH_MANAGE_CMD_PEERS_REMOVE             0x04 /* action: forget one peer; args: station(2 BE) */
#define IOTDATA_MESH_MANAGE_CMD_PEERS_CLEAR              0x05 /* action: clear the neighbour table, re-discover (no args) */
#define IOTDATA_MESH_MANAGE_CMD_FILTER_REPORT_REQUEST    0x10 /* dump the station filter table (no args) */
#define IOTDATA_MESH_MANAGE_CMD_FILTER_REPORT_RESPONSE   (IOTDATA_MESH_MANAGE_CMD_FILTER_REPORT_REQUEST | IOTDATA_MESH_MANAGE_RESPONSE)
#define IOTDATA_MESH_MANAGE_CMD_FILTER_INSERT            0x11 /* action; args: station(2 BE) + action(1) */
#define IOTDATA_MESH_MANAGE_CMD_FILTER_REMOVE            0x12 /* action; args: station(2 BE) */
#define IOTDATA_MESH_MANAGE_CMD_FILTER_CLEAR             0x13 /* action; args: scope(1) */
/* request opcodes 0x05..0x0F, 0x14..0x7F reserved (reboot, set-param, ...); 0x80+ are their responses */

/* Station-filter action (byte in FILTER_INSERT args) and clear scope (FILTER_CLEAR args) */
#define IOTDATA_MESH_MANAGE_FILTER_BLOCK                 0x00 /* blacklist: drop this station's frames */
#define IOTDATA_MESH_MANAGE_FILTER_ALLOW                 0x01 /* whitelist: when any exist, only these pass */
#define IOTDATA_MESH_MANAGE_FILTER_SCOPE_ALL             0x00
#define IOTDATA_MESH_MANAGE_FILTER_SCOPE_MANUAL          0x01
#define IOTDATA_MESH_MANAGE_FILTER_SCOPE_AUTO            0x02

#define IOTDATA_MESH_MANAGE_TARGET_ALL                   0xFFF /* broadcast: every node executes */

/* Route error reasons (lower nibble of byte 4) */
#define IOTDATA_MESH_REASON_PARENT_LOST                  0x0
#define IOTDATA_MESH_REASON_OVERLOADED                   0x1
#define IOTDATA_MESH_REASON_SHUTDOWN                     0x2

/* Beacon flags */
#define IOTDATA_MESH_FLAG_ACCEPTING                      0x01 /* gateway is accepting forwards */

/* Special values */
#define IOTDATA_MESH_PARENT_NONE                         0xFFF /* orphaned — no parent */
#define IOTDATA_MESH_STATION_RESERVED                    0x000 /* do not assign to nodes */

/* Protocol limits */
#define IOTDATA_MESH_TTL_DEFAULT                         7
#define IOTDATA_MESH_TTL_MAX                             255
#define IOTDATA_MESH_GENERATION_HALF                     2048 /* for modular comparison */
#define IOTDATA_MESH_GENERATION_MOD                      4096
#define IOTDATA_MESH_MAX_NEIGHBOURS                      63

/* Packet sizes */
#define IOTDATA_MESH_BEACON_SIZE                         9
#define IOTDATA_MESH_FORWARD_HDR_SIZE                    6 /* + inner packet bytes */
#define IOTDATA_MESH_ACK_SIZE                            8
#define IOTDATA_MESH_ROUTE_ERROR_SIZE                    5
#define IOTDATA_MESH_NEIGHBOUR_HDR_SIZE                  10 /* + 3 per entry */
#define IOTDATA_MESH_NEIGHBOUR_ENTRY_SZ                  3
#define IOTDATA_MESH_PING_SIZE                           8
#define IOTDATA_MESH_PONG_SIZE                           8
#define IOTDATA_MESH_MANAGE_HDR_SIZE                     8 /* + arg_len bytes of command args */

/* Dedup ring default size */
#define IOTDATA_MESH_DEDUP_RING_SIZE                     64

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
 * BEACON (ctrl_type 0x0) — 9 bytes
 *
 * byte 4-5: ctrl(4) | gateway_id(12)
 * byte 6:   cost(8)
 * byte 7:   flags(4) | generation[11:8](4)
 * byte 8:   generation[7:0](8)
 * ----------------------------------------------------------------------- */

typedef struct {
    uint16_t sender_station;
    uint16_t sender_seq;
    uint16_t gateway_id;
    uint8_t cost;
    uint8_t flags;
    uint16_t generation;
} iotdata_mesh_beacon_t;

static inline void iotdata_mesh_pack_beacon(uint8_t *buf, const iotdata_mesh_beacon_t *b) {
    iotdata_mesh_pack_header(buf, b->sender_station, b->sender_seq);
    iotdata_mesh_pack_4_12(&buf[4], IOTDATA_MESH_CTRL_BEACON, b->gateway_id);
    buf[6] = b->cost;
    buf[7] = (uint8_t)(((b->flags & 0x0F) << 4) | ((b->generation >> 8) & 0x0F));
    buf[8] = (uint8_t)(b->generation & 0xFF);
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

static inline void iotdata_mesh_pack_forward(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint8_t ttl, const uint8_t *inner, int inner_len) {
    iotdata_mesh_pack_header(buf, sender_station, sender_seq);
    buf[4] = (uint8_t)((IOTDATA_MESH_CTRL_FORWARD << 4) | ((ttl >> 4) & 0x0F));
    buf[5] = (uint8_t)((ttl & 0x0F) << 4);
    memcpy(&buf[6], inner, (size_t)inner_len);
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

static inline void iotdata_mesh_pack_ack(uint8_t *buf, const iotdata_mesh_ack_t *a) {
    iotdata_mesh_pack_header(buf, a->sender_station, a->sender_seq);
    iotdata_mesh_pack_4_12(&buf[4], IOTDATA_MESH_CTRL_ACK, a->origin_station);
    buf[6] = (uint8_t)(a->origin_sequence >> 8);
    buf[7] = (uint8_t)(a->origin_sequence & 0xFF);
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
static inline int iotdata_mesh_pack_neighbour_report(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t parent_id, uint8_t my_cost, uint16_t gateway_id, const iotdata_mesh_nbr_entry_t *entries, int count) {
    if (count < 0)
        count = 0;
    if (count > IOTDATA_MESH_NBR_MAX)
        count = IOTDATA_MESH_NBR_MAX;
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

static inline void iotdata_mesh_pack_route_error(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint8_t reason) {
    iotdata_mesh_pack_header(buf, sender_station, sender_seq);
    buf[4] = (uint8_t)((IOTDATA_MESH_CTRL_ROUTE_ERROR << 4) | (reason & 0x0F));
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
 * MANAGE (ctrl_type 0x7) — 8 + N bytes — node management / diagnostics
 *
 * byte 4-5: ctrl(4) | target_station(12)   (0xFFF = broadcast to all nodes)
 * byte 6:   command(8)
 * byte 7:   arg_len(8)
 * byte 8+:  args (command-specific, arg_len bytes)
 *
 * A supervisory channel: a gateway (or, later, a relay) addresses a command at one
 * node or all of them. New commands are new opcodes with their own args — the frame
 * shape never changes. See IOTDATA_MESH_MANAGE_CMD_* for the command vocabulary.
 * ----------------------------------------------------------------------- */

typedef struct {
    uint16_t sender_station;
    uint16_t sender_seq;
    uint16_t target_station; /* IOTDATA_MESH_MANAGE_TARGET_ALL (0xFFF) = broadcast */
    uint8_t command;
    uint8_t arg_len;
    const uint8_t *args; /* pointer into the receive buffer, not owned; NULL if arg_len == 0 */
} iotdata_mesh_manage_t;

/* Returns the total packet length (IOTDATA_MESH_MANAGE_HDR_SIZE + arg_len). */
static inline int iotdata_mesh_pack_manage(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station, uint8_t command, const uint8_t *args, uint8_t arg_len) {
    iotdata_mesh_pack_header(buf, sender_station, sender_seq);
    iotdata_mesh_pack_4_12(&buf[4], IOTDATA_MESH_CTRL_MANAGE, target_station);
    buf[6] = command;
    buf[7] = arg_len;
    if (arg_len > 0 && args != NULL)
        memcpy(&buf[8], args, arg_len);
    return IOTDATA_MESH_MANAGE_HDR_SIZE + (int)arg_len;
}

static inline int iotdata_mesh_pack_manage_status(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station) {
    return iotdata_mesh_pack_manage(buf, sender_station, sender_seq, target_station, IOTDATA_MESH_MANAGE_CMD_STATUS_REQUEST, NULL, 0);
}

static inline int iotdata_mesh_pack_manage_stations_report(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station) {
    return iotdata_mesh_pack_manage(buf, sender_station, sender_seq, target_station, IOTDATA_MESH_MANAGE_CMD_STATIONS_REPORT_REQUEST, NULL, 0);
}

static inline int iotdata_mesh_pack_manage_peers_report(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station) {
    return iotdata_mesh_pack_manage(buf, sender_station, sender_seq, target_station, IOTDATA_MESH_MANAGE_CMD_PEERS_REPORT_REQUEST, NULL, 0);
}
static inline int iotdata_mesh_pack_manage_peers_remove(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station, uint16_t station) {
    const uint8_t args[2] = { (uint8_t)(station >> 8), (uint8_t)(station & 0xFFu) };
    return iotdata_mesh_pack_manage(buf, sender_station, sender_seq, target_station, IOTDATA_MESH_MANAGE_CMD_PEERS_REMOVE, args, 2);
}
static inline int iotdata_mesh_pack_manage_peers_clear(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station) {
    return iotdata_mesh_pack_manage(buf, sender_station, sender_seq, target_station, IOTDATA_MESH_MANAGE_CMD_PEERS_CLEAR, NULL, 0);
}

static inline int iotdata_mesh_pack_manage_filter_report(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station) {
    return iotdata_mesh_pack_manage(buf, sender_station, sender_seq, target_station, IOTDATA_MESH_MANAGE_CMD_FILTER_REPORT_REQUEST, NULL, 0);
}
static inline int iotdata_mesh_pack_manage_filter_insert(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station, uint16_t station, uint8_t action) {
    const uint8_t args[3] = { (uint8_t)(station >> 8), (uint8_t)(station & 0xFFu), action };
    return iotdata_mesh_pack_manage(buf, sender_station, sender_seq, target_station, IOTDATA_MESH_MANAGE_CMD_FILTER_INSERT, args, 3);
}
static inline int iotdata_mesh_pack_manage_filter_remove(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station, uint16_t station) {
    const uint8_t args[2] = { (uint8_t)(station >> 8), (uint8_t)(station & 0xFFu) };
    return iotdata_mesh_pack_manage(buf, sender_station, sender_seq, target_station, IOTDATA_MESH_MANAGE_CMD_FILTER_REMOVE, args, 2);
}
static inline int iotdata_mesh_pack_manage_filter_clear(uint8_t *buf, uint16_t sender_station, uint16_t sender_seq, uint16_t target_station, uint8_t scope) {
    const uint8_t args[1] = { scope };
    return iotdata_mesh_pack_manage(buf, sender_station, sender_seq, target_station, IOTDATA_MESH_MANAGE_CMD_FILTER_CLEAR, args, 1);
}

static inline bool iotdata_mesh_unpack_manage(const uint8_t *buf, int len, iotdata_mesh_manage_t *m) {
    if (len < IOTDATA_MESH_MANAGE_HDR_SIZE)
        return false;
    uint8_t ctrl;
    iotdata_mesh_unpack_4_12(&buf[0], &ctrl, &m->sender_station);
    m->sender_seq = ((uint16_t)buf[2] << 8) | buf[3];
    iotdata_mesh_unpack_4_12(&buf[4], &ctrl, &m->target_station);
    m->command = buf[6];
    m->arg_len = buf[7];
    if (len < IOTDATA_MESH_MANAGE_HDR_SIZE + (int)m->arg_len)
        return false; /* truncated args */
    m->args = (m->arg_len > 0) ? &buf[8] : NULL;
    return true;
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

/* returns true if this is a NEW packet (not a duplicate) */
static inline bool iotdata_mesh_dedup_check_and_add(iotdata_mesh_dedup_ring_t *ring, uint16_t station_id, uint16_t sequence) {
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
    case IOTDATA_MESH_CTRL_MANAGE:
        return "MANAGE";
    default:
        return "UNKNOWN";
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
