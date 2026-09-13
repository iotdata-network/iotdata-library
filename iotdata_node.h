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
 * DOWN: addressing a command to a node
 *
 * iotdata is one-way telemetry: a frame's station field says who SENT it, and the 32-bit header is
 * fully allocated, so there is no destination field to add one to. A frame whose sequence is
 * IOTDATA_SEQUENCE_DOWN (0xFFFF) inverts the meaning of that one field: the station is who
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

#define IOTDATA_NODE_TLV_RECEIVE                 0x00 /* when this node can be reached (see RECEIVE above)  */
#define IOTDATA_NODE_TLV_VERSION                 0x01 /* what this node IS: firmware, hardware, platform    */
#define IOTDATA_NODE_TLV_VARIANT                 0x02 /* which telemetry variant it is producing            */
#define IOTDATA_NODE_TLV_CONTROL                 0x03 /* commands TO a node, and its command inventory      */
#define IOTDATA_NODE_TLV_STATUS                  0x04 /* how it is DOING: uptime, restarts, supply, heap    */
#define IOTDATA_NODE_TLV_CONFIG                  0x05 /* settable operating parameters                      */
#define IOTDATA_NODE_TLV_DIAGNOSTICS             0x06 /* recorded diagnostic data (blackbox)                */
#define IOTDATA_NODE_TLV_CONTENT                 0x07 /* bulk payload: firmware image, user data            */
/* The mesh tables. These are types, not a special case: the numbering rule already put their
   request keys where a type's request belongs -- 8 << 3 is 0x40, 9 << 3 is 0x48, 10 << 3 is 0x50,
   which are exactly MESH_STATIONS/PEERS/FILTERS_REQUEST. So asking for a table is an ordinary
   request answered by an ordinary report, and the generic machinery needs no teaching. */
#define IOTDATA_NODE_TLV_MESH_STATIONS           0x08 /* who this node can hear                             */
#define IOTDATA_NODE_TLV_MESH_PEERS              0x09 /* who it could route through                          */
#define IOTDATA_NODE_TLV_MESH_FILTERS            0x0A /* who it is refusing, and why                         */
#define IOTDATA_NODE_TLV_SYSTEM_LAST             IOTDATA_NODE_TLV_MESH_FILTERS
/* How many system types exist, for anything indexing per-type state BY type -- a reporting cadence,
   a last-sent stamp. It belongs here because the answer changes when a type is added, and a node
   that sized its arrays from its own copy of the number would quietly index off the end of them.
   (Distinct from IOTDATA_TLV_TYPE_SYSTEM_MAX, which is how much room the type FIELD has: 0x1F.
   This is how much of that room is assigned.) */
#define IOTDATA_NODE_TLV_SYSTEM_COUNT            (IOTDATA_NODE_TLV_SYSTEM_LAST + 1)
#define IOTDATA_NODE_TLV_NONE                    0xFF

/* -------------------------------------------------------------------------
 * RECEIVE keys (0x00)
 *
 * Both optional -- an empty RECEIVE means "listening, for anything, for a period I am not telling
 * you". A sender holding a frame for this node transmits it on hearing this TLV.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_RECEIVE_DURATION            0x00 /* u16 milliseconds the receiver stays on            */
#define IOTDATA_NODE_RECEIVE_TYPES               0x01 /* u32 bitmask, bit N = "I accept TLV type N"        */

/* -------------------------------------------------------------------------
 * VERSION keys (0x01)
 *
 * FOUR BUCKETS AND AN INVENTORY. The split is the one every device-information standard arrives
 * at -- BLE's Device Information Service, LwM2M's Device object, Matter's Basic Information all
 * carry a hardware revision, the software beneath the application, the application itself, and an
 * identity -- because those four change independently. Reflashing the SDK does not change the
 * board; swapping the board does not change the SDK; neither touches the serial.
 *
 * The grammars are fixed, because these strings are parsed and compared, not just displayed:
 *
 *   hardware      board/arch            "esp32c3/riscv32"  "pizero/armv6l"  "pc/x86_64"
 *   firmware      stack/version[+low]   "idf/6.1+bl1.20"   "linux/6.12.96"
 *   software      app/semver/stamp      "gateway/1.0.0/202609231743"
 *   serial        hex, no separators    "DC326291ACF3"
 *   capabilities  16-bit entries        see iotdata_version.h
 *
 * THE STAMP IS THE ORDERING KEY. `stamp` is yyyymmddhhmm, fixed width, so a plain string compare
 * of two `software` values from the SAME app orders them by build time -- which is why there is no
 * separate numeric version field of the kind Matter and ESP-IDF carry for exactly that purpose.
 * The SEMVER is normative and says which release line a build tracks; it is NOT the ordering key
 * and must never be compared as one (`rc10` sorts below `rc9`, and a dev build off 1.4.3 is not
 * "newer" than 1.4.3). Compare stamps; display semvers.
 *
 * No library version. It would say nothing the stamp does not: the library compiles from the same
 * tree as the application, so one stamp identifies both. And the protocol does not need one --
 * unknown TLV types and unknown keys are skipped and counted rather than fatal, and evolution is
 * by reservation, so there is no version gate for a peer to fail.
 *
 * Everything is built by iotdata_version.h, which owns the per-platform detection and the
 * encoding. An application should not assemble these itself.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_VERSION_HARDWARE            0x00 /* "board/arch"                                   */
#define IOTDATA_NODE_VERSION_FIRMWARE            0x01 /* "stack[+low]": what runs beneath the app       */
#define IOTDATA_NODE_VERSION_SOFTWARE            0x02 /* "app/semver/stamp"                             */
#define IOTDATA_NODE_VERSION_SERIAL              0x03 /* stable per-device id, never a removable NIC    */
#define IOTDATA_NODE_VERSION_CAPABILITIES        0x04 /* 16-bit entries: [key:4][mask:12], big-endian   */

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

