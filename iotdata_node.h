/* iotdata_node.h
 *
 * System TLV definitions for iotdata NODES (gateways, relays, sensors).
 *
 * Every iotdata node has a station id, and every node must support these TLVs. They ride in
 * ordinary iotdata packets, so they can be sent unicast or broadcast, and are independent of the
 * packet's variant -- a node speaks this regardless of what telemetry it produces.
 *
 * This header owns the SYSTEM type and key identifiers and the helpers for them. The framing
 * itself -- how a key-value pair is laid out -- lives in iotdata_fields.h and knows nothing about
 * these assignments. That split is deliberate: proprietary types and keys use the same framing
 * without needing anything from here.
 *
 * ---------------------------------------------------------------------------------------------
 * TYPES  (TLV type field, 6 bits)
 *
 *   bit 5 clear  SYSTEM, assigned below (0x00..0x1F).
 *   bit 5 set    PROPRIETARY, free for a vendor or application (0x20..0x3F).
 *
 * KEYS  (the K of a key-value pair, 1 byte)
 *
 *   bit 7 clear  SYSTEM, assigned below, per type -- key numbering restarts for each type.
 *   bit 7 set    PROPRIETARY, free for a vendor or application.
 *
 * Both fields split on their own top bit, so it is one rule at two widths: 32 system types and
 * 128 system keys per type, with the same again for proprietary use.
 *
 * A reader walks pairs it does not recognise and skips them (the length makes this safe), so a
 * new key never breaks an older peer, and proprietary keys coexist with system ones. Use
 * iotdata_tlv_type_is_system() / iotdata_tlv_key_is_system() to tell them apart.
 *
 * ---------------------------------------------------------------------------------------------
 * ENCODING
 *
 * These TLVs use the "kvr" framing (iotdata_fields.h): [K u8][L u8][V ...L bytes], carried in a
 * TLV whose FMT bit is RAW. Values are binary, with the width implied by the key -- see the
 * per-key comments and iotdata_node_key_width(). Build with iotdata_kvr_*, walk with
 * iotdata_kvr_next().
 *
 * The "kvs" text framing (FMT_STRING) exists in iotdata_fields.h but is NOT used by the system
 * TLVs: the 6-bit string alphabet is [A-Za-z0-9 ] with no punctuation, so it cannot carry a
 * dotted version, a negative temperature, or CSV. Node handling assumes kvr throughout.
 *
 * A value whose width does not match what the key expects is not fatal: the iotdata_kvr_* readers
 * return the caller's default, so a peer that encodes a key oddly degrades a field rather than
 * the packet.
 *
 * ---------------------------------------------------------------------------------------------
 * CONTROL
 *
 * Each control command is its own key; a single CONTROL TLV may therefore carry several commands,
 * and each command's value is that command's own argument encoding (sub-command, operation,
 * arguments -- defined per command, opaque here). Two rules:
 *
 *   - commands execute in WIRE ORDER, so e.g. "clear diagnostics" then "reboot" does both;
 *   - an unrecognised command key is SKIPPED, not an error -- which is what lets a mixed-vintage
 *     fleet accept a packet containing commands only some of them implement.
 *
 * Requests are not correlated with a token: a requester simply waits for the corresponding TLV to
 * arrive, and treats solicited and unsolicited (periodic) reports identically.
 *
 * ---------------------------------------------------------------------------------------------
 * DOWNSTREAM: addressing a command to a node
 *
 * iotdata is one-way telemetry: a frame's station field says who SENT it, and the 32-bit header is
 * fully allocated, so there is no destination field to add one to. A frame whose sequence is
 * IOTDATA_SEQUENCE_DOWNSTREAM (0xFFFF) inverts the meaning of that one field: the station is who
 * the frame is FOR. A node processes such a frame if the station is its own or
 * IOTDATA_STATION_BROADCAST, and ignores it otherwise. Nothing else about the frame changes, so a
 * downstream frame is decoded by an ordinary decoder and carries ordinary system TLVs.
 *
 * This is deliberately not routed. A downstream frame is transmitted by whoever has it -- usually
 * a gateway -- and any relay that hears one holds a single copy per target station and rebroadcasts
 * it. A relay compares an inbound downstream frame against the one it is already holding for that
 * target: identical means it is the echo of its own or a neighbour's rebroadcast, so it is ignored;
 * different means a newer command has superseded the old one, so it replaces it and is sent on.
 * That single comparison is both the loop-breaker and the supersede rule, and it bounds propagation
 * at one transmission per relay without needing a hop count -- which is just as well, because the
 * header has no room for one either.
 *
 * Held frames are never evicted on a timer, only oldest-first when the table is full. A sensor
 * buried under snow for a week therefore still collects its pending diagnostics request on the day
 * it comes back, which is exactly when it is wanted. Indefinite hold is only safe because commands
 * are expected to be IDEMPOTENT and state-relative rather than imperative -- "clear diagnostics up
 * to record N", not "clear diagnostics" -- so a stale command that finally lands is a no-op rather
 * than a surprise. There is no acknowledgement anywhere in this path; idempotence is what replaces
 * it. Design new control keys accordingly.
 *
 * Broadcast cannot be held -- no station ever transmits as IOTDATA_STATION_BROADCAST, so there is
 * no arrival to trigger delivery on. Broadcast downstream is therefore immediate-only, and should
 * be confined to the read-only requests.
 *
 * ---------------------------------------------------------------------------------------------
 * RECEIVE: when a node can be reached
 *
 * Most nodes are asleep. A sensor transmits and returns to deep sleep with its radio off, so a
 * downstream frame aimed at it lands on a deaf node. RECEIVE is how a node says otherwise: it
 * appears in a node's own outbound frame and means "I am listening after this one". Whoever hears
 * that frame and is holding something for that station sends it then.
 *
 * The node decides. Only it knows its power budget, and only it knows whether it is configured to
 * accept firmware; nothing upstream needs to model any of that, because a node that does not want
 * to be reached simply never sends RECEIVE and is never sent to. That is also the default -- the
 * TLV is absent, and costs nothing.
 *
 * Every key is optional, so an empty RECEIVE TLV is a complete and useful message: "listening, for
 * anything, for as long as I choose". That is two bytes on the wire, which is the smallest thing
 * this format can express, and far less than the receive window it announces costs in energy. A
 * node with more to say adds DURATION (so a sender can skip a window too short for the frame it is
 * holding) and TYPES (so a node that declines firmware is not sent any).
 *
 * ---------------------------------------------------------------------------------------------
 * SIZE
 *
 * A TLV's length field is 8 bits, so one TLV carries at most 255 bytes, and a packet at most
 * IOTDATA_TLV_MAX of them -- less on a radio link, where the frame bounds the whole packet.
 * Anything larger (diagnostics records, firmware content) is therefore split across packets by
 * the sending node, in a manner chosen by that node and defined per TLV type; the TLV layer
 * provides no fragmentation of its own.
 */

