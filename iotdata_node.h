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

#define IOTDATA_NODE_TLV_RECEIVE               0x00 /* when this node can be reached (see RECEIVE above)  */
#define IOTDATA_NODE_TLV_VERSION               0x01 /* what this node IS: firmware, hardware, platform    */
#define IOTDATA_NODE_TLV_VARIANT               0x02 /* which telemetry variant it is producing            */
#define IOTDATA_NODE_TLV_STATUS                0x04 /* how it is DOING: uptime, restarts, supply, heap    */
#define IOTDATA_NODE_TLV_CONFIG                0x05 /* settable operating parameters                      */
#define IOTDATA_NODE_TLV_CONTROL               0x03 /* commands TO a node, and its command inventory      */
#define IOTDATA_NODE_TLV_DIAGNOSTICS           0x06 /* recorded diagnostic data (blackbox)                */
#define IOTDATA_NODE_TLV_CONTENT               0x07 /* bulk payload: firmware image, user data            */
#define IOTDATA_NODE_TLV_DISCRIMINATOR         0x08 /* opaque bytes; a hash-changer                       */
#define IOTDATA_NODE_TLV_SETTINGS              0x09 /* the PROTOCOL's own settings: read and write        */
#define IOTDATA_NODE_TLV_SYSTEM_LAST           IOTDATA_NODE_TLV_SETTINGS
#define IOTDATA_NODE_TLV_SYSTEM_COUNT          (IOTDATA_NODE_TLV_SYSTEM_LAST + 1)
#define IOTDATA_NODE_TLV_NONE                  0xFF /* sentinal, not encodeable */

/* -------------------------------------------------------------------------
 * RECEIVE keys (0x00)
 *
 * Both optional -- an empty RECEIVE means "listening, for anything, for a period I am not telling
 * you". A sender holding a frame for this node transmits it on hearing this TLV.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_RECEIVE_DURATION          0x00 /* u16 milliseconds the receiver stays on            */