#define IOTDATA_NODE_VARIANT_FIELD_BITS          12
#define IOTDATA_NODE_VARIANT_FIELD_MAX           0x0FFEu
#define IOTDATA_NODE_VARIANT_FIELD_NONE          0x0FFFu /* a presence slot carrying nothing              */

/* The manifest: which variants this node defines, one bit per variant number. It sits above the
   variant numbers (0x00..0x0E) in the key space, and not at 0x0F, which would read as "variant 15"
   -- the mesh variant, deliberately absent here. */
#define IOTDATA_NODE_VARIANT_MANIFEST            0x10 /* u16 bitmask, bit N = "I define variant N"          */

/* -------------------------------------------------------------------------
 * CONTROL keys (0x03)
 *
 * EIGHT COMMANDS PER SUBJECT. A command's value is (subject << 3) + n, where `subject` is the TLV
 * type the command concerns and n is 0..7. So the request for a TLV is always (type << 3) -- e.g.
 * DIAGNOSTICS is TLV 0x06, so DIAGNOSTICS_REQUEST is 0x30 -- and the seven slots after it belong
 * to that same subject. Any command reachable from a key tells you what it acts on by arithmetic,
 * and adding a verb to a subject never disturbs another subject's numbering.
 *
 *   0x00..0x07  generic system control.
 *   0x08..0x0F  VERSION      (TLV 0x01)
 *   0x10..0x17  VARIANT      (TLV 0x02)
 *   0x18..0x1F  CONTROL      (TLV 0x03) -- the node's own command inventory
 *   0x20..0x27  STATUS       (TLV 0x04)
 *   0x28..0x2F  CONFIG       (TLV 0x05)
 *   0x30..0x37  DIAGNOSTICS  (TLV 0x06)
 *   0x38..0x3F  CONTENT      (TLV 0x07)
 *   0x40..0x7F  MESH management, eight per subject again (stations, peers, filter, ...)
 *   0x80..0xFF  PROPRIETARY (bit 7)
 *
 * A *_REQUEST command's value is an optional, command-specific argument; absent means "everything
 * you have". The response is a TLV of the corresponding type -- there is no response flag, because
 * the reply is self-describing. That is the difference from the MANAGE control frame this replaces,
 * where request and response shared one type and were told apart by a high bit.
 * ----------------------------------------------------------------------- */

/* --- 0x00..0x07  generic system control ------------------------------------------------------ */
#define IOTDATA_NODE_CONTROL_REBOOT                 0x00 /* value: optional u16 delay seconds       */

/*
 * PLACEHOLDERS -- reserved, not yet implemented. Numbered now so the space is not reused.
 *
 * Unlike everything else in this namespace these are DESTRUCTIVE and IRREVERSIBLE, so they carry
 * an extra obligation: the value must confirm WHICH node is meant -- its own identity, e.g. the
 * factory MAC. A node executes only if the token matches itself.
 *
 * That is not belt-and-braces. Every other command here is safe to broadcast: IOTDATA_STATION_
 * BROADCAST and MESH_TARGET_ALL exist precisely so one frame can reach the whole fleet, and a
 * stray REBOOT costs a cycle. A broadcast factory reset would cost the fleet its identity and
 * configuration, in the field, with no acknowledgement to tell you it had happened. Requiring the
 * target's own identity in the payload makes broadcast structurally harmless: no single token
 * matches every node.
 *
 * The token encoding is deliberately NOT fixed yet. The full 6-byte MAC is the safe choice --
 * device-unique and not otherwise on the wire. The 16-bit unit id used elsewhere is cheaper but
 * weak, and a station id is only 12 bits and rides in every frame, so an eavesdropper already
 * has it.
 */
#define IOTDATA_NODE_CONTROL_RESET_FACTORY          0x01 /* value: identity token; wipe everything   */
#define IOTDATA_NODE_CONTROL_RESET_DEFAULTS         0x02 /* value: identity token; config to default */
/* 0x03..0x07 free */

/* --- 0x08..0x3F  one subject per TLV type ---------------------------------------------------- */
#define IOTDATA_NODE_CONTROL_VERSION_REQUEST        0x08

#define IOTDATA_NODE_CONTROL_VARIANT_REQUEST        0x10

#define IOTDATA_NODE_CONTROL_CONTROL_REQUEST        0x18 /* "what commands do you support?"         */

#define IOTDATA_NODE_CONTROL_STATUS_REQUEST         0x20 /* value: optional STATUS_SCOPE_* bitmask  */

#define IOTDATA_NODE_CONTROL_CONFIG_REQUEST         0x28

#define IOTDATA_NODE_CONTROL_DIAGNOSTICS_REQUEST    0x30
#define IOTDATA_NODE_CONTROL_DIAGNOSTICS_ENABLE     0x31 /* value: u8 0=stop, 1=start recording     */
#define IOTDATA_NODE_CONTROL_DIAGNOSTICS_CLEAR      0x32 /* value: optional u32 stamp (idempotent)  */
#define IOTDATA_NODE_CONTROL_DIAGNOSTICS_DUMP       0x33 /* dump stored records to the node console */

#define IOTDATA_NODE_CONTROL_CONTENT_REQUEST        0x38

/* --- 0x40..0x7F  mesh management ------------------------------------------------------------- */
#define IOTDATA_NODE_CONTROL_MESH_BASE              0x40

/*
 * Each table has a REQUEST and a DUMP, and they are not the same operation.
 *
 * REQUEST is answered by the corresponding report TLV, which travels back and is published -- the
 * useful thing for a manager. DUMP writes the table to wherever that node's console goes, which is
 * the useful thing when you are watching a serial log and want it in line with everything else.
 * Keeping both means neither has to pretend to be the other.
 */