#ifndef IOTDATA_NODE_H
#define IOTDATA_NODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "iotdata.h" /* iotdata_variant_def_t and the presence-slot geometry, for VARIANT */

/* -------------------------------------------------------------------------
 * System TLV types
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_TLV_RECEIVE     0x00 /* when this node can be reached (see RECEIVE above)  */
#define IOTDATA_NODE_TLV_VERSION     0x01 /* what this node IS: firmware, hardware, platform    */
#define IOTDATA_NODE_TLV_VARIANT     0x02 /* which telemetry variant it is producing            */
#define IOTDATA_NODE_TLV_CONTROL     0x03 /* commands TO a node, and its command inventory      */
#define IOTDATA_NODE_TLV_STATUS      0x04 /* how it is DOING: uptime, restarts, supply, heap    */
#define IOTDATA_NODE_TLV_CONFIG      0x05 /* settable operating parameters                      */
#define IOTDATA_NODE_TLV_DIAGNOSTICS 0x06 /* recorded diagnostic data (blackbox)                */
#define IOTDATA_NODE_TLV_CONTENT     0x07 /* bulk payload: firmware image, user data            */

/* Returned by the lookups below when there is no corresponding type. Not 0: 0x00 is a type. */
#define IOTDATA_NODE_TLV_NONE        0xFF

/* -------------------------------------------------------------------------
 * RECEIVE keys (0x00)
 *
 * Both optional -- an empty RECEIVE means "listening, for anything, for a period I am not telling
 * you". A sender holding a frame for this node transmits it on hearing this TLV.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_RECEIVE_DURATION 0x00 /* u16 milliseconds the receiver stays on            */
#define IOTDATA_NODE_RECEIVE_TYPES    0x01 /* u32 bitmask, bit N = "I accept TLV type N"        */

/* -------------------------------------------------------------------------
 * VERSION keys (0x01) -- all values are strings (no NUL on the wire)
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_VERSION_FIRMWARE    0x00 /* application firmware version, e.g. "1.4.2"     */
#define IOTDATA_NODE_VERSION_HARDWARE    0x01 /* board revision                                 */
#define IOTDATA_NODE_VERSION_PLATFORM    0x02 /* chip/target, e.g. "esp32c3"                    */
#define IOTDATA_NODE_VERSION_LIBRARY     0x03 /* iotdata library version                        */
#define IOTDATA_NODE_VERSION_BUILD       0x04 /* build stamp: date/time or vcs hash             */
#define IOTDATA_NODE_VERSION_APPLICATION 0x05 /* application name, e.g. "iotdata_relay"         */
#define IOTDATA_NODE_VERSION_SERIAL      0x06 /* stable per-device id (MAC-derived, etc.)       */