#define IOTDATA_NODE_RECEIVE_TYPES             0x01 /* u32 bitmask, bit N = "I accept TLV type N"        */
#define IOTDATA_NODE_RECEIVE_OFFSET            0x02 /* u16 seconds until the window opens                */

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
 *   capabilities  16-bit entries        see iotdata_node_version.h
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
 * CAPABILITIES ARE STATIC PROPERTIES; CONTROL IS OPERATIVE ACTIONS. The two must not be derived
 * from one another. "I have mesh capability" is a property and belongs here; "stop meshing" is an
 * action and belongs in CONTROL. A node may have a capability it exposes no command for, and a
 * command that matches no declared capability.
 *
 * The entries are 16 bits, [key:4][mask:12], so rule (a) applies twice over: keys 0..7 are system
 * and 8..15 the implementer's, and within each key bits 0..5 are system and 6..11 the
 * implementer's. The keys and bits themselves live in iotdata_node_version.h; two are worth
 * knowing about from here, because other parts of the protocol turn on them:
 *
 *   FEATURES / MESH            this node runs a mesh, so its STATUS carries the 0x40..0x7F range
 *                              and its CONTROL accepts the MESH subject
 *   FEATURES / CONFIG_PERSIST  this node's APPLICATION configuration survives a power cycle.
 *                              Protocol configuration always does and needs no declaration; only
 *                              the device's own settings are in question, and a manager that sees
 *                              this absent knows to re-apply them after a restart (which it spots
 *                              from STATUS's `restarts` and `uptime`).
 *
 * Everything is built by iotdata_node_version.h, which owns the per-platform detection and the
 * encoding. An application should not assemble these itself.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_VERSION_HARDWARE          0x00 /* "board/arch"                                   */
#define IOTDATA_NODE_VERSION_FIRMWARE          0x01 /* "stack[+low]": what runs beneath the app       */
#define IOTDATA_NODE_VERSION_SOFTWARE          0x02 /* "app/semver/stamp"                             */
#define IOTDATA_NODE_VERSION_SERIAL            0x03 /* stable per-device id, never a removable NIC    */
#define IOTDATA_NODE_VERSION_CAPABILITIES      0x04 /* 16-bit entries: [key:4][mask:12], big-endian   */

/* -------------------------------------------------------------------------
 * VARIANT keys (0x02)
 *
 * A VARIANT ID IS LOCAL. A FIELD ID IS GLOBAL. That is the whole design and everything follows
 * from it. The variant number in a frame's header, and the presence map it selects, mean something
 * only on the node that SENT them: node A's variant 3 and node B's variant 3 are unrelated. What
 * is shared across the system is the identity of a FIELD -- "air temperature in centi-degrees" is
 * the same thing everywhere. This TLV translates one node's local numbering into global terms, so
 * a decoder that has never met this node can read its telemetry.
 *
 * TWO KEYS, EACH REPEATED ONCE PER VARIANT:
 *
 *   ENTRY  [ variant:u8 ][ field id:u16 ] *      what a decoder needs
 *   NAMES  [ variant:u8 ][ name NUL ] [ name NUL ] *   what a PERSON needs
 *
 * The key used to BE the variant number, which was neat and spent the whole key space on one
 * thing -- leaving nowhere to put anything else about a variant. Carrying the number in the value
 * costs one byte and buys the second key.
 *
 * Ids are POSITIONAL: index N is the field carried by presence slot N, and an unused slot in the
 * MIDDLE is IOTDATA_NODE_VARIANT_FIELD_NONE rather than skipped, because dropping it would shift
 * every slot after it. TRAILING empty slots ARE dropped -- nothing follows them to shift -- so a
 * slot beyond the value's length reads as NONE.
 *
 * NAMES are the variant's own name, then one per slot in the same order. They are DESCRIPTIVE and
 * advisory: a decoder works from the ids alone, and nothing may depend on a name being present,
 * spelled a particular way, or unique. A name that does not fit the value is dropped from the tail
 * the same way an empty slot is, so a reader takes an absent name as "not stated".
 *
 * NAMES ARE NOT SENT BY DEFAULT. An unqualified request answers with ENTRY alone, because the ids
 * are what make telemetry readable and the names are several times their size -- the same trade
 * STATUS makes when its default scope leaves out the tables. A caller that wants them asks:
 *
 *   request(VARIANT)                 -> ENTRY for every variant
 *   request(VARIANT, NAMES)          -> NAMES for every variant
 *   request(VARIANT, ENTRY, NAMES)   -> both
 *
 * A node with no telemetry variants at all, a gateway or a relay, sends the TLV EMPTY rather than
 * omitting it: "I produce none" is an answer. And a node reports what it PRODUCES, not what its
 * compiled-in tables let it DECODE -- a gateway holds the maps of every node it reads.
 *
 * Variant 15 never appears: it is the mesh variant and carries no telemetry.
 *
 * CHUNKING: a whole suite need not fit one packet, so a node emits as many as fit and continues in
 * the next. No fragmentation is needed, because each pair is self-contained: one either arrives
 * whole or arrives later, receiving one twice is harmless, and the receiver merges by variant id.
 * What tells a receiver it has them ALL is PARTIAL, which is why there is no manifest here.
 *
 * REGISTRY (unfinished): a field id is its iotdata_field_type_t value today. That enum is generated
 * per build from the variant suite selected for it, so an id currently means something only within
 * one build and must not be persisted or compared between nodes. Making it global is a change to
 * the enum's numbering, not to anything here -- the encoding, the width and the framing are already
 * what they will be.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_VARIANT_ENTRY             0x00 /* [variant:u8][field id:u16]*                        */
#define IOTDATA_NODE_VARIANT_NAMES             0x01 /* [variant:u8][variant name NUL][field name NUL]*    */

/* All ones, the sentinel convention: a slot carrying nothing, and what a reader gets for any slot
   past the end of a value. */
#define IOTDATA_NODE_VARIANT_FIELD_NONE        0xFFFFu
/* The upper half of the id space is the implementer's, by the same rule as every other space. */
#define IOTDATA_NODE_VARIANT_FIELD_PROPRIETARY 0x8000u
/* Not a variant: what a malformed value reads as. Variant numbers are 0..14. */
#define IOTDATA_NODE_VARIANT_NONE              0xFFu
#define IOTDATA_NODE_VARIANT_FIELD_NONE        0xFFFFu

/* -------------------------------------------------------------------------
 * CONTROL keys (0x03)
 *
 * TWO KEYS, AND EVERYTHING ELSE IS VOCABULARY.
 *
 *   REQUEST   value: [ subject ][ subject-specific selection ... ]
 *   CONTROL   value: [ subject ][ action ][ subject-specific arguments ... ]
 *
 * An empty selection is the unqualified request -- "everything you have for that subject". A
 * non-empty one is read by the subject, because each subject owns its inner protocol: STATUS reads
 * it as a scope byte, and a proprietary subject reads it however its vendor likes.
 *
 * This replaces a scheme where a command key was DERIVED from the type as (type << 3), reserving
 * eight keys per type. That had three problems and the third is the one that mattered:
 *
 *   - it spent half the system key space on eight types, mostly on slots nothing used
 *   - it collided: RECEIVE is type 0x00, so its derived request key was 0x00, which was REBOOT
 *   - it could not address the PROPRIETARY half at all. Type 0x2A derives 0x150, off the end of a
 *     byte, so a vendor's own types were unrequestable -- which quietly contradicted the rule that
 *     the upper half of every space belongs to the implementer.
 *
 * It also gave a QUALIFIED request nowhere to live: VERSION_REQUEST took no value, so "tell me
 * just your serial" was unsayable. Now it is [ VERSION ][ SERIAL ].
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_CONTROL_REQUEST           0x00 /* [subject][selection...]                 */
#define IOTDATA_NODE_CONTROL_CONTROL           0x01 /* [subject][action][args...]              */

/* -------------------------------------------------------------------------
 * SUBJECTS
 *
 * A subject is a superset of a TLV type. Most subjects ARE types, but setting a station id is
 * about no type at all, and neither is blocking a station -- so the byte addresses a wider space:
 *
 *   0x00..0x3F  the TLV types, verbatim, keeping their own system/proprietary split
 *   0x40..0x7F  system subjects that are NOT types
 *   0x80..0xFF  proprietary subjects
 *
 * That gives the mesh an honest home. It was never a TLV type -- its state is reported inside
 * STATUS's upper key range -- so its MUTATION had nowhere to sit except bolted onto a report type.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_SUBJECT_NODE              0x40 /* the node itself: reboot, reset, identity */
#define IOTDATA_NODE_SUBJECT_MESH              0x41 /* the mesh: peers, filters                 */
#define IOTDATA_NODE_SUBJECT_PROPRIETARY       0x80

static inline bool iotdata_node_subject_is_type(const uint8_t subject) {
    return subject <= IOTDATA_TLV_TYPE_MAX;
}

static inline bool iotdata_node_tlv_is_reportable(const uint8_t type) {
    switch (type) {
    case IOTDATA_NODE_TLV_RECEIVE:
    case IOTDATA_NODE_TLV_VERSION:
    case IOTDATA_NODE_TLV_VARIANT:
    case IOTDATA_NODE_TLV_CONTROL:
    case IOTDATA_NODE_TLV_STATUS:
    case IOTDATA_NODE_TLV_CONFIG:
    case IOTDATA_NODE_TLV_DIAGNOSTICS:
    case IOTDATA_NODE_TLV_CONTENT:
    case IOTDATA_NODE_TLV_SETTINGS:
        return true;
    default:
        return false;
    }
}

/* -------------------------------------------------------------------------
 * ACTIONS
 *
 * An action is scoped by its SUBJECT, exactly as a key is scoped by its type and an event id by
 * the report it belongs to. DIAGNOSTICS' `enable` and MESH's `peers-clear` cannot collide because
 * they share no namespace.
 *
 *   0x00..0x7F  the subject's own
 *   0x80..0xFF  proprietary
 *
 * EVERY ACTION IS A VERB. There is no action that assigns a value: writing one is a write, and a
 * write belongs to a mutable TYPE -- SETTINGS for the protocol's own values, CONFIG for the
 * device's. This range once had a COMMON band at 0x00..0x0F for set-trigger and get-trigger, which
 * were writes wearing a command's clothes; they are SETTINGS keys now, and the band went with them.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_ACTION_PROPRIETARY                0x80

/* --- NODE actions ---------------------------------------------------------------------------- */
#define IOTDATA_NODE_ACTION_NODE_REBOOT                0x00 /* args: optional u16 delay seconds         */
#define IOTDATA_NODE_ACTION_NODE_RESET                 0x01 /* args: identity token, then RESET_*       */

#define IOTDATA_NODE_RESET_FACTORY                     0x00 /* wipe everything                          */
#define IOTDATA_NODE_RESET_DEFAULTS                    0x01 /* configuration back to default            */

/* --- DIAGNOSTICS actions --------------------------------------------------------------------- */
#define IOTDATA_NODE_ACTION_DIAGNOSTICS_ENABLE         0x00 /* args: u8 0=stop 1=start, [u8 category]   */
#define IOTDATA_NODE_ACTION_DIAGNOSTICS_CLEAR          0x01 /* args: optional u32 stamp (idempotent)    */
#define IOTDATA_NODE_ACTION_DIAGNOSTICS_DUMP           0x02 /* to the node's own console                */

/* --- STATUS actions -------------------------------------------------------------------------- */
#define IOTDATA_NODE_ACTION_STATUS_STATIONS_DUMP       0x00
#define IOTDATA_NODE_ACTION_STATUS_FILTERS_DUMP        0x01
#define IOTDATA_NODE_ACTION_STATUS_FILTERS_UPDATE      0x02 /* args: N x { u16 station, u8 FILTERS_* }  */
#define IOTDATA_NODE_ACTION_STATUS_FILTERS_CLEAR       0x03 /* args: u8 FILTERS_SCOPE_*                 */

/* --- MESH actions ---------------------------------------------------------------------------- */
#define IOTDATA_NODE_ACTION_MESH_PEERS_UPDATE          0x00 /* args: N x { u16 station, u8 PEER_* }     */
#define IOTDATA_NODE_ACTION_MESH_PEERS_CLEAR           0x01 /* forget all, re-discover                  */
#define IOTDATA_NODE_ACTION_MESH_PEERS_DUMP            0x02 /* to the node's own console                */

/*
 * Peer vocabulary. Only NONE exists and probably only ever will -- a peer is discovered, not
 * configured, so there is nothing to insert. UPDATE still takes the batched { station, action }
 * shape rather than a bare station, purely so both tables are driven the same way.
 *
 * Forgetting a neighbour is not the same as blocking it: it may be rediscovered on the next
 * beacon. Use the filter table to keep it out.
 */
#define IOTDATA_NODE_CONTROL_MESH_PEER_NONE            0x00 /* forget it (may be rediscovered)         */

/*
 * Filter vocabulary. A filter is stated per station, and the table is a blacklist until something
 * is allowed, at which point it is a whitelist. Stations not named are left alone. To empty the
 * table use FILTERS_CLEAR, which takes a scope because "forget what I told you" and "forget what
 * you learned" are different requests.
 *
 * FILTERS is a STATUS subject rather than a MESH one, because a filter is not a mesh concept: any
 * node that hears traffic can refuse a station, and a gateway with no mesh has a filter table.
 */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_NONE         0x00 /* no filter: remove any entry for it      */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_BLOCK        0x01 /* blacklist: drop this station's frames   */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_ALLOW        0x02 /* whitelist: if any exist, only these     */

#define IOTDATA_NODE_CONTROL_MESH_FILTERS_SCOPE_ALL    0x00
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_SCOPE_MANUAL 0x01 /* only entries a command put there     */
#define IOTDATA_NODE_CONTROL_MESH_FILTERS_SCOPE_AUTO   0x02 /* only entries the node added itself    */

/* One entry of any *_UPDATE argument: u16 station (big-endian) + u8 action. Shared by
   FILTERS_UPDATE and PEERS_UPDATE -- an encoder or decoder divides the argument length by this. */
#define IOTDATA_NODE_CONTROL_MESH_UPDATE_ENTRY_SIZE    3

/* -------------------------------------------------------------------------
 * STATUS keys (0x04)
 *
 * TWO RANGES, AND NOTHING FINER:
 *
 *   0x00..0x3F  ANY NODE  -- packed linearly from 0x00, extended as needed
 *   0x40..0x7F  THE MESH  -- packed linearly from 0x40, and only on a node that runs one
 *   0x80..0xFF  PROPRIETARY (bit 7)
 *
 * There is deliberately NO finer structure -- no scalar sub-range, no table sub-range. Keys are
 * allocated in the order they are invented and a reader is told what each one is by its keydef,
 * not by arithmetic on its number. A count key happening to sit next to its entry key is
 * incidental; nothing derives one from the other, and the mapping is the explicit table below.
 *
 * STATIONS and FILTERS are in the ANY NODE range because neither is a mesh concept: any node that
 * hears traffic has stations it can hear, and any node can refuse one. A gateway with no mesh has
 * both. Only PEERS -- who a node could route THROUGH -- is meaningless without a mesh.
 *
 * STATUS_REQUEST's value is an optional bitmask of the groups wanted, which is what lets one
 * command serve both "how are you" and the reporting that used to need its own types.
 *
 * ABSENT MEANS THE SCALARS, not everything. A table is unbounded and spans frames, so a bare
 * request must not be able to cost twenty frames of airtime by accident: a caller that wants a
 * table says so.
 *
 * COUNTS ARE SCALARS, and a report emits every scalar BEFORE any entry. A truncated or lost tail
 * then still leaves a reader knowing how big each table was, which is the more useful half. A
 * caller asking only for a table's entries gets no count and that is its own choice to make.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_STATUS_SCOPE_NODE                 0x01 /* the node's scalars, counts included             */
#define IOTDATA_NODE_STATUS_SCOPE_MESH                 0x02 /* the mesh's scalars, counts included             */
#define IOTDATA_NODE_STATUS_SCOPE_STATIONS             0x04 /* the stations table's entries                    */
#define IOTDATA_NODE_STATUS_SCOPE_FILTERS              0x08 /* the filters table's entries                     */
#define IOTDATA_NODE_STATUS_SCOPE_MESH_PEERS           0x10 /* the peers table's entries                       */
#define IOTDATA_NODE_STATUS_SCOPE_CONTENT              0x20 /* the content table's entries                     */
#define IOTDATA_NODE_STATUS_SCOPE_DEFAULT              (IOTDATA_NODE_STATUS_SCOPE_NODE | IOTDATA_NODE_STATUS_SCOPE_MESH)

/* --- 0x00..0x3F  any node -------------------------------------------------------------------- */

#define IOTDATA_NODE_STATUS_UPTIME                     0x00 /* u32 seconds, this session                       */
#define IOTDATA_NODE_STATUS_LIFETIME                   0x01 /* u32 seconds, cumulative across boots            */
#define IOTDATA_NODE_STATUS_RESTARTS                   0x02 /* u16 boots                                       */
#define IOTDATA_NODE_STATUS_REASON                     0x03 /* u8, IOTDATA_NODE_REASON_*                       */
#define IOTDATA_NODE_STATUS_TEMPERATURE                0x04 /* i8 degrees C                                    */
#define IOTDATA_NODE_STATUS_SUPPLY                     0x05 /* u16 millivolts                                  */
#define IOTDATA_NODE_STATUS_HEAP_FREE                  0x06 /* u32 bytes free now                              */
#define IOTDATA_NODE_STATUS_HEAP_MIN                   0x07 /* u32 bytes, lowest free seen                     */
#define IOTDATA_NODE_STATUS_ACTIVE                     0x08 /* u32 seconds awake this session (vs asleep)      */

#define IOTDATA_NODE_STATUS_STATIONS_COUNT             0x09 /* u8 rows in the whole table                      */
#define IOTDATA_NODE_STATUS_STATIONS_ENTRY             0x0A /* one row, repeated -- see below                  */
#define IOTDATA_NODE_STATUS_FILTERS_COUNT              0x0B /* u8 rows in the whole table                      */
#define IOTDATA_NODE_STATUS_FILTERS_ENTRY              0x0C /* one row, repeated -- see below                  */
#define IOTDATA_NODE_STATUS_CONTENT_COUNT              0x0D /* u8 rows in the whole table                      */
#define IOTDATA_NODE_STATUS_CONTENT_ENTRY              0x0E /* one row, repeated -- see below                  */

/* --- 0x40..0x7F  the mesh ---------------------------------------------------------------------
   Was the MANAGE STATUS response. As status keys these compose with the node's own, so one STATUS
   TLV can carry both and a plain sensor simply omits the range. Counters are since boot. */

#define IOTDATA_NODE_STATUS_MESH_STATE                 0x40 /* u8, IOTDATA_NODE_STATUS_MESH_STATE_*            */
#define IOTDATA_NODE_STATUS_MESH_PARENT                0x41 /* u16 station, valid iff state == JOINED          */
#define IOTDATA_NODE_STATUS_MESH_COST                  0x42 /* u8 hops to the gateway (parent cost + 1)        */
#define IOTDATA_NODE_STATUS_MESH_GENERATION            0x43 /* u16 beacon generation of the tree round we're in*/
#define IOTDATA_NODE_STATUS_MESH_PARENT_RSSI           0x44 /* i8 dBm, last heard from the parent              */
#define IOTDATA_NODE_STATUS_MESH_ACCEPTING             0x45 /* u8 bool, we advertise as a usable parent        */
#define IOTDATA_NODE_STATUS_MESH_BEACON_RX             0x46 /* u32 beacons heard                               */
#define IOTDATA_NODE_STATUS_MESH_BEACON_TX             0x47 /* u32 beacons sent                                */
#define IOTDATA_NODE_STATUS_MESH_PEER_NEW              0x48 /* u32 neighbours discovered                       */
#define IOTDATA_NODE_STATUS_MESH_REPARENT              0x49 /* u32 parent changes (better route found)         */
#define IOTDATA_NODE_STATUS_MESH_FAILOVER              0x4A /* u32 parent changes forced by loss               */
#define IOTDATA_NODE_STATUS_MESH_ORPHAN                0x4B /* u32 times left with no usable parent            */
#define IOTDATA_NODE_STATUS_MESH_RERR_RX               0x4C /* u32 route errors received                       */
#define IOTDATA_NODE_STATUS_MESH_RERR_TX               0x4D /* u32 route errors sent                           */
#define IOTDATA_NODE_STATUS_MESH_FORWARDS              0x4E /* u32 frames relayed on behalf of others          */
#define IOTDATA_NODE_STATUS_MESH_DUPLICATES            0x4F /* u32 frames dropped as already seen              */
#define IOTDATA_NODE_STATUS_MESH_PEERS_COUNT           0x50 /* u8 rows in the whole table                      */
#define IOTDATA_NODE_STATUS_MESH_PEERS_ENTRY           0x51 /* one row, repeated -- see below                  */

/* Mesh join state (STATUS_MESH_STATE). */
#define IOTDATA_NODE_STATUS_MESH_STATE_DETACHED        0x00 /* no parent, not looking yet                      */
#define IOTDATA_NODE_STATUS_MESH_STATE_SEARCHING       0x01 /* listening for a beacon to join                  */
#define IOTDATA_NODE_STATUS_MESH_STATE_JOINED          0x02 /* have a parent and a cost                        */
#define IOTDATA_NODE_STATUS_MESH_STATE_ORPHANED        0x03 /* had a parent, lost it, searching again          */
#define IOTDATA_NODE_STATUS_MESH_STATE_GATEWAY         0x04 /* we ARE the root; cost 0                         */

/*
 * The mesh group's SHAPE, the scope predicate and the encoder are not here: they are how a report
 * is built, which belongs with the rest of the building, in iotdata-common's
 * iotdata_node_status.h. What stays here is the key space above, and the names of the values a
 * decoder meets (see iotdata_node_tlv_status_reason_str / _mesh_state_str below).
 */

/* -------------------------------------------------------------------------
 * STATUS table entries (the 0x11, 0x13 and 0x41 keys)
 *
 * One key per table, repeated once per row, each carrying a fixed-width record. Repeated rather
 * than indexed because a kvr is a SEQUENCE, not a map: an index-as-key would make the key mean
 * "position", which is the one thing the reader does not need, and would break the rule that a key
 * names a field.
 *
 * A table rarely fits one frame -- 24 stations at 11 bytes plus kvr overhead is over 300 -- so a
 * report carries as many rows as fit and the rest follow in the next one, the same way
 * DIAGNOSTICS already chunks. The matching COUNT key is the WHOLE table's size, so a reader can
 * tell a short frame from a short table.
 *
 * There is no longer any such thing as "a table TLV". A row's width is declared by its KEY, like
 * every other key's width, so the per-type row_size/is_table machinery that used to exist has
 * gone: a table is simply a repeated STATUS key that happens to carry a record.
 *
 * Records are big-endian and packed by hand rather than being a struct, because a struct's padding
 * is a property of the compiler that happens to be building the node, not of the wire.
 * ----------------------------------------------------------------------- */

/* STATIONS entry, 11 bytes: who this node can hear.
 *   [0:1] u16 station   [2] u8 kind (STATIONS_KIND_*)   [3] u8 variant   [4] i8 rssi dBm
 *   [5:6] u16 age seconds since last heard              [7:10] u32 frames received         */
#define IOTDATA_NODE_STATUS_STATIONS_ENTRY_SIZE        11
#define IOTDATA_NODE_STATUS_STATIONS_KIND_UNKNOWN      0x00
#define IOTDATA_NODE_STATUS_STATIONS_KIND_GATEWAY      0x01
#define IOTDATA_NODE_STATUS_STATIONS_KIND_RELAY        0x02
#define IOTDATA_NODE_STATUS_STATIONS_KIND_SENSOR       0x03

/* FILTERS entry, 4 bytes: who this node is refusing, and why.
 *   [0:1] u16 station   [2] u8 action (MESH_FILTERS_BLOCK/ALLOW)   [3] u8 source (SCOPE_MANUAL/AUTO) */
#define IOTDATA_NODE_STATUS_FILTERS_ENTRY_SIZE         4

/* MESH_PEERS entry, 11 bytes: who this node could route through.
 *   [0:1] u16 station   [2:3] u16 gateway   [4] u8 cost   [5:6] u16 generation
 *   [7] i8 rssi dBm     [8:9] u16 age seconds            [10] u8 flags (MESH_PEERS_FLAG_*) */
#define IOTDATA_NODE_STATUS_MESH_PEERS_ENTRY_SIZE      11
#define IOTDATA_NODE_STATUS_MESH_PEERS_FLAG_ACCEPTING  0x01 /* advertises itself as a usable parent  */
#define IOTDATA_NODE_STATUS_MESH_PEERS_FLAG_PARENT     0x02 /* the one we are currently routing via  */

/* CONTENT entry, 13 bytes: what this node holds, and what it is in the middle of.
 *   [0:2] u24 content-id   [3] u8 state (CONTENT_STATE_*)   [4] u8 flags (CONTENT_FLAG_*)
 *   [5:6] u16 chunks held  [7:8] u16 chunks total (0 = no manifest seen)
 *   [9:10] u16 minutes until expiry (0xFFFF indefinite)
 *   [11:12] u16 seconds since last progress, saturating -- what makes a STALL visible, since
 *           FETCHING says a node is trying and only the elapsed time says it is getting nowhere */
#define IOTDATA_NODE_STATUS_CONTENT_ENTRY_SIZE         13
#define IOTDATA_NODE_STATUS_CONTENT_STATE_HOLDING      0x00 /* complete and verified; can serve it   */
#define IOTDATA_NODE_STATUS_CONTENT_STATE_FETCHING     0x01 /* incomplete, actively requesting       */
#define IOTDATA_NODE_STATUS_CONTENT_STATE_CACHING      0x02 /* incomplete, accumulated passively     */
#define IOTDATA_NODE_STATUS_CONTENT_STATE_FAILED       0x03 /* verification failed, or abandoned     */
/* Per CONTENT, not per node: a node may stage one object in RAM and another in its OTA partition.
   Without it, a half-finished transfer that will restart from nothing on the next power cycle is
   indistinguishable from one that will eventually complete. */
#define IOTDATA_NODE_STATUS_CONTENT_FLAG_PERSISTENT    0x01

#define IOTDATA_NODE_STATUS_ENTRY_MAX                  13 /* the widest entry above: a builder's scratch */

/*
 * What a table IS, stated rather than derived.
 *
 * Nothing computes one of these fields from another -- a count key is not "the entry key minus
 * one", it is simply the key named here. The numbering happens to be adjacent today and that is
 * incidental; a table added later can take whatever numbers are free without anything breaking.
 * This one table is what the per-TYPE row_size/is_table machinery used to be.
 */
typedef struct {
    uint8_t count; /* the scalar key carrying the WHOLE table's size */
    uint8_t entry; /* the key carrying one row, repeated             */
    uint8_t width; /* bytes in one row                               */
    uint8_t scope; /* the STATUS_REQUEST bit that asks for the rows  */
} iotdata_node_status_table_t;

#define IOTDATA_NODE_STATUS_TABLE_COUNT 4

/* In report order, which is also the order the entries are emitted in. */
static const iotdata_node_status_table_t iotdata_node_status_tables[IOTDATA_NODE_STATUS_TABLE_COUNT] = {
    { IOTDATA_NODE_STATUS_STATIONS_COUNT, IOTDATA_NODE_STATUS_STATIONS_ENTRY, IOTDATA_NODE_STATUS_STATIONS_ENTRY_SIZE, IOTDATA_NODE_STATUS_SCOPE_STATIONS },
    { IOTDATA_NODE_STATUS_FILTERS_COUNT, IOTDATA_NODE_STATUS_FILTERS_ENTRY, IOTDATA_NODE_STATUS_FILTERS_ENTRY_SIZE, IOTDATA_NODE_STATUS_SCOPE_FILTERS },
    { IOTDATA_NODE_STATUS_MESH_PEERS_COUNT, IOTDATA_NODE_STATUS_MESH_PEERS_ENTRY, IOTDATA_NODE_STATUS_MESH_PEERS_ENTRY_SIZE, IOTDATA_NODE_STATUS_SCOPE_MESH_PEERS },
    { IOTDATA_NODE_STATUS_CONTENT_COUNT, IOTDATA_NODE_STATUS_CONTENT_ENTRY, IOTDATA_NODE_STATUS_CONTENT_ENTRY_SIZE, IOTDATA_NODE_STATUS_SCOPE_CONTENT },
};

/* The table an ENTRY key belongs to, or NULL if that key carries no rows. */
static inline const iotdata_node_status_table_t *iotdata_node_status_table_of(const uint8_t entry_key) {
    for (size_t i = 0; i < IOTDATA_NODE_STATUS_TABLE_COUNT; i++)
        if (iotdata_node_status_tables[i].entry == entry_key)
            return &iotdata_node_status_tables[i];
    return NULL;
}

/* The width of a table's entry, or 0 if that key is not a table entry at all. */
static inline uint8_t iotdata_node_status_entry_size(const uint8_t entry_key) {
    const iotdata_node_status_table_t *const t = iotdata_node_status_table_of(entry_key);
    return (t != NULL) ? t->width : 0u;
}

/* Big-endian field accessors, so a builder and a reader cannot disagree about the layout. */
static inline void iotdata_node_entry_put_u16(uint8_t *const row, const size_t at, const uint16_t v) {
    row[at] = (uint8_t)(v >> 8);
    row[at + 1] = (uint8_t)v;
}
static inline uint16_t iotdata_node_entry_get_u16(const uint8_t *const row, const size_t at) {
    return (uint16_t)(((uint16_t)row[at] << 8) | row[at + 1]);
}
static inline void iotdata_node_entry_put_u32(uint8_t *const row, const size_t at, const uint32_t v) {
    row[at] = (uint8_t)(v >> 24);
    row[at + 1] = (uint8_t)(v >> 16);
    row[at + 2] = (uint8_t)(v >> 8);
    row[at + 3] = (uint8_t)v;
}
static inline uint32_t iotdata_node_entry_get_u32(const uint8_t *const row, const size_t at) {
    return ((uint32_t)row[at] << 24) | ((uint32_t)row[at + 1] << 16) | ((uint32_t)row[at + 2] << 8) | (uint32_t)row[at + 3];
}

/* Reset reasons (STATUS_REASON). A node maps its platform's reason onto these. */
#define IOTDATA_NODE_REASON_UNKNOWN        0x00
#define IOTDATA_NODE_REASON_POWER_ON       0x01
#define IOTDATA_NODE_REASON_SOFTWARE       0x02
#define IOTDATA_NODE_REASON_WATCHDOG       0x03
#define IOTDATA_NODE_REASON_BROWNOUT       0x04
#define IOTDATA_NODE_REASON_PANIC          0x05
#define IOTDATA_NODE_REASON_DEEPSLEEP      0x06
#define IOTDATA_NODE_REASON_EXTERNAL       0x07
#define IOTDATA_NODE_REASON_OTA            0x08

/* -------------------------------------------------------------------------
 * CONFIG keys (0x05) -- the system half is RESERVED and EMPTY
 *
 * PROTOCOL CONFIGURATION IS A CONTROL ACTION; APPLICATION CONFIGURATION IS CONFIG. The line is not
 * "node protocol versus the rest" -- a station id is BASE protocol and still lands on the control
 * side. What unites the control side is that it configures the PROTOCOL'S OWN behaviour; what
 * unites this type is that it configures the thing the device is FOR: a sample interval, a
 * threshold, a calibration coefficient.
 *
 * So everything that used to be here has gone. PERIOD_<type> and STARTUP were reporting cadence,
 * which is protocol behaviour, and they are now the trigger record carried by
 * control(<subject>, set-trigger) -- one mechanism that serves a PROPRIETARY type as readily as a
 * system one, which a per-type CONFIG key never could.
 *
 * The system half (0x00..0x7F) is kept RESERVED rather than abolished. Nothing occupies it today
 * and a device puts its own settings in the proprietary half like any other proprietary key -- but
 * if an application setting ever proves universal enough to standardise, its number must not
 * already be in use across a fleet, or standardising it becomes a breaking change rather than an
 * addition. 128 keys is more than any device here plausibly needs.
 *
 * A node with nothing application-shaped to configure answers a CONFIG request with an EMPTY kvr,
 * which is an answer and not a refusal.
 * ----------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * SETTINGS keys (0x09) -- the PROTOCOL's own values, read and written
 *
 * THREE TYPES CARRY THREE DIFFERENT THINGS, and keeping them apart is what makes each simple:
 *
 *   SETTINGS  values that configure the PROTOCOL       station id, reporting, receive schedule
 *   CONFIG    values that configure the DEVICE         sample interval, thresholds, calibration
 *   CONTROL   verbs                                    reboot, reset, dump, update a table
 *
 * A write is not a verb. `set-station` and `set-trigger` were CONTROL actions that assigned a
 * value, which left them with no defined answer -- a rejected or clamped write looked exactly like
 * a successful one. As keys of a MUTABLE type they inherit the rule that already governs CONFIG: a
 * qualified request assigns and then returns WHAT IS NOW TRUE, so a manager compares what came back
 * with what it sent and needs no per-value error channel to see what happened.
 *
 * Reading matters as much as writing. Under the action form, seeing a node's whole schedule meant
 * one round trip per subject; here it is one request.
 *
 * SETTINGS MUST PERSIST across a power cycle, whether or not the device persists any application
 * configuration of its own -- a node whose station id cannot be changed and retained cannot be
 * commissioned in the field. The store is the node state block (iotdata_node_state.h), NOT the
 * config system, and a write FLUSHES rather than deferring: write-behind would make the response a
 * promise rather than a fact, and a power cut in between would leave the node disagreeing with what
 * it just told a manager.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_SETTINGS_STATION      0x00 /* u16. MUST persist: this is commissioning.        */
#define IOTDATA_NODE_SETTINGS_REPORT       0x01 /* repeated, one per subject -- see below           */
#define IOTDATA_NODE_SETTINGS_RECEIVE      0x02 /* [window:u16][interval:u16][offset:u16]           */

/*
 * REPORT entry, 9 bytes: WHEN this node emits one subject, and nothing about what it emits.
 *
 *   [0]     u8   subject -- a TLV type, or a proprietary one
 *   [1:2]   u16  flags: which triggers are armed
 *   [3:4]   u16  period seconds -- the on-period cadence
 *   [5:6]   u16  change seconds -- the MINIMUM interval between on-change emissions, a flap guard
 *   [7:8]   u16  events -- a bitmask whose meaning the SUBJECT owns
 *
 * The subject is inside the VALUE and the key repeats, rather than there being a key per subject.
 * That is what lets a PROPRIETARY type be scheduled at all: a per-subject key would have to be
 * allocated by the vendor and would be unknown to the library, so one implementation could not
 * serve both halves of the space. It is the same shape STATUS's table entries and VARIANT's
 * definitions take, for the same reason.
 *
 * FLAGS is u16 rather than u8 deliberately. Under the system/proprietary rule a u8 yields exactly
 * four system bits, and the four triggers defined here would have filled it on the day it shipped.
 */
#define IOTDATA_NODE_SETTINGS_REPORT_SIZE  9
#define IOTDATA_NODE_SETTINGS_RECEIVE_SIZE 6

#define IOTDATA_NODE_REPORT_AT_STARTUP     0x0001u /* once, at boot                                    */
#define IOTDATA_NODE_REPORT_ON_PERIOD      0x0002u /* on a cadence; `period` says how often            */
#define IOTDATA_NODE_REPORT_ON_CHANGE      0x0004u /* a value that was X is now Y; `change` rate-limits */
#define IOTDATA_NODE_REPORT_ON_EVENT       0x0008u /* something happened; `events` says which          */
#define IOTDATA_NODE_REPORT_PROPRIETARY    0x0100u /* the upper half of the flags is the implementer's */

/* A node MUST NOT count a monotonically-varying value -- an uptime, a lifetime, a counter -- as a
   change for the purposes of ON_CHANGE. The subscription is per-subject and cannot say "watch this
   key", so the discrimination is an obligation on the emitter: wire ON_CHANGE to a continuously
   varying report and it becomes a flood rather than a notification. */

static inline void iotdata_node_settings_put_u16(uint8_t *const b, const size_t at, const uint16_t v) {
    b[at] = (uint8_t)(v >> 8);
    b[at + 1u] = (uint8_t)v;
}
static inline uint16_t iotdata_node_settings_get_u16(const uint8_t *const b, const size_t at) {
    return (uint16_t)(((uint16_t)b[at] << 8) | b[at + 1u]);
}

/* -------------------------------------------------------------------------
 * DIAGNOSTICS keys (0x06)
 *
 * TYPE says what the accompanying DATA is (which record set, or which slice of it); DATA carries
 * the records themselves. A node emits as many of these as it chooses per packet -- one record or
 * several -- and splits across packets however suits it.
 * ----------------------------------------------------------------------- */

#define IOTDATA_NODE_DIAGNOSTICS_TYPE 0x00 /* u8, IOTDATA_NODE_DIAG_* -- what DATA holds        */
#define IOTDATA_NODE_DIAGNOSTICS_DATA 0x01 /* bytes: the records (blackbox CSV lines)           */

#define IOTDATA_NODE_DIAG_BLACKBOX    0x00 /* blackbox records, CSV as stored                      */

/* -------------------------------------------------------------------------
 * CONTENT keys (0x07)
 * ----------------------------------------------------------------------- */

/* CONTENT is NOT a kvr and has no keys. It is packed positional behind a 32-bit header carrying a
   sub-kind, a tag and a content-id -- see iotdata-specs/NOTES_CONTENT.md §5. The FIRMWARE/USERDATA
   keys that used to live here described a design that the sub-kinds replaced outright. */

/* -------------------------------------------------------------------------
 * Naming and widths
 *
 * Values are binary, so a decoder needs these to render a system TLV meaningfully (JSON, a status
 * line, a console dump). A proprietary key has no name here and is shown as hex -- honest, rather
 * than guessed at.
 * ----------------------------------------------------------------------- */

/* Expected value width in bytes; 0 means variable (string or opaque bytes). */
#define IOTDATA_NODE_WIDTH_VARIABLE   0

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
    { IOTDATA_NODE_VERSION_HARDWARE, IOTDATA_NODE_WIDTH_VARIABLE, "hardware" }, { IOTDATA_NODE_VERSION_FIRMWARE, IOTDATA_NODE_WIDTH_VARIABLE, "firmware" },         { IOTDATA_NODE_VERSION_SOFTWARE, IOTDATA_NODE_WIDTH_VARIABLE, "software" },
    { IOTDATA_NODE_VERSION_SERIAL, IOTDATA_NODE_WIDTH_VARIABLE, "serial" },     { IOTDATA_NODE_VERSION_CAPABILITIES, IOTDATA_NODE_WIDTH_VARIABLE, "capabilities" },
};