#define IOTDATA_NODE_CONTROL_MESH_STATIONS_REQUEST  0x40 /* -> MESH_STATIONS report                 */
#define IOTDATA_NODE_CONTROL_MESH_STATIONS_DUMP     0x41 /* -> the node's own console               */

#define IOTDATA_NODE_CONTROL_MESH_PEERS_REQUEST     0x48 /* -> MESH_PEERS report                    */
#define IOTDATA_NODE_CONTROL_MESH_PEERS_UPDATE      0x49 /* value: N x { u16 station, u8 PEER_* }    */
#define IOTDATA_NODE_CONTROL_MESH_PEERS_CLEAR       0x4A /* forget all, re-discover                 */
#define IOTDATA_NODE_CONTROL_MESH_PEERS_DUMP        0x4B /* -> the node's own console               */

/*
 * Peer vocabulary. Only NONE exists and probably only ever will -- a peer is discovered, not
 * configured, so there is nothing to insert. UPDATE still takes the batched
 * { station, action } shape rather than a bare station, purely so both mesh tables are driven the
 * same way: one verb, a list, and NONE meaning "no entry". The cost is one byte per station and
 * the saving is that a caller (or a CLI, or the monitor) has one pattern to learn instead of two.
 *
 * Forgetting a neighbour is not the same as blocking it: it may be rediscovered on the next
 * beacon. Use the filter table to keep it out.
 */
#define IOTDATA_NODE_CONTROL_MESH_PEER_NONE         0x00 /* forget it (may be rediscovered)         */

#define IOTDATA_NODE_CONTROL_MESH_FILTERS_REQUEST   0x50 /* -> MESH_FILTERS report                  */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_UPDATE    0x51 /* value: N x { u16 station, u8 FILTER_* }  */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_CLEAR     0x52 /* value: u8 FILTER_SCOPE_*                */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_DUMP      0x53 /* -> the node's own console               */

/*
 * Filter vocabulary, carried as MESH_FILTER_* command values (was the MANAGE filter set).
 *
 * FILTER_UPDATE replaced the separate INSERT and REMOVE commands. Its value is a sequence of
 * { u16 station (big-endian), u8 action } triples -- as many as fit -- so a whole set of changes
 * travels as one command, and FILTER_NONE is how an entry is removed: it says "this station has no
 * filter", which is the same statement as "forget it".
 *
 * That makes the command state-relative and therefore IDEMPOTENT: it describes what the table
 * should say about each station named, not a change to apply, so a retransmission is a no-op and
 * two orderings of the same batch land identically. On a mesh with no acknowledgement, that
 * property is the acknowledgement -- the same reasoning as DIAGNOSTICS_CLEAR taking a stamp.
 *
 * Stations not named are left alone. To empty the table use FILTER_CLEAR, which takes a scope
 * because "forget what I told you" and "forget what you learned" are different requests.
 *
 * PEERS_UPDATE has the same shape; see MESH_UPDATE_ENTRY_SIZE.
 */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_NONE       0x00 /* no filter: remove any entry for it      */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_BLOCK      0x01 /* blacklist: drop this station's frames   */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_ALLOW      0x02 /* whitelist: if any exist, only these     */

#define IOTDATA_NODE_CONTROL_MESH_FILTERS_SCOPE_ALL    0x00
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_SCOPE_MANUAL 0x01 /* only entries a command put there     */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_SCOPE_AUTO   0x02 /* only entries the node added itself    */

/* One entry of any mesh *_UPDATE value: u16 station (big-endian) + u8 action. Shared by
   FILTER_UPDATE and PEERS_UPDATE -- an encoder or decoder divides the value length by this. */
#define IOTDATA_NODE_CONTROL_MESH_UPDATE_ENTRY_SIZE 3

/* -------------------------------------------------------------------------
 * STATUS keys (0x04)
 *
 * Grouped by scope, so a reader can tell what a key is about and a requester can ask for one group
 * rather than everything:
 *
 *   0x00..0x1F  the node itself -- always present
 *   0x20..0x3F  the mesh -- present only on a node that runs one
 *   0x40..0x7F  free
 *   0x80..0xFF  PROPRIETARY (bit 7)
 *
 * STATUS_REQUEST's value is an optional bitmask of the groups wanted (absent = all of them), which
 * is what lets one command serve both "how are you" and the mesh reporting that used to need its
 * own MANAGE frame.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_STATUS_SCOPE_NODE           0x01 /* STATUS_REQUEST value bit: the node group        */
#define IOTDATA_NODE_STATUS_SCOPE_MESH           0x02 /* STATUS_REQUEST value bit: the mesh group        */

/* --- 0x00..0x1F  the node ------------------------------------------------------------------- */

#define IOTDATA_NODE_STATUS_UPTIME               0x00 /* u32 seconds, this session                       */
#define IOTDATA_NODE_STATUS_LIFETIME             0x01 /* u32 seconds, cumulative across boots            */
#define IOTDATA_NODE_STATUS_RESTARTS             0x02 /* u16 boots                                       */
#define IOTDATA_NODE_STATUS_REASON               0x03 /* u8, IOTDATA_NODE_REASON_*                       */
#define IOTDATA_NODE_STATUS_TEMPERATURE          0x04 /* i8 degrees C                                    */
#define IOTDATA_NODE_STATUS_SUPPLY               0x05 /* u16 millivolts                                  */
#define IOTDATA_NODE_STATUS_HEAP_FREE            0x06 /* u32 bytes free now                              */
#define IOTDATA_NODE_STATUS_HEAP_MIN             0x07 /* u32 bytes, lowest free seen                     */
#define IOTDATA_NODE_STATUS_ACTIVE               0x08 /* u32 seconds awake this session (vs asleep)      */