/* -------------------------------------------------------------------------
 * VARIANT keys (0x02) -- the key IS the variant number
 *
 * A node reports the whole variant suite it can produce, one key-value pair per variant, so the
 * key is the variant number itself: 0..IOTDATA_VARIANT_MAX, never 15, which is the mesh variant
 * and carries no telemetry. A node with no telemetry variants at all -- a gateway -- sends the TLV
 * EMPTY rather than omitting it, because "I produce none" is an answer and not a failure to reply.
 *
 * Each value describes one variant:
 *
 *   [nlen u8][name, nlen bytes][field ids, 12 bits each, tightly packed]
 *
 * The field ids are POSITIONAL: one per presence slot, in slot order, so index N is the field
 * carried by presence slot N. An unused slot is emitted as IOTDATA_NODE_VARIANT_FIELD_NONE rather
 * than skipped -- dropping it would shift every slot after it. The number of fields is whatever
 * fits in what remains after the name, so no count is carried.
 *
 * CHUNKING: a whole suite does not fit one packet -- nine variants is already ~240 bytes, and a
 * LoRa frame at a high spreading factor holds far less -- so a node emits as many variants as fit
 * and continues in the next packet. No fragmentation mechanism is needed for this, because each
 * key-value pair is self-contained: a variant either arrives whole or arrives later, receiving one
 * twice is harmless, and the receiver merges by key.
 *
 * What that alone cannot tell a receiver is when it has them ALL, so a node also sends
 * IOTDATA_NODE_VARIANT_MANIFEST: a bitmask of the variants it defines. It goes in EVERY chunk, not
 * just the first -- it costs two bytes, and it makes each packet self-describing on a lossy link
 * where the first one may not have arrived. A receiver has the complete suite once it holds every
 * variant the manifest names; the population count is the total, so no separate count is carried.
 *
 * REGISTRY (unfinished): a field id is its iotdata_field_type_t value. That enum is generated per
 * build from the variant suite selected for it, so today an id only means anything within a single
 * build and must not be persisted or compared between nodes. Making it global is a change to the
 * enum's numbering, not to anything here -- the encoder, the framing and the width are already
 * what they will be. 12 bits allows 4095 assignments, which should be room enough.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_VARIANT_FIELD_BITS 12
#define IOTDATA_NODE_VARIANT_FIELD_MAX  0x0FFEu
#define IOTDATA_NODE_VARIANT_FIELD_NONE 0x0FFFu /* a presence slot carrying nothing              */

/* The manifest: which variants this node defines, one bit per variant number. It sits above the
   variant numbers (0x00..0x0E) in the key space, and not at 0x0F, which would read as "variant 15"
   -- the mesh variant, deliberately absent here. */
#define IOTDATA_NODE_VARIANT_MANIFEST 0x10 /* u16 bitmask, bit N = "I define variant N"          */

/* -------------------------------------------------------------------------
 * CONTROL keys (0x03)
 *
 * 0x00..0x0F  ask the node to send a TLV of the corresponding type. The value is an optional,
 *             command-specific argument (absent = "everything you have"). Corresponds to the 
 *             TLV numeric value.
 * 0x10..0x1F  generic system control.
 * 0x20..0x7F  mesh management, migrated from the MANAGE control frame (iotdata_mesh.h).
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_CONTROL_REQUEST_VERSION     0x01
#define IOTDATA_NODE_CONTROL_REQUEST_VARIANT     0x02
#define IOTDATA_NODE_CONTROL_REQUEST_CONTROL     0x03
#define IOTDATA_NODE_CONTROL_REQUEST_STATUS      0x04
#define IOTDATA_NODE_CONTROL_REQUEST_CONFIG      0x05
#define IOTDATA_NODE_CONTROL_REQUEST_DIAGNOSTICS 0x06
#define IOTDATA_NODE_CONTROL_REQUEST_CONTENT     0x07

#define IOTDATA_NODE_CONTROL_REBOOT              0x10 /* value: optional u16 delay seconds      */

#define IOTDATA_NODE_CONTROL_MESH_BASE           0x20 /* mesh commands start here (migration)   */