/* One key per variant number, so this table is the variant space itself rather than a set of named
   fields. 15 is absent: it is the mesh variant and carries no telemetry. */
static const iotdata_node_keydef_t iotdata_node_tlv_keys_variant[] = {
    { IOTDATA_NODE_VARIANT_ENTRY, IOTDATA_NODE_WIDTH_VARIABLE, "variant" },
    { IOTDATA_NODE_VARIANT_NAMES, IOTDATA_NODE_WIDTH_VARIABLE, "names" },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_control[] = {
    { IOTDATA_NODE_CONTROL_REQUEST, IOTDATA_NODE_WIDTH_VARIABLE, "request" },
    { IOTDATA_NODE_CONTROL_CONTROL, IOTDATA_NODE_WIDTH_VARIABLE, "control" },
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
    { IOTDATA_NODE_STATUS_STATIONS_COUNT, 1, "stations_count" },
    { IOTDATA_NODE_STATUS_STATIONS_ENTRY, IOTDATA_NODE_STATUS_STATIONS_ENTRY_SIZE, "station" },
    { IOTDATA_NODE_STATUS_FILTERS_COUNT, 1, "filters_count" },
    { IOTDATA_NODE_STATUS_FILTERS_ENTRY, IOTDATA_NODE_STATUS_FILTERS_ENTRY_SIZE, "filter" },
    { IOTDATA_NODE_STATUS_CONTENT_COUNT, 1, "content_count" },
    { IOTDATA_NODE_STATUS_CONTENT_ENTRY, IOTDATA_NODE_STATUS_CONTENT_ENTRY_SIZE, "content" },
    /* the mesh (0x40..0x7F) -- absent on a node that does not run one */
    { IOTDATA_NODE_STATUS_MESH_STATE, 1, "mesh_state" },
    { IOTDATA_NODE_STATUS_MESH_PARENT, 2, "mesh_parent" },
    { IOTDATA_NODE_STATUS_MESH_COST, 1, "mesh_cost" },
    { IOTDATA_NODE_STATUS_MESH_GENERATION, 2, "mesh_generation" },
    { IOTDATA_NODE_STATUS_MESH_PARENT_RSSI, 1, "mesh_parent_rssi" },
    { IOTDATA_NODE_STATUS_MESH_ACCEPTING, 1, "mesh_accepting" },
    { IOTDATA_NODE_STATUS_MESH_BEACON_RX, 4, "mesh_beacon_rx" },
    { IOTDATA_NODE_STATUS_MESH_BEACON_TX, 4, "mesh_beacon_tx" },
    { IOTDATA_NODE_STATUS_MESH_PEER_NEW, 4, "mesh_peer_new" },
    { IOTDATA_NODE_STATUS_MESH_REPARENT, 4, "mesh_reparent" },
    { IOTDATA_NODE_STATUS_MESH_FAILOVER, 4, "mesh_failover" },
    { IOTDATA_NODE_STATUS_MESH_ORPHAN, 4, "mesh_orphan" },
    { IOTDATA_NODE_STATUS_MESH_RERR_RX, 4, "mesh_rerr_rx" },
    { IOTDATA_NODE_STATUS_MESH_RERR_TX, 4, "mesh_rerr_tx" },
    { IOTDATA_NODE_STATUS_MESH_FORWARDS, 4, "mesh_forwards" },
    { IOTDATA_NODE_STATUS_MESH_DUPLICATES, 4, "mesh_duplicates" },
    { IOTDATA_NODE_STATUS_MESH_PEERS_COUNT, 1, "mesh_peers_count" },
    { IOTDATA_NODE_STATUS_MESH_PEERS_ENTRY, IOTDATA_NODE_STATUS_MESH_PEERS_ENTRY_SIZE, "mesh_peer" },
};

/* Empty: the system half is reserved and unassigned, and a device's own settings are proprietary
   keys, which a keydef does not name. See the CONFIG key space above. */
static const iotdata_node_keydef_t iotdata_node_tlv_keys_config[] = {
    { 0, 0, NULL },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_settings[] = {
    { IOTDATA_NODE_SETTINGS_STATION, 2, "station" },
    { IOTDATA_NODE_SETTINGS_REPORT, IOTDATA_NODE_SETTINGS_REPORT_SIZE, "report" },
    { IOTDATA_NODE_SETTINGS_RECEIVE, IOTDATA_NODE_SETTINGS_RECEIVE_SIZE, "receive" },
};

static const iotdata_node_keydef_t iotdata_node_tlv_keys_diagnostics[] = {
    { IOTDATA_NODE_DIAGNOSTICS_TYPE, 1, "type" },
    { IOTDATA_NODE_DIAGNOSTICS_DATA, IOTDATA_NODE_WIDTH_VARIABLE, "data" },
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
    case IOTDATA_NODE_TLV_DISCRIMINATOR:
        return "discriminator";
    case IOTDATA_NODE_TLV_SETTINGS:
        return "settings";
    case IOTDATA_TLV_TYPE_PARTIAL:
        return "partial";
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
    case IOTDATA_NODE_TLV_SETTINGS:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_settings);
    case IOTDATA_NODE_TLV_DIAGNOSTICS:
        IOTDATA_NODE_KEYS_RET(iotdata_node_tlv_keys_diagnostics);
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

/* -------------------------------------------------------------------------
 * RECEIVE
 *
 * Reading the advertisement a node puts in its own outbound frame. Every key is optional, so an
 * empty payload is the complete message "listening, for anything, for a period I am not stating" --
 * which is why absence of a key means "no restriction" rather than "restricted to nothing".
 * ----------------------------------------------------------------------- */

typedef struct {
    uint16_t duration_ms; /* 0 when unstated: assume IOTDATA_NODE_RECEIVE_WINDOW_MS_DEFAULT */
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

/* A station that sleeps STATES its window; absence means "assume the default" rather than "I am
   taking responsibility", because a holder has to decide whether a frame fits before sending it and
   cannot do that against an unknown. There is no "always on" station in this model: a permanently
   listening node advertises a very large window and is handled by exactly the same rules. */
#ifndef IOTDATA_NODE_RECEIVE_WINDOW_MS_DEFAULT
#define IOTDATA_NODE_RECEIVE_WINDOW_MS_DEFAULT 5000u
#endif

/* Safety room inside a window, beyond the frame's own airtime. FIXED rather than proportional: the
   uncertainty being covered is the delay between deciding to send and the frame leaving -- queue
   depth and transmit jitter -- and that does not scale with the frame. */
#ifndef IOTDATA_NODE_RECEIVE_MARGIN_MS
#define IOTDATA_NODE_RECEIVE_MARGIN_MS 250u
#endif

static inline uint32_t iotdata_node_receive_window_ms(const iotdata_node_receive_t *const r) {
    return (r->has_duration && r->duration_ms > 0) ? (uint32_t)r->duration_ms : (uint32_t)IOTDATA_NODE_RECEIVE_WINDOW_MS_DEFAULT;
}

/* Does `bytes` fit in `window_ms` at `bps`, with the margin? This is the form a holder wants, since
   what it has is the window REMAINING rather than the window advertised. */
static inline bool iotdata_node_receive_fits_ms(const size_t bytes, const uint32_t bps, const uint32_t window_ms) {
    if (bps == 0)
        return true;
    return (((uint32_t)((bytes * 8u * 1000u) / bps)) + (uint32_t)IOTDATA_NODE_RECEIVE_MARGIN_MS) <= window_ms;
}

/* Is the window this station advertised long enough to be worth transmitting `bytes` into? */
static inline bool iotdata_node_receive_fits(const iotdata_node_receive_t *const r, const size_t bytes, const uint32_t bps) {
    return iotdata_node_receive_fits_ms(bytes, bps, iotdata_node_receive_window_ms(r));
}

/* -------------------------------------------------------------------------
 * VARIANT codec
 *
 * Packs one variant into the value of an ENTRY or a NAMES pair, and reads either back. ENTRY is a
 * variant number then a big-endian u16 per presence slot, so there is no bit cursor and nothing
 * straddles a byte: it used to pack 12-bit ids across byte boundaries in a two-phase pattern,
 * which the move to a 16-bit global id space made unnecessary.
 * ----------------------------------------------------------------------- */

/* Presence slots addressed by a definition with this many presence bytes: the first byte spends
   two bits on the extension and TLV flags, every later byte only one. */
static inline size_t iotdata_node_variant_slots(const uint8_t num_pres_bytes) {
    return (num_pres_bytes == 0) ? 0u : (size_t)IOTDATA_PRES0_DATA_FIELDS + (size_t)IOTDATA_PRESN_DATA_FIELDS * (size_t)(num_pres_bytes - 1u);
}

/* The registry seam: a field's wire id is its iotdata_field_type_t value. That enum is generated
   per build today, so the ids only mean anything within one; when its values become globally
   assigned the same identity holds and this function does not change -- the registry work is in
   the enum, not here. Anything the library does not consider a usable field id -- IOTDATA_FIELD_
   NONE, and the reserved TLV pseudo-field, which is a presence flag rather than a data slot --
   reads as an empty slot, and so does anything that would land in the implementer's half. */
static inline uint16_t iotdata_node_variant_field_id(const iotdata_field_type_t type) {
    if (!IOTDATA_FIELD_VALID(type) || (unsigned)type >= (unsigned)IOTDATA_NODE_VARIANT_FIELD_PROPRIETARY)
        return IOTDATA_NODE_VARIANT_FIELD_NONE;
    return (uint16_t)type;
}

/* How many slots a definition actually states, with trailing empties trimmed: a definition with
   two fields in a three-byte presence map would otherwise spend 50 bytes saying "nothing" 25
   times. A slot past the end reads as NONE, so the trimmed and untrimmed forms decode the same. */
static inline size_t iotdata_node_variant_stated_slots(const iotdata_variant_def_t *const def) {
    if (def == NULL)
        return 0;
    size_t slots = iotdata_node_variant_slots(def->num_pres_bytes);
    if (slots > IOTDATA_MAX_DATA_FIELDS)
        slots = IOTDATA_MAX_DATA_FIELDS;
    while (slots > 0 && iotdata_node_variant_field_id(def->fields[slots - 1u].type) == IOTDATA_NODE_VARIANT_FIELD_NONE)
        slots--;
    return slots;
}

/* Encode one ENTRY value. Returns the length, or 0 if it does not fit. */
static inline size_t iotdata_node_variant_encode(const iotdata_variant_def_t *const def, const uint8_t variant, uint8_t *const buf, const size_t size) {
    if (def == NULL || buf == NULL || size < 1u)
        return 0;
    const size_t slots = iotdata_node_variant_stated_slots(def), need = 1u + slots * 2u;
    if (need > size || need > 0xFFu)
        return 0;
    buf[0] = variant;
    for (size_t i = 0; i < slots; i++) {
        const uint16_t id = iotdata_node_variant_field_id(def->fields[i].type);
        buf[1u + i * 2u] = (uint8_t)(id >> 8);
        buf[2u + i * 2u] = (uint8_t)id;
    }
    return need;
}

/* Encode one NAMES value: the variant's name, then one per slot in slot order, each NUL-terminated.
   Names that do not fit are dropped from the TAIL rather than failing the value -- they are
   descriptive, and a reader takes an absent one as "not stated". Returns the length, or 0 if not
   even the variant's own name fits. */
static inline size_t iotdata_node_variant_encode_names(const iotdata_variant_def_t *const def, const uint8_t variant, uint8_t *const buf, const size_t size) {
    if (def == NULL || buf == NULL || size < 2u)
        return 0;
    const size_t cap = (size < 0xFFu) ? size : 0xFFu;
    const char *const vname = (def->name != NULL) ? def->name : "";
    size_t n = 1u, l = strlen(vname);
    if (1u + l + 1u > cap)
        return 0;
    buf[0] = variant;
    memcpy(&buf[n], vname, l);
    n += l;
    buf[n++] = '\0';
    const size_t slots = iotdata_node_variant_stated_slots(def);
    for (size_t i = 0; i < slots; i++) {
        const char *const f = (def->fields[i].label != NULL) ? def->fields[i].label : "";
        l = strlen(f);
        if (n + l + 1u > cap)
            break; /* the tail is dropped, not the value */
        memcpy(&buf[n], f, l);
        n += l;
        buf[n++] = '\0';
    }
    return n;
}

/* The variant a value describes, for either key. NONE if the value is malformed. */
static inline uint8_t iotdata_node_variant_of(const uint8_t *const v, const size_t vlen) {
    if (v == NULL || vlen < 1u || v[0] > IOTDATA_VARIANT_MAX)
        return IOTDATA_NODE_VARIANT_NONE;
    return v[0];
}

/* How many slots an ENTRY value states. An odd remainder cannot be an array of u16 and reads as 0. */
static inline size_t iotdata_node_variant_decode(const uint8_t *const v, const size_t vlen) {
    if (iotdata_node_variant_of(v, vlen) == IOTDATA_NODE_VARIANT_NONE || ((vlen - 1u) % 2u) != 0u)
        return 0;
    return (vlen - 1u) / 2u;
}

/* The field in one slot of an ENTRY value. Past the end is NONE, which is what makes trimming safe. */
static inline uint16_t iotdata_node_variant_field_at(const uint8_t *const v, const size_t vlen, const size_t slot) {
    if (slot >= iotdata_node_variant_decode(v, vlen))
        return IOTDATA_NODE_VARIANT_FIELD_NONE;
    return (uint16_t)(((uint16_t)v[1u + slot * 2u] << 8) | v[2u + slot * 2u]);
}

/* A name from a NAMES value: index 0 is the variant's own, 1..N the field in slot 0..N-1. Returns
   NULL when that name was not stated, which includes every index past a dropped tail. The result
   points into the value and is NUL-terminated there. */
static inline const char *iotdata_node_variant_name_at(const uint8_t *const v, const size_t vlen, const size_t index) {
    if (iotdata_node_variant_of(v, vlen) == IOTDATA_NODE_VARIANT_NONE)
        return NULL;
    size_t at = 1u;
    for (size_t i = 0;; i++) {
        if (at >= vlen)
            return NULL;
        const size_t start = at;
        while (at < vlen && v[at] != 0u)
            at++;
        if (at >= vlen)
            return NULL; /* unterminated: malformed rather than read past */
        at++;
        if (i == index)
            return (const char *)&v[start];
    }
}

#endif /* IOTDATA_NODE_H */