/* --- 0x20..0x3F  the mesh ------------------------------------------------------------------- */
/* Was the MANAGE STATUS response. As status keys these compose with the node's own, so one STATUS
   TLV can carry both and a plain sensor simply omits the group. Counters are since boot. */

#define IOTDATA_NODE_STATUS_MESH_STATE           0x20 /* u8, IOTDATA_NODE_STATUS_MESH_STATE_*            */
#define IOTDATA_NODE_STATUS_MESH_PARENT          0x21 /* u16 station, valid iff state == JOINED          */
#define IOTDATA_NODE_STATUS_MESH_COST            0x22 /* u8 hops to the gateway (parent cost + 1)        */
#define IOTDATA_NODE_STATUS_MESH_GENERATION      0x23 /* u16 beacon generation of the tree round we're in*/
#define IOTDATA_NODE_STATUS_MESH_PARENT_RSSI     0x24 /* i8 dBm, last heard from the parent              */
#define IOTDATA_NODE_STATUS_MESH_PEERS           0x25 /* u8 neighbours currently in the table            */
#define IOTDATA_NODE_STATUS_MESH_ACCEPTING       0x26 /* u8 bool, we advertise as a usable parent        */
#define IOTDATA_NODE_STATUS_MESH_BEACON_RX       0x27 /* u32 beacons heard                               */
#define IOTDATA_NODE_STATUS_MESH_BEACON_TX       0x28 /* u32 beacons sent                                */
#define IOTDATA_NODE_STATUS_MESH_PEER_NEW        0x29 /* u32 neighbours discovered                       */
#define IOTDATA_NODE_STATUS_MESH_REPARENT        0x2A /* u32 parent changes (better route found)         */
#define IOTDATA_NODE_STATUS_MESH_FAILOVER        0x2B /* u32 parent changes forced by loss               */
#define IOTDATA_NODE_STATUS_MESH_ORPHAN          0x2C /* u32 times left with no usable parent            */
#define IOTDATA_NODE_STATUS_MESH_RERR_RX         0x2D /* u32 route errors received                       */
#define IOTDATA_NODE_STATUS_MESH_RERR_TX         0x2E /* u32 route errors sent                           */
#define IOTDATA_NODE_STATUS_MESH_FORWARDS        0x2F /* u32 frames relayed on behalf of others          */
#define IOTDATA_NODE_STATUS_MESH_DUPLICATES      0x30 /* u32 frames dropped as already seen              */

/* Mesh join state (STATUS_MESH_STATE). */
#define IOTDATA_NODE_STATUS_MESH_STATE_DETACHED  0x00 /* no parent, not looking yet                      */
#define IOTDATA_NODE_STATUS_MESH_STATE_SEARCHING 0x01 /* listening for a beacon to join                  */
#define IOTDATA_NODE_STATUS_MESH_STATE_JOINED    0x02 /* have a parent and a cost                        */
#define IOTDATA_NODE_STATUS_MESH_STATE_ORPHANED  0x03 /* had a parent, lost it, searching again          */
#define IOTDATA_NODE_STATUS_MESH_STATE_GATEWAY   0x04 /* we ARE the root; cost 0                         */

/*
 * The mesh group's SHAPE, the scope predicate and the encoder are not here: they are how a report
 * is built, which belongs with the rest of the building, in iotdata-common's
 * iotdata_node_status.h. What stays here is the key space above, and the names of the values a
 * decoder meets (see iotdata_node_tlv_status_reason_str / _mesh_state_str below).
 */

/* -------------------------------------------------------------------------
 * The mesh table reports (types 0x08, 0x09, 0x0A)
 *
 * One key per table, repeated once per row, each carrying a fixed-width record. Repeated rather
 * than indexed because a kvr is a SEQUENCE, not a map: an index-as-key would make the key mean
 * "position", which is the one thing the reader does not need, and would break the rule that a key
 * names a field.
 *
 * A table rarely fits one frame -- 24 stations at 11 bytes plus kvr overhead is over 300 -- so a
 * report carries as many rows as fit and the rest follow in the next one, the same way
 * DIAGNOSTICS already chunks. COUNT is sent first and is the WHOLE table's size, so a reader can
 * tell a short frame from a short table.
 *
 * Records are big-endian and packed by hand rather than being a struct, because a struct's padding
 * is a property of the compiler that happens to be building the node, not of the wire.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_TABLE_COUNT                 0x00 /* u8: rows in the whole table, not this frame */
#define IOTDATA_NODE_TABLE_ROW                   0x01 /* one record, repeated -- see below           */

/* MESH_STATIONS row, 11 bytes: who this node can hear.
 *   [0:1] u16 station   [2] u8 kind (TABLE_KIND_*)   [3] u8 variant   [4] i8 rssi dBm
 *   [5:6] u16 age seconds since last heard           [7:10] u32 frames received            */
#define IOTDATA_NODE_TABLE_STATIONS_ROW_SIZE     11
#define IOTDATA_NODE_TABLE_KIND_UNKNOWN          0x00
#define IOTDATA_NODE_TABLE_KIND_GATEWAY          0x01
#define IOTDATA_NODE_TABLE_KIND_RELAY            0x02
#define IOTDATA_NODE_TABLE_KIND_SENSOR           0x03

/* MESH_PEERS row, 11 bytes: who this node could route through.
 *   [0:1] u16 station   [2:3] u16 gateway   [4] u8 cost   [5:6] u16 generation
 *   [7] i8 rssi dBm     [8:9] u16 age seconds            [10] u8 flags (TABLE_PEER_*)      */