/* -------------------------------------------------------------------------
 * STATUS keys (0x04)
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_STATUS_UPTIME      0x00 /* u32 seconds, this session                       */
#define IOTDATA_NODE_STATUS_LIFETIME    0x01 /* u32 seconds, cumulative across boots            */
#define IOTDATA_NODE_STATUS_RESTARTS    0x02 /* u16 boots                                       */
#define IOTDATA_NODE_STATUS_REASON      0x03 /* u8, IOTDATA_NODE_REASON_*                       */
#define IOTDATA_NODE_STATUS_TEMPERATURE 0x04 /* i8 degrees C                                    */
#define IOTDATA_NODE_STATUS_SUPPLY      0x05 /* u16 millivolts                                  */
#define IOTDATA_NODE_STATUS_HEAP_FREE   0x06 /* u32 bytes free now                              */
#define IOTDATA_NODE_STATUS_HEAP_MIN    0x07 /* u32 bytes, lowest free seen                     */
#define IOTDATA_NODE_STATUS_ACTIVE      0x08 /* u32 seconds awake this session (vs asleep)      */

/* Reset reasons (STATUS_REASON). A node maps its platform's reason onto these. */
#define IOTDATA_NODE_REASON_UNKNOWN   0x00
#define IOTDATA_NODE_REASON_POWER_ON  0x01
#define IOTDATA_NODE_REASON_SOFTWARE  0x02
#define IOTDATA_NODE_REASON_WATCHDOG  0x03
#define IOTDATA_NODE_REASON_BROWNOUT  0x04
#define IOTDATA_NODE_REASON_PANIC     0x05
#define IOTDATA_NODE_REASON_DEEPSLEEP 0x06
#define IOTDATA_NODE_REASON_EXTERNAL  0x07
#define IOTDATA_NODE_REASON_OTA       0x08

/* -------------------------------------------------------------------------
 * CONFIG keys (0x05)
 *
 * Reporting cadence, one key per reportable type: u16 seconds, 0 = do not send periodically.
 * STARTUP is a bitmask of IOTDATA_NODE_STARTUP_* saying which reports to emit once at boot.
 * Application/device settings are proprietary keys (bit 7 set) for now.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_CONFIG_PERIOD_VERSION     0x01 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_VARIANT     0x02 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_CONTROL     0x03 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_STATUS      0x04 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_CONFIG      0x05 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_DIAGNOSTICS 0x06 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_STARTUP            0x10 /* u16 bitmask, IOTDATA_NODE_STARTUP_*      */

#define IOTDATA_NODE_STARTUP_VERSION     0x0002
#define IOTDATA_NODE_STARTUP_VARIANT     0x0004
#define IOTDATA_NODE_STARTUP_CONTROL     0x0008
#define IOTDATA_NODE_STARTUP_STATUS      0x0010
#define IOTDATA_NODE_STARTUP_CONFIG      0x0020
#define IOTDATA_NODE_STARTUP_DIAGNOSTICS 0x0040

/* -------------------------------------------------------------------------
 * DIAGNOSTICS keys (0x06)
 *
 * TYPE says what the accompanying DATA is (which record set, or which slice of it); DATA carries
 * the records themselves. A node emits as many of these as it chooses per packet -- one record or
 * several -- and splits across packets however suits it.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_DIAGNOSTICS_TYPE 0x00 /* u8, IOTDATA_NODE_DIAG_* -- what DATA holds        */
#define IOTDATA_NODE_DIAGNOSTICS_DATA 0x01 /* bytes: the records (blackbox CSV lines)           */

#define IOTDATA_NODE_DIAG_BLACKBOX 0x00 /* blackbox records, CSV as stored                      */

/* -------------------------------------------------------------------------
 * CONTENT keys (0x07)
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_CONTENT_FIRMWARE 0x00 /* bytes: OTA image data                             */
#define IOTDATA_NODE_CONTENT_USERDATA 0x01 /* bytes: free-form application data                 */

/* -------------------------------------------------------------------------
 * Naming and widths
 *
 * Values are binary, so a decoder needs these to render a system TLV meaningfully (JSON, a status
 * line, a console dump). A proprietary key has no name here and is shown as hex -- honest, rather
 * than guessed at.
 * ----------------------------------------------------------------------- */

/* Expected value width in bytes; 0 means variable (string or opaque bytes). */
#define IOTDATA_NODE_WIDTH_VARIABLE 0

typedef struct {
    uint8_t key;
    uint8_t width;
    const char *name;
} iotdata_node_keydef_t;

static const iotdata_node_keydef_t iotdata_node_tlv_keys_receive[] = {
    { IOTDATA_NODE_RECEIVE_DURATION, 2, "duration" },
    { IOTDATA_NODE_RECEIVE_TYPES, 4, "types" },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_version[] = {
    { IOTDATA_NODE_VERSION_FIRMWARE, IOTDATA_NODE_WIDTH_VARIABLE, "firmware" },
    { IOTDATA_NODE_VERSION_HARDWARE, IOTDATA_NODE_WIDTH_VARIABLE, "hardware" },
    { IOTDATA_NODE_VERSION_PLATFORM, IOTDATA_NODE_WIDTH_VARIABLE, "platform" },
    { IOTDATA_NODE_VERSION_LIBRARY, IOTDATA_NODE_WIDTH_VARIABLE, "library" },
    { IOTDATA_NODE_VERSION_BUILD, IOTDATA_NODE_WIDTH_VARIABLE, "build" },
    { IOTDATA_NODE_VERSION_APPLICATION, IOTDATA_NODE_WIDTH_VARIABLE, "application" },
    { IOTDATA_NODE_VERSION_SERIAL, IOTDATA_NODE_WIDTH_VARIABLE, "serial" },
};

/* One key per variant number, so this table is the variant space itself rather than a set of named
   fields. 15 is absent: it is the mesh variant and carries no telemetry. */
static const iotdata_node_keydef_t iotdata_node_tlv_keys_variant[] = {
    { 0, IOTDATA_NODE_WIDTH_VARIABLE, "variant_0" },   { 1, IOTDATA_NODE_WIDTH_VARIABLE, "variant_1" },
    { 2, IOTDATA_NODE_WIDTH_VARIABLE, "variant_2" },   { 3, IOTDATA_NODE_WIDTH_VARIABLE, "variant_3" },
    { 4, IOTDATA_NODE_WIDTH_VARIABLE, "variant_4" },   { 5, IOTDATA_NODE_WIDTH_VARIABLE, "variant_5" },
    { 6, IOTDATA_NODE_WIDTH_VARIABLE, "variant_6" },   { 7, IOTDATA_NODE_WIDTH_VARIABLE, "variant_7" },
    { 8, IOTDATA_NODE_WIDTH_VARIABLE, "variant_8" },   { 9, IOTDATA_NODE_WIDTH_VARIABLE, "variant_9" },
    { 10, IOTDATA_NODE_WIDTH_VARIABLE, "variant_10" }, { 11, IOTDATA_NODE_WIDTH_VARIABLE, "variant_11" },
    { 12, IOTDATA_NODE_WIDTH_VARIABLE, "variant_12" }, { 13, IOTDATA_NODE_WIDTH_VARIABLE, "variant_13" },
    { 14, IOTDATA_NODE_WIDTH_VARIABLE, "variant_14" }, { IOTDATA_NODE_VARIANT_MANIFEST, 2, "manifest" },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_control[] = {
    { IOTDATA_NODE_CONTROL_REQUEST_VERSION, IOTDATA_NODE_WIDTH_VARIABLE, "request_version" },
    { IOTDATA_NODE_CONTROL_REQUEST_VARIANT, IOTDATA_NODE_WIDTH_VARIABLE, "request_variant" },
    { IOTDATA_NODE_CONTROL_REQUEST_CONTROL, IOTDATA_NODE_WIDTH_VARIABLE, "request_control" },
    { IOTDATA_NODE_CONTROL_REQUEST_STATUS, IOTDATA_NODE_WIDTH_VARIABLE, "request_status" },
    { IOTDATA_NODE_CONTROL_REQUEST_CONFIG, IOTDATA_NODE_WIDTH_VARIABLE, "request_config" },
    { IOTDATA_NODE_CONTROL_REQUEST_DIAGNOSTICS, IOTDATA_NODE_WIDTH_VARIABLE, "request_diagnostics" },
    { IOTDATA_NODE_CONTROL_REQUEST_CONTENT, IOTDATA_NODE_WIDTH_VARIABLE, "request_content" },
    { IOTDATA_NODE_CONTROL_REBOOT, IOTDATA_NODE_WIDTH_VARIABLE, "reboot" },
};
static const iotdata_node_keydef_t iotdata_node_tlv_keys_status[] = {
    { IOTDATA_NODE_STATUS_UPTIME, 4, "uptime" },
    { IOTDATA_NODE_STATUS_LIFETIME, 4, "lifetime" },
    { IOTDATA_NODE_STATUS_RESTARTS, 2, "restarts" },
    { IOTDATA_NODE_STATUS_REASON, 1, "reason" },
    { IOTDATA_NODE_STATUS_TEMPERATURE, 1, "temperature" },
    { IOTDATA_NODE_STATUS_SUPPLY, 2, "supply" },
    { IOTDATA_NODE_STATUS_HEAP_FREE, 4, "heap_free" },
    { IOTDATA_NODE_STATUS_HEAP_MIN, 4, "heap_min" },
    { IOTDATA_NODE_STATUS_ACTIVE, 4, "active" },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_config[] = {
    { IOTDATA_NODE_CONFIG_PERIOD_VERSION, 2, "period_version" },
    { IOTDATA_NODE_CONFIG_PERIOD_VARIANT, 2, "period_variant" },
    { IOTDATA_NODE_CONFIG_PERIOD_CONTROL, 2, "period_control" },
    { IOTDATA_NODE_CONFIG_PERIOD_STATUS, 2, "period_status" },
    { IOTDATA_NODE_CONFIG_PERIOD_CONFIG, 2, "period_config" },
    { IOTDATA_NODE_CONFIG_PERIOD_DIAGNOSTICS, 2, "period_diagnostics" },
    { IOTDATA_NODE_CONFIG_STARTUP, 2, "startup" },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_diagnostics[] = {
    { IOTDATA_NODE_DIAGNOSTICS_TYPE, 1, "type" },
    { IOTDATA_NODE_DIAGNOSTICS_DATA, IOTDATA_NODE_WIDTH_VARIABLE, "data" },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_content[] = {
    { IOTDATA_NODE_CONTENT_FIRMWARE, IOTDATA_NODE_WIDTH_VARIABLE, "firmware" },
    { IOTDATA_NODE_CONTENT_USERDATA, IOTDATA_NODE_WIDTH_VARIABLE, "userdata" },
};

static inline const char *iotdata_node_tlv_name(const uint8_t type) {
    switch (type) {
    case IOTDATA_NODE_TLV_RECEIVE:
        return "receive";
    case IOTDATA_NODE_TLV_VERSION:
        return "version";
    case IOTDATA_NODE_TLV_VARIANT:
        return "variant";
    case IOTDATA_NODE_TLV_CONTROL:
        return "control";
    case IOTDATA_NODE_TLV_STATUS:
        return "status";
    case IOTDATA_NODE_TLV_CONFIG:
        return "config";
    case IOTDATA_NODE_TLV_DIAGNOSTICS:
        return "diagnostics";
    case IOTDATA_NODE_TLV_CONTENT:
        return "content";
    default:
        return NULL; /* proprietary or unassigned */
    }
}

static inline const iotdata_node_keydef_t *iotdata_node_tlv_keys(const uint8_t type, size_t *count) {
#define IOTDATA_NODE_KEYS_RET(tbl) \
    do { \
        if (count != NULL) \
            *count = sizeof(tbl) / sizeof((tbl)[0]); \
        return (tbl); \
    } while (0)
    switch (type) {
    case IOTDATA_NODE_TLV_RECEIVE:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_receive);
    case IOTDATA_NODE_TLV_VERSION:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_version);
    case IOTDATA_NODE_TLV_VARIANT:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_variant);
    case IOTDATA_NODE_TLV_CONTROL:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_control);
    case IOTDATA_NODE_TLV_STATUS:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_status);
    case IOTDATA_NODE_TLV_CONFIG:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_config);
    case IOTDATA_NODE_TLV_DIAGNOSTICS:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_diagnostics);
    case IOTDATA_NODE_TLV_CONTENT:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_content);
    default:
        if (count != NULL)
            *count = 0;
        return NULL;
    }