#define IOTDATA_NODE_TABLE_PEERS_ROW_SIZE        11
#define IOTDATA_NODE_TABLE_PEER_ACCEPTING        0x01 /* advertises itself as a usable parent  */
#define IOTDATA_NODE_TABLE_PEER_PARENT           0x02 /* the one we are currently routing via  */

/* MESH_FILTERS row, 4 bytes: who this node is refusing, and why.
 *   [0:1] u16 station   [2] u8 action (MESH_FILTERS_BLOCK/ALLOW)   [3] u8 source (SCOPE_MANUAL/AUTO) */
#define IOTDATA_NODE_TABLE_FILTERS_ROW_SIZE      4

#define IOTDATA_NODE_TABLE_ROW_MAX               11 /* the widest row above: a builder's scratch size */

/* The row size for a table type, or 0 if that type is not a table. */
static inline uint8_t iotdata_node_table_row_size(const uint8_t type) {
    switch (type) {
    case IOTDATA_NODE_TLV_MESH_STATIONS:
        return IOTDATA_NODE_TABLE_STATIONS_ROW_SIZE;
    case IOTDATA_NODE_TLV_MESH_PEERS:
        return IOTDATA_NODE_TABLE_PEERS_ROW_SIZE;
    case IOTDATA_NODE_TLV_MESH_FILTERS:
        return IOTDATA_NODE_TABLE_FILTERS_ROW_SIZE;
    default:
        return 0;
    }
}

static inline bool iotdata_node_tlv_is_table(const uint8_t type) {
    return iotdata_node_table_row_size(type) != 0;
}

/* Big-endian field accessors, so a builder and a reader cannot disagree about the layout. */
static inline void iotdata_node_table_put_u16(uint8_t *const row, const size_t at, const uint16_t v) {
    row[at] = (uint8_t)(v >> 8);
    row[at + 1] = (uint8_t)v;
}
static inline uint16_t iotdata_node_table_get_u16(const uint8_t *const row, const size_t at) {
    return (uint16_t)(((uint16_t)row[at] << 8) | row[at + 1]);
}
static inline void iotdata_node_table_put_u32(uint8_t *const row, const size_t at, const uint32_t v) {
    row[at] = (uint8_t)(v >> 24);
    row[at + 1] = (uint8_t)(v >> 16);
    row[at + 2] = (uint8_t)(v >> 8);
    row[at + 3] = (uint8_t)v;
}
static inline uint32_t iotdata_node_table_get_u32(const uint8_t *const row, const size_t at) {
    return ((uint32_t)row[at] << 24) | ((uint32_t)row[at + 1] << 16) | ((uint32_t)row[at + 2] << 8) | (uint32_t)row[at + 3];
}

/* Reset reasons (STATUS_REASON). A node maps its platform's reason onto these. */
#define IOTDATA_NODE_REASON_UNKNOWN              0x00
#define IOTDATA_NODE_REASON_POWER_ON             0x01
#define IOTDATA_NODE_REASON_SOFTWARE             0x02
#define IOTDATA_NODE_REASON_WATCHDOG             0x03
#define IOTDATA_NODE_REASON_BROWNOUT             0x04
#define IOTDATA_NODE_REASON_PANIC                0x05
#define IOTDATA_NODE_REASON_DEEPSLEEP            0x06
#define IOTDATA_NODE_REASON_EXTERNAL             0x07
#define IOTDATA_NODE_REASON_OTA                  0x08

/* -------------------------------------------------------------------------
 * CONFIG keys (0x05)
 *
 * Reporting cadence, one key per reportable type: u16 seconds, 0 = do not send periodically.
 * STARTUP is a bitmask of IOTDATA_NODE_STARTUP_* saying which reports to emit once at boot.
 * Application/device settings are proprietary keys (bit 7 set) for now.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_CONFIG_PERIOD_VERSION       0x01 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_VARIANT       0x02 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_CONTROL       0x03 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_STATUS        0x04 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_CONFIG        0x05 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_PERIOD_DIAGNOSTICS   0x06 /* u16 seconds                              */
#define IOTDATA_NODE_CONFIG_STARTUP              0x10 /* u16 bitmask, IOTDATA_NODE_STARTUP_*      */

#define IOTDATA_NODE_STARTUP_VERSION             0x0002
#define IOTDATA_NODE_STARTUP_VARIANT             0x0004
#define IOTDATA_NODE_STARTUP_CONTROL             0x0008
#define IOTDATA_NODE_STARTUP_STATUS              0x0010
#define IOTDATA_NODE_STARTUP_CONFIG              0x0020
#define IOTDATA_NODE_STARTUP_DIAGNOSTICS         0x0040

/* -------------------------------------------------------------------------
 * DIAGNOSTICS keys (0x06)
 *
 * TYPE says what the accompanying DATA is (which record set, or which slice of it); DATA carries
 * the records themselves. A node emits as many of these as it chooses per packet -- one record or
 * several -- and splits across packets however suits it.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_DIAGNOSTICS_TYPE            0x00 /* u8, IOTDATA_NODE_DIAG_* -- what DATA holds        */
#define IOTDATA_NODE_DIAGNOSTICS_DATA            0x01 /* bytes: the records (blackbox CSV lines)           */

#define IOTDATA_NODE_DIAG_BLACKBOX               0x00 /* blackbox records, CSV as stored                      */

/* -------------------------------------------------------------------------
 * CONTENT keys (0x07)
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_CONTENT_FIRMWARE            0x00 /* bytes: OTA image data                             */
#define IOTDATA_NODE_CONTENT_USERDATA            0x01 /* bytes: free-form application data                 */

/* -------------------------------------------------------------------------
 * Naming and widths
 *
 * Values are binary, so a decoder needs these to render a system TLV meaningfully (JSON, a status
 * line, a console dump). A proprietary key has no name here and is shown as hex -- honest, rather
 * than guessed at.
 * ----------------------------------------------------------------------- */