#undef IOTDATA_NODE_KEYS_RET
}

static inline const iotdata_node_keydef_t *__iotdata_node_tlv_keydef(const uint8_t type, const uint8_t key) {
    size_t n = 0;
    const iotdata_node_keydef_t *const tbl = iotdata_node_tlv_keys(type, &n);
    for (size_t i = 0; tbl != NULL && i < n; i++)
        if (tbl[i].key == key)
            return &tbl[i];
    return NULL;
}

static inline const char *iotdata_node_tlv_key_name(const uint8_t type, const uint8_t key) {
    const iotdata_node_keydef_t *const d = __iotdata_node_tlv_keydef(type, key);
    return (d != NULL) ? d->name : NULL;
}

static inline uint8_t iotdata_node_tlv_key_width(const uint8_t type, const uint8_t key) {
    const iotdata_node_keydef_t *const d = __iotdata_node_tlv_keydef(type, key);
    return (d != NULL) ? d->width : IOTDATA_NODE_WIDTH_VARIABLE;
}

static inline const char *iotdata_node_tlv_status_reason_str(const uint8_t reason) {
    switch (reason) {
    case IOTDATA_NODE_REASON_POWER_ON:
        return "power_on";
    case IOTDATA_NODE_REASON_SOFTWARE:
        return "software";
    case IOTDATA_NODE_REASON_WATCHDOG:
        return "watchdog";
    case IOTDATA_NODE_REASON_BROWNOUT:
        return "brownout";
    case IOTDATA_NODE_REASON_PANIC:
        return "panic";
    case IOTDATA_NODE_REASON_DEEPSLEEP:
        return "deepsleep";
    case IOTDATA_NODE_REASON_EXTERNAL:
        return "external";
    case IOTDATA_NODE_REASON_OTA:
        return "ota";
    default:
        return "unknown";
    }
}

/* CONTROL request key <-> the TLV type it asks for, both returning IOTDATA_NODE_TLV_NONE when there
   is no counterpart. RECEIVE deliberately has neither: it is volunteered by a node when its own
   power budget allows, and asking for it would answer a question about a moment already past. */

static inline uint8_t iotdata_node_tlv_control_type(const uint8_t key) {
    switch (key) {
    case IOTDATA_NODE_CONTROL_REQUEST_VERSION:
        return IOTDATA_NODE_TLV_VERSION;
    case IOTDATA_NODE_CONTROL_REQUEST_VARIANT:
        return IOTDATA_NODE_TLV_VARIANT;
    case IOTDATA_NODE_CONTROL_REQUEST_CONTROL:
        return IOTDATA_NODE_TLV_CONTROL;
    case IOTDATA_NODE_CONTROL_REQUEST_STATUS:
        return IOTDATA_NODE_TLV_STATUS;
    case IOTDATA_NODE_CONTROL_REQUEST_CONFIG:
        return IOTDATA_NODE_TLV_CONFIG;
    case IOTDATA_NODE_CONTROL_REQUEST_DIAGNOSTICS:
        return IOTDATA_NODE_TLV_DIAGNOSTICS;
    case IOTDATA_NODE_CONTROL_REQUEST_CONTENT:
        return IOTDATA_NODE_TLV_CONTENT;
    default:
        return IOTDATA_NODE_TLV_NONE;
    }
}

static inline uint8_t iotdata_node_tlv_control_key(const uint8_t type) {
    switch (type) {
    case IOTDATA_NODE_TLV_VERSION:
        return IOTDATA_NODE_CONTROL_REQUEST_VERSION;
    case IOTDATA_NODE_TLV_VARIANT:
        return IOTDATA_NODE_CONTROL_REQUEST_VARIANT;
    case IOTDATA_NODE_TLV_CONTROL:
        return IOTDATA_NODE_CONTROL_REQUEST_CONTROL;
    case IOTDATA_NODE_TLV_STATUS:
        return IOTDATA_NODE_CONTROL_REQUEST_STATUS;
    case IOTDATA_NODE_TLV_CONFIG:
        return IOTDATA_NODE_CONTROL_REQUEST_CONFIG;
    case IOTDATA_NODE_TLV_DIAGNOSTICS:
        return IOTDATA_NODE_CONTROL_REQUEST_DIAGNOSTICS;
    case IOTDATA_NODE_TLV_CONTENT:
        return IOTDATA_NODE_CONTROL_REQUEST_CONTENT;
    default:
        return IOTDATA_NODE_TLV_NONE;
    }
}

/* -------------------------------------------------------------------------
 * VARIANT codec
 *
 * Packs a variant definition into the value of one VARIANT key-value pair, and reads it back. The
 * 12-bit field ids straddle byte boundaries in a fixed two-phase pattern -- an even index starts
 * on a byte, an odd index starts mid-byte -- so index arithmetic replaces a bit cursor.
 * ----------------------------------------------------------------------- */

/* Presence slots addressed by a definition with this many presence bytes: the first byte spends
   two bits on the extension and TLV flags, every later byte only one. */
static inline size_t iotdata_node_variant_slots(const uint8_t num_pres_bytes) {
    return (num_pres_bytes == 0) ? 0u
                                 : (size_t)IOTDATA_PRES0_DATA_FIELDS +
                                       (size_t)IOTDATA_PRESN_DATA_FIELDS * (size_t)(num_pres_bytes - 1u);
}

static inline size_t iotdata_node_variant_field_bytes(const size_t count) {
    return (count * IOTDATA_NODE_VARIANT_FIELD_BITS + 7u) / 8u;
}

/* The caller zeroes the region first: an even index leaves the low nibble of its last byte to the
   next id, so both writes preserve what they do not own. */
static inline void iotdata_node_variant_field_set(uint8_t *const buf, const size_t index, const uint16_t id) {
    const size_t off = index * IOTDATA_NODE_VARIANT_FIELD_BITS, b = off / 8u;
    if ((off % 8u) == 0u) { /* iiiiiiii iiii.... */
        buf[b] = (uint8_t)(id >> 4);
        buf[b + 1u] = (uint8_t)((buf[b + 1u] & 0x0Fu) | ((id & 0x0Fu) << 4));
    } else { /* ....iiii iiiiiiii */
        buf[b] = (uint8_t)((buf[b] & 0xF0u) | ((id >> 8) & 0x0Fu));
        buf[b + 1u] = (uint8_t)(id & 0xFFu);
    }
}