/* Expected value width in bytes; 0 means variable (string or opaque bytes). */
#define IOTDATA_NODE_WIDTH_VARIABLE              0

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
    { IOTDATA_NODE_VERSION_HARDWARE, IOTDATA_NODE_WIDTH_VARIABLE, "hardware" }, { IOTDATA_NODE_VERSION_FIRMWARE, IOTDATA_NODE_WIDTH_VARIABLE, "firmware" },
    { IOTDATA_NODE_VERSION_SOFTWARE, IOTDATA_NODE_WIDTH_VARIABLE, "software" }, { IOTDATA_NODE_VERSION_SERIAL, IOTDATA_NODE_WIDTH_VARIABLE, "serial" },
    { IOTDATA_NODE_VERSION_CAPABILITIES, IOTDATA_NODE_WIDTH_VARIABLE, "capabilities" },
};

/* One key per variant number, so this table is the variant space itself rather than a set of named
   fields. 15 is absent: it is the mesh variant and carries no telemetry. */
static const iotdata_node_keydef_t iotdata_node_tlv_keys_variant[] = {
    { 0, IOTDATA_NODE_WIDTH_VARIABLE, "variant_0" },   { 1, IOTDATA_NODE_WIDTH_VARIABLE, "variant_1" },   { 2, IOTDATA_NODE_WIDTH_VARIABLE, "variant_2" },   { 3, IOTDATA_NODE_WIDTH_VARIABLE, "variant_3" },
    { 4, IOTDATA_NODE_WIDTH_VARIABLE, "variant_4" },   { 5, IOTDATA_NODE_WIDTH_VARIABLE, "variant_5" },   { 6, IOTDATA_NODE_WIDTH_VARIABLE, "variant_6" },   { 7, IOTDATA_NODE_WIDTH_VARIABLE, "variant_7" },
    { 8, IOTDATA_NODE_WIDTH_VARIABLE, "variant_8" },   { 9, IOTDATA_NODE_WIDTH_VARIABLE, "variant_9" },   { 10, IOTDATA_NODE_WIDTH_VARIABLE, "variant_10" }, { 11, IOTDATA_NODE_WIDTH_VARIABLE, "variant_11" },
    { 12, IOTDATA_NODE_WIDTH_VARIABLE, "variant_12" }, { 13, IOTDATA_NODE_WIDTH_VARIABLE, "variant_13" }, { 14, IOTDATA_NODE_WIDTH_VARIABLE, "variant_14" }, { IOTDATA_NODE_VARIANT_MANIFEST, 2, "manifest" },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_control[] = {
    /* generic system control */
    { IOTDATA_NODE_CONTROL_REBOOT, IOTDATA_NODE_WIDTH_VARIABLE, "reboot" },
    /* one subject per TLV type: the request, then that subject's verbs */
    { IOTDATA_NODE_CONTROL_VERSION_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "version_request" },
    { IOTDATA_NODE_CONTROL_VARIANT_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "variant_request" },
    { IOTDATA_NODE_CONTROL_CONTROL_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "control_request" },
    { IOTDATA_NODE_CONTROL_STATUS_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "status_request" },
    { IOTDATA_NODE_CONTROL_CONFIG_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "config_request" },
    { IOTDATA_NODE_CONTROL_DIAGNOSTICS_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "diagnostics_request" },
    { IOTDATA_NODE_CONTROL_DIAGNOSTICS_ENABLE, 1, "diagnostics_enable" },
    { IOTDATA_NODE_CONTROL_DIAGNOSTICS_CLEAR, IOTDATA_NODE_WIDTH_VARIABLE, "diagnostics_clear" },
    { IOTDATA_NODE_CONTROL_DIAGNOSTICS_DUMP, IOTDATA_NODE_WIDTH_VARIABLE, "diagnostics_dump" },
    { IOTDATA_NODE_CONTROL_CONTENT_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "content_request" },
    /* mesh management */
    { IOTDATA_NODE_CONTROL_MESH_STATIONS_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "mesh_stations_request" },
    { IOTDATA_NODE_CONTROL_MESH_STATIONS_DUMP, IOTDATA_NODE_WIDTH_VARIABLE, "mesh_stations_dump" },
    { IOTDATA_NODE_CONTROL_MESH_PEERS_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "mesh_peers_request" },
    { IOTDATA_NODE_CONTROL_MESH_PEERS_UPDATE, IOTDATA_NODE_WIDTH_VARIABLE, "mesh_peers_update" },
    { IOTDATA_NODE_CONTROL_MESH_PEERS_CLEAR, IOTDATA_NODE_WIDTH_VARIABLE, "mesh_peers_clear" },
    { IOTDATA_NODE_CONTROL_MESH_PEERS_DUMP, IOTDATA_NODE_WIDTH_VARIABLE, "mesh_peers_dump" },
    { IOTDATA_NODE_CONTROL_MESH_FILTERS_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "mesh_filters_request" },
    { IOTDATA_NODE_CONTROL_MESH_FILTERS_UPDATE, IOTDATA_NODE_WIDTH_VARIABLE, "mesh_filters_update" },
    { IOTDATA_NODE_CONTROL_MESH_FILTERS_CLEAR, 1, "mesh_filters_clear" },
    { IOTDATA_NODE_CONTROL_MESH_FILTERS_DUMP, IOTDATA_NODE_WIDTH_VARIABLE, "mesh_filters_dump" },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_status[] = {
    { IOTDATA_NODE_STATUS_UPTIME, 4, "uptime" },       { IOTDATA_NODE_STATUS_LIFETIME, 4, "lifetime" },       { IOTDATA_NODE_STATUS_RESTARTS, 2, "restarts" },
    { IOTDATA_NODE_STATUS_REASON, 1, "reason" },       { IOTDATA_NODE_STATUS_TEMPERATURE, 1, "temperature" }, { IOTDATA_NODE_STATUS_SUPPLY, 2, "supply" },
    { IOTDATA_NODE_STATUS_HEAP_FREE, 4, "heap_free" }, { IOTDATA_NODE_STATUS_HEAP_MIN, 4, "heap_min" },       { IOTDATA_NODE_STATUS_ACTIVE, 4, "active" },
    /* the mesh group (0x20..0x3F) -- absent on a node that does not run one */
    { IOTDATA_NODE_STATUS_MESH_STATE, 1, "mesh_state" },           { IOTDATA_NODE_STATUS_MESH_PARENT, 2, "mesh_parent" },
    { IOTDATA_NODE_STATUS_MESH_COST, 1, "mesh_cost" },             { IOTDATA_NODE_STATUS_MESH_GENERATION, 2, "mesh_generation" },
    { IOTDATA_NODE_STATUS_MESH_PARENT_RSSI, 1, "mesh_parent_rssi" }, { IOTDATA_NODE_STATUS_MESH_PEERS, 1, "mesh_peers" },
    { IOTDATA_NODE_STATUS_MESH_ACCEPTING, 1, "mesh_accepting" },   { IOTDATA_NODE_STATUS_MESH_BEACON_RX, 4, "mesh_beacon_rx" },
    { IOTDATA_NODE_STATUS_MESH_BEACON_TX, 4, "mesh_beacon_tx" },   { IOTDATA_NODE_STATUS_MESH_PEER_NEW, 4, "mesh_peer_new" },
    { IOTDATA_NODE_STATUS_MESH_REPARENT, 4, "mesh_reparent" },     { IOTDATA_NODE_STATUS_MESH_FAILOVER, 4, "mesh_failover" },
    { IOTDATA_NODE_STATUS_MESH_ORPHAN, 4, "mesh_orphan" },         { IOTDATA_NODE_STATUS_MESH_RERR_RX, 4, "mesh_rerr_rx" },
    { IOTDATA_NODE_STATUS_MESH_RERR_TX, 4, "mesh_rerr_tx" },       { IOTDATA_NODE_STATUS_MESH_FORWARDS, 4, "mesh_forwards" },
    { IOTDATA_NODE_STATUS_MESH_DUPLICATES, 4, "mesh_duplicates" },
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
    case IOTDATA_NODE_TLV_MESH_STATIONS:
        return "mesh_stations";
    case IOTDATA_NODE_TLV_MESH_PEERS:
        return "mesh_peers";
    case IOTDATA_NODE_TLV_MESH_FILTERS:
        return "mesh_filters";
    default:
        return NULL; /* proprietary or unassigned */
    }
}

/* The table reports share a key set: a count and a repeated row. Only the row WIDTH differs, so
   the width is declared per type by iotdata_node_table_row_size() rather than here -- the keydef
   width is VARIABLE so one table can serve all three. */
static const iotdata_node_keydef_t iotdata_node_tlv_keys_table_stations[] = {
    { IOTDATA_NODE_TABLE_COUNT, 1, "count" },
    { IOTDATA_NODE_TABLE_ROW, IOTDATA_NODE_TABLE_STATIONS_ROW_SIZE, "station" },
};
static const iotdata_node_keydef_t iotdata_node_tlv_keys_table_peers[] = {
    { IOTDATA_NODE_TABLE_COUNT, 1, "count" },
    { IOTDATA_NODE_TABLE_ROW, IOTDATA_NODE_TABLE_PEERS_ROW_SIZE, "peer" },
};
static const iotdata_node_keydef_t iotdata_node_tlv_keys_table_filters[] = {
    { IOTDATA_NODE_TABLE_COUNT, 1, "count" },
    { IOTDATA_NODE_TABLE_ROW, IOTDATA_NODE_TABLE_FILTERS_ROW_SIZE, "filter" },
};

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
    case IOTDATA_NODE_TLV_MESH_STATIONS:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_table_stations);
    case IOTDATA_NODE_TLV_MESH_PEERS:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_table_peers);
    case IOTDATA_NODE_TLV_MESH_FILTERS:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_table_filters);
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