static inline uint16_t iotdata_node_variant_field_get(const uint8_t *const buf, const size_t index) {
    const size_t off = index * IOTDATA_NODE_VARIANT_FIELD_BITS, b = off / 8u;
    if ((off % 8u) == 0u)
        return (uint16_t)(((uint16_t)buf[b] << 4) | (buf[b + 1u] >> 4));
    return (uint16_t)(((uint16_t)(buf[b] & 0x0Fu) << 8) | buf[b + 1u]);
}

/* The registry seam: a field's wire id is its iotdata_field_type_t value. That enum is generated
   per build today, so the ids only mean anything within one; when its values become globally
   assigned the same identity holds and this function does not change -- the registry work is in
   the enum, not here. Anything the library does not consider a usable field id -- IOTDATA_FIELD_
   NONE, and the reserved TLV pseudo-field, which is a presence flag rather than a data slot --
   encodes as an empty slot. */
static inline uint16_t iotdata_node_variant_field_id(const iotdata_field_type_t type) {
    if (!IOTDATA_FIELD_VALID(type) || (int)type > (int)IOTDATA_NODE_VARIANT_FIELD_MAX)
        return IOTDATA_NODE_VARIANT_FIELD_NONE;
    return (uint16_t)type;
}

/* The variants this node defines, as a bitmask -- what goes in IOTDATA_NODE_VARIANT_MANIFEST.
   Variant 15 is never included: it is the mesh variant and carries no telemetry. */
static inline uint16_t iotdata_node_variant_manifest(void) {
    uint16_t manifest = 0;
    for (uint8_t v = 0; v <= IOTDATA_VARIANT_MAX; v++)
        if (iotdata_get_variant(v) != NULL)
            manifest |= (uint16_t)(1u << v);
    return manifest;
}

static inline bool iotdata_node_variant_manifest_has(const uint16_t manifest, const uint8_t variant) {
    return (variant <= IOTDATA_VARIANT_MAX) && ((manifest >> variant) & 1u) != 0u;
}

/* How many variants the manifest names -- the total a receiver is waiting to collect. */
static inline uint8_t iotdata_node_variant_manifest_count(const uint16_t manifest) {
    uint8_t n = 0;
    for (uint8_t v = 0; v <= IOTDATA_VARIANT_MAX; v++)
        if (((manifest >> v) & 1u) != 0u)
            n++;
    return n;
}

/* Encode one variant definition. Returns the value length, or 0 if it does not fit. */
static inline size_t iotdata_node_variant_encode(const iotdata_variant_def_t *const def, uint8_t *const buf,
                                                 const size_t size) {
    if (def == NULL || buf == NULL)
        return 0;
    const char *const name = (def->name != NULL) ? def->name : "";
    const size_t nlen = strlen(name);
    size_t slots = iotdata_node_variant_slots(def->num_pres_bytes);
    if (slots > IOTDATA_MAX_DATA_FIELDS)
        slots = IOTDATA_MAX_DATA_FIELDS;
    const size_t fbytes = iotdata_node_variant_field_bytes(slots), need = 1u + nlen + fbytes;
    if (nlen > 0xFFu || need > size)
        return 0;
    buf[0] = (uint8_t)nlen;
    memcpy(&buf[1], name, nlen);
    uint8_t *const f = &buf[1u + nlen];
    memset(f, 0, fbytes);
    for (size_t i = 0; i < slots; i++)
        iotdata_node_variant_field_set(f, i, iotdata_node_variant_field_id(def->fields[i].type));
    return need;
}

/* Decode one variant value. Returns the field count, and points name/fields into the value itself
   (the name is NOT NUL-terminated -- namelen bounds it). Returns 0 on a malformed value. */
static inline size_t iotdata_node_variant_decode(const uint8_t *const v, const size_t vlen, const char **const name,
                                                 size_t *const namelen, const uint8_t **const fields) {
    if (v == NULL || vlen < 1u)
        return 0;
    const size_t nlen = v[0];
    if (1u + nlen > vlen)
        return 0;
    if (name != NULL)
        *name = (const char *)&v[1];
    if (namelen != NULL)
        *namelen = nlen;
    if (fields != NULL)
        *fields = &v[1u + nlen];
    return ((vlen - 1u - nlen) * 8u) / IOTDATA_NODE_VARIANT_FIELD_BITS;
}

#endif /* IOTDATA_NODE_H */