static inline const char *iotdata_node_tlv_status_mesh_state_str(const uint8_t state) {
    switch (state) {
    case IOTDATA_NODE_STATUS_MESH_STATE_DETACHED:
        return "detached";
    case IOTDATA_NODE_STATUS_MESH_STATE_SEARCHING:
        return "searching";
    case IOTDATA_NODE_STATUS_MESH_STATE_JOINED:
        return "joined";
    case IOTDATA_NODE_STATUS_MESH_STATE_ORPHANED:
        return "orphaned";
    case IOTDATA_NODE_STATUS_MESH_STATE_GATEWAY:
        return "gateway";
    default:
        return "unknown";
    }
}

/* CONTROL request key <-> the TLV type it asks for, both returning IOTDATA_NODE_TLV_NONE when
   there is no counterpart. RECEIVE deliberately has neither: it is volunteered by a node when its
   own power budget allows, and asking for it would answer a question about a moment already past.

   Arithmetic, not a table: a command's value is (subject << 3) + n, so a *_REQUEST is exactly a
   TLV type shifted left three. These were a pair of switches that had to be edited in step with
   the defines, and drifting apart was only a matter of time. */

#define IOTDATA_NODE_CONTROL_SUBJECT_SHIFT 3
#define IOTDATA_NODE_CONTROL_SUBJECT_MASK  0x07 /* the n within a subject */

static inline uint8_t iotdata_node_tlv_control_type(const uint8_t key) {
    if ((key & IOTDATA_NODE_CONTROL_SUBJECT_MASK) != 0) /* a verb, not the plain request */
        return IOTDATA_NODE_TLV_NONE;
    const uint8_t type = (uint8_t)(key >> IOTDATA_NODE_CONTROL_SUBJECT_SHIFT);
    /* RECEIVE (0x00) is advertised, never requested; above the last assigned type is unassigned. */
    if (type == IOTDATA_NODE_TLV_RECEIVE || type > IOTDATA_NODE_TLV_SYSTEM_LAST)
        return IOTDATA_NODE_TLV_NONE;
    return type;
}

static inline uint8_t iotdata_node_tlv_control_key(const uint8_t type) {
    if (type == IOTDATA_NODE_TLV_RECEIVE || type > IOTDATA_NODE_TLV_SYSTEM_LAST)
        return IOTDATA_NODE_TLV_NONE;
    return (uint8_t)(type << IOTDATA_NODE_CONTROL_SUBJECT_SHIFT);
}

/* -------------------------------------------------------------------------
 * RECEIVE
 *
 * Reading the advertisement a node puts in its own outbound frame. Every key is optional, so an
 * empty payload is the complete message "listening, for anything, for a period I am not stating" --
 * which is why absence of a key means "no restriction" rather than "restricted to nothing".
 * ----------------------------------------------------------------------- */

typedef struct {
    uint16_t duration_ms; /* 0 when unstated: the node is taking responsibility for being awake */
    uint32_t types;       /* bitmask, bit N = TLV type N; meaningless unless has_types */
    bool has_duration, has_types;
} iotdata_node_receive_t;

static inline void iotdata_node_receive_parse(const uint8_t *const kv, const size_t kvlen, iotdata_node_receive_t *const out) {
    out->duration_ms = 0;
    out->types = 0;
    out->has_duration = false;
    out->has_types = false;
    size_t cur = 0;
    uint8_t key, vlen;
    const uint8_t *val;
    while (iotdata_kvr_next(kv, kvlen, &cur, &key, &val, &vlen)) {
        if (key == IOTDATA_NODE_RECEIVE_DURATION) {
            out->duration_ms = iotdata_kvr_u16(val, vlen, 0);
            out->has_duration = true;
        } else if (key == IOTDATA_NODE_RECEIVE_TYPES) {
            out->types = iotdata_kvr_u32(val, vlen, 0);
            out->has_types = true;
        }
    }
}

/* Build a RECEIVE payload. duration_ms 0 and types 0 are both "unstated", and an unstated pair
   leaves the payload empty -- which is the complete message "listening, for anything". */
static inline size_t iotdata_node_receive_build(uint8_t *const buf, const size_t size, const uint16_t duration_ms, const uint32_t types) {
    iotdata_kvr_t kv;
    iotdata_kvr_init(&kv, buf, size);
    if (duration_ms != 0)
        iotdata_kvr_add_u16(&kv, IOTDATA_NODE_RECEIVE_DURATION, duration_ms);
    if (types != 0)
        iotdata_kvr_add_u32(&kv, IOTDATA_NODE_RECEIVE_TYPES, types);
    return kv.overflow ? 0 : kv.len;
}

/* The two header predicates every node applies to an arriving frame. Kept here rather than in each
   app so "what counts as addressed to me" has one definition. */
static inline bool iotdata_node_is_down(const uint16_t sequence) {
    return sequence == IOTDATA_SEQUENCE_DOWN;
}

static inline bool iotdata_node_addressed_to(const uint16_t header_station, const uint16_t my_station) {
    return header_station == my_station || header_station == IOTDATA_STATION_BROADCAST;
}

/* Find a RECEIVE advertisement in an already-decoded frame. Decoding is the expensive part, so a
   caller with a hold table should check that it holds something for this station FIRST. */
static inline bool iotdata_node_receive_find(const iotdata_decoder_t *const dec, iotdata_node_receive_t *const out) {
    for (uint8_t i = 0; i < dec->tlv_count; i++)
        if (dec->tlv[i].type == IOTDATA_NODE_TLV_RECEIVE && dec->tlv[i].format == IOTDATA_TLV_FMT_RAW) {
            iotdata_node_receive_parse(dec->tlv[i].raw, dec->tlv[i].length, out);
            return true;
        }
    return false;
}

/* Would this node accept a frame carrying `type`? An unstated TYPES means no restriction. */
static inline bool iotdata_node_receive_accepts(const iotdata_node_receive_t *const r, const uint8_t type) {
    if (!r->has_types)
        return true;
    return type <= IOTDATA_TLV_TYPE_SYSTEM_MAX && ((r->types >> type) & 1u) != 0u;
}

/* Is a window of `duration_ms` long enough to be worth transmitting `bytes` into, at `bps` on air?
   An unstated duration means the node is taking responsibility, so we always try. */
static inline bool iotdata_node_receive_fits(const iotdata_node_receive_t *const r, const size_t bytes, const uint32_t bps) {
    if (!r->has_duration || r->duration_ms == 0 || bps == 0)
        return true;
    return ((bytes * 8u * 1000u) / bps) <= (size_t)r->duration_ms;
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
    return (num_pres_bytes == 0) ? 0u : (size_t)IOTDATA_PRES0_DATA_FIELDS + (size_t)IOTDATA_PRESN_DATA_FIELDS * (size_t)(num_pres_bytes - 1u);
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
static inline size_t iotdata_node_variant_encode(const iotdata_variant_def_t *const def, uint8_t *const buf, const size_t size) {
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
static inline size_t iotdata_node_variant_decode(const uint8_t *const v, const size_t vlen, const char **const name, size_t *const namelen, const uint8_t **const fields) {
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
