

/*
 * GENERATED FILE -- DO NOT EDIT.
 *
 * Built by generators/iotdata-generator-fields.js from the field specifiers in
 * specifiers/iotdata-fields-*.xml, through two templates:
 * generators/templates/fields.c.h.tmpl for the declarations below, and
 * generators/templates/fields.c.c.tmpl for the definitions in the second half.
 * `make generate` rewrites it and `make generate-check` proves it reproduces.
 */

#if !defined(IOTDATA_FIELDS_H_DECLARATIONS)
#define IOTDATA_FIELDS_H_DECLARATIONS

/* ---------------------------------------------------------------------------
 * Field identifiers
 *
 * EVERY FIELD CARRIES TWO NUMBERS, AND THEY ARE NOT THE SAME NUMBER.
 *
 *   IOTDATA_FIELD_<NAME>     the iotdata_field_type_t enum member    BUILD-LOCAL
 *   IOTDATA_FIELD_ID_<NAME>  the identifier, defined in its block    STABLE
 *
 * The enum is dense and moves with IOTDATA_ENABLE_*, because it doubles as a bit position in the
 * uint32_t presence mask and as an index into a flat ops table. That makes it a good handle
 * inside one program and meaningless between two: enable one more field and every ordinal after
 * it shifts. The identifier does not move, so it is the one that can be written down, kept, or
 * handed to a peer that was built differently.
 *
 * NOTHING IN THE BASE PROTOCOL USES THEM, and that is the point of having them separate. A
 * point-to-point, mesh or relay implementation encodes and decodes entirely from its own
 * compiled-in fields and never needs a number anyone else agrees with. The identifiers exist for
 * the layer above: the node VARIANT TLV in iotdata_node.h puts them on the wire so a peer can
 * learn what this node's presence slots carry, which is what turns a purely local map into
 * something a stranger can read. See that header's VARIANT notes.
 *
 * NUMBERING. Identifiers are opaque -- compared, never calculated with. They are grouped only so
 * a person reading a capture can see the shape: sixteen to a family, and by convention x0..x7 are
 * a family's atomic fields while x8..xF are the composites over them, so ENVIRONMENT at 0x0018 is
 * temperature, pressure and humidity in one slot. The gaps let a new sibling sit beside its
 * relatives instead of on the end. 0x0000 is left unassigned so that a zeroed buffer is not a
 * valid field, and the upper half of the space is the implementer's -- iotdata_node.h states the
 * boundary, because it owns the wire.
 *
 * TLV HAS NO IDENTIFIER. It is reserved, it never occupies a data presence slot, and it is the
 * escape hatch from typed telemetry rather than a kind of it.
 * -------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
 * Field BATTERY, BATTERY_LEVEL, BATTERY_CHARGE
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_BATTERY_LEVEL) || defined(IOTDATA_ENABLE_BATTERY)
#define IOTDATA_FIELDS_DAT_BATTERY_LEVEL uint8_t battery_level;
typedef struct {
    IOTDATA_FIELDS_DAT_BATTERY_LEVEL
} iotdata_field_battery_level_t;
#define IOTDATA_BATTERY_LEVEL_MAX  100
#define IOTDATA_BATTERY_LEVEL_BITS 5
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_battery_level(struct iotdata_encoder_t_ *const enc, uint8_t level_percent);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_battery_level_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t level_percent);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_battery_level(struct iotdata_decoder_t_ *const dec, uint8_t *level_percent);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_BATTERY_LEVEL(S)  S(IOTDATA_ERR_BATTERY_LEVEL_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_BATTERY_LEVEL(D) D(IOTDATA_ERR_BATTERY_LEVEL_HIGH, "Battery level above 100%")
#else
#define IOTDATA_FIELDS_DAT_BATTERY_LEVEL
#define IOTDATA_FIELDS_ERR_IDENTS_BATTERY_LEVEL(S)
#define IOTDATA_FIELDS_ERR_STRINGS_BATTERY_LEVEL(D)
#endif
#if defined(IOTDATA_ENABLE_BATTERY_LEVEL)
#define IOTDATA_FIELDS_MEM_BATTERY_LEVEL    iotdata_field_battery_level_t battery_level;
#define IOTDATA_FIELD_ID_BATTERY_LEVEL      0x0001u
#define IOTDATA_FIELDS_DEF_BATTERY_LEVEL(F) F(BATTERY_LEVEL)
#define IOTDATA_FIELDS_OPS_BATTERY_LEVEL(E) E(BATTERY_LEVEL, _iotdata_field_def_battery_level)
#else
#define IOTDATA_FIELDS_MEM_BATTERY_LEVEL
#define IOTDATA_FIELDS_DEF_BATTERY_LEVEL(F)
#define IOTDATA_FIELDS_OPS_BATTERY_LEVEL(E)
#endif
#if defined(IOTDATA_ENABLE_BATTERY_CHARGE) || defined(IOTDATA_ENABLE_BATTERY)
#define IOTDATA_FIELDS_DAT_BATTERY_CHARGE bool battery_charging;
typedef struct {
    IOTDATA_FIELDS_DAT_BATTERY_CHARGE
} iotdata_field_battery_charge_t;
#define IOTDATA_BATTERY_CHARGE_BITS 1
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_battery_charge(struct iotdata_encoder_t_ *const enc, bool charging);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_battery_charge_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, bool charging);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_battery_charge(struct iotdata_decoder_t_ *const dec, bool *charging);
#endif
#else
#define IOTDATA_FIELDS_DAT_BATTERY_CHARGE
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_BATTERY_CHARGE(S)
#define IOTDATA_FIELDS_ERR_STRINGS_BATTERY_CHARGE(D)
#if defined(IOTDATA_ENABLE_BATTERY_CHARGE)
#define IOTDATA_FIELDS_MEM_BATTERY_CHARGE    iotdata_field_battery_charge_t battery_charge;
#define IOTDATA_FIELD_ID_BATTERY_CHARGE      0x0003u
#define IOTDATA_FIELDS_DEF_BATTERY_CHARGE(F) F(BATTERY_CHARGE)
#define IOTDATA_FIELDS_OPS_BATTERY_CHARGE(E) E(BATTERY_CHARGE, _iotdata_field_def_battery_charge)
#else
#define IOTDATA_FIELDS_MEM_BATTERY_CHARGE
#define IOTDATA_FIELDS_DEF_BATTERY_CHARGE(F)
#define IOTDATA_FIELDS_OPS_BATTERY_CHARGE(E)
#endif
#if defined(IOTDATA_ENABLE_BATTERY)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_battery(struct iotdata_encoder_t_ *const enc, uint8_t level_percent, bool charging);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_battery_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t level_percent, bool charging);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_battery(struct iotdata_decoder_t_ *const dec, uint8_t *level_percent, bool *charging);
#endif
typedef struct {
    iotdata_field_battery_level_t battery_level;
    iotdata_field_battery_charge_t battery_charge;
} iotdata_field_battery_t;
#define IOTDATA_FIELDS_MEM_BATTERY    iotdata_field_battery_t battery;
#define IOTDATA_FIELD_ID_BATTERY      0x0008u
#define IOTDATA_FIELDS_DEF_BATTERY(F) F(BATTERY)
#define IOTDATA_FIELDS_OPS_BATTERY(E) E(BATTERY, _iotdata_field_def_battery)
#else
#define IOTDATA_FIELDS_MEM_BATTERY
#define IOTDATA_FIELDS_DEF_BATTERY(F)
#define IOTDATA_FIELDS_OPS_BATTERY(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_BATTERY(S)
#define IOTDATA_FIELDS_ERR_STRINGS_BATTERY(D)
#define IOTDATA_FIELDS_DAT_BATTERY IOTDATA_FIELDS_DAT_BATTERY_LEVEL IOTDATA_FIELDS_DAT_BATTERY_CHARGE

/* ---------------------------------------------------------------------------
 * Field LINK, LINK_RSSI, LINK_SNR
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_LINK_RSSI) || defined(IOTDATA_ENABLE_LINK)
#define IOTDATA_FIELDS_DAT_LINK_RSSI int8_t link_rssi;
typedef struct {
    IOTDATA_FIELDS_DAT_LINK_RSSI
} iotdata_field_link_rssi_t;
#define IOTDATA_LINK_RSSI_MIN  (-120)
#define IOTDATA_LINK_RSSI_MAX  (-60)
#define IOTDATA_LINK_RSSI_STEP (4)
#define IOTDATA_LINK_RSSI_BITS (4)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_link_rssi(struct iotdata_encoder_t_ *const enc, int8_t rssi_dbm);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_link_rssi_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, int8_t rssi_dbm);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_link_rssi(struct iotdata_decoder_t_ *const dec, int8_t *rssi_dbm);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_LINK_RSSI(S) \
    S(IOTDATA_ERR_LINK_RSSI_LOW) \
    S(IOTDATA_ERR_LINK_RSSI_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_LINK_RSSI(D) \
    D(IOTDATA_ERR_LINK_RSSI_LOW, "RSSI below -120 dBm") \
    D(IOTDATA_ERR_LINK_RSSI_HIGH, "RSSI above -60 dBm")
#else
#define IOTDATA_FIELDS_DAT_LINK_RSSI
#define IOTDATA_FIELDS_ERR_IDENTS_LINK_RSSI(S)
#define IOTDATA_FIELDS_ERR_STRINGS_LINK_RSSI(D)
#endif
#if defined(IOTDATA_ENABLE_LINK_RSSI)
#define IOTDATA_FIELDS_MEM_LINK_RSSI    iotdata_field_link_rssi_t link_rssi;
#define IOTDATA_FIELD_ID_LINK_RSSI      0x0002u
#define IOTDATA_FIELDS_DEF_LINK_RSSI(F) F(LINK_RSSI)
#define IOTDATA_FIELDS_OPS_LINK_RSSI(E) E(LINK_RSSI, _iotdata_field_def_link_rssi)
#else
#define IOTDATA_FIELDS_MEM_LINK_RSSI
#define IOTDATA_FIELDS_DEF_LINK_RSSI(F)
#define IOTDATA_FIELDS_OPS_LINK_RSSI(E)
#endif
#if defined(IOTDATA_ENABLE_LINK_SNR) || defined(IOTDATA_ENABLE_LINK)
#define IOTDATA_FIELDS_DAT_LINK_SNR iotdata_float_t link_snr;
typedef struct {
    IOTDATA_FIELDS_DAT_LINK_SNR
} iotdata_field_link_snr_t;
#if !defined(IOTDATA_NO_FLOATING)
#define IOTDATA_LINK_SNR_MIN  (-20.0f)
#define IOTDATA_LINK_SNR_MAX  (10.0f)
#define IOTDATA_LINK_SNR_STEP (10.0f)
#else
#define IOTDATA_LINK_SNR_MIN  (-200)
#define IOTDATA_LINK_SNR_MAX  (100)
#define IOTDATA_LINK_SNR_STEP (100)
#endif
#define IOTDATA_LINK_SNR_BITS (2)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_link_snr(struct iotdata_encoder_t_ *const enc, iotdata_float_t snr_db);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_link_snr_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, iotdata_float_t snr_db);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_link_snr(struct iotdata_decoder_t_ *const dec, iotdata_float_t *snr_db);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_LINK_SNR(S) \
    S(IOTDATA_ERR_LINK_SNR_LOW) \
    S(IOTDATA_ERR_LINK_SNR_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_LINK_SNR(D) \
    D(IOTDATA_ERR_LINK_SNR_LOW, "SNR below -20 dB") \
    D(IOTDATA_ERR_LINK_SNR_HIGH, "SNR above +10 dB")
#else
#define IOTDATA_FIELDS_DAT_LINK_SNR
#define IOTDATA_FIELDS_ERR_IDENTS_LINK_SNR(S)
#define IOTDATA_FIELDS_ERR_STRINGS_LINK_SNR(D)
#endif
#if defined(IOTDATA_ENABLE_LINK_SNR)
#define IOTDATA_FIELDS_MEM_LINK_SNR    iotdata_field_link_snr_t link_snr;
#define IOTDATA_FIELD_ID_LINK_SNR      0x0004u
#define IOTDATA_FIELDS_DEF_LINK_SNR(F) F(LINK_SNR)
#define IOTDATA_FIELDS_OPS_LINK_SNR(E) E(LINK_SNR, _iotdata_field_def_link_snr)
#else
#define IOTDATA_FIELDS_MEM_LINK_SNR
#define IOTDATA_FIELDS_DEF_LINK_SNR(F)
#define IOTDATA_FIELDS_OPS_LINK_SNR(E)
#endif
#if defined(IOTDATA_ENABLE_LINK)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_link(struct iotdata_encoder_t_ *const enc, int8_t rssi_dbm, iotdata_float_t snr_db);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_link_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, int8_t rssi_dbm, iotdata_float_t snr_db);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_link(struct iotdata_decoder_t_ *const dec, int8_t *rssi_dbm, iotdata_float_t *snr_db);
#endif
typedef struct {
    iotdata_field_link_rssi_t link_rssi;
    iotdata_field_link_snr_t link_snr;
} iotdata_field_link_t;
#define IOTDATA_FIELDS_MEM_LINK    iotdata_field_link_t link;
#define IOTDATA_FIELD_ID_LINK      0x0009u
#define IOTDATA_FIELDS_DEF_LINK(F) F(LINK)
#define IOTDATA_FIELDS_OPS_LINK(E) E(LINK, _iotdata_field_def_link)
#else
#define IOTDATA_FIELDS_MEM_LINK
#define IOTDATA_FIELDS_DEF_LINK(F)
#define IOTDATA_FIELDS_OPS_LINK(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_LINK(S)
#define IOTDATA_FIELDS_ERR_STRINGS_LINK(D)
#define IOTDATA_FIELDS_DAT_LINK IOTDATA_FIELDS_DAT_LINK_RSSI IOTDATA_FIELDS_DAT_LINK_SNR

/* ---------------------------------------------------------------------------
 * Field ENVIRONMENT, TEMPERATURE, PRESSURE, HUMIDITY
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_TEMPERATURE) || defined(IOTDATA_ENABLE_ENVIRONMENT)
#define IOTDATA_FIELDS_DAT_TEMPERATURE iotdata_float_t temperature;
typedef struct {
    IOTDATA_FIELDS_DAT_TEMPERATURE
} iotdata_field_temperature_t;
#if !defined(IOTDATA_NO_FLOATING)
#define IOTDATA_TEMPERATURE_MIN (-40.0f)
#define IOTDATA_TEMPERATURE_MAX (80.0f)
#define IOTDATA_TEMPERATURE_RES (0.25f)
#else
#define IOTDATA_TEMPERATURE_MIN (-4000)
#define IOTDATA_TEMPERATURE_MAX (8000)
#define IOTDATA_TEMPERATURE_RES (25)
#endif
#define IOTDATA_TEMPERATURE_BITS 9
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_temperature(struct iotdata_encoder_t_ *const enc, iotdata_float_t temperature_c);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_temperature_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, iotdata_float_t temperature_c);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_temperature(struct iotdata_decoder_t_ *const dec, iotdata_float_t *temperature_c);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_TEMPERATURE(S) \
    S(IOTDATA_ERR_TEMPERATURE_LOW) \
    S(IOTDATA_ERR_TEMPERATURE_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_TEMPERATURE(D) \
    D(IOTDATA_ERR_TEMPERATURE_LOW, "Temperature below -40C") \
    D(IOTDATA_ERR_TEMPERATURE_HIGH, "Temperature above +80C")
#else
#define IOTDATA_FIELDS_DAT_TEMPERATURE
#define IOTDATA_FIELDS_ERR_IDENTS_TEMPERATURE(S)
#define IOTDATA_FIELDS_ERR_STRINGS_TEMPERATURE(D)
#endif
#if defined(IOTDATA_ENABLE_TEMPERATURE)
#define IOTDATA_FIELDS_MEM_TEMPERATURE    iotdata_field_temperature_t temperature;
#define IOTDATA_FIELD_ID_TEMPERATURE      0x0010u
#define IOTDATA_FIELDS_DEF_TEMPERATURE(F) F(TEMPERATURE)
#define IOTDATA_FIELDS_OPS_TEMPERATURE(E) E(TEMPERATURE, _iotdata_field_def_temperature)
#else
#define IOTDATA_FIELDS_MEM_TEMPERATURE
#define IOTDATA_FIELDS_DEF_TEMPERATURE(F)
#define IOTDATA_FIELDS_OPS_TEMPERATURE(E)
#endif
#if defined(IOTDATA_ENABLE_PRESSURE) || defined(IOTDATA_ENABLE_ENVIRONMENT)
#define IOTDATA_FIELDS_DAT_PRESSURE uint16_t pressure;
typedef struct {
    IOTDATA_FIELDS_DAT_PRESSURE
} iotdata_field_pressure_t;
#define IOTDATA_PRESSURE_MIN  850
#define IOTDATA_PRESSURE_MAX  1105
#define IOTDATA_PRESSURE_BITS 8
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_pressure(struct iotdata_encoder_t_ *const enc, uint16_t pressure_hpa);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_pressure_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint16_t pressure_hpa);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_pressure(struct iotdata_decoder_t_ *const dec, uint16_t *pressure_hpa);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_PRESSURE(S) \
    S(IOTDATA_ERR_PRESSURE_LOW) \
    S(IOTDATA_ERR_PRESSURE_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_PRESSURE(D) \
    D(IOTDATA_ERR_PRESSURE_LOW, "Pressure below 850 hPa") \
    D(IOTDATA_ERR_PRESSURE_HIGH, "Pressure above 1105 hPa")
#else
#define IOTDATA_FIELDS_DAT_PRESSURE
#define IOTDATA_FIELDS_ERR_IDENTS_PRESSURE(S)
#define IOTDATA_FIELDS_ERR_STRINGS_PRESSURE(D)
#endif
#if defined(IOTDATA_ENABLE_PRESSURE)
#define IOTDATA_FIELDS_MEM_PRESSURE    iotdata_field_pressure_t pressure;
#define IOTDATA_FIELD_ID_PRESSURE      0x0011u
#define IOTDATA_FIELDS_DEF_PRESSURE(F) F(PRESSURE)
#define IOTDATA_FIELDS_OPS_PRESSURE(E) E(PRESSURE, _iotdata_field_def_pressure)
#else
#define IOTDATA_FIELDS_MEM_PRESSURE
#define IOTDATA_FIELDS_DEF_PRESSURE(F)
#define IOTDATA_FIELDS_OPS_PRESSURE(E)
#endif
#if defined(IOTDATA_ENABLE_HUMIDITY) || defined(IOTDATA_ENABLE_ENVIRONMENT)
#define IOTDATA_FIELDS_DAT_HUMIDITY uint8_t humidity;
typedef struct {
    IOTDATA_FIELDS_DAT_HUMIDITY
} iotdata_field_humidity_t;
#define IOTDATA_HUMIDITY_MAX  100
#define IOTDATA_HUMIDITY_BITS 7
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_humidity(struct iotdata_encoder_t_ *const enc, uint8_t humidity_pct);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_humidity_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t humidity_pct);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_humidity(struct iotdata_decoder_t_ *const dec, uint8_t *humidity_pct);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_HUMIDITY(S)  S(IOTDATA_ERR_HUMIDITY_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_HUMIDITY(D) D(IOTDATA_ERR_HUMIDITY_HIGH, "Humidity above 100%")
#else
#define IOTDATA_FIELDS_DAT_HUMIDITY
#define IOTDATA_FIELDS_ERR_IDENTS_HUMIDITY(S)
#define IOTDATA_FIELDS_ERR_STRINGS_HUMIDITY(D)
#endif
#if defined(IOTDATA_ENABLE_HUMIDITY)
#define IOTDATA_FIELDS_MEM_HUMIDITY    iotdata_field_humidity_t humidity;
#define IOTDATA_FIELD_ID_HUMIDITY      0x0012u
#define IOTDATA_FIELDS_DEF_HUMIDITY(F) F(HUMIDITY)
#define IOTDATA_FIELDS_OPS_HUMIDITY(E) E(HUMIDITY, _iotdata_field_def_humidity)
#else
#define IOTDATA_FIELDS_MEM_HUMIDITY
#define IOTDATA_FIELDS_DEF_HUMIDITY(F)
#define IOTDATA_FIELDS_OPS_HUMIDITY(E)
#endif
#if defined(IOTDATA_ENABLE_ENVIRONMENT)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_environment(struct iotdata_encoder_t_ *const enc, iotdata_float_t temp_c, uint16_t pressure_hpa, uint8_t humidity_pct);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_environment_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, iotdata_float_t temp_c, uint16_t pressure_hpa, uint8_t humidity_pct);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_environment(struct iotdata_decoder_t_ *const dec, iotdata_float_t *temp_c, uint16_t *pressure_hpa, uint8_t *humidity_pct);
#endif
typedef struct {
    iotdata_field_temperature_t temperature;
    iotdata_field_pressure_t pressure;
    iotdata_field_humidity_t humidity;
} iotdata_field_environment_t;
#define IOTDATA_FIELDS_MEM_ENVIRONMENT    iotdata_field_environment_t environment;
#define IOTDATA_FIELD_ID_ENVIRONMENT      0x0018u
#define IOTDATA_FIELDS_DEF_ENVIRONMENT(F) F(ENVIRONMENT)
#define IOTDATA_FIELDS_OPS_ENVIRONMENT(E) E(ENVIRONMENT, _iotdata_field_def_environment)
#else
#define IOTDATA_FIELDS_MEM_ENVIRONMENT
#define IOTDATA_FIELDS_DEF_ENVIRONMENT(F)
#define IOTDATA_FIELDS_OPS_ENVIRONMENT(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_ENVIRONMENT(S)
#define IOTDATA_FIELDS_ERR_STRINGS_ENVIRONMENT(D)
#define IOTDATA_FIELDS_DAT_ENVIRONMENT IOTDATA_FIELDS_DAT_TEMPERATURE IOTDATA_FIELDS_DAT_PRESSURE IOTDATA_FIELDS_DAT_HUMIDITY

/* ---------------------------------------------------------------------------
 * Field WIND, WIND_SPEED, WIND_DIRECTION, WIND_GUST
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_WIND_SPEED) || defined(IOTDATA_ENABLE_WIND)
#define IOTDATA_FIELDS_DAT_WIND_SPEED iotdata_float_t wind_speed;
typedef struct {
    IOTDATA_FIELDS_DAT_WIND_SPEED
} iotdata_field_wind_speed_t;
#if !defined(IOTDATA_NO_FLOATING)
#define IOTDATA_WIND_SPEED_RES (0.5f)
#define IOTDATA_WIND_SPEED_MAX (63.5f)
#else
#define IOTDATA_WIND_SPEED_RES (50)
#define IOTDATA_WIND_SPEED_MAX (6350)
#endif
#define IOTDATA_WIND_SPEED_BITS 7
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_wind_speed(struct iotdata_encoder_t_ *const enc, iotdata_float_t speed_ms);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_wind_speed_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, iotdata_float_t speed_ms);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_wind_speed(struct iotdata_decoder_t_ *const dec, iotdata_float_t *speed_ms);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_WIND_SPEED(S)  S(IOTDATA_ERR_WIND_SPEED_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_WIND_SPEED(D) D(IOTDATA_ERR_WIND_SPEED_HIGH, "Wind speed above 63.5 m/s")
#else
#define IOTDATA_FIELDS_DAT_WIND_SPEED
#define IOTDATA_FIELDS_ERR_IDENTS_WIND_SPEED(S)
#define IOTDATA_FIELDS_ERR_STRINGS_WIND_SPEED(D)
#endif
#if defined(IOTDATA_ENABLE_WIND_SPEED)
#define IOTDATA_FIELDS_MEM_WIND_SPEED    iotdata_field_wind_speed_t wind_speed;
#define IOTDATA_FIELD_ID_WIND_SPEED      0x0020u
#define IOTDATA_FIELDS_DEF_WIND_SPEED(F) F(WIND_SPEED)
#define IOTDATA_FIELDS_OPS_WIND_SPEED(E) E(WIND_SPEED, _iotdata_field_def_wind_speed)
#else
#define IOTDATA_FIELDS_MEM_WIND_SPEED
#define IOTDATA_FIELDS_DEF_WIND_SPEED(F)
#define IOTDATA_FIELDS_OPS_WIND_SPEED(E)
#endif
#if defined(IOTDATA_ENABLE_WIND_DIRECTION) || defined(IOTDATA_ENABLE_WIND)
#define IOTDATA_FIELDS_DAT_WIND_DIRECTION uint16_t wind_direction;
typedef struct {
    IOTDATA_FIELDS_DAT_WIND_DIRECTION
} iotdata_field_wind_direction_t;
#define IOTDATA_WIND_DIRECTION_MAX  359
#define IOTDATA_WIND_DIRECTION_BITS 8
#if !defined(IOTDATA_NO_FLOATING)
#define IOTDATA_WIND_DIRECTION_SCALE ((float)(IOTDATA_WIND_DIRECTION_MAX + 1) / (float)(1 << IOTDATA_WIND_DIRECTION_BITS))
#endif
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_wind_direction(struct iotdata_encoder_t_ *const enc, uint16_t direction_deg);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_wind_direction_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint16_t direction_deg);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_wind_direction(struct iotdata_decoder_t_ *const dec, uint16_t *direction_deg);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_WIND_DIRECTION(S)  S(IOTDATA_ERR_WIND_DIRECTION_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_WIND_DIRECTION(D) D(IOTDATA_ERR_WIND_DIRECTION_HIGH, "Wind direction above 359 degrees")
#else
#define IOTDATA_FIELDS_DAT_WIND_DIRECTION
#define IOTDATA_FIELDS_ERR_IDENTS_WIND_DIRECTION(S)
#define IOTDATA_FIELDS_ERR_STRINGS_WIND_DIRECTION(D)
#endif
#if defined(IOTDATA_ENABLE_WIND_DIRECTION)
#define IOTDATA_FIELDS_MEM_WIND_DIRECTION    iotdata_field_wind_direction_t wind_direction;
#define IOTDATA_FIELD_ID_WIND_DIRECTION      0x0021u
#define IOTDATA_FIELDS_DEF_WIND_DIRECTION(F) F(WIND_DIRECTION)
#define IOTDATA_FIELDS_OPS_WIND_DIRECTION(E) E(WIND_DIRECTION, _iotdata_field_def_wind_direction)
#else
#define IOTDATA_FIELDS_MEM_WIND_DIRECTION
#define IOTDATA_FIELDS_DEF_WIND_DIRECTION(F)
#define IOTDATA_FIELDS_OPS_WIND_DIRECTION(E)
#endif
#if defined(IOTDATA_ENABLE_WIND_GUST) || defined(IOTDATA_ENABLE_WIND)
#define IOTDATA_FIELDS_DAT_WIND_GUST iotdata_float_t wind_gust;
typedef struct {
    IOTDATA_FIELDS_DAT_WIND_GUST
} iotdata_field_wind_gust_t;
#if !defined(IOTDATA_NO_FLOATING)
#define IOTDATA_WIND_GUST_RES (0.5f)
#define IOTDATA_WIND_GUST_MAX (63.5f)
#else
#define IOTDATA_WIND_GUST_RES (50)
#define IOTDATA_WIND_GUST_MAX (6350)
#endif
#define IOTDATA_WIND_GUST_BITS 7
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_wind_gust(struct iotdata_encoder_t_ *const enc, iotdata_float_t gust_ms);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_wind_gust_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, iotdata_float_t gust_ms);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_wind_gust(struct iotdata_decoder_t_ *const dec, iotdata_float_t *gust_ms);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_WIND_GUST(S)  S(IOTDATA_ERR_WIND_GUST_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_WIND_GUST(D) D(IOTDATA_ERR_WIND_GUST_HIGH, "Wind gust above 63.5 m/s")
#else
#define IOTDATA_FIELDS_DAT_WIND_GUST
#define IOTDATA_FIELDS_ERR_IDENTS_WIND_GUST(S)
#define IOTDATA_FIELDS_ERR_STRINGS_WIND_GUST(D)
#endif
#if defined(IOTDATA_ENABLE_WIND_GUST)
#define IOTDATA_FIELDS_MEM_WIND_GUST    iotdata_field_wind_gust_t wind_gust;
#define IOTDATA_FIELD_ID_WIND_GUST      0x0022u
#define IOTDATA_FIELDS_DEF_WIND_GUST(F) F(WIND_GUST)
#define IOTDATA_FIELDS_OPS_WIND_GUST(E) E(WIND_GUST, _iotdata_field_def_wind_gust)
#else
#define IOTDATA_FIELDS_MEM_WIND_GUST
#define IOTDATA_FIELDS_DEF_WIND_GUST(F)
#define IOTDATA_FIELDS_OPS_WIND_GUST(E)
#endif
#if defined(IOTDATA_ENABLE_WIND)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_wind(struct iotdata_encoder_t_ *const enc, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_wind_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_wind(struct iotdata_decoder_t_ *const dec, iotdata_float_t *speed_ms, uint16_t *direction_deg, iotdata_float_t *gust_ms);
#endif
typedef struct {
    iotdata_field_wind_speed_t wind_speed;
    iotdata_field_wind_direction_t wind_direction;
    iotdata_field_wind_gust_t wind_gust;
} iotdata_field_wind_t;
#define IOTDATA_FIELDS_MEM_WIND    iotdata_field_wind_t wind;
#define IOTDATA_FIELD_ID_WIND      0x0028u
#define IOTDATA_FIELDS_DEF_WIND(F) F(WIND)
#define IOTDATA_FIELDS_OPS_WIND(E) E(WIND, _iotdata_field_def_wind)
#else
#define IOTDATA_FIELDS_MEM_WIND
#define IOTDATA_FIELDS_DEF_WIND(F)
#define IOTDATA_FIELDS_OPS_WIND(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_WIND(S)
#define IOTDATA_FIELDS_ERR_STRINGS_WIND(D)
#define IOTDATA_FIELDS_DAT_WIND IOTDATA_FIELDS_DAT_WIND_SPEED IOTDATA_FIELDS_DAT_WIND_DIRECTION IOTDATA_FIELDS_DAT_WIND_GUST

/* ---------------------------------------------------------------------------
 * Field RAIN, RAIN_RATE, RAIN_SIZE
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_RAIN_RATE) || defined(IOTDATA_ENABLE_RAIN)
#define IOTDATA_FIELDS_DAT_RAIN_RATE uint8_t rain_rate;
typedef struct {
    IOTDATA_FIELDS_DAT_RAIN_RATE
} iotdata_field_rain_rate_t;
#define IOTDATA_RAIN_RATE_MAX  255
#define IOTDATA_RAIN_RATE_BITS 8
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_rain_rate(struct iotdata_encoder_t_ *const enc, uint8_t rate_mmhr);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_rain_rate_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t rate_mmhr);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_rain_rate(struct iotdata_decoder_t_ *const dec, uint8_t *rate_mmhr);
#endif
#else
#define IOTDATA_FIELDS_DAT_RAIN_RATE
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_RAIN_RATE(S)
#define IOTDATA_FIELDS_ERR_STRINGS_RAIN_RATE(D)
#if defined(IOTDATA_ENABLE_RAIN_RATE)
#define IOTDATA_FIELDS_MEM_RAIN_RATE    iotdata_field_rain_rate_t rain_rate;
#define IOTDATA_FIELD_ID_RAIN_RATE      0x0030u
#define IOTDATA_FIELDS_DEF_RAIN_RATE(F) F(RAIN_RATE)
#define IOTDATA_FIELDS_OPS_RAIN_RATE(E) E(RAIN_RATE, _iotdata_field_def_rain_rate)
#else
#define IOTDATA_FIELDS_MEM_RAIN_RATE
#define IOTDATA_FIELDS_DEF_RAIN_RATE(F)
#define IOTDATA_FIELDS_OPS_RAIN_RATE(E)
#endif
#if defined(IOTDATA_ENABLE_RAIN_SIZE) || defined(IOTDATA_ENABLE_RAIN)
#define IOTDATA_FIELDS_DAT_RAIN_SIZE uint8_t rain_size10;
typedef struct {
    IOTDATA_FIELDS_DAT_RAIN_SIZE
} iotdata_field_rain_size_t;
#define IOTDATA_RAIN_SIZE_MAX   15
#define IOTDATA_RAIN_SIZE_BITS  4
#define IOTDATA_RAIN_SIZE_SCALE 4
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_rain_size(struct iotdata_encoder_t_ *const enc, uint8_t size10_mmd);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_rain_size_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t size10_mmd);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_rain_size(struct iotdata_decoder_t_ *const dec, uint8_t *size10_mmd);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_RAIN_SIZE(S)  S(IOTDATA_ERR_RAIN_SIZE_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_RAIN_SIZE(D) D(IOTDATA_ERR_RAIN_SIZE_HIGH, "Rain size above 6.0 mm/d")
#else
#define IOTDATA_FIELDS_DAT_RAIN_SIZE
#define IOTDATA_FIELDS_ERR_IDENTS_RAIN_SIZE(S)
#define IOTDATA_FIELDS_ERR_STRINGS_RAIN_SIZE(D)
#endif
#if defined(IOTDATA_ENABLE_RAIN_SIZE)
#define IOTDATA_FIELDS_MEM_RAIN_SIZE    iotdata_field_rain_size_t rain_size;
#define IOTDATA_FIELD_ID_RAIN_SIZE      0x0031u
#define IOTDATA_FIELDS_DEF_RAIN_SIZE(F) F(RAIN_SIZE)
#define IOTDATA_FIELDS_OPS_RAIN_SIZE(E) E(RAIN_SIZE, _iotdata_field_def_rain_size)
#else
#define IOTDATA_FIELDS_MEM_RAIN_SIZE
#define IOTDATA_FIELDS_DEF_RAIN_SIZE(F)
#define IOTDATA_FIELDS_OPS_RAIN_SIZE(E)
#endif
#if defined(IOTDATA_ENABLE_RAIN)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_rain(struct iotdata_encoder_t_ *const enc, uint8_t rate_mmhr, uint8_t size10_mmd);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_rain_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t rate_mmhr, uint8_t size10_mmd);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_rain(struct iotdata_decoder_t_ *const dec, uint8_t *rate_mmhr, uint8_t *size10_mmd);
#endif
typedef struct {
    iotdata_field_rain_rate_t rain_rate;
    iotdata_field_rain_size_t rain_size;
} iotdata_field_rain_t;
#define IOTDATA_FIELDS_MEM_RAIN    iotdata_field_rain_t rain;
#define IOTDATA_FIELD_ID_RAIN      0x0038u
#define IOTDATA_FIELDS_DEF_RAIN(F) F(RAIN)
#define IOTDATA_FIELDS_OPS_RAIN(E) E(RAIN, _iotdata_field_def_rain)
#else
#define IOTDATA_FIELDS_MEM_RAIN
#define IOTDATA_FIELDS_DEF_RAIN(F)
#define IOTDATA_FIELDS_OPS_RAIN(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_RAIN(S)
#define IOTDATA_FIELDS_ERR_STRINGS_RAIN(D)
#define IOTDATA_FIELDS_DAT_RAIN IOTDATA_FIELDS_DAT_RAIN_RATE IOTDATA_FIELDS_DAT_RAIN_SIZE

/* ---------------------------------------------------------------------------
 * Field SOLAR, SOLAR_IRRADIANCE, SOLAR_ULTRAVIOLET
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_SOLAR_IRRADIANCE) || defined(IOTDATA_ENABLE_SOLAR)
#define IOTDATA_FIELDS_DAT_SOLAR_IRRADIANCE uint16_t solar_irradiance;
typedef struct {
    IOTDATA_FIELDS_DAT_SOLAR_IRRADIANCE
} iotdata_field_solar_irradiance_t;
#define IOTDATA_SOLAR_IRRADIANCE_MAX  1023
#define IOTDATA_SOLAR_IRRADIANCE_BITS 10
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_solar_irradiance(struct iotdata_encoder_t_ *const enc, uint16_t irradiance_wm2);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_solar_irradiance_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint16_t irradiance_wm2);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_solar_irradiance(struct iotdata_decoder_t_ *const dec, uint16_t *irradiance_wm2);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_SOLAR_IRRADIANCE(S)  S(IOTDATA_ERR_SOLAR_IRRADIANCE_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_SOLAR_IRRADIANCE(D) D(IOTDATA_ERR_SOLAR_IRRADIANCE_HIGH, "Solar irradiance above 1023 W/m2")
#else
#define IOTDATA_FIELDS_DAT_SOLAR_IRRADIANCE
#define IOTDATA_FIELDS_ERR_IDENTS_SOLAR_IRRADIANCE(S)
#define IOTDATA_FIELDS_ERR_STRINGS_SOLAR_IRRADIANCE(D)
#endif
#if defined(IOTDATA_ENABLE_SOLAR_IRRADIANCE)
#define IOTDATA_FIELDS_MEM_SOLAR_IRRADIANCE    iotdata_field_solar_irradiance_t solar_irradiance;
#define IOTDATA_FIELD_ID_SOLAR_IRRADIANCE      0x0040u
#define IOTDATA_FIELDS_DEF_SOLAR_IRRADIANCE(F) F(SOLAR_IRRADIANCE)
#define IOTDATA_FIELDS_OPS_SOLAR_IRRADIANCE(E) E(SOLAR_IRRADIANCE, _iotdata_field_def_solar_irradiance)
#else
#define IOTDATA_FIELDS_MEM_SOLAR_IRRADIANCE
#define IOTDATA_FIELDS_DEF_SOLAR_IRRADIANCE(F)
#define IOTDATA_FIELDS_OPS_SOLAR_IRRADIANCE(E)
#endif
#if defined(IOTDATA_ENABLE_SOLAR_ULTRAVIOLET) || defined(IOTDATA_ENABLE_SOLAR)
#define IOTDATA_FIELDS_DAT_SOLAR_ULTRAVIOLET uint8_t solar_ultraviolet;
typedef struct {
    IOTDATA_FIELDS_DAT_SOLAR_ULTRAVIOLET
} iotdata_field_solar_ultraviolet_t;
#define IOTDATA_SOLAR_ULTRAVIOLET_MAX  15
#define IOTDATA_SOLAR_ULTRAVIOLET_BITS 4
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_solar_ultraviolet(struct iotdata_encoder_t_ *const enc, uint8_t ultraviolet_index);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_solar_ultraviolet_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t ultraviolet_index);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_solar_ultraviolet(struct iotdata_decoder_t_ *const dec, uint8_t *ultraviolet_index);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_SOLAR_ULTRAVIOLET(S)  S(IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_SOLAR_ULTRAVIOLET(D) D(IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH, "Solar UV index above 15")
#else
#define IOTDATA_FIELDS_DAT_SOLAR_ULTRAVIOLET
#define IOTDATA_FIELDS_ERR_IDENTS_SOLAR_ULTRAVIOLET(S)
#define IOTDATA_FIELDS_ERR_STRINGS_SOLAR_ULTRAVIOLET(D)
#endif
#if defined(IOTDATA_ENABLE_SOLAR_ULTRAVIOLET)
#define IOTDATA_FIELDS_MEM_SOLAR_ULTRAVIOLET    iotdata_field_solar_ultraviolet_t solar_ultraviolet;
#define IOTDATA_FIELD_ID_SOLAR_ULTRAVIOLET      0x0042u
#define IOTDATA_FIELDS_DEF_SOLAR_ULTRAVIOLET(F) F(SOLAR_ULTRAVIOLET)
#define IOTDATA_FIELDS_OPS_SOLAR_ULTRAVIOLET(E) E(SOLAR_ULTRAVIOLET, _iotdata_field_def_solar_ultraviolet)
#else
#define IOTDATA_FIELDS_MEM_SOLAR_ULTRAVIOLET
#define IOTDATA_FIELDS_DEF_SOLAR_ULTRAVIOLET(F)
#define IOTDATA_FIELDS_OPS_SOLAR_ULTRAVIOLET(E)
#endif
#if defined(IOTDATA_ENABLE_SOLAR)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_solar(struct iotdata_encoder_t_ *const enc, uint16_t irradiance_wm2, uint8_t ultraviolet_index);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_solar_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint16_t irradiance_wm2, uint8_t ultraviolet_index);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_solar(struct iotdata_decoder_t_ *const dec, uint16_t *irradiance_wm2, uint8_t *ultraviolet_index);
#endif
typedef struct {
    iotdata_field_solar_irradiance_t solar_irradiance;
    iotdata_field_solar_ultraviolet_t solar_ultraviolet;
} iotdata_field_solar_t;
#define IOTDATA_FIELDS_MEM_SOLAR    iotdata_field_solar_t solar;
#define IOTDATA_FIELD_ID_SOLAR      0x0048u
#define IOTDATA_FIELDS_DEF_SOLAR(F) F(SOLAR)
#define IOTDATA_FIELDS_OPS_SOLAR(E) E(SOLAR, _iotdata_field_def_solar)
#else
#define IOTDATA_FIELDS_MEM_SOLAR
#define IOTDATA_FIELDS_DEF_SOLAR(F)
#define IOTDATA_FIELDS_OPS_SOLAR(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_SOLAR(S)
#define IOTDATA_FIELDS_ERR_STRINGS_SOLAR(D)
#define IOTDATA_FIELDS_DAT_SOLAR IOTDATA_FIELDS_DAT_SOLAR_IRRADIANCE IOTDATA_FIELDS_DAT_SOLAR_ULTRAVIOLET

/* ---------------------------------------------------------------------------
 * Field CLOUDS
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_CLOUDS)
#define IOTDATA_FIELDS_DAT_CLOUDS uint8_t clouds;
typedef struct {
    IOTDATA_FIELDS_DAT_CLOUDS
} iotdata_field_clouds_t;
#define IOTDATA_CLOUDS_MAX  8
#define IOTDATA_CLOUDS_BITS 4
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_clouds(struct iotdata_encoder_t_ *const enc, uint8_t okta);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_clouds_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t okta);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_clouds(struct iotdata_decoder_t_ *const dec, uint8_t *okta);
#endif
#define IOTDATA_FIELDS_MEM_CLOUDS            iotdata_field_clouds_t clouds;
#define IOTDATA_FIELD_ID_CLOUDS              0x0041u
#define IOTDATA_FIELDS_DEF_CLOUDS(F)         F(CLOUDS)
#define IOTDATA_FIELDS_OPS_CLOUDS(E)         E(CLOUDS, _iotdata_field_def_clouds)
#define IOTDATA_FIELDS_ERR_IDENTS_CLOUDS(S)  S(IOTDATA_ERR_CLOUDS_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_CLOUDS(D) D(IOTDATA_ERR_CLOUDS_HIGH, "Cloud cover above 8 okta")
#else
#define IOTDATA_FIELDS_DAT_CLOUDS
#define IOTDATA_FIELDS_MEM_CLOUDS
#define IOTDATA_FIELDS_DEF_CLOUDS(F)
#define IOTDATA_FIELDS_OPS_CLOUDS(E)
#define IOTDATA_FIELDS_ERR_IDENTS_CLOUDS(S)
#define IOTDATA_FIELDS_ERR_STRINGS_CLOUDS(D)
#endif

/* ---------------------------------------------------------------------------
 * Field AIR_QUALITY, AIR_QUALITY_INDEX, AIR_QUALITY_PM, AIR_QUALITY_GAS
 * -------------------------------------------------------------------------*/

/* --- Sub-field: AQ_INDEX (AQI 0-500, 9 bits) --- */
#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX) || defined(IOTDATA_ENABLE_AIR_QUALITY)
#define IOTDATA_FIELDS_DAT_AIR_QUALITY_INDEX uint16_t aq_index;
typedef struct {
    IOTDATA_FIELDS_DAT_AIR_QUALITY_INDEX
} iotdata_field_air_quality_index_t;
#define IOTDATA_AIR_QUALITY_INDEX_MAX  500
#define IOTDATA_AIR_QUALITY_INDEX_BITS 9
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_air_quality_index(struct iotdata_encoder_t_ *const enc, uint16_t aq_index);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_air_quality_index_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint16_t aq_index);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_air_quality_index(struct iotdata_decoder_t_ *const dec, uint16_t *aq_index);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_INDEX(S)  S(IOTDATA_ERR_AIR_QUALITY_INDEX_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_INDEX(D) D(IOTDATA_ERR_AIR_QUALITY_INDEX_HIGH, "AQ index above 500 AQI")
#else
#define IOTDATA_FIELDS_DAT_AIR_QUALITY_INDEX
#define IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_INDEX(S)
#define IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_INDEX(D)
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX)
#define IOTDATA_FIELDS_MEM_AIR_QUALITY_INDEX    iotdata_field_air_quality_index_t air_quality_index;
#define IOTDATA_FIELD_ID_AIR_QUALITY_INDEX      0x0050u
#define IOTDATA_FIELDS_DEF_AIR_QUALITY_INDEX(F) F(AIR_QUALITY_INDEX)
#define IOTDATA_FIELDS_OPS_AIR_QUALITY_INDEX(E) E(AIR_QUALITY_INDEX, _iotdata_field_def_air_quality_index)
#else
#define IOTDATA_FIELDS_MEM_AIR_QUALITY_INDEX
#define IOTDATA_FIELDS_DEF_AIR_QUALITY_INDEX(F)
#define IOTDATA_FIELDS_OPS_AIR_QUALITY_INDEX(E)
#endif
/* --- Sub-field: AQ_PM (4 channels, presence + 8 bits each, res 5 ug/m3) --- */
#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM) || defined(IOTDATA_ENABLE_AIR_QUALITY)
#define IOTDATA_AIR_QUALITY_PM_COUNT 4
#define IOTDATA_FIELDS_DAT_AIR_QUALITY_PM \
    uint8_t aq_pm_present; \
    uint16_t aq_pm[IOTDATA_AIR_QUALITY_PM_COUNT];
typedef struct {
    IOTDATA_FIELDS_DAT_AIR_QUALITY_PM
} iotdata_field_air_quality_pm_t;
#define IOTDATA_AIR_QUALITY_PM_PRESENT_BITS 4
#define IOTDATA_AIR_QUALITY_PM_VALUE_BITS   8
#define IOTDATA_AIR_QUALITY_PM_VALUE_RES    5
#define IOTDATA_AIR_QUALITY_PM_VALUE_MAX    1275 /* 255 * 5 */
#define IOTDATA_AIR_QUALITY_PM_INDEX_PM1    0
#define IOTDATA_AIR_QUALITY_PM_INDEX_PM25   1
#define IOTDATA_AIR_QUALITY_PM_INDEX_PM4    2
#define IOTDATA_AIR_QUALITY_PM_INDEX_PM10   3
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_air_quality_pm(struct iotdata_encoder_t_ *const enc, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_air_quality_pm_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_air_quality_pm(struct iotdata_decoder_t_ *const dec, uint8_t *pm_present, uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_PM(S)  S(IOTDATA_ERR_AIR_QUALITY_PM_VALUE_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_PM(D) D(IOTDATA_ERR_AIR_QUALITY_PM_VALUE_HIGH, "AQ PM value above 1275 ug/m3")
#else
#define IOTDATA_FIELDS_DAT_AIR_QUALITY_PM
#define IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_PM(S)
#define IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_PM(D)
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM)
#define IOTDATA_FIELDS_MEM_AIR_QUALITY_PM    iotdata_field_air_quality_pm_t air_quality_pm;
#define IOTDATA_FIELD_ID_AIR_QUALITY_PM      0x0051u
#define IOTDATA_FIELDS_DEF_AIR_QUALITY_PM(F) F(AIR_QUALITY_PM)
#define IOTDATA_FIELDS_OPS_AIR_QUALITY_PM(E) E(AIR_QUALITY_PM, _iotdata_field_def_air_quality_pm)
#else
#define IOTDATA_FIELDS_MEM_AIR_QUALITY_PM
#define IOTDATA_FIELDS_DEF_AIR_QUALITY_PM(F)
#define IOTDATA_FIELDS_OPS_AIR_QUALITY_PM(E)
#endif
/* --- Sub-field: AQ_GAS (8 slots, presence + variable bits per slot) --- */
#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS) || defined(IOTDATA_ENABLE_AIR_QUALITY)
#define IOTDATA_AIR_QUALITY_GAS_COUNT 8
#define IOTDATA_FIELDS_DAT_AIR_QUALITY_GAS \
    uint8_t aq_gas_present; \
    uint16_t aq_gas[IOTDATA_AIR_QUALITY_GAS_COUNT];
typedef struct {
    IOTDATA_FIELDS_DAT_AIR_QUALITY_GAS
} iotdata_field_air_quality_gas_t;
#define IOTDATA_AIR_QUALITY_GAS_PRESENT_BITS 8
#define IOTDATA_AIR_QUALITY_GAS_INDEX_VOC    0
#define IOTDATA_AIR_QUALITY_GAS_INDEX_NOX    1
#define IOTDATA_AIR_QUALITY_GAS_INDEX_CO2    2
#define IOTDATA_AIR_QUALITY_GAS_INDEX_CO     3
#define IOTDATA_AIR_QUALITY_GAS_INDEX_HCHO   4
#define IOTDATA_AIR_QUALITY_GAS_INDEX_O3     5
#define IOTDATA_AIR_QUALITY_GAS_INDEX_RSVD6  6
#define IOTDATA_AIR_QUALITY_GAS_INDEX_RSVD7  7
/* Bit width per slot: indices=8, concentrations=10 */
#define IOTDATA_AIR_QUALITY_GAS_BITS_VOC     8  /* 0-510, res 2 index pts   */
#define IOTDATA_AIR_QUALITY_GAS_BITS_NOX     8  /* 0-510, res 2 index pts   */
#define IOTDATA_AIR_QUALITY_GAS_BITS_CO2     10 /* 0-51150, res 50 ppm      */
#define IOTDATA_AIR_QUALITY_GAS_BITS_CO      10 /* 0-1023, res 1 ppm        */
#define IOTDATA_AIR_QUALITY_GAS_BITS_HCHO    10 /* 0-5115, res 5 ppb        */
#define IOTDATA_AIR_QUALITY_GAS_BITS_O3      10 /* 0-1023, res 1 ppb        */
#define IOTDATA_AIR_QUALITY_GAS_BITS_RSVD6   10
#define IOTDATA_AIR_QUALITY_GAS_BITS_RSVD7   10
/* Resolution per slot */
#define IOTDATA_AIR_QUALITY_GAS_RES_VOC      2
#define IOTDATA_AIR_QUALITY_GAS_RES_NOX      2
#define IOTDATA_AIR_QUALITY_GAS_RES_CO2      50
#define IOTDATA_AIR_QUALITY_GAS_RES_CO       1
#define IOTDATA_AIR_QUALITY_GAS_RES_HCHO     5
#define IOTDATA_AIR_QUALITY_GAS_RES_O3       1
#define IOTDATA_AIR_QUALITY_GAS_RES_RSVD6    1
#define IOTDATA_AIR_QUALITY_GAS_RES_RSVD7    1
/* Max physical value per slot */
#define IOTDATA_AIR_QUALITY_GAS_MAX_VOC      510
#define IOTDATA_AIR_QUALITY_GAS_MAX_NOX      510
#define IOTDATA_AIR_QUALITY_GAS_MAX_CO2      51150
#define IOTDATA_AIR_QUALITY_GAS_MAX_CO       1023
#define IOTDATA_AIR_QUALITY_GAS_MAX_HCHO     5115
#define IOTDATA_AIR_QUALITY_GAS_MAX_O3       1023
#define IOTDATA_AIR_QUALITY_GAS_MAX_RSVD6    1023
#define IOTDATA_AIR_QUALITY_GAS_MAX_RSVD7    1023
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_air_quality_gas(struct iotdata_encoder_t_ *const enc, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_air_quality_gas_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_air_quality_gas(struct iotdata_decoder_t_ *const dec, uint8_t *gas_present, uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_GAS(S)  S(IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_GAS(D) D(IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH, "AQ gas value above slot maximum")
#else
#define IOTDATA_FIELDS_DAT_AIR_QUALITY_GAS
#define IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_GAS(S)
#define IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_GAS(D)
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS)
#define IOTDATA_FIELDS_MEM_AIR_QUALITY_GAS    iotdata_field_air_quality_gas_t air_quality_gas;
#define IOTDATA_FIELD_ID_AIR_QUALITY_GAS      0x0052u
#define IOTDATA_FIELDS_DEF_AIR_QUALITY_GAS(F) F(AIR_QUALITY_GAS)
#define IOTDATA_FIELDS_OPS_AIR_QUALITY_GAS(E) E(AIR_QUALITY_GAS, _iotdata_field_def_air_quality_gas)
#else
#define IOTDATA_FIELDS_MEM_AIR_QUALITY_GAS
#define IOTDATA_FIELDS_DEF_AIR_QUALITY_GAS(F)
#define IOTDATA_FIELDS_OPS_AIR_QUALITY_GAS(E)
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_air_quality(struct iotdata_encoder_t_ *const enc, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present,
                                                  const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_air_quality_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present,
                                                          const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_air_quality(struct iotdata_decoder_t_ *const dec, uint16_t *aq_index, uint8_t *pm_present, uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t *gas_present, uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#endif
typedef struct {
    iotdata_field_air_quality_index_t air_quality_index;
    iotdata_field_air_quality_pm_t air_quality_pm;
    iotdata_field_air_quality_gas_t air_quality_gas;
} iotdata_field_air_quality_t;
#define IOTDATA_FIELDS_MEM_AIR_QUALITY    iotdata_field_air_quality_t air_quality;
#define IOTDATA_FIELD_ID_AIR_QUALITY      0x0058u
#define IOTDATA_FIELDS_DEF_AIR_QUALITY(F) F(AIR_QUALITY)
#define IOTDATA_FIELDS_OPS_AIR_QUALITY(E) E(AIR_QUALITY, _iotdata_field_def_air_quality)
#else
#define IOTDATA_FIELDS_MEM_AIR_QUALITY
#define IOTDATA_FIELDS_DEF_AIR_QUALITY(F)
#define IOTDATA_FIELDS_OPS_AIR_QUALITY(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY(S)
#define IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY(D)
#define IOTDATA_FIELDS_DAT_AIR_QUALITY IOTDATA_FIELDS_DAT_AIR_QUALITY_INDEX IOTDATA_FIELDS_DAT_AIR_QUALITY_PM IOTDATA_FIELDS_DAT_AIR_QUALITY_GAS

/* ---------------------------------------------------------------------------
 * Field RADIATION, RADIATION_CPM, RADIATION_DOSE
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_RADIATION_CPM) || defined(IOTDATA_ENABLE_RADIATION)
#define IOTDATA_FIELDS_DAT_RADIATION_CPM uint16_t radiation_cpm;
typedef struct {
    IOTDATA_FIELDS_DAT_RADIATION_CPM
} iotdata_field_radiation_cpm_t;
#define IOTDATA_RADIATION_CPM_MAX  16383
#define IOTDATA_RADIATION_CPM_BITS 14
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_radiation_cpm(struct iotdata_encoder_t_ *const enc, uint16_t cpm);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_radiation_cpm_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint16_t cpm);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_radiation_cpm(struct iotdata_decoder_t_ *const dec, uint16_t *cpm);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_RADIATION_CPM(S)  S(IOTDATA_ERR_RADIATION_CPM_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_RADIATION_CPM(D) D(IOTDATA_ERR_RADIATION_CPM_HIGH, "Radiation CPM above 16383")
#else
#define IOTDATA_FIELDS_DAT_RADIATION_CPM
#define IOTDATA_FIELDS_ERR_IDENTS_RADIATION_CPM(S)
#define IOTDATA_FIELDS_ERR_STRINGS_RADIATION_CPM(D)
#endif
#if defined(IOTDATA_ENABLE_RADIATION_CPM)
#define IOTDATA_FIELDS_MEM_RADIATION_CPM    iotdata_field_radiation_cpm_t radiation_cpm;
#define IOTDATA_FIELD_ID_RADIATION_CPM      0x0060u
#define IOTDATA_FIELDS_DEF_RADIATION_CPM(F) F(RADIATION_CPM)
#define IOTDATA_FIELDS_OPS_RADIATION_CPM(E) E(RADIATION_CPM, _iotdata_field_def_radiation_cpm)
#else
#define IOTDATA_FIELDS_MEM_RADIATION_CPM
#define IOTDATA_FIELDS_DEF_RADIATION_CPM(F)
#define IOTDATA_FIELDS_OPS_RADIATION_CPM(E)
#endif
#if defined(IOTDATA_ENABLE_RADIATION_DOSE) || defined(IOTDATA_ENABLE_RADIATION)
#define IOTDATA_FIELDS_DAT_RADIATION_DOSE iotdata_float_t radiation_dose;
typedef struct {
    IOTDATA_FIELDS_DAT_RADIATION_DOSE
} iotdata_field_radiation_dose_t;
#if !defined(IOTDATA_NO_FLOATING)
#define IOTDATA_RADIATION_DOSE_MAX_RAW 16383
#define IOTDATA_RADIATION_DOSE_RES     (0.01f)
#define IOTDATA_RADIATION_DOSE_MAX     (163.83f)
#else
#define IOTDATA_RADIATION_DOSE_MAX 16383
#define IOTDATA_RADIATION_DOSE_RES 1
#endif
#define IOTDATA_RADIATION_DOSE_BITS 14
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_radiation_dose(struct iotdata_encoder_t_ *const enc, iotdata_float_t usvh);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_radiation_dose_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, iotdata_float_t usvh);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_radiation_dose(struct iotdata_decoder_t_ *const dec, iotdata_float_t *usvh);
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_RADIATION_DOSE(S)  S(IOTDATA_ERR_RADIATION_DOSE_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_RADIATION_DOSE(D) D(IOTDATA_ERR_RADIATION_DOSE_HIGH, "Radiation dose above 163.83 uSv/h")
#else
#define IOTDATA_FIELDS_DAT_RADIATION_DOSE
#define IOTDATA_FIELDS_ERR_IDENTS_RADIATION_DOSE(S)
#define IOTDATA_FIELDS_ERR_STRINGS_RADIATION_DOSE(D)
#endif
#if defined(IOTDATA_ENABLE_RADIATION_DOSE)
#define IOTDATA_FIELDS_MEM_RADIATION_DOSE    iotdata_field_radiation_dose_t radiation_dose;
#define IOTDATA_FIELD_ID_RADIATION_DOSE      0x0061u
#define IOTDATA_FIELDS_DEF_RADIATION_DOSE(F) F(RADIATION_DOSE)
#define IOTDATA_FIELDS_OPS_RADIATION_DOSE(E) E(RADIATION_DOSE, _iotdata_field_def_radiation_dose)
#else
#define IOTDATA_FIELDS_MEM_RADIATION_DOSE
#define IOTDATA_FIELDS_DEF_RADIATION_DOSE(F)
#define IOTDATA_FIELDS_OPS_RADIATION_DOSE(E)
#endif
#if defined(IOTDATA_ENABLE_RADIATION)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_radiation(struct iotdata_encoder_t_ *const enc, uint16_t cpm, iotdata_float_t usvh);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_radiation_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint16_t cpm, iotdata_float_t usvh);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_radiation(struct iotdata_decoder_t_ *const dec, uint16_t *cpm, iotdata_float_t *usvh);
#endif
typedef struct {
    iotdata_field_radiation_cpm_t radiation_cpm;
    iotdata_field_radiation_dose_t radiation_dose;
} iotdata_field_radiation_t;
#define IOTDATA_FIELDS_MEM_RADIATION    iotdata_field_radiation_t radiation;
#define IOTDATA_FIELD_ID_RADIATION      0x0068u
#define IOTDATA_FIELDS_DEF_RADIATION(F) F(RADIATION)
#define IOTDATA_FIELDS_OPS_RADIATION(E) E(RADIATION, _iotdata_field_def_radiation)
#else
#define IOTDATA_FIELDS_MEM_RADIATION
#define IOTDATA_FIELDS_DEF_RADIATION(F)
#define IOTDATA_FIELDS_OPS_RADIATION(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_RADIATION(S)
#define IOTDATA_FIELDS_ERR_STRINGS_RADIATION(D)
#define IOTDATA_FIELDS_DAT_RADIATION IOTDATA_FIELDS_DAT_RADIATION_CPM IOTDATA_FIELDS_DAT_RADIATION_DOSE

/* ---------------------------------------------------------------------------
 * Field DEPTH
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_DEPTH)
#define IOTDATA_FIELDS_DAT_DEPTH uint16_t depth;
typedef struct {
    IOTDATA_FIELDS_DAT_DEPTH
} iotdata_field_depth_t;
#define IOTDATA_DEPTH_MAX  1023
#define IOTDATA_DEPTH_BITS 10
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_depth(struct iotdata_encoder_t_ *const enc, uint16_t depth_cm);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_depth_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint16_t depth_cm);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_depth(struct iotdata_decoder_t_ *const dec, uint16_t *depth_cm);
#endif
#define IOTDATA_FIELDS_MEM_DEPTH            iotdata_field_depth_t depth;
#define IOTDATA_FIELD_ID_DEPTH              0x0070u
#define IOTDATA_FIELDS_DEF_DEPTH(F)         F(DEPTH)
#define IOTDATA_FIELDS_OPS_DEPTH(E)         E(DEPTH, _iotdata_field_def_depth)
#define IOTDATA_FIELDS_ERR_IDENTS_DEPTH(S)  S(IOTDATA_ERR_DEPTH_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_DEPTH(D) D(IOTDATA_ERR_DEPTH_HIGH, "Depth above 1023 cm")
#else
#define IOTDATA_FIELDS_DAT_DEPTH
#define IOTDATA_FIELDS_MEM_DEPTH
#define IOTDATA_FIELDS_DEF_DEPTH(F)
#define IOTDATA_FIELDS_OPS_DEPTH(E)
#define IOTDATA_FIELDS_ERR_IDENTS_DEPTH(S)
#define IOTDATA_FIELDS_ERR_STRINGS_DEPTH(D)
#endif

/* ---------------------------------------------------------------------------
 * Field POSITION (LATITUDE, LONGITUDE)
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_POSITION)
#define IOTDATA_POSITION_LAT_OFFSET   90.0
#define IOTDATA_POSITION_LAT_RANGE    180.0
#define IOTDATA_POSITION_LON_OFFSET   180.0
#define IOTDATA_POSITION_LON_RANGE    360.0
#define IOTDATA_POSITION_LAT_OFFSET_I INT64_C(900000000)
#define IOTDATA_POSITION_LAT_RANGE_I  INT64_C(1800000000)
#define IOTDATA_POSITION_LON_OFFSET_I INT64_C(1800000000)
#define IOTDATA_POSITION_LON_RANGE_I  INT64_C(3600000000)
#define IOTDATA_FIELDS_DAT_POSITION \
    iotdata_double_t position_lat; \
    iotdata_double_t position_lon;
typedef struct {
    IOTDATA_FIELDS_DAT_POSITION
} iotdata_field_position_t;
#if !defined(IOTDATA_NO_FLOATING)
#define IOTDATA_POSITION_LAT_LOW  (-90.0)
#define IOTDATA_POSITION_LAT_HIGH (90.0)
#define IOTDATA_POSITION_LON_LOW  (-180.0)
#define IOTDATA_POSITION_LON_HIGH (180.0)
#else
#define IOTDATA_POSITION_LAT_LOW  (-900000000)
#define IOTDATA_POSITION_LAT_HIGH (900000000)
#define IOTDATA_POSITION_LON_LOW  (-1800000000)
#define IOTDATA_POSITION_LON_HIGH (1800000000)
#endif
#define IOTDATA_POSITION_LAT_BITS 24
#define IOTDATA_POSITION_LON_BITS 24
#define IOTDATA_POSITION_SCALE    16777215 /* (1 << 24) - 1 */
#define IOTDATA_POSITION_BITS     (IOTDATA_POSITION_LAT_BITS + IOTDATA_POSITION_LON_BITS)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_position(struct iotdata_encoder_t_ *const enc, iotdata_double_t latitude, iotdata_double_t longitude);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_position_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, iotdata_double_t latitude, iotdata_double_t longitude);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_position(struct iotdata_decoder_t_ *const dec, iotdata_double_t *latitude, iotdata_double_t *longitude);
#endif
#define IOTDATA_FIELDS_MEM_POSITION    iotdata_field_position_t position;
#define IOTDATA_FIELD_ID_POSITION      0x0080u
#define IOTDATA_FIELDS_DEF_POSITION(F) F(POSITION)
#define IOTDATA_FIELDS_OPS_POSITION(E) E(POSITION, _iotdata_field_def_position)
#define IOTDATA_FIELDS_ERR_IDENTS_POSITION(S) \
    S(IOTDATA_ERR_POSITION_LAT_LOW) \
    S(IOTDATA_ERR_POSITION_LAT_HIGH) \
    S(IOTDATA_ERR_POSITION_LON_LOW) \
    S(IOTDATA_ERR_POSITION_LON_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_POSITION(D) \
    D(IOTDATA_ERR_POSITION_LAT_LOW, "Latitude below -90") \
    D(IOTDATA_ERR_POSITION_LAT_HIGH, "Latitude above +90") \
    D(IOTDATA_ERR_POSITION_LON_LOW, "Longitude below -180") \
    D(IOTDATA_ERR_POSITION_LON_HIGH, "Longitude above +180")
#else
#define IOTDATA_FIELDS_DAT_POSITION
#define IOTDATA_FIELDS_MEM_POSITION
#define IOTDATA_FIELDS_DEF_POSITION(F)
#define IOTDATA_FIELDS_OPS_POSITION(E)
#define IOTDATA_FIELDS_ERR_IDENTS_POSITION(S)
#define IOTDATA_FIELDS_ERR_STRINGS_POSITION(D)
#endif

/* ---------------------------------------------------------------------------
 * Field DATETIME
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_DATETIME)
#define IOTDATA_FIELDS_DAT_DATETIME uint32_t datetime_secs;
typedef struct {
    IOTDATA_FIELDS_DAT_DATETIME
} iotdata_field_datetime_t;
#define IOTDATA_DATETIME_BITS 24
#define IOTDATA_DATETIME_RES  5
#define IOTDATA_DATETIME_MAX  (((uint32_t)1 << IOTDATA_DATETIME_BITS) - 1)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_datetime(struct iotdata_encoder_t_ *const enc, uint32_t seconds_from_year_start);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_datetime_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint32_t seconds_from_year_start);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_datetime(struct iotdata_decoder_t_ *const dec, uint32_t *seconds_from_year_start);
#endif
#define IOTDATA_FIELDS_MEM_DATETIME            iotdata_field_datetime_t datetime;
#define IOTDATA_FIELD_ID_DATETIME              0x0081u
#define IOTDATA_FIELDS_DEF_DATETIME(F)         F(DATETIME)
#define IOTDATA_FIELDS_OPS_DATETIME(E)         E(DATETIME, _iotdata_field_def_datetime)
#define IOTDATA_FIELDS_ERR_IDENTS_DATETIME(S)  S(IOTDATA_ERR_DATETIME_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_DATETIME(D) D(IOTDATA_ERR_DATETIME_HIGH, "Datetime ticks above maximum")
#else
#define IOTDATA_FIELDS_DAT_DATETIME
#define IOTDATA_FIELDS_MEM_DATETIME
#define IOTDATA_FIELDS_DEF_DATETIME(F)
#define IOTDATA_FIELDS_OPS_DATETIME(E)
#define IOTDATA_FIELDS_ERR_IDENTS_DATETIME(S)
#define IOTDATA_FIELDS_ERR_STRINGS_DATETIME(D)
#endif

static inline uint32_t iotdata_datetime_convert_utc_to_seconds_from_year_start(const int64_t utc) {
    if (utc <= 0)
        return 0;
    const int32_t d = (int32_t)(utc / 86400), r = (int32_t)(utc % 86400);
    int32_t y = (int32_t)((INT64_C(10000) * (int64_t)d + 14780) / 3652425) + 1970;
    int32_t j = y * 365 + y / 4 - y / 100 + y / 400 - 719527;
    if (j > d) {
        y--;
        j = y * 365 + y / 4 - y / 100 + y / 400 - 719527;
    }
    return (uint32_t)((d - j) * 86400 + r);
}

/* ---------------------------------------------------------------------------
 * Field FLAGS
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_FLAGS)
#define IOTDATA_FIELDS_DAT_FLAGS uint8_t flags;
typedef struct {
    IOTDATA_FIELDS_DAT_FLAGS
} iotdata_field_flags_t;
#define IOTDATA_FLAGS_BITS 8
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_flags(struct iotdata_encoder_t_ *const enc, uint8_t flags);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_flags_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint8_t flags);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_flags(struct iotdata_decoder_t_ *const dec, uint8_t *flags);
#endif
#define IOTDATA_FIELDS_MEM_FLAGS    iotdata_field_flags_t flags;
#define IOTDATA_FIELD_ID_FLAGS      0x00A0u
#define IOTDATA_FIELDS_DEF_FLAGS(F) F(FLAGS)
#define IOTDATA_FIELDS_OPS_FLAGS(E) E(FLAGS, _iotdata_field_def_flags)
#else
#define IOTDATA_FIELDS_DAT_FLAGS
#define IOTDATA_FIELDS_MEM_FLAGS
#define IOTDATA_FIELDS_DEF_FLAGS(F)
#define IOTDATA_FIELDS_OPS_FLAGS(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_FLAGS(S)
#define IOTDATA_FIELDS_ERR_STRINGS_FLAGS(D)

/* ---------------------------------------------------------------------------
 * Field BITS32
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_BITS32)
#define IOTDATA_FIELDS_DAT_BITS32 uint32_t bits32;
typedef struct {
    IOTDATA_FIELDS_DAT_BITS32
} iotdata_field_bits32_t;
#define IOTDATA_BITS32_BITS 32
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_bits32(struct iotdata_encoder_t_ *const enc, uint32_t bits32);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_bits32_by_slot(struct iotdata_encoder_t_ *const enc, const int slot, uint32_t bits32);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_bits32(struct iotdata_decoder_t_ *const dec, uint32_t *bits32);
#endif
#define IOTDATA_FIELDS_MEM_BITS32    iotdata_field_bits32_t bits32;
#define IOTDATA_FIELD_ID_BITS32      0x00A1u
#define IOTDATA_FIELDS_DEF_BITS32(F) F(BITS32)
#define IOTDATA_FIELDS_OPS_BITS32(E) E(BITS32, _iotdata_field_def_bits32)
#else
#define IOTDATA_FIELDS_DAT_BITS32
#define IOTDATA_FIELDS_MEM_BITS32
#define IOTDATA_FIELDS_DEF_BITS32(F)
#define IOTDATA_FIELDS_OPS_BITS32(E)
#endif
#define IOTDATA_FIELDS_ERR_IDENTS_BITS32(S)
#define IOTDATA_FIELDS_ERR_STRINGS_BITS32(D)

/* ---------------------------------------------------------------------------
 * Master
 * -------------------------------------------------------------------------*/

#define IOTDATA_FIELDS_SELECTED(F) \
    IOTDATA_FIELDS_DEF_BATTERY_LEVEL(F) \
    IOTDATA_FIELDS_DEF_LINK_RSSI(F) \
    IOTDATA_FIELDS_DEF_BATTERY_CHARGE(F) \
    IOTDATA_FIELDS_DEF_LINK_SNR(F) \
    IOTDATA_FIELDS_DEF_BATTERY(F) \
    IOTDATA_FIELDS_DEF_LINK(F) \
    IOTDATA_FIELDS_DEF_TEMPERATURE(F) \
    IOTDATA_FIELDS_DEF_PRESSURE(F) \
    IOTDATA_FIELDS_DEF_HUMIDITY(F) \
    IOTDATA_FIELDS_DEF_ENVIRONMENT(F) \
    IOTDATA_FIELDS_DEF_WIND_SPEED(F) \
    IOTDATA_FIELDS_DEF_WIND_DIRECTION(F) \
    IOTDATA_FIELDS_DEF_WIND_GUST(F) \
    IOTDATA_FIELDS_DEF_WIND(F) \
    IOTDATA_FIELDS_DEF_RAIN_RATE(F) \
    IOTDATA_FIELDS_DEF_RAIN_SIZE(F) \
    IOTDATA_FIELDS_DEF_RAIN(F) \
    IOTDATA_FIELDS_DEF_SOLAR_IRRADIANCE(F) \
    IOTDATA_FIELDS_DEF_CLOUDS(F) \
    IOTDATA_FIELDS_DEF_SOLAR_ULTRAVIOLET(F) \
    IOTDATA_FIELDS_DEF_SOLAR(F) \
    IOTDATA_FIELDS_DEF_AIR_QUALITY_INDEX(F) \
    IOTDATA_FIELDS_DEF_AIR_QUALITY_PM(F) \
    IOTDATA_FIELDS_DEF_AIR_QUALITY_GAS(F) \
    IOTDATA_FIELDS_DEF_AIR_QUALITY(F) \
    IOTDATA_FIELDS_DEF_RADIATION_CPM(F) \
    IOTDATA_FIELDS_DEF_RADIATION_DOSE(F) \
    IOTDATA_FIELDS_DEF_RADIATION(F) \
    IOTDATA_FIELDS_DEF_DEPTH(F) \
    IOTDATA_FIELDS_DEF_POSITION(F) \
    IOTDATA_FIELDS_DEF_DATETIME(F) \
    IOTDATA_FIELDS_DEF_IMAGE(F) \
    IOTDATA_FIELDS_DEF_FLAGS(F) \
    IOTDATA_FIELDS_DEF_BITS32(F)

#define IOTDATA_FIELDS_RESERVED(F) IOTDATA_FIELDS_DEF_TLV(F)

#define IOTDATA_FIELDS_MEM_ENCODER_FIELDS \
    IOTDATA_FIELDS_MEM_BATTERY_LEVEL \
    IOTDATA_FIELDS_MEM_LINK_RSSI \
    IOTDATA_FIELDS_MEM_BATTERY_CHARGE \
    IOTDATA_FIELDS_MEM_LINK_SNR \
    IOTDATA_FIELDS_MEM_BATTERY \
    IOTDATA_FIELDS_MEM_LINK \
    IOTDATA_FIELDS_MEM_TEMPERATURE \
    IOTDATA_FIELDS_MEM_PRESSURE \
    IOTDATA_FIELDS_MEM_HUMIDITY \
    IOTDATA_FIELDS_MEM_ENVIRONMENT \
    IOTDATA_FIELDS_MEM_WIND_SPEED \
    IOTDATA_FIELDS_MEM_WIND_DIRECTION \
    IOTDATA_FIELDS_MEM_WIND_GUST \
    IOTDATA_FIELDS_MEM_WIND \
    IOTDATA_FIELDS_MEM_RAIN_RATE \
    IOTDATA_FIELDS_MEM_RAIN_SIZE \
    IOTDATA_FIELDS_MEM_RAIN \
    IOTDATA_FIELDS_MEM_SOLAR_IRRADIANCE \
    IOTDATA_FIELDS_MEM_CLOUDS \
    IOTDATA_FIELDS_MEM_SOLAR_ULTRAVIOLET \
    IOTDATA_FIELDS_MEM_SOLAR \
    IOTDATA_FIELDS_MEM_AIR_QUALITY_INDEX \
    IOTDATA_FIELDS_MEM_AIR_QUALITY_PM \
    IOTDATA_FIELDS_MEM_AIR_QUALITY_GAS \
    IOTDATA_FIELDS_MEM_AIR_QUALITY \
    IOTDATA_FIELDS_MEM_RADIATION_CPM \
    IOTDATA_FIELDS_MEM_RADIATION_DOSE \
    IOTDATA_FIELDS_MEM_RADIATION \
    IOTDATA_FIELDS_MEM_DEPTH \
    IOTDATA_FIELDS_MEM_POSITION \
    IOTDATA_FIELDS_MEM_DATETIME \
    IOTDATA_FIELDS_MEM_IMAGE_ENCODE \
    IOTDATA_FIELDS_MEM_FLAGS \
    IOTDATA_FIELDS_MEM_BITS32

#if defined(IOTDATA_OPERATE_ORDERED)
#define IOTDATA_FIELDS_MEM_ENCODER IOTDATA_FIELDS_DAT_TLV_ENCODE
#else
#define IOTDATA_FIELDS_MEM_ENCODER \
    IOTDATA_FIELDS_MEM_ENCODER_FIELDS \
    IOTDATA_FIELDS_DAT_TLV_ENCODE
#endif

#define IOTDATA_FIELDS_MEM_DECODER_FIELDS \
    IOTDATA_FIELDS_MEM_BATTERY_LEVEL \
    IOTDATA_FIELDS_MEM_LINK_RSSI \
    IOTDATA_FIELDS_MEM_BATTERY_CHARGE \
    IOTDATA_FIELDS_MEM_LINK_SNR \
    IOTDATA_FIELDS_MEM_BATTERY \
    IOTDATA_FIELDS_MEM_LINK \
    IOTDATA_FIELDS_MEM_TEMPERATURE \
    IOTDATA_FIELDS_MEM_PRESSURE \
    IOTDATA_FIELDS_MEM_HUMIDITY \
    IOTDATA_FIELDS_MEM_ENVIRONMENT \
    IOTDATA_FIELDS_MEM_WIND_SPEED \
    IOTDATA_FIELDS_MEM_WIND_DIRECTION \
    IOTDATA_FIELDS_MEM_WIND_GUST \
    IOTDATA_FIELDS_MEM_WIND \
    IOTDATA_FIELDS_MEM_RAIN_RATE \
    IOTDATA_FIELDS_MEM_RAIN_SIZE \
    IOTDATA_FIELDS_MEM_RAIN \
    IOTDATA_FIELDS_MEM_SOLAR_IRRADIANCE \
    IOTDATA_FIELDS_MEM_CLOUDS \
    IOTDATA_FIELDS_MEM_SOLAR_ULTRAVIOLET \
    IOTDATA_FIELDS_MEM_SOLAR \
    IOTDATA_FIELDS_MEM_AIR_QUALITY_INDEX \
    IOTDATA_FIELDS_MEM_AIR_QUALITY_PM \
    IOTDATA_FIELDS_MEM_AIR_QUALITY_GAS \
    IOTDATA_FIELDS_MEM_AIR_QUALITY \
    IOTDATA_FIELDS_MEM_RADIATION_CPM \
    IOTDATA_FIELDS_MEM_RADIATION_DOSE \
    IOTDATA_FIELDS_MEM_RADIATION \
    IOTDATA_FIELDS_MEM_DEPTH \
    IOTDATA_FIELDS_MEM_POSITION \
    IOTDATA_FIELDS_MEM_DATETIME \
    IOTDATA_FIELDS_MEM_IMAGE_DECODE \
    IOTDATA_FIELDS_MEM_FLAGS \
    IOTDATA_FIELDS_MEM_BITS32

#if defined(IOTDATA_OPERATE_ORDERED)
#define IOTDATA_FIELDS_MEM_DECODER IOTDATA_FIELDS_DAT_TLV_DECODE
#else
#define IOTDATA_FIELDS_MEM_DECODER \
    IOTDATA_FIELDS_MEM_DECODER_FIELDS \
    IOTDATA_FIELDS_DAT_TLV_DECODE
#endif

#define IOTDATA_FIELDS_OPS(E) \
    IOTDATA_FIELDS_OPS_BATTERY_LEVEL(E) \
    IOTDATA_FIELDS_OPS_LINK_RSSI(E) \
    IOTDATA_FIELDS_OPS_BATTERY_CHARGE(E) \
    IOTDATA_FIELDS_OPS_LINK_SNR(E) \
    IOTDATA_FIELDS_OPS_BATTERY(E) \
    IOTDATA_FIELDS_OPS_LINK(E) \
    IOTDATA_FIELDS_OPS_TEMPERATURE(E) \
    IOTDATA_FIELDS_OPS_PRESSURE(E) \
    IOTDATA_FIELDS_OPS_HUMIDITY(E) \
    IOTDATA_FIELDS_OPS_ENVIRONMENT(E) \
    IOTDATA_FIELDS_OPS_WIND_SPEED(E) \
    IOTDATA_FIELDS_OPS_WIND_DIRECTION(E) \
    IOTDATA_FIELDS_OPS_WIND_GUST(E) \
    IOTDATA_FIELDS_OPS_WIND(E) \
    IOTDATA_FIELDS_OPS_RAIN_RATE(E) \
    IOTDATA_FIELDS_OPS_RAIN_SIZE(E) \
    IOTDATA_FIELDS_OPS_RAIN(E) \
    IOTDATA_FIELDS_OPS_SOLAR_IRRADIANCE(E) \
    IOTDATA_FIELDS_OPS_CLOUDS(E) \
    IOTDATA_FIELDS_OPS_SOLAR_ULTRAVIOLET(E) \
    IOTDATA_FIELDS_OPS_SOLAR(E) \
    IOTDATA_FIELDS_OPS_AIR_QUALITY_INDEX(E) \
    IOTDATA_FIELDS_OPS_AIR_QUALITY_PM(E) \
    IOTDATA_FIELDS_OPS_AIR_QUALITY_GAS(E) \
    IOTDATA_FIELDS_OPS_AIR_QUALITY(E) \
    IOTDATA_FIELDS_OPS_RADIATION_CPM(E) \
    IOTDATA_FIELDS_OPS_RADIATION_DOSE(E) \
    IOTDATA_FIELDS_OPS_RADIATION(E) \
    IOTDATA_FIELDS_OPS_DEPTH(E) \
    IOTDATA_FIELDS_OPS_POSITION(E) \
    IOTDATA_FIELDS_OPS_DATETIME(E) \
    IOTDATA_FIELDS_OPS_IMAGE(E) \
    IOTDATA_FIELDS_OPS_FLAGS(E) \
    IOTDATA_FIELDS_OPS_BITS32(E)

#define IOTDATA_FIELDS_ERR_IDENTS(S) \
    IOTDATA_FIELDS_ERR_IDENTS_TLV(S) \
    IOTDATA_FIELDS_ERR_IDENTS_BATTERY_LEVEL(S) \
    IOTDATA_FIELDS_ERR_IDENTS_LINK_RSSI(S) \
    IOTDATA_FIELDS_ERR_IDENTS_BATTERY_CHARGE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_LINK_SNR(S) \
    IOTDATA_FIELDS_ERR_IDENTS_BATTERY(S) \
    IOTDATA_FIELDS_ERR_IDENTS_LINK(S) \
    IOTDATA_FIELDS_ERR_IDENTS_TEMPERATURE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_PRESSURE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_HUMIDITY(S) \
    IOTDATA_FIELDS_ERR_IDENTS_ENVIRONMENT(S) \
    IOTDATA_FIELDS_ERR_IDENTS_WIND_SPEED(S) \
    IOTDATA_FIELDS_ERR_IDENTS_WIND_DIRECTION(S) \
    IOTDATA_FIELDS_ERR_IDENTS_WIND_GUST(S) \
    IOTDATA_FIELDS_ERR_IDENTS_WIND(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RAIN_RATE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RAIN_SIZE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RAIN(S) \
    IOTDATA_FIELDS_ERR_IDENTS_SOLAR_IRRADIANCE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_CLOUDS(S) \
    IOTDATA_FIELDS_ERR_IDENTS_SOLAR_ULTRAVIOLET(S) \
    IOTDATA_FIELDS_ERR_IDENTS_SOLAR(S) \
    IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_INDEX(S) \
    IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_PM(S) \
    IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_GAS(S) \
    IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RADIATION_CPM(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RADIATION_DOSE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RADIATION(S) \
    IOTDATA_FIELDS_ERR_IDENTS_DEPTH(S) \
    IOTDATA_FIELDS_ERR_IDENTS_POSITION(S) \
    IOTDATA_FIELDS_ERR_IDENTS_DATETIME(S) \
    IOTDATA_FIELDS_ERR_IDENTS_IMAGE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_FLAGS(S) \
    IOTDATA_FIELDS_ERR_IDENTS_BITS32(S)

#define IOTDATA_FIELDS_ERR_STRINGS(D) \
    IOTDATA_FIELDS_ERR_STRINGS_TLV(D) \
    IOTDATA_FIELDS_ERR_STRINGS_BATTERY_LEVEL(D) \
    IOTDATA_FIELDS_ERR_STRINGS_LINK_RSSI(D) \
    IOTDATA_FIELDS_ERR_STRINGS_BATTERY_CHARGE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_LINK_SNR(D) \
    IOTDATA_FIELDS_ERR_STRINGS_BATTERY(D) \
    IOTDATA_FIELDS_ERR_STRINGS_LINK(D) \
    IOTDATA_FIELDS_ERR_STRINGS_TEMPERATURE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_PRESSURE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_HUMIDITY(D) \
    IOTDATA_FIELDS_ERR_STRINGS_ENVIRONMENT(D) \
    IOTDATA_FIELDS_ERR_STRINGS_WIND_SPEED(D) \
    IOTDATA_FIELDS_ERR_STRINGS_WIND_DIRECTION(D) \
    IOTDATA_FIELDS_ERR_STRINGS_WIND_GUST(D) \
    IOTDATA_FIELDS_ERR_STRINGS_WIND(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RAIN_RATE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RAIN_SIZE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RAIN(D) \
    IOTDATA_FIELDS_ERR_STRINGS_SOLAR_IRRADIANCE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_CLOUDS(D) \
    IOTDATA_FIELDS_ERR_STRINGS_SOLAR_ULTRAVIOLET(D) \
    IOTDATA_FIELDS_ERR_STRINGS_SOLAR(D) \
    IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_INDEX(D) \
    IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_PM(D) \
    IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_GAS(D) \
    IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RADIATION_CPM(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RADIATION_DOSE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RADIATION(D) \
    IOTDATA_FIELDS_ERR_STRINGS_DEPTH(D) \
    IOTDATA_FIELDS_ERR_STRINGS_POSITION(D) \
    IOTDATA_FIELDS_ERR_STRINGS_DATETIME(D) \
    IOTDATA_FIELDS_ERR_STRINGS_IMAGE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_FLAGS(D) \
    IOTDATA_FIELDS_ERR_STRINGS_BITS32(D)

#define IOTDATA_FIELDS_ENCODE_FROM_JSON_SCRATCH \
    IOTDATA_IMAGE_ENCODE_FROM_JSON_SCRATCH \
    IOTDATA_TLV_ENCODE_FROM_JSON_SCRATCH

#define IOTDATA_FIELDS_DECODE_TO_JSON_SCRATCH \
    IOTDATA_IMAGE_DECODE_TO_JSON_SCRATCH \
    IOTDATA_TLV_DECODE_TO_JSON_SCRATCH

#if !defined(IOTDATA_NO_DECODE)
typedef union {
    uint32_t _any; /* so the union is never empty, however few fields are enabled */
    IOTDATA_FIELDS_MEM_DECODER_FIELDS
} iotdata_field_any_t;
#endif

/* ---------------------------------------------------------------------------
 * End
 * -------------------------------------------------------------------------*/

#endif /* IOTDATA_FIELDS_H_DECLARATIONS */

#if defined(IOTDATA_FIELDS_IMPLEMENTATION) && !defined(IOTDATA_FIELDS_H_DEFINITIONS)
#define IOTDATA_FIELDS_H_DEFINITIONS

/* ---------------------------------------------------------------------------
 * Definitions -- from generators/templates/fields.c.c.tmpl.
 * -------------------------------------------------------------------------*/

/* =========================================================================
 * Field BATTERY, BATTERY_LEVEL, BATTERY_CHARGE
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_BATTERY_LEVEL) || defined(IOTDATA_ENABLE_BATTERY)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_battery_level(uint8_t pct) {
    return (uint32_t)(((uint32_t)pct * ((1 << IOTDATA_BATTERY_LEVEL_BITS) - 1) + IOTDATA_BATTERY_LEVEL_MAX / 2) / IOTDATA_BATTERY_LEVEL_MAX);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint8_t dequantise_battery_level(uint32_t raw) {
    return (uint8_t)((raw * IOTDATA_BATTERY_LEVEL_MAX + ((1 << IOTDATA_BATTERY_LEVEL_BITS) - 1) / 2) / ((1 << IOTDATA_BATTERY_LEVEL_BITS) - 1));
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_battery_level(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_battery_level_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_battery_level(v->battery_level), IOTDATA_BATTERY_LEVEL_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_LEVEL) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_battery_level_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t level_percent) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_BATTERY_LEVEL, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (level_percent > IOTDATA_BATTERY_LEVEL_MAX)
        return IOTDATA_ERR_BATTERY_LEVEL_HIGH;
#endif
    iotdata_field_battery_level_t v;
    v.battery_level = level_percent;
    PACK_DIRECT(enc, pack_battery_level, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BATTERY_LEVEL);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_battery_level(iotdata_encoder_t *const enc, uint8_t level_percent) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BATTERY_LEVEL, si);
    return iotdata_encode_battery_level_by_slot(enc, si, level_percent);
}
#else
iotdata_status_t iotdata_encode_battery_level(iotdata_encoder_t *const enc, uint8_t level_percent) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BATTERY_LEVEL, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (level_percent > IOTDATA_BATTERY_LEVEL_MAX)
        return IOTDATA_ERR_BATTERY_LEVEL_HIGH;
#endif
    enc->battery_level.battery_level = level_percent;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BATTERY_LEVEL);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_battery_level(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_battery_level_t *const v = vv;
    if (*bp + IOTDATA_BATTERY_LEVEL_BITS > bb)
        return false;
    v->battery_level = dequantise_battery_level(bits_read(buf, bb, bp, IOTDATA_BATTERY_LEVEL_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_LEVEL) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_battery_level(iotdata_decoder_t *dec, uint8_t *level_percent) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_battery_level_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_BATTERY_LEVEL, unpack_battery_level, &v);
    if (level_percent)
        *level_percent = v.battery_level;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_BATTERY_LEVEL);
    if (level_percent)
        *level_percent = dec->battery_level.battery_level;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_LEVEL) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_battery_level(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_battery_level(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_battery_level(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_battery_level_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->battery_level);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_battery_level(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_BATTERY_LEVEL_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 "%%", dequantise_battery_level(r));
    n = dump_add(dump, n, s, IOTDATA_BATTERY_LEVEL_BITS, r, dump->_dec_buf, "0..100%, 5b quant", "battery_level");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_LEVEL) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_battery_level(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_battery_level_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 "%%\n", label, _padd(label), v->battery_level);
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_LEVEL)
static const iotdata_field_ops_t _iotdata_field_def_battery_level = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, battery_level)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, battery_level)) _IOTDATA_OP_NAME("battery_level")
                                                                          _IOTDATA_OP_BITS(IOTDATA_BATTERY_LEVEL_BITS) _IOTDATA_OP_PACK(pack_battery_level) _IOTDATA_OP_UNPACK(unpack_battery_level) _IOTDATA_OP_DUMP(dump_battery_level)
                                                                              _IOTDATA_OP_PRINT(print_battery_level) _IOTDATA_OP_JSON_SET(json_set_battery_level) _IOTDATA_OP_JSON_GET(json_get_battery_level) };
#endif
#endif
#if defined(IOTDATA_ENABLE_BATTERY_CHARGE) || defined(IOTDATA_ENABLE_BATTERY)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_battery_state(bool charging) {
    return (uint32_t)(charging ? 1 : 0);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static bool dequantise_battery_state(uint32_t raw) {
    return (bool)(raw & 1);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_battery_charge(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_battery_charge_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_battery_state(v->battery_charging), IOTDATA_BATTERY_CHARGE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_CHARGE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_battery_charge_by_slot(iotdata_encoder_t *const enc, const int slot, bool charging) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_BATTERY_CHARGE, si);
    iotdata_field_battery_charge_t v;
    v.battery_charging = charging;
    PACK_DIRECT(enc, pack_battery_charge, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BATTERY_CHARGE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_battery_charge(iotdata_encoder_t *const enc, bool charging) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BATTERY_CHARGE, si);
    return iotdata_encode_battery_charge_by_slot(enc, si, charging);
}
#else
iotdata_status_t iotdata_encode_battery_charge(iotdata_encoder_t *const enc, bool charging) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BATTERY_CHARGE, si);
    enc->battery_charge.battery_charging = charging;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BATTERY_CHARGE);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_battery_charge(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_battery_charge_t *const v = vv;
    if (*bp + IOTDATA_BATTERY_CHARGE_BITS > bb)
        return false;
    v->battery_charging = dequantise_battery_state(bits_read(buf, bb, bp, IOTDATA_BATTERY_CHARGE_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_CHARGE) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_battery_charge(iotdata_decoder_t *dec, bool *charging) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_battery_charge_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_BATTERY_CHARGE, unpack_battery_charge, &v);
    if (charging)
        *charging = v.battery_charging;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_BATTERY_CHARGE);
    if (charging)
        *charging = dec->battery_charge.battery_charging;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_CHARGE) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_battery_charge(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_battery_charge(enc, cJSON_IsTrue(j));
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_battery_charge(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_battery_charge_t *const v = vv;
    (void)scratch;
    cJSON_AddBoolToObject(root, label, v->battery_charging);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_battery_charge(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_BATTERY_CHARGE_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%s", dequantise_battery_state(r) ? "charging" : "discharging");
    n = dump_add(dump, n, s, IOTDATA_BATTERY_CHARGE_BITS, r, dump->_dec_buf, "0/1", "battery_state");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_CHARGE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_battery_charge(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_battery_charge_t *const v = vv;
    bprintf(bp, "  %s:%s %s\n", label, _padd(label), v->battery_charging ? "charging" : "discharging");
}
#endif
#if defined(IOTDATA_ENABLE_BATTERY_CHARGE)
static const iotdata_field_ops_t _iotdata_field_def_battery_charge = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, battery_charge)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, battery_charge)) _IOTDATA_OP_NAME("battery_charge")
                                                                           _IOTDATA_OP_BITS(IOTDATA_BATTERY_CHARGE_BITS) _IOTDATA_OP_PACK(pack_battery_charge) _IOTDATA_OP_UNPACK(unpack_battery_charge) _IOTDATA_OP_DUMP(dump_battery_charge)
                                                                               _IOTDATA_OP_PRINT(print_battery_charge) _IOTDATA_OP_JSON_SET(json_set_battery_charge) _IOTDATA_OP_JSON_GET(json_get_battery_charge) };
#endif
#endif
#if defined(IOTDATA_ENABLE_BATTERY)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_battery(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_battery_t *const v = vv;
    return pack_battery_level(buf, bb, bp, &v->battery_level) && pack_battery_charge(buf, bb, bp, &v->battery_charge);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_battery_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t level_percent, bool charging) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_BATTERY, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (level_percent > IOTDATA_BATTERY_LEVEL_MAX)
        return IOTDATA_ERR_BATTERY_LEVEL_HIGH;
#endif
    iotdata_field_battery_t v;
    v.battery_level.battery_level = level_percent;
    v.battery_charge.battery_charging = charging;
    PACK_DIRECT(enc, pack_battery, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BATTERY);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_battery(iotdata_encoder_t *const enc, uint8_t level_percent, bool charging) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BATTERY, si);
    return iotdata_encode_battery_by_slot(enc, si, level_percent, charging);
}
#else
iotdata_status_t iotdata_encode_battery(iotdata_encoder_t *const enc, uint8_t level_percent, bool charging) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BATTERY, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (level_percent > IOTDATA_BATTERY_LEVEL_MAX)
        return IOTDATA_ERR_BATTERY_LEVEL_HIGH;
#endif
    enc->battery.battery_level.battery_level = level_percent;
    enc->battery.battery_charge.battery_charging = charging;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BATTERY);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_battery(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_battery_t *const v = vv;
    return unpack_battery_level(buf, bb, bp, &v->battery_level) && unpack_battery_charge(buf, bb, bp, &v->battery_charge);
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_battery(iotdata_decoder_t *dec, uint8_t *level_percent, bool *charging) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_battery_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_BATTERY, unpack_battery, &v);
    if (level_percent)
        *level_percent = v.battery_level.battery_level;
    if (charging)
        *charging = v.battery_charge.battery_charging;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_BATTERY);
    if (level_percent)
        *level_percent = dec->battery.battery_level.battery_level;
    if (charging)
        *charging = dec->battery.battery_charge.battery_charging;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_battery(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    cJSON *j_level = cJSON_GetObjectItem(j, "level"), *j_state = cJSON_GetObjectItem(j, "state");
    if (!j_level)
        return IOTDATA_OK;
    return iotdata_encode_battery(enc, (uint8_t)j_level->valueint, cJSON_IsTrue(j_state));
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_battery(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_battery_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_battery_level(obj, &v->battery_level, "level", scratch);
    json_set_battery_charge(obj, &v->battery_charge, "state", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_battery(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    n = dump_battery_level(buf, bb, bp, dump, n, label);
    n = dump_battery_charge(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_battery(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_battery_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 "%% %s\n", label, _padd(label), v->battery_level.battery_level, v->battery_charge.battery_charging ? "(charging)" : "(discharging)");
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_battery = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, battery)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, battery)) _IOTDATA_OP_NAME("battery")
                                                                    _IOTDATA_OP_BITS(IOTDATA_BATTERY_LEVEL_BITS + IOTDATA_BATTERY_CHARGE_BITS) _IOTDATA_OP_PACK(pack_battery) _IOTDATA_OP_UNPACK(unpack_battery) _IOTDATA_OP_DUMP(dump_battery)
                                                                        _IOTDATA_OP_PRINT(print_battery) _IOTDATA_OP_JSON_SET(json_set_battery) _IOTDATA_OP_JSON_GET(json_get_battery) };
#endif

/* =========================================================================
 * Field LINK, LINK_RSSI, LINK_SNR
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_LINK_RSSI) || defined(IOTDATA_ENABLE_LINK)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_link_rssi(int8_t rssi) {
    return (uint32_t)(((rssi < IOTDATA_LINK_RSSI_MIN ? IOTDATA_LINK_RSSI_MIN : rssi > IOTDATA_LINK_RSSI_MAX ? IOTDATA_LINK_RSSI_MAX : rssi) - IOTDATA_LINK_RSSI_MIN) / IOTDATA_LINK_RSSI_STEP);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int8_t dequantise_link_rssi(uint32_t raw) {
    return (int8_t)(IOTDATA_LINK_RSSI_MIN + (int)raw * IOTDATA_LINK_RSSI_STEP);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_link_rssi(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_link_rssi_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_link_rssi(v->link_rssi), IOTDATA_LINK_RSSI_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_LINK_RSSI) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_link_rssi_by_slot(iotdata_encoder_t *const enc, const int slot, int8_t rssi_dbm) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_LINK_RSSI, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (rssi_dbm < IOTDATA_LINK_RSSI_MIN)
        return IOTDATA_ERR_LINK_RSSI_LOW;
    if (rssi_dbm > IOTDATA_LINK_RSSI_MAX)
        return IOTDATA_ERR_LINK_RSSI_HIGH;
#endif
    iotdata_field_link_rssi_t v;
    v.link_rssi = rssi_dbm;
    PACK_DIRECT(enc, pack_link_rssi, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_LINK_RSSI);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_link_rssi(iotdata_encoder_t *const enc, int8_t rssi_dbm) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_LINK_RSSI, si);
    return iotdata_encode_link_rssi_by_slot(enc, si, rssi_dbm);
}
#else
iotdata_status_t iotdata_encode_link_rssi(iotdata_encoder_t *const enc, int8_t rssi_dbm) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_LINK_RSSI, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (rssi_dbm < IOTDATA_LINK_RSSI_MIN)
        return IOTDATA_ERR_LINK_RSSI_LOW;
    if (rssi_dbm > IOTDATA_LINK_RSSI_MAX)
        return IOTDATA_ERR_LINK_RSSI_HIGH;
#endif
    enc->link_rssi.link_rssi = rssi_dbm;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_LINK_RSSI);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_link_rssi(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_link_rssi_t *const v = vv;
    if (*bp + IOTDATA_LINK_RSSI_BITS > bb)
        return false;
    v->link_rssi = dequantise_link_rssi(bits_read(buf, bb, bp, IOTDATA_LINK_RSSI_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_LINK_RSSI) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_link_rssi(iotdata_decoder_t *dec, int8_t *rssi_dbm) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_link_rssi_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_LINK_RSSI, unpack_link_rssi, &v);
    if (rssi_dbm)
        *rssi_dbm = v.link_rssi;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_LINK_RSSI);
    if (rssi_dbm)
        *rssi_dbm = dec->link_rssi.link_rssi;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_LINK_RSSI) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_link_rssi(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_link_rssi(enc, (int8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_link_rssi(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_link_rssi_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->link_rssi);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_link_rssi(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_LINK_RSSI_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIi8 " dBm", dequantise_link_rssi(r));
    n = dump_add(dump, n, s, IOTDATA_LINK_RSSI_BITS, r, dump->_dec_buf, "-120..-60, 4dBm", "link_rssi");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_LINK_RSSI) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_link_rssi(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_link_rssi_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIi8 " dBm RSSI\n", label, _padd(label), v->link_rssi);
}
#endif
#if defined(IOTDATA_ENABLE_LINK_RSSI)
static const iotdata_field_ops_t _iotdata_field_def_link_rssi = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, link_rssi)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, link_rssi)) _IOTDATA_OP_NAME("link_rssi")
                                                                      _IOTDATA_OP_BITS(IOTDATA_LINK_RSSI_BITS) _IOTDATA_OP_PACK(pack_link_rssi) _IOTDATA_OP_UNPACK(unpack_link_rssi) _IOTDATA_OP_DUMP(dump_link_rssi)
                                                                          _IOTDATA_OP_PRINT(print_link_rssi) _IOTDATA_OP_JSON_SET(json_set_link_rssi) _IOTDATA_OP_JSON_GET(json_get_link_rssi) };
#endif
#endif
#if defined(IOTDATA_ENABLE_LINK_SNR) || defined(IOTDATA_ENABLE_LINK)
#if !defined(IOTDATA_NO_FLOATING)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_link_snr(float snr) {
    return (uint32_t)roundf(((snr < IOTDATA_LINK_SNR_MIN ? IOTDATA_LINK_SNR_MIN : snr > IOTDATA_LINK_SNR_MAX ? IOTDATA_LINK_SNR_MAX : snr) - IOTDATA_LINK_SNR_MIN) / IOTDATA_LINK_SNR_STEP);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static float dequantise_link_snr(uint32_t raw) {
    return IOTDATA_LINK_SNR_MIN + (float)raw * IOTDATA_LINK_SNR_STEP;
}
#endif
#else
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_link_snr(int32_t snr10) {
    return (uint32_t)(((snr10 < IOTDATA_LINK_SNR_MIN ? IOTDATA_LINK_SNR_MIN : snr10 > IOTDATA_LINK_SNR_MAX ? IOTDATA_LINK_SNR_MAX : snr10) - IOTDATA_LINK_SNR_MIN + (IOTDATA_LINK_SNR_STEP / 2)) / IOTDATA_LINK_SNR_STEP);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int32_t dequantise_link_snr(uint32_t raw) {
    return IOTDATA_LINK_SNR_MIN + (int32_t)raw * IOTDATA_LINK_SNR_STEP;
}
#endif
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_link_snr(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_link_snr_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_link_snr(v->link_snr), IOTDATA_LINK_SNR_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_LINK_SNR) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_link_snr_by_slot(iotdata_encoder_t *const enc, const int slot, iotdata_float_t snr_db) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_LINK_SNR, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (snr_db < IOTDATA_LINK_SNR_MIN)
        return IOTDATA_ERR_LINK_SNR_LOW;
    if (snr_db > IOTDATA_LINK_SNR_MAX)
        return IOTDATA_ERR_LINK_SNR_HIGH;
#endif
    iotdata_field_link_snr_t v;
    v.link_snr = snr_db;
    PACK_DIRECT(enc, pack_link_snr, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_LINK_SNR);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_link_snr(iotdata_encoder_t *const enc, iotdata_float_t snr_db) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_LINK_SNR, si);
    return iotdata_encode_link_snr_by_slot(enc, si, snr_db);
}
#else
iotdata_status_t iotdata_encode_link_snr(iotdata_encoder_t *const enc, iotdata_float_t snr_db) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_LINK_SNR, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (snr_db < IOTDATA_LINK_SNR_MIN)
        return IOTDATA_ERR_LINK_SNR_LOW;
    if (snr_db > IOTDATA_LINK_SNR_MAX)
        return IOTDATA_ERR_LINK_SNR_HIGH;
#endif
    enc->link_snr.link_snr = snr_db;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_LINK_SNR);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_link_snr(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_link_snr_t *const v = vv;
    if (*bp + IOTDATA_LINK_SNR_BITS > bb)
        return false;
    v->link_snr = dequantise_link_snr(bits_read(buf, bb, bp, IOTDATA_LINK_SNR_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_LINK_SNR) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_link_snr(iotdata_decoder_t *dec, iotdata_float_t *snr_db) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_link_snr_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_LINK_SNR, unpack_link_snr, &v);
    if (snr_db)
        *snr_db = v.link_snr;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_LINK_SNR);
    if (snr_db)
        *snr_db = dec->link_snr.link_snr;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_LINK_SNR) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_link_snr(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_link_snr(enc, (iotdata_float_t)j->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_link_snr(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_link_snr_t *const v = vv;
    (void)scratch;
#if !defined(IOTDATA_NO_FLOATING)
    json_add_number_fixed(root, label, (double)v->link_snr, 0);
#else
    cJSON_AddNumberToObject(root, label, v->link_snr);
#endif
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_link_snr(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_LINK_SNR_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.0f dB", (double)dequantise_link_snr(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_link_snr(r), 10, "dB");
#endif
    n = dump_add(dump, n, s, IOTDATA_LINK_SNR_BITS, r, dump->_dec_buf, "-20..+10, 10dB", "link_snr");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_LINK_SNR) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_link_snr(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_link_snr_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.0f dB SNR\n", label, _padd(label), (double)v->link_snr);
#else
    bprintf(bp, "  %s:%s %" PRId32 ".%" PRId32 " dB SNR\n", label, _padd(label), v->link_snr / 10, v->link_snr % 10);
#endif
}
#endif
#if defined(IOTDATA_ENABLE_LINK_SNR)
static const iotdata_field_ops_t _iotdata_field_def_link_snr = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, link_snr)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, link_snr)) _IOTDATA_OP_NAME("link_snr")
                                                                     _IOTDATA_OP_BITS(IOTDATA_LINK_SNR_BITS) _IOTDATA_OP_PACK(pack_link_snr) _IOTDATA_OP_UNPACK(unpack_link_snr) _IOTDATA_OP_DUMP(dump_link_snr)
                                                                         _IOTDATA_OP_PRINT(print_link_snr) _IOTDATA_OP_JSON_SET(json_set_link_snr) _IOTDATA_OP_JSON_GET(json_get_link_snr) };
#endif
#endif
#if defined(IOTDATA_ENABLE_LINK)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_link(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_link_t *const v = vv;
    return pack_link_rssi(buf, bb, bp, &v->link_rssi) && pack_link_snr(buf, bb, bp, &v->link_snr);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_link_by_slot(iotdata_encoder_t *const enc, const int slot, int8_t rssi_dbm, iotdata_float_t snr_db) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_LINK, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (rssi_dbm < IOTDATA_LINK_RSSI_MIN)
        return IOTDATA_ERR_LINK_RSSI_LOW;
    if (rssi_dbm > IOTDATA_LINK_RSSI_MAX)
        return IOTDATA_ERR_LINK_RSSI_HIGH;
    if (snr_db < IOTDATA_LINK_SNR_MIN)
        return IOTDATA_ERR_LINK_SNR_LOW;
    if (snr_db > IOTDATA_LINK_SNR_MAX)
        return IOTDATA_ERR_LINK_SNR_HIGH;
#endif
    iotdata_field_link_t v;
    v.link_rssi.link_rssi = rssi_dbm;
    v.link_snr.link_snr = snr_db;
    PACK_DIRECT(enc, pack_link, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_LINK);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_link(iotdata_encoder_t *const enc, int8_t rssi_dbm, iotdata_float_t snr_db) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_LINK, si);
    return iotdata_encode_link_by_slot(enc, si, rssi_dbm, snr_db);
}
#else
iotdata_status_t iotdata_encode_link(iotdata_encoder_t *const enc, int8_t rssi_dbm, iotdata_float_t snr_db) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_LINK, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (rssi_dbm < IOTDATA_LINK_RSSI_MIN)
        return IOTDATA_ERR_LINK_RSSI_LOW;
    if (rssi_dbm > IOTDATA_LINK_RSSI_MAX)
        return IOTDATA_ERR_LINK_RSSI_HIGH;
    if (snr_db < IOTDATA_LINK_SNR_MIN)
        return IOTDATA_ERR_LINK_SNR_LOW;
    if (snr_db > IOTDATA_LINK_SNR_MAX)
        return IOTDATA_ERR_LINK_SNR_HIGH;
#endif
    enc->link.link_rssi.link_rssi = rssi_dbm;
    enc->link.link_snr.link_snr = snr_db;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_LINK);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_link(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_link_t *const v = vv;
    return unpack_link_rssi(buf, bb, bp, &v->link_rssi) && unpack_link_snr(buf, bb, bp, &v->link_snr);
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_link(iotdata_decoder_t *dec, int8_t *rssi_dbm, iotdata_float_t *snr_db) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_link_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_LINK, unpack_link, &v);
    if (rssi_dbm)
        *rssi_dbm = v.link_rssi.link_rssi;
    if (snr_db)
        *snr_db = v.link_snr.link_snr;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_LINK);
    if (rssi_dbm)
        *rssi_dbm = dec->link.link_rssi.link_rssi;
    if (snr_db)
        *snr_db = dec->link.link_snr.link_snr;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_link(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    cJSON *j_rssi = cJSON_GetObjectItem(j, "rssi"), *j_snr = cJSON_GetObjectItem(j, "snr");
    if (!j_rssi || !j_snr)
        return IOTDATA_OK;
    return iotdata_encode_link(enc, (int8_t)j_rssi->valueint, (iotdata_float_t)j_snr->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_link(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_link_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_link_rssi(obj, &v->link_rssi, "rssi", scratch);
    json_set_link_snr(obj, &v->link_snr, "snr", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_link(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    n = dump_link_rssi(buf, bb, bp, dump, n, label);
    n = dump_link_snr(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_link(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_link_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %" PRIi8 " dBm RSSI, %.0f dB SNR\n", label, _padd(label), v->link_rssi.link_rssi, (double)v->link_snr.link_snr);
#else
    bprintf(bp, "  %s:%s %" PRIi8 " dBm RSSI, %" PRId32 ".%" PRId32 " dB SNR\n", label, _padd(label), v->link_rssi.link_rssi, v->link_snr.link_snr / 10, v->link_snr.link_snr % 10);
#endif
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_link = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, link)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, link)) _IOTDATA_OP_NAME("link")
                                                                 _IOTDATA_OP_BITS(IOTDATA_LINK_RSSI_BITS + IOTDATA_LINK_SNR_BITS) _IOTDATA_OP_PACK(pack_link) _IOTDATA_OP_UNPACK(unpack_link) _IOTDATA_OP_DUMP(dump_link)
                                                                     _IOTDATA_OP_PRINT(print_link) _IOTDATA_OP_JSON_SET(json_set_link) _IOTDATA_OP_JSON_GET(json_get_link) };
#endif

/* =========================================================================
 * Field ENVIRONMENT, TEMPERATURE, PRESSURE, HUMIDITY
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_TEMPERATURE) || defined(IOTDATA_ENABLE_ENVIRONMENT)
#if !defined(IOTDATA_NO_FLOATING)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_temperature(float temperature) {
    return (uint32_t)roundf((temperature - IOTDATA_TEMPERATURE_MIN) / IOTDATA_TEMPERATURE_RES);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static float dequantise_temperature(uint32_t raw) {
    return IOTDATA_TEMPERATURE_MIN + (float)raw * IOTDATA_TEMPERATURE_RES;
}
#endif
#else
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_temperature(int32_t temperature100) {
    return (uint32_t)((temperature100 - IOTDATA_TEMPERATURE_MIN + (IOTDATA_TEMPERATURE_RES / 2)) / IOTDATA_TEMPERATURE_RES);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int32_t dequantise_temperature(uint32_t raw) {
    return (int32_t)raw * IOTDATA_TEMPERATURE_RES + IOTDATA_TEMPERATURE_MIN;
}
#endif
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_temperature(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_temperature_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_temperature(v->temperature), IOTDATA_TEMPERATURE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_TEMPERATURE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_temperature_by_slot(iotdata_encoder_t *const enc, const int slot, iotdata_float_t temperature_c) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_TEMPERATURE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (temperature_c < IOTDATA_TEMPERATURE_MIN)
        return IOTDATA_ERR_TEMPERATURE_LOW;
    if (temperature_c > IOTDATA_TEMPERATURE_MAX)
        return IOTDATA_ERR_TEMPERATURE_HIGH;
#endif
    iotdata_field_temperature_t v;
    v.temperature = temperature_c;
    PACK_DIRECT(enc, pack_temperature, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_TEMPERATURE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_temperature(iotdata_encoder_t *const enc, iotdata_float_t temperature_c) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_TEMPERATURE, si);
    return iotdata_encode_temperature_by_slot(enc, si, temperature_c);
}
#else
iotdata_status_t iotdata_encode_temperature(iotdata_encoder_t *const enc, iotdata_float_t temperature_c) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_TEMPERATURE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (temperature_c < IOTDATA_TEMPERATURE_MIN)
        return IOTDATA_ERR_TEMPERATURE_LOW;
    if (temperature_c > IOTDATA_TEMPERATURE_MAX)
        return IOTDATA_ERR_TEMPERATURE_HIGH;
#endif
    enc->temperature.temperature = temperature_c;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_TEMPERATURE);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_temperature(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_temperature_t *const v = vv;
    if (*bp + IOTDATA_TEMPERATURE_BITS > bb)
        return false;
    v->temperature = dequantise_temperature(bits_read(buf, bb, bp, IOTDATA_TEMPERATURE_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_TEMPERATURE) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_temperature(iotdata_decoder_t *dec, iotdata_float_t *temperature_c) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_temperature_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_TEMPERATURE, unpack_temperature, &v);
    if (temperature_c)
        *temperature_c = v.temperature;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_TEMPERATURE);
    if (temperature_c)
        *temperature_c = dec->temperature.temperature;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_TEMPERATURE) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_temperature(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_temperature(enc, (iotdata_float_t)j->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_temperature(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_temperature_t *const v = vv;
    (void)scratch;
#if !defined(IOTDATA_NO_FLOATING)
    json_add_number_fixed(root, label, (double)v->temperature, 2);
#else
    cJSON_AddNumberToObject(root, label, v->temperature);
#endif
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_temperature(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_TEMPERATURE_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.2f C", (double)dequantise_temperature(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_temperature(r), 100, "C");
#endif
    n = dump_add(dump, n, s, IOTDATA_TEMPERATURE_BITS, r, dump->_dec_buf, "-40..+80C, 0.25C", "temperature");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_TEMPERATURE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_temperature(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_temperature_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.2f C\n", label, _padd(label), (double)v->temperature);
#else
    const int32_t ta = v->temperature < 0 ? -(v->temperature) : v->temperature;
    bprintf(bp, "  %s:%s %s%" PRId32 ".%02" PRId32 " C\n", label, _padd(label), v->temperature < 0 ? "-" : "", ta / 100, ta % 100);
#endif
}
#endif
#if defined(IOTDATA_ENABLE_TEMPERATURE)
static const iotdata_field_ops_t _iotdata_field_def_temperature = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, temperature)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, temperature)) _IOTDATA_OP_NAME("temperature")
                                                                        _IOTDATA_OP_BITS(IOTDATA_TEMPERATURE_BITS) _IOTDATA_OP_PACK(pack_temperature) _IOTDATA_OP_UNPACK(unpack_temperature) _IOTDATA_OP_DUMP(dump_temperature)
                                                                            _IOTDATA_OP_PRINT(print_temperature) _IOTDATA_OP_JSON_SET(json_set_temperature) _IOTDATA_OP_JSON_GET(json_get_temperature) };
#endif
#endif

#if defined(IOTDATA_ENABLE_PRESSURE) || defined(IOTDATA_ENABLE_ENVIRONMENT)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_pressure(uint16_t pressure) {
    return (uint32_t)(pressure - IOTDATA_PRESSURE_MIN);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint16_t dequantise_pressure(uint32_t raw) {
    return (uint16_t)(raw + IOTDATA_PRESSURE_MIN);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_pressure(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_pressure_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_pressure(v->pressure), IOTDATA_PRESSURE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_PRESSURE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_pressure_by_slot(iotdata_encoder_t *const enc, const int slot, uint16_t pressure_hpa) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_PRESSURE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (pressure_hpa < IOTDATA_PRESSURE_MIN)
        return IOTDATA_ERR_PRESSURE_LOW;
    if (pressure_hpa > IOTDATA_PRESSURE_MAX)
        return IOTDATA_ERR_PRESSURE_HIGH;
#endif
    iotdata_field_pressure_t v;
    v.pressure = pressure_hpa;
    PACK_DIRECT(enc, pack_pressure, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_PRESSURE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_pressure(iotdata_encoder_t *const enc, uint16_t pressure_hpa) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_PRESSURE, si);
    return iotdata_encode_pressure_by_slot(enc, si, pressure_hpa);
}
#else
iotdata_status_t iotdata_encode_pressure(iotdata_encoder_t *const enc, uint16_t pressure_hpa) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_PRESSURE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (pressure_hpa < IOTDATA_PRESSURE_MIN)
        return IOTDATA_ERR_PRESSURE_LOW;
    if (pressure_hpa > IOTDATA_PRESSURE_MAX)
        return IOTDATA_ERR_PRESSURE_HIGH;
#endif
    enc->pressure.pressure = pressure_hpa;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_PRESSURE);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_pressure(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_pressure_t *const v = vv;
    if (*bp + IOTDATA_PRESSURE_BITS > bb)
        return false;
    v->pressure = dequantise_pressure(bits_read(buf, bb, bp, IOTDATA_PRESSURE_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_PRESSURE) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_pressure(iotdata_decoder_t *dec, uint16_t *pressure_hpa) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_pressure_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_PRESSURE, unpack_pressure, &v);
    if (pressure_hpa)
        *pressure_hpa = v.pressure;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_PRESSURE);
    if (pressure_hpa)
        *pressure_hpa = dec->pressure.pressure;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_PRESSURE) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_pressure(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_pressure(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_pressure(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_pressure_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->pressure);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_pressure(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_PRESSURE_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " hPa", dequantise_pressure(r));
    n = dump_add(dump, n, s, IOTDATA_PRESSURE_BITS, r, dump->_dec_buf, "850..1105 hPa", "pressure");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_PRESSURE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_pressure(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_pressure_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " hPa\n", label, _padd(label), v->pressure);
}
#endif
#if defined(IOTDATA_ENABLE_PRESSURE)
static const iotdata_field_ops_t _iotdata_field_def_pressure = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, pressure)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, pressure)) _IOTDATA_OP_NAME("pressure")
                                                                     _IOTDATA_OP_BITS(IOTDATA_PRESSURE_BITS) _IOTDATA_OP_PACK(pack_pressure) _IOTDATA_OP_UNPACK(unpack_pressure) _IOTDATA_OP_DUMP(dump_pressure)
                                                                         _IOTDATA_OP_PRINT(print_pressure) _IOTDATA_OP_JSON_SET(json_set_pressure) _IOTDATA_OP_JSON_GET(json_get_pressure) };
#endif
#endif

#if defined(IOTDATA_ENABLE_HUMIDITY) || defined(IOTDATA_ENABLE_ENVIRONMENT)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_humidity(uint8_t humidity) {
    return (uint32_t)humidity;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint8_t dequantise_humidity(uint32_t raw) {
    return (uint8_t)raw;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_humidity(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_humidity_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_humidity(v->humidity), IOTDATA_HUMIDITY_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_HUMIDITY) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_humidity_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_HUMIDITY, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (humidity_pct > IOTDATA_HUMIDITY_MAX)
        return IOTDATA_ERR_HUMIDITY_HIGH;
#endif
    iotdata_field_humidity_t v;
    v.humidity = humidity_pct;
    PACK_DIRECT(enc, pack_humidity, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_HUMIDITY);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_humidity(iotdata_encoder_t *const enc, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_HUMIDITY, si);
    return iotdata_encode_humidity_by_slot(enc, si, humidity_pct);
}
#else
iotdata_status_t iotdata_encode_humidity(iotdata_encoder_t *const enc, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_HUMIDITY, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (humidity_pct > IOTDATA_HUMIDITY_MAX)
        return IOTDATA_ERR_HUMIDITY_HIGH;
#endif
    enc->humidity.humidity = humidity_pct;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_HUMIDITY);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_humidity(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_humidity_t *const v = vv;
    if (*bp + IOTDATA_HUMIDITY_BITS > bb)
        return false;
    v->humidity = dequantise_humidity(bits_read(buf, bb, bp, IOTDATA_HUMIDITY_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_HUMIDITY) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_humidity(iotdata_decoder_t *dec, uint8_t *humidity_pct) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_humidity_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_HUMIDITY, unpack_humidity, &v);
    if (humidity_pct)
        *humidity_pct = v.humidity;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_HUMIDITY);
    if (humidity_pct)
        *humidity_pct = dec->humidity.humidity;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_HUMIDITY) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_humidity(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_humidity(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_humidity(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_humidity_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->humidity);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_humidity(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_HUMIDITY_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 "%%", dequantise_humidity(r));
    n = dump_add(dump, n, s, IOTDATA_HUMIDITY_BITS, r, dump->_dec_buf, "0..100%", "humidity");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_HUMIDITY) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_humidity(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_humidity_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 "%%\n", label, _padd(label), v->humidity);
}
#endif
#if defined(IOTDATA_ENABLE_HUMIDITY)
static const iotdata_field_ops_t _iotdata_field_def_humidity = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, humidity)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, humidity)) _IOTDATA_OP_NAME("humidity")
                                                                     _IOTDATA_OP_BITS(IOTDATA_HUMIDITY_BITS) _IOTDATA_OP_PACK(pack_humidity) _IOTDATA_OP_UNPACK(unpack_humidity) _IOTDATA_OP_DUMP(dump_humidity)
                                                                         _IOTDATA_OP_PRINT(print_humidity) _IOTDATA_OP_JSON_SET(json_set_humidity) _IOTDATA_OP_JSON_GET(json_get_humidity) };
#endif
#endif

#if defined(IOTDATA_ENABLE_ENVIRONMENT)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_environment(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_environment_t *const v = vv;
    return pack_temperature(buf, bb, bp, &v->temperature) && pack_pressure(buf, bb, bp, &v->pressure) && pack_humidity(buf, bb, bp, &v->humidity);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_environment_by_slot(iotdata_encoder_t *const enc, const int slot, iotdata_float_t temp_c, uint16_t pressure_hpa, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_ENVIRONMENT, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (temp_c < IOTDATA_TEMPERATURE_MIN)
        return IOTDATA_ERR_TEMPERATURE_LOW;
    if (temp_c > IOTDATA_TEMPERATURE_MAX)
        return IOTDATA_ERR_TEMPERATURE_HIGH;
    if (pressure_hpa < IOTDATA_PRESSURE_MIN)
        return IOTDATA_ERR_PRESSURE_LOW;
    if (pressure_hpa > IOTDATA_PRESSURE_MAX)
        return IOTDATA_ERR_PRESSURE_HIGH;
    if (humidity_pct > IOTDATA_HUMIDITY_MAX)
        return IOTDATA_ERR_HUMIDITY_HIGH;
#endif
    iotdata_field_environment_t v;
    v.temperature.temperature = temp_c;
    v.pressure.pressure = pressure_hpa;
    v.humidity.humidity = humidity_pct;
    PACK_DIRECT(enc, pack_environment, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_ENVIRONMENT);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_environment(iotdata_encoder_t *const enc, iotdata_float_t temp_c, uint16_t pressure_hpa, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_ENVIRONMENT, si);
    return iotdata_encode_environment_by_slot(enc, si, temp_c, pressure_hpa, humidity_pct);
}
#else
iotdata_status_t iotdata_encode_environment(iotdata_encoder_t *const enc, iotdata_float_t temp_c, uint16_t pressure_hpa, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_ENVIRONMENT, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (temp_c < IOTDATA_TEMPERATURE_MIN)
        return IOTDATA_ERR_TEMPERATURE_LOW;
    if (temp_c > IOTDATA_TEMPERATURE_MAX)
        return IOTDATA_ERR_TEMPERATURE_HIGH;
    if (pressure_hpa < IOTDATA_PRESSURE_MIN)
        return IOTDATA_ERR_PRESSURE_LOW;
    if (pressure_hpa > IOTDATA_PRESSURE_MAX)
        return IOTDATA_ERR_PRESSURE_HIGH;
    if (humidity_pct > IOTDATA_HUMIDITY_MAX)
        return IOTDATA_ERR_HUMIDITY_HIGH;
#endif
    enc->environment.temperature.temperature = temp_c;
    enc->environment.pressure.pressure = pressure_hpa;
    enc->environment.humidity.humidity = humidity_pct;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_ENVIRONMENT);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_environment(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_environment_t *const v = vv;
    return unpack_temperature(buf, bb, bp, &v->temperature) && unpack_pressure(buf, bb, bp, &v->pressure) && unpack_humidity(buf, bb, bp, &v->humidity);
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_environment(iotdata_decoder_t *dec, iotdata_float_t *temp_c, uint16_t *pressure_hpa, uint8_t *humidity_pct) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_environment_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_ENVIRONMENT, unpack_environment, &v);
    if (temp_c)
        *temp_c = v.temperature.temperature;
    if (pressure_hpa)
        *pressure_hpa = v.pressure.pressure;
    if (humidity_pct)
        *humidity_pct = v.humidity.humidity;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_ENVIRONMENT);
    if (temp_c)
        *temp_c = dec->environment.temperature.temperature;
    if (pressure_hpa)
        *pressure_hpa = dec->environment.pressure.pressure;
    if (humidity_pct)
        *humidity_pct = dec->environment.humidity.humidity;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_environment(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    cJSON *j_temperature = cJSON_GetObjectItem(j, "temperature"), *j_pressure = cJSON_GetObjectItem(j, "pressure"), *j_humidity = cJSON_GetObjectItem(j, "humidity");
    if (!j_temperature || !j_pressure || !j_humidity)
        return IOTDATA_OK;
    return iotdata_encode_environment(enc, (iotdata_float_t)j_temperature->valuedouble, (uint16_t)j_pressure->valueint, (uint8_t)j_humidity->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_environment(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_environment_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_temperature(obj, &v->temperature, "temperature", scratch);
    json_set_pressure(obj, &v->pressure, "pressure", scratch);
    json_set_humidity(obj, &v->humidity, "humidity", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_environment(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    n = dump_temperature(buf, bb, bp, dump, n, label);
    n = dump_pressure(buf, bb, bp, dump, n, label);
    n = dump_humidity(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_environment(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_environment_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.2f C, %" PRIu16 " hPa, %" PRIu8 "%%\n", label, _padd(label), (double)v->temperature.temperature, v->pressure.pressure, v->humidity.humidity);
#else
    const int32_t ta = v->temperature.temperature < 0 ? -(v->temperature.temperature) : v->temperature.temperature;
    bprintf(bp, "  %s:%s %s%" PRId32 ".%02" PRId32 " C, %" PRIu16 " hPa, %" PRIu8 "%%\n", label, _padd(label), v->temperature.temperature < 0 ? "-" : "", ta / 100, ta % 100, v->pressure.pressure, v->humidity.humidity);
#endif
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_environment = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, environment)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, environment)) _IOTDATA_OP_NAME("environment")
                                                                        _IOTDATA_OP_BITS(IOTDATA_TEMPERATURE_BITS + IOTDATA_PRESSURE_BITS + IOTDATA_HUMIDITY_BITS) _IOTDATA_OP_PACK(pack_environment) _IOTDATA_OP_UNPACK(unpack_environment)
                                                                            _IOTDATA_OP_DUMP(dump_environment) _IOTDATA_OP_PRINT(print_environment) _IOTDATA_OP_JSON_SET(json_set_environment) _IOTDATA_OP_JSON_GET(json_get_environment) };
#endif

/* =========================================================================
 * Field WIND, WIND_SPEED, WIND_DIRECTION, WIND_GUST
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_WIND_SPEED) || defined(IOTDATA_ENABLE_WIND)
#if !defined(IOTDATA_NO_FLOATING)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_wind_speed(float speed) {
    return (uint32_t)roundf(speed / IOTDATA_WIND_SPEED_RES);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static float dequantise_wind_speed(uint32_t raw) {
    return (float)raw * IOTDATA_WIND_SPEED_RES;
}
#endif
#else
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_wind_speed(int32_t speed100) {
    return (uint32_t)((speed100 + (IOTDATA_WIND_SPEED_RES / 2)) / IOTDATA_WIND_SPEED_RES);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int32_t dequantise_wind_speed(uint32_t raw) {
    return (int32_t)raw * IOTDATA_WIND_SPEED_RES;
}
#endif
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_wind_speed(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_wind_speed_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_wind_speed(v->wind_speed), IOTDATA_WIND_SPEED_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_WIND_SPEED) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_wind_speed_by_slot(iotdata_encoder_t *const enc, const int slot, iotdata_float_t speed_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_WIND_SPEED, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (speed_ms < 0 || speed_ms > IOTDATA_WIND_SPEED_MAX)
        return IOTDATA_ERR_WIND_SPEED_HIGH;
#endif
    iotdata_field_wind_speed_t v;
    v.wind_speed = speed_ms;
    PACK_DIRECT(enc, pack_wind_speed, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND_SPEED);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_wind_speed(iotdata_encoder_t *const enc, iotdata_float_t speed_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_SPEED, si);
    return iotdata_encode_wind_speed_by_slot(enc, si, speed_ms);
}
#else
iotdata_status_t iotdata_encode_wind_speed(iotdata_encoder_t *const enc, iotdata_float_t speed_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_SPEED, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (speed_ms < 0 || speed_ms > IOTDATA_WIND_SPEED_MAX)
        return IOTDATA_ERR_WIND_SPEED_HIGH;
#endif
    enc->wind_speed.wind_speed = speed_ms;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND_SPEED);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_wind_speed(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_wind_speed_t *const v = vv;
    if (*bp + IOTDATA_WIND_SPEED_BITS > bb)
        return false;
    v->wind_speed = dequantise_wind_speed(bits_read(buf, bb, bp, IOTDATA_WIND_SPEED_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_SPEED) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_wind_speed(iotdata_decoder_t *dec, iotdata_float_t *speed_ms) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_wind_speed_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_WIND_SPEED, unpack_wind_speed, &v);
    if (speed_ms)
        *speed_ms = v.wind_speed;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_WIND_SPEED);
    if (speed_ms)
        *speed_ms = dec->wind_speed.wind_speed;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_SPEED) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_wind_speed(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_wind_speed(enc, (iotdata_float_t)j->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_wind_speed(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_wind_speed_t *const v = vv;
    (void)scratch;
#if !defined(IOTDATA_NO_FLOATING)
    json_add_number_fixed(root, label, (double)v->wind_speed, 1);
#else
    cJSON_AddNumberToObject(root, label, v->wind_speed);
#endif
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_wind_speed(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_WIND_SPEED_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.1f m/s", (double)dequantise_wind_speed(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_wind_speed(r), 100, "m/s");
#endif
    n = dump_add(dump, n, s, IOTDATA_WIND_SPEED_BITS, r, dump->_dec_buf, "0..63.5, 0.5m/s", "wind_speed");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_SPEED) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_wind_speed(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_wind_speed_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.1f m/s\n", label, _padd(label), (double)v->wind_speed);
#else
    bprintf(bp, "  %s:%s %" PRId32 ".%02" PRId32 " m/s\n", label, _padd(label), v->wind_speed / 100, v->wind_speed % 100);
#endif
}
#endif
#if defined(IOTDATA_ENABLE_WIND_SPEED)
static const iotdata_field_ops_t _iotdata_field_def_wind_speed = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, wind_speed)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, wind_speed)) _IOTDATA_OP_NAME("wind_speed")
                                                                       _IOTDATA_OP_BITS(IOTDATA_WIND_SPEED_BITS) _IOTDATA_OP_PACK(pack_wind_speed) _IOTDATA_OP_UNPACK(unpack_wind_speed) _IOTDATA_OP_DUMP(dump_wind_speed)
                                                                           _IOTDATA_OP_PRINT(print_wind_speed) _IOTDATA_OP_JSON_SET(json_set_wind_speed) _IOTDATA_OP_JSON_GET(json_get_wind_speed) };
#endif
#endif

#if defined(IOTDATA_ENABLE_WIND_DIRECTION) || defined(IOTDATA_ENABLE_WIND)
#if !defined(IOTDATA_NO_FLOATING)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_wind_direction(uint16_t deg) {
    return (uint32_t)roundf((float)deg / IOTDATA_WIND_DIRECTION_SCALE);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint16_t dequantise_wind_direction(uint32_t raw) {
    return (uint16_t)roundf((float)raw * IOTDATA_WIND_DIRECTION_SCALE);
}
#endif
#else
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_wind_direction(uint16_t deg) {
    return (uint32_t)((deg * (1 << IOTDATA_WIND_DIRECTION_BITS) + (IOTDATA_WIND_DIRECTION_MAX + 1) / 2) / (IOTDATA_WIND_DIRECTION_MAX + 1));
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint16_t dequantise_wind_direction(uint32_t raw) {
    return (uint16_t)((raw * (IOTDATA_WIND_DIRECTION_MAX + 1) + (1 << IOTDATA_WIND_DIRECTION_BITS) / 2) / (1 << IOTDATA_WIND_DIRECTION_BITS));
}
#endif
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_wind_direction(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_wind_direction_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_wind_direction(v->wind_direction), IOTDATA_WIND_DIRECTION_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_WIND_DIRECTION) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_wind_direction_by_slot(iotdata_encoder_t *const enc, const int slot, uint16_t direction_deg) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_WIND_DIRECTION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (direction_deg > IOTDATA_WIND_DIRECTION_MAX)
        return IOTDATA_ERR_WIND_DIRECTION_HIGH;
#endif
    iotdata_field_wind_direction_t v;
    v.wind_direction = direction_deg;
    PACK_DIRECT(enc, pack_wind_direction, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND_DIRECTION);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_wind_direction(iotdata_encoder_t *const enc, uint16_t direction_deg) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_DIRECTION, si);
    return iotdata_encode_wind_direction_by_slot(enc, si, direction_deg);
}
#else
iotdata_status_t iotdata_encode_wind_direction(iotdata_encoder_t *const enc, uint16_t direction_deg) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_DIRECTION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (direction_deg > IOTDATA_WIND_DIRECTION_MAX)
        return IOTDATA_ERR_WIND_DIRECTION_HIGH;
#endif
    enc->wind_direction.wind_direction = direction_deg;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND_DIRECTION);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_wind_direction(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_wind_direction_t *const v = vv;
    if (*bp + IOTDATA_WIND_DIRECTION_BITS > bb)
        return false;
    v->wind_direction = dequantise_wind_direction(bits_read(buf, bb, bp, IOTDATA_WIND_DIRECTION_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_DIRECTION) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_wind_direction(iotdata_decoder_t *dec, uint16_t *direction_deg) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_wind_direction_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_WIND_DIRECTION, unpack_wind_direction, &v);
    if (direction_deg)
        *direction_deg = v.wind_direction;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_WIND_DIRECTION);
    if (direction_deg)
        *direction_deg = dec->wind_direction.wind_direction;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_DIRECTION) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_wind_direction(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_wind_direction(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_wind_direction(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_wind_direction_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->wind_direction);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_wind_direction(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_WIND_DIRECTION_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " deg", dequantise_wind_direction(r));
    n = dump_add(dump, n, s, IOTDATA_WIND_DIRECTION_BITS, r, dump->_dec_buf, "0..355, ~1.4deg", "wind_direction");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_DIRECTION) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_wind_direction(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_wind_direction_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " deg\n", label, _padd(label), v->wind_direction);
}
#endif
#if defined(IOTDATA_ENABLE_WIND_DIRECTION)
static const iotdata_field_ops_t _iotdata_field_def_wind_direction = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, wind_direction)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, wind_direction)) _IOTDATA_OP_NAME("wind_direction")
                                                                           _IOTDATA_OP_BITS(IOTDATA_WIND_DIRECTION_BITS) _IOTDATA_OP_PACK(pack_wind_direction) _IOTDATA_OP_UNPACK(unpack_wind_direction) _IOTDATA_OP_DUMP(dump_wind_direction)
                                                                               _IOTDATA_OP_PRINT(print_wind_direction) _IOTDATA_OP_JSON_SET(json_set_wind_direction) _IOTDATA_OP_JSON_GET(json_get_wind_direction) };
#endif
#endif

#if defined(IOTDATA_ENABLE_WIND_GUST) || defined(IOTDATA_ENABLE_WIND)
#if !defined(IOTDATA_NO_FLOATING)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_wind_gust(float gust) {
    return (uint32_t)roundf(gust / IOTDATA_WIND_GUST_RES);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static float dequantise_wind_gust(uint32_t raw) {
    return (float)raw * IOTDATA_WIND_GUST_RES;
}
#endif
#else
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_wind_gust(int32_t gust100) {
    return (uint32_t)((gust100 + (IOTDATA_WIND_GUST_RES / 2)) / IOTDATA_WIND_GUST_RES);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int32_t dequantise_wind_gust(uint32_t raw) {
    return (int32_t)raw * IOTDATA_WIND_GUST_RES;
}
#endif
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_wind_gust(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_wind_gust_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_wind_gust(v->wind_gust), IOTDATA_WIND_GUST_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_WIND_GUST) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_wind_gust_by_slot(iotdata_encoder_t *const enc, const int slot, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_WIND_GUST, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (gust_ms < 0 || gust_ms > IOTDATA_WIND_GUST_MAX)
        return IOTDATA_ERR_WIND_GUST_HIGH;
#endif
    iotdata_field_wind_gust_t v;
    v.wind_gust = gust_ms;
    PACK_DIRECT(enc, pack_wind_gust, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND_GUST);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_wind_gust(iotdata_encoder_t *const enc, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_GUST, si);
    return iotdata_encode_wind_gust_by_slot(enc, si, gust_ms);
}
#else
iotdata_status_t iotdata_encode_wind_gust(iotdata_encoder_t *const enc, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_GUST, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (gust_ms < 0 || gust_ms > IOTDATA_WIND_GUST_MAX)
        return IOTDATA_ERR_WIND_GUST_HIGH;
#endif
    enc->wind_gust.wind_gust = gust_ms;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND_GUST);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_wind_gust(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_wind_gust_t *const v = vv;
    if (*bp + IOTDATA_WIND_GUST_BITS > bb)
        return false;
    v->wind_gust = dequantise_wind_gust(bits_read(buf, bb, bp, IOTDATA_WIND_GUST_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_GUST) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_wind_gust(iotdata_decoder_t *dec, iotdata_float_t *gust_ms) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_wind_gust_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_WIND_GUST, unpack_wind_gust, &v);
    if (gust_ms)
        *gust_ms = v.wind_gust;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_WIND_GUST);
    if (gust_ms)
        *gust_ms = dec->wind_gust.wind_gust;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_GUST) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_wind_gust(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_wind_gust(enc, (iotdata_float_t)j->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_wind_gust(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_wind_gust_t *const v = vv;
    (void)scratch;
#if !defined(IOTDATA_NO_FLOATING)
    json_add_number_fixed(root, label, (double)v->wind_gust, 1);
#else
    cJSON_AddNumberToObject(root, label, v->wind_gust);
#endif
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_wind_gust(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_WIND_GUST_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.1f m/s", (double)dequantise_wind_gust(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_wind_gust(r), 100, "m/s");
#endif
    n = dump_add(dump, n, s, IOTDATA_WIND_GUST_BITS, r, dump->_dec_buf, "0..63.5, 0.5m/s", "wind_gust");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_GUST) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_wind_gust(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_wind_gust_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.1f m/s\n", label, _padd(label), (double)v->wind_gust);
#else
    bprintf(bp, "  %s:%s %" PRId32 ".%02" PRId32 " m/s\n", label, _padd(label), v->wind_gust / 100, v->wind_gust % 100);
#endif
}
#endif
#if defined(IOTDATA_ENABLE_WIND_GUST)
static const iotdata_field_ops_t _iotdata_field_def_wind_gust = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, wind_gust)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, wind_gust)) _IOTDATA_OP_NAME("wind_gust")
                                                                      _IOTDATA_OP_BITS(IOTDATA_WIND_GUST_BITS) _IOTDATA_OP_PACK(pack_wind_gust) _IOTDATA_OP_UNPACK(unpack_wind_gust) _IOTDATA_OP_DUMP(dump_wind_gust)
                                                                          _IOTDATA_OP_PRINT(print_wind_gust) _IOTDATA_OP_JSON_SET(json_set_wind_gust) _IOTDATA_OP_JSON_GET(json_get_wind_gust) };
#endif
#endif

#if defined(IOTDATA_ENABLE_WIND)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_wind(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_wind_t *const v = vv;
    return pack_wind_speed(buf, bb, bp, &v->wind_speed) && pack_wind_direction(buf, bb, bp, &v->wind_direction) && pack_wind_gust(buf, bb, bp, &v->wind_gust);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_wind_by_slot(iotdata_encoder_t *const enc, const int slot, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_WIND, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (speed_ms < 0 || speed_ms > IOTDATA_WIND_SPEED_MAX)
        return IOTDATA_ERR_WIND_SPEED_HIGH;
    if (direction_deg > IOTDATA_WIND_DIRECTION_MAX)
        return IOTDATA_ERR_WIND_DIRECTION_HIGH;
    if (gust_ms < 0 || gust_ms > IOTDATA_WIND_GUST_MAX)
        return IOTDATA_ERR_WIND_GUST_HIGH;
#endif
    iotdata_field_wind_t v;
    v.wind_speed.wind_speed = speed_ms;
    v.wind_direction.wind_direction = direction_deg;
    v.wind_gust.wind_gust = gust_ms;
    PACK_DIRECT(enc, pack_wind, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_wind(iotdata_encoder_t *const enc, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND, si);
    return iotdata_encode_wind_by_slot(enc, si, speed_ms, direction_deg, gust_ms);
}
#else
iotdata_status_t iotdata_encode_wind(iotdata_encoder_t *const enc, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (speed_ms < 0 || speed_ms > IOTDATA_WIND_SPEED_MAX)
        return IOTDATA_ERR_WIND_SPEED_HIGH;
    if (direction_deg > IOTDATA_WIND_DIRECTION_MAX)
        return IOTDATA_ERR_WIND_DIRECTION_HIGH;
    if (gust_ms < 0 || gust_ms > IOTDATA_WIND_GUST_MAX)
        return IOTDATA_ERR_WIND_GUST_HIGH;
#endif
    enc->wind.wind_speed.wind_speed = speed_ms;
    enc->wind.wind_direction.wind_direction = direction_deg;
    enc->wind.wind_gust.wind_gust = gust_ms;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_wind(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_wind_t *const v = vv;
    return unpack_wind_speed(buf, bb, bp, &v->wind_speed) && unpack_wind_direction(buf, bb, bp, &v->wind_direction) && unpack_wind_gust(buf, bb, bp, &v->wind_gust);
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_wind(iotdata_decoder_t *dec, iotdata_float_t *speed_ms, uint16_t *direction_deg, iotdata_float_t *gust_ms) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_wind_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_WIND, unpack_wind, &v);
    if (speed_ms)
        *speed_ms = v.wind_speed.wind_speed;
    if (direction_deg)
        *direction_deg = v.wind_direction.wind_direction;
    if (gust_ms)
        *gust_ms = v.wind_gust.wind_gust;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_WIND);
    if (speed_ms)
        *speed_ms = dec->wind.wind_speed.wind_speed;
    if (direction_deg)
        *direction_deg = dec->wind.wind_direction.wind_direction;
    if (gust_ms)
        *gust_ms = dec->wind.wind_gust.wind_gust;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_wind(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    cJSON *j_speed = cJSON_GetObjectItem(j, "speed"), *j_direction = cJSON_GetObjectItem(j, "direction"), *j_gust = cJSON_GetObjectItem(j, "gust");
    if (!j_speed || !j_direction || !j_gust)
        return IOTDATA_OK;
    return iotdata_encode_wind(enc, (iotdata_float_t)j_speed->valuedouble, (uint16_t)j_direction->valueint, (iotdata_float_t)j_gust->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_wind(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_wind_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_wind_speed(obj, &v->wind_speed, "speed", scratch);
    json_set_wind_direction(obj, &v->wind_direction, "direction", scratch);
    json_set_wind_gust(obj, &v->wind_gust, "gust", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_wind(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    n = dump_wind_speed(buf, bb, bp, dump, n, label);
    n = dump_wind_direction(buf, bb, bp, dump, n, label);
    n = dump_wind_gust(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_wind(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_wind_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.1f m/s, %" PRIu16 " deg, gust %.1f m/s\n", label, _padd(label), (double)v->wind_speed.wind_speed, v->wind_direction.wind_direction, (double)v->wind_gust.wind_gust);
#else
    bprintf(bp, "  %s:%s %" PRId32 ".%02" PRId32 " m/s, %" PRIu16 " deg, gust %" PRId32 ".%02" PRId32 " m/s\n", label, _padd(label), v->wind_speed.wind_speed / 100, v->wind_speed.wind_speed % 100, v->wind_direction.wind_direction,
            v->wind_gust.wind_gust / 100, v->wind_gust.wind_gust % 100);
#endif
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_wind = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, wind)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, wind)) _IOTDATA_OP_NAME("wind")
                                                                 _IOTDATA_OP_BITS(IOTDATA_WIND_SPEED_BITS + IOTDATA_WIND_DIRECTION_BITS + IOTDATA_WIND_GUST_BITS) _IOTDATA_OP_PACK(pack_wind) _IOTDATA_OP_UNPACK(unpack_wind)
                                                                     _IOTDATA_OP_DUMP(dump_wind) _IOTDATA_OP_PRINT(print_wind) _IOTDATA_OP_JSON_SET(json_set_wind) _IOTDATA_OP_JSON_GET(json_get_wind) };
#endif

/* =========================================================================
 * Field RAIN, RAIN_RATE, RAIN_SIZE
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_RAIN_RATE) || defined(IOTDATA_ENABLE_RAIN)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_rain_rate(uint8_t rain_rate) {
    return (uint32_t)rain_rate;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint8_t dequantise_rain_rate(uint32_t raw) {
    return (uint8_t)raw;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_rain_rate(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_rain_rate_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_rain_rate(v->rain_rate), IOTDATA_RAIN_RATE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_RATE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_rain_rate_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t rate_mmhr) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_RAIN_RATE, si);
    iotdata_field_rain_rate_t v;
    v.rain_rate = rate_mmhr;
    PACK_DIRECT(enc, pack_rain_rate, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RAIN_RATE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_rain_rate(iotdata_encoder_t *const enc, uint8_t rate_mmhr) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RAIN_RATE, si);
    return iotdata_encode_rain_rate_by_slot(enc, si, rate_mmhr);
}
#else
iotdata_status_t iotdata_encode_rain_rate(iotdata_encoder_t *const enc, uint8_t rate_mmhr) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RAIN_RATE, si);
    enc->rain_rate.rain_rate = rate_mmhr;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RAIN_RATE);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_rain_rate(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_rain_rate_t *const v = vv;
    if (*bp + IOTDATA_RAIN_RATE_BITS > bb)
        return false;
    v->rain_rate = dequantise_rain_rate(bits_read(buf, bb, bp, IOTDATA_RAIN_RATE_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_RATE) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_rain_rate(iotdata_decoder_t *dec, uint8_t *rate_mmhr) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_rain_rate_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_RAIN_RATE, unpack_rain_rate, &v);
    if (rate_mmhr)
        *rate_mmhr = v.rain_rate;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_RAIN_RATE);
    if (rate_mmhr)
        *rate_mmhr = dec->rain_rate.rain_rate;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_RATE) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_rain_rate(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_rain_rate(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_rain_rate(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_rain_rate_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->rain_rate);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_rain_rate(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_RAIN_RATE_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 " mm/hr", dequantise_rain_rate(r));
    n = dump_add(dump, n, s, IOTDATA_RAIN_RATE_BITS, r, dump->_dec_buf, "0..255 mm/hr", "rain_rate");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_RATE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_rain_rate(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_rain_rate_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 " mm/hr\n", label, _padd(label), v->rain_rate);
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_RATE)
static const iotdata_field_ops_t _iotdata_field_def_rain_rate = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, rain_rate)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, rain_rate)) _IOTDATA_OP_NAME("rain_rate")
                                                                      _IOTDATA_OP_BITS(IOTDATA_RAIN_RATE_BITS) _IOTDATA_OP_PACK(pack_rain_rate) _IOTDATA_OP_UNPACK(unpack_rain_rate) _IOTDATA_OP_DUMP(dump_rain_rate)
                                                                          _IOTDATA_OP_PRINT(print_rain_rate) _IOTDATA_OP_JSON_SET(json_set_rain_rate) _IOTDATA_OP_JSON_GET(json_get_rain_rate) };
#endif
#endif

#if defined(IOTDATA_ENABLE_RAIN_SIZE) || defined(IOTDATA_ENABLE_RAIN)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_rain_size(uint8_t rain_size10) {
    return (uint32_t)(rain_size10 / IOTDATA_RAIN_SIZE_SCALE);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint8_t dequantise_rain_size(uint32_t raw) {
    return (uint8_t)(raw * IOTDATA_RAIN_SIZE_SCALE);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_rain_size(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_rain_size_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_rain_size(v->rain_size10), IOTDATA_RAIN_SIZE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_SIZE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_rain_size_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t size10_mmd) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_RAIN_SIZE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (size10_mmd > IOTDATA_RAIN_SIZE_MAX * IOTDATA_RAIN_SIZE_SCALE)
        return IOTDATA_ERR_RAIN_SIZE_HIGH;
#endif
    iotdata_field_rain_size_t v;
    v.rain_size10 = size10_mmd;
    PACK_DIRECT(enc, pack_rain_size, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RAIN_SIZE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_rain_size(iotdata_encoder_t *const enc, uint8_t size10_mmd) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RAIN_SIZE, si);
    return iotdata_encode_rain_size_by_slot(enc, si, size10_mmd);
}
#else
iotdata_status_t iotdata_encode_rain_size(iotdata_encoder_t *const enc, uint8_t size10_mmd) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RAIN_SIZE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (size10_mmd > IOTDATA_RAIN_SIZE_MAX * IOTDATA_RAIN_SIZE_SCALE)
        return IOTDATA_ERR_RAIN_SIZE_HIGH;
#endif
    enc->rain_size.rain_size10 = size10_mmd;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RAIN_SIZE);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_rain_size(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_rain_size_t *const v = vv;
    if (*bp + IOTDATA_RAIN_SIZE_BITS > bb)
        return false;
    v->rain_size10 = dequantise_rain_size(bits_read(buf, bb, bp, IOTDATA_RAIN_SIZE_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_SIZE) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_rain_size(iotdata_decoder_t *dec, uint8_t *size10_mmd) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_rain_size_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_RAIN_SIZE, unpack_rain_size, &v);
    if (size10_mmd)
        *size10_mmd = v.rain_size10;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_RAIN_SIZE);
    if (size10_mmd)
        *size10_mmd = dec->rain_size.rain_size10;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_SIZE) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_rain_size(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_rain_size(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_rain_size(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_rain_size_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->rain_size10);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_rain_size(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_RAIN_SIZE_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 ".%" PRIu8 " mm/d", dequantise_rain_size(r) / 10, dequantise_rain_size(r) % 10);
    n = dump_add(dump, n, s, IOTDATA_RAIN_SIZE_BITS, r, dump->_dec_buf, "0..6.3 mm/d", "rain_size");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_SIZE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_rain_size(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_rain_size_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 ".%" PRIu8 " mm/d\n", label, _padd(label), v->rain_size10 / 10, v->rain_size10 % 10);
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_SIZE)
static const iotdata_field_ops_t _iotdata_field_def_rain_size = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, rain_size)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, rain_size)) _IOTDATA_OP_NAME("rain_size")
                                                                      _IOTDATA_OP_BITS(IOTDATA_RAIN_SIZE_BITS) _IOTDATA_OP_PACK(pack_rain_size) _IOTDATA_OP_UNPACK(unpack_rain_size) _IOTDATA_OP_DUMP(dump_rain_size)
                                                                          _IOTDATA_OP_PRINT(print_rain_size) _IOTDATA_OP_JSON_SET(json_set_rain_size) _IOTDATA_OP_JSON_GET(json_get_rain_size) };
#endif
#endif

#if defined(IOTDATA_ENABLE_RAIN)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_rain(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_rain_t *const v = vv;
    return pack_rain_rate(buf, bb, bp, &v->rain_rate) && pack_rain_size(buf, bb, bp, &v->rain_size);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_rain_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t rate_mmhr, uint8_t size10_mmd) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_RAIN, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (size10_mmd > IOTDATA_RAIN_SIZE_MAX * IOTDATA_RAIN_SIZE_SCALE)
        return IOTDATA_ERR_RAIN_SIZE_HIGH;
#endif
    iotdata_field_rain_t v;
    v.rain_rate.rain_rate = rate_mmhr;
    v.rain_size.rain_size10 = size10_mmd;
    PACK_DIRECT(enc, pack_rain, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RAIN);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_rain(iotdata_encoder_t *const enc, uint8_t rate_mmhr, uint8_t size10_mmd) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RAIN, si);
    return iotdata_encode_rain_by_slot(enc, si, rate_mmhr, size10_mmd);
}
#else
iotdata_status_t iotdata_encode_rain(iotdata_encoder_t *const enc, uint8_t rate_mmhr, uint8_t size10_mmd) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RAIN, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (size10_mmd > IOTDATA_RAIN_SIZE_MAX * IOTDATA_RAIN_SIZE_SCALE)
        return IOTDATA_ERR_RAIN_SIZE_HIGH;
#endif
    enc->rain.rain_rate.rain_rate = rate_mmhr;
    enc->rain.rain_size.rain_size10 = size10_mmd;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RAIN);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_rain(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_rain_t *const v = vv;
    return unpack_rain_rate(buf, bb, bp, &v->rain_rate) && unpack_rain_size(buf, bb, bp, &v->rain_size);
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_rain(iotdata_decoder_t *dec, uint8_t *rate_mmhr, uint8_t *size10_mmd) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_rain_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_RAIN, unpack_rain, &v);
    if (rate_mmhr)
        *rate_mmhr = v.rain_rate.rain_rate;
    if (size10_mmd)
        *size10_mmd = v.rain_size.rain_size10;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_RAIN);
    if (rate_mmhr)
        *rate_mmhr = dec->rain.rain_rate.rain_rate;
    if (size10_mmd)
        *size10_mmd = dec->rain.rain_size.rain_size10;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_rain(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    cJSON *j_rate = cJSON_GetObjectItem(j, "rate"), *j_size = cJSON_GetObjectItem(j, "size");
    if (!j_rate || !j_size)
        return IOTDATA_OK;
    return iotdata_encode_rain(enc, (uint8_t)j_rate->valueint, (uint8_t)j_size->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_rain(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_rain_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_rain_rate(obj, &v->rain_rate, "rate", scratch);
    json_set_rain_size(obj, &v->rain_size, "size", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_rain(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    n = dump_rain_rate(buf, bb, bp, dump, n, label);
    n = dump_rain_size(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_rain(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_rain_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 " mm/hr, %" PRIu8 ".%" PRIu8 " mm/d\n", label, _padd(label), v->rain_rate.rain_rate, v->rain_size.rain_size10 / 10, v->rain_size.rain_size10 % 10);
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_rain = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, rain)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, rain)) _IOTDATA_OP_NAME("rain")
                                                                 _IOTDATA_OP_BITS(IOTDATA_RAIN_RATE_BITS + IOTDATA_RAIN_SIZE_BITS) _IOTDATA_OP_PACK(pack_rain) _IOTDATA_OP_UNPACK(unpack_rain) _IOTDATA_OP_DUMP(dump_rain)
                                                                     _IOTDATA_OP_PRINT(print_rain) _IOTDATA_OP_JSON_SET(json_set_rain) _IOTDATA_OP_JSON_GET(json_get_rain) };
#endif

/* =========================================================================
 * Field SOLAR, SOLAR_IRRADIANCE, SOLAR_ULTRAVIOLET
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_SOLAR_IRRADIANCE) || defined(IOTDATA_ENABLE_SOLAR)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_solar_irradiance(uint16_t solar_irradiance) {
    return (uint32_t)solar_irradiance;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint16_t dequantise_solar_irradiance(uint32_t raw) {
    return (uint16_t)raw;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_solar_irradiance(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_solar_irradiance_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_solar_irradiance(v->solar_irradiance), IOTDATA_SOLAR_IRRADIANCE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_IRRADIANCE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_solar_irradiance_by_slot(iotdata_encoder_t *const enc, const int slot, uint16_t irradiance_wm2) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_SOLAR_IRRADIANCE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (irradiance_wm2 > IOTDATA_SOLAR_IRRADIANCE_MAX)
        return IOTDATA_ERR_SOLAR_IRRADIANCE_HIGH;
#endif
    iotdata_field_solar_irradiance_t v;
    v.solar_irradiance = irradiance_wm2;
    PACK_DIRECT(enc, pack_solar_irradiance, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_SOLAR_IRRADIANCE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_solar_irradiance(iotdata_encoder_t *const enc, uint16_t irradiance_wm2) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_SOLAR_IRRADIANCE, si);
    return iotdata_encode_solar_irradiance_by_slot(enc, si, irradiance_wm2);
}
#else
iotdata_status_t iotdata_encode_solar_irradiance(iotdata_encoder_t *const enc, uint16_t irradiance_wm2) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_SOLAR_IRRADIANCE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (irradiance_wm2 > IOTDATA_SOLAR_IRRADIANCE_MAX)
        return IOTDATA_ERR_SOLAR_IRRADIANCE_HIGH;
#endif
    enc->solar_irradiance.solar_irradiance = irradiance_wm2;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_SOLAR_IRRADIANCE);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_solar_irradiance(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_solar_irradiance_t *const v = vv;
    if (*bp + IOTDATA_SOLAR_IRRADIANCE_BITS > bb)
        return false;
    v->solar_irradiance = dequantise_solar_irradiance(bits_read(buf, bb, bp, IOTDATA_SOLAR_IRRADIANCE_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_IRRADIANCE) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_solar_irradiance(iotdata_decoder_t *dec, uint16_t *irradiance_wm2) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_solar_irradiance_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_SOLAR_IRRADIANCE, unpack_solar_irradiance, &v);
    if (irradiance_wm2)
        *irradiance_wm2 = v.solar_irradiance;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_SOLAR_IRRADIANCE);
    if (irradiance_wm2)
        *irradiance_wm2 = dec->solar_irradiance.solar_irradiance;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_IRRADIANCE) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_solar_irradiance(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_solar_irradiance(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_solar_irradiance(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_solar_irradiance_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->solar_irradiance);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_solar_irradiance(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_SOLAR_IRRADIANCE_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " W/m2", dequantise_solar_irradiance(r));
    n = dump_add(dump, n, s, IOTDATA_SOLAR_IRRADIANCE_BITS, r, dump->_dec_buf, "0..1023 W/m2", "solar_ir");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_IRRADIANCE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_solar_irradiance(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_solar_irradiance_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " W/m2\n", label, _padd(label), v->solar_irradiance);
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_IRRADIANCE)
static const iotdata_field_ops_t _iotdata_field_def_solar_irradiance = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, solar_irradiance)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, solar_irradiance))
                                                                             _IOTDATA_OP_NAME("solar_irradiance") _IOTDATA_OP_BITS(IOTDATA_SOLAR_IRRADIANCE_BITS) _IOTDATA_OP_PACK(pack_solar_irradiance)
                                                                                 _IOTDATA_OP_UNPACK(unpack_solar_irradiance) _IOTDATA_OP_DUMP(dump_solar_irradiance) _IOTDATA_OP_PRINT(print_solar_irradiance)
                                                                                     _IOTDATA_OP_JSON_SET(json_set_solar_irradiance) _IOTDATA_OP_JSON_GET(json_get_solar_irradiance) };
#endif
#endif
#if defined(IOTDATA_ENABLE_SOLAR_ULTRAVIOLET) || defined(IOTDATA_ENABLE_SOLAR)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_solar_ultraviolet(uint8_t solar_ultraviolet) {
    return (uint32_t)solar_ultraviolet;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint8_t dequantise_solar_ultraviolet(uint32_t raw) {
    return (uint8_t)raw;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_solar_ultraviolet(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_solar_ultraviolet_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_solar_ultraviolet(v->solar_ultraviolet), IOTDATA_SOLAR_ULTRAVIOLET_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_ULTRAVIOLET) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_solar_ultraviolet_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t ultraviolet_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_SOLAR_ULTRAVIOLET, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (ultraviolet_index > IOTDATA_SOLAR_ULTRAVIOLET_MAX)
        return IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH;
#endif
    iotdata_field_solar_ultraviolet_t v;
    v.solar_ultraviolet = ultraviolet_index;
    PACK_DIRECT(enc, pack_solar_ultraviolet, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_SOLAR_ULTRAVIOLET);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_solar_ultraviolet(iotdata_encoder_t *const enc, uint8_t ultraviolet_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_SOLAR_ULTRAVIOLET, si);
    return iotdata_encode_solar_ultraviolet_by_slot(enc, si, ultraviolet_index);
}
#else
iotdata_status_t iotdata_encode_solar_ultraviolet(iotdata_encoder_t *const enc, uint8_t ultraviolet_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_SOLAR_ULTRAVIOLET, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (ultraviolet_index > IOTDATA_SOLAR_ULTRAVIOLET_MAX)
        return IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH;
#endif
    enc->solar_ultraviolet.solar_ultraviolet = ultraviolet_index;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_SOLAR_ULTRAVIOLET);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_solar_ultraviolet(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_solar_ultraviolet_t *const v = vv;
    if (*bp + IOTDATA_SOLAR_ULTRAVIOLET_BITS > bb)
        return false;
    v->solar_ultraviolet = dequantise_solar_ultraviolet(bits_read(buf, bb, bp, IOTDATA_SOLAR_ULTRAVIOLET_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_ULTRAVIOLET) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_solar_ultraviolet(iotdata_decoder_t *dec, uint8_t *ultraviolet_index) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_solar_ultraviolet_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_SOLAR_ULTRAVIOLET, unpack_solar_ultraviolet, &v);
    if (ultraviolet_index)
        *ultraviolet_index = v.solar_ultraviolet;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_SOLAR_ULTRAVIOLET);
    if (ultraviolet_index)
        *ultraviolet_index = dec->solar_ultraviolet.solar_ultraviolet;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_ULTRAVIOLET) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_solar_ultraviolet(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_solar_ultraviolet(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_solar_ultraviolet(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_solar_ultraviolet_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->solar_ultraviolet);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_solar_ultraviolet(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_SOLAR_ULTRAVIOLET_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8, dequantise_solar_ultraviolet(r));
    n = dump_add(dump, n, s, IOTDATA_SOLAR_ULTRAVIOLET_BITS, r, dump->_dec_buf, "0..15", "solar_uv");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_ULTRAVIOLET) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_solar_ultraviolet(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_solar_ultraviolet_t *const v = vv;
    bprintf(bp, "  %s:%s UV %" PRIu8 "\n", label, _padd(label), v->solar_ultraviolet);
}
#endif
#if defined(IOTDATA_ENABLE_SOLAR_ULTRAVIOLET)
static const iotdata_field_ops_t _iotdata_field_def_solar_ultraviolet = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, solar_ultraviolet)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, solar_ultraviolet))
                                                                              _IOTDATA_OP_NAME("solar_ultraviolet") _IOTDATA_OP_BITS(IOTDATA_SOLAR_ULTRAVIOLET_BITS) _IOTDATA_OP_PACK(pack_solar_ultraviolet)
                                                                                  _IOTDATA_OP_UNPACK(unpack_solar_ultraviolet) _IOTDATA_OP_DUMP(dump_solar_ultraviolet) _IOTDATA_OP_PRINT(print_solar_ultraviolet)
                                                                                      _IOTDATA_OP_JSON_SET(json_set_solar_ultraviolet) _IOTDATA_OP_JSON_GET(json_get_solar_ultraviolet) };
#endif
#endif
#if defined(IOTDATA_ENABLE_SOLAR)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_solar(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_solar_t *const v = vv;
    return pack_solar_irradiance(buf, bb, bp, &v->solar_irradiance) && pack_solar_ultraviolet(buf, bb, bp, &v->solar_ultraviolet);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_solar_by_slot(iotdata_encoder_t *const enc, const int slot, uint16_t irradiance_wm2, uint8_t ultraviolet_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_SOLAR, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (irradiance_wm2 > IOTDATA_SOLAR_IRRADIANCE_MAX)
        return IOTDATA_ERR_SOLAR_IRRADIANCE_HIGH;
    if (ultraviolet_index > IOTDATA_SOLAR_ULTRAVIOLET_MAX)
        return IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH;
#endif
    iotdata_field_solar_t v;
    v.solar_irradiance.solar_irradiance = irradiance_wm2;
    v.solar_ultraviolet.solar_ultraviolet = ultraviolet_index;
    PACK_DIRECT(enc, pack_solar, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_SOLAR);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_solar(iotdata_encoder_t *const enc, uint16_t irradiance_wm2, uint8_t ultraviolet_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_SOLAR, si);
    return iotdata_encode_solar_by_slot(enc, si, irradiance_wm2, ultraviolet_index);
}
#else
iotdata_status_t iotdata_encode_solar(iotdata_encoder_t *const enc, uint16_t irradiance_wm2, uint8_t ultraviolet_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_SOLAR, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (irradiance_wm2 > IOTDATA_SOLAR_IRRADIANCE_MAX)
        return IOTDATA_ERR_SOLAR_IRRADIANCE_HIGH;
    if (ultraviolet_index > IOTDATA_SOLAR_ULTRAVIOLET_MAX)
        return IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH;
#endif
    enc->solar.solar_irradiance.solar_irradiance = irradiance_wm2;
    enc->solar.solar_ultraviolet.solar_ultraviolet = ultraviolet_index;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_SOLAR);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_solar(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_solar_t *const v = vv;
    return unpack_solar_irradiance(buf, bb, bp, &v->solar_irradiance) && unpack_solar_ultraviolet(buf, bb, bp, &v->solar_ultraviolet);
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_solar(iotdata_decoder_t *dec, uint16_t *irradiance_wm2, uint8_t *ultraviolet_index) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_solar_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_SOLAR, unpack_solar, &v);
    if (irradiance_wm2)
        *irradiance_wm2 = v.solar_irradiance.solar_irradiance;
    if (ultraviolet_index)
        *ultraviolet_index = v.solar_ultraviolet.solar_ultraviolet;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_SOLAR);
    if (irradiance_wm2)
        *irradiance_wm2 = dec->solar.solar_irradiance.solar_irradiance;
    if (ultraviolet_index)
        *ultraviolet_index = dec->solar.solar_ultraviolet.solar_ultraviolet;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_solar(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    cJSON *j_irradiance = cJSON_GetObjectItem(j, "irradiance"), *j_ultraviolet = cJSON_GetObjectItem(j, "ultraviolet");
    if (!j_irradiance || !j_ultraviolet)
        return IOTDATA_OK;
    return iotdata_encode_solar(enc, (uint16_t)j_irradiance->valueint, (uint8_t)j_ultraviolet->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_solar(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_solar_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_solar_irradiance(obj, &v->solar_irradiance, "irradiance", scratch);
    json_set_solar_ultraviolet(obj, &v->solar_ultraviolet, "ultraviolet", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_solar(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    n = dump_solar_irradiance(buf, bb, bp, dump, n, label);
    n = dump_solar_ultraviolet(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_solar(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_solar_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " W/m2, UV %" PRIu8 "\n", label, _padd(label), v->solar_irradiance.solar_irradiance, v->solar_ultraviolet.solar_ultraviolet);
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_solar = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, solar)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, solar)) _IOTDATA_OP_NAME("solar")
                                                                  _IOTDATA_OP_BITS(IOTDATA_SOLAR_IRRADIANCE_BITS + IOTDATA_SOLAR_ULTRAVIOLET_BITS) _IOTDATA_OP_PACK(pack_solar) _IOTDATA_OP_UNPACK(unpack_solar) _IOTDATA_OP_DUMP(dump_solar)
                                                                      _IOTDATA_OP_PRINT(print_solar) _IOTDATA_OP_JSON_SET(json_set_solar) _IOTDATA_OP_JSON_GET(json_get_solar) };
#endif

/* =========================================================================
 * Field CLOUDS
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_CLOUDS)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_clouds(uint8_t clouds) {
    return (uint32_t)clouds;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint8_t dequantise_clouds(uint32_t raw) {
    return (uint8_t)raw;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_clouds(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_clouds_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_clouds(v->clouds), IOTDATA_CLOUDS_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_clouds_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t okta) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_CLOUDS, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (okta > IOTDATA_CLOUDS_MAX)
        return IOTDATA_ERR_CLOUDS_HIGH;
#endif
    iotdata_field_clouds_t v;
    v.clouds = okta;
    PACK_DIRECT(enc, pack_clouds, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_CLOUDS);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_clouds(iotdata_encoder_t *const enc, uint8_t okta) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_CLOUDS, si);
    return iotdata_encode_clouds_by_slot(enc, si, okta);
}
#else
iotdata_status_t iotdata_encode_clouds(iotdata_encoder_t *const enc, uint8_t okta) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_CLOUDS, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (okta > IOTDATA_CLOUDS_MAX)
        return IOTDATA_ERR_CLOUDS_HIGH;
#endif
    enc->clouds.clouds = okta;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_CLOUDS);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_clouds(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_clouds_t *const v = vv;
    if (*bp + IOTDATA_CLOUDS_BITS > bb)
        return false;
    v->clouds = dequantise_clouds(bits_read(buf, bb, bp, IOTDATA_CLOUDS_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_clouds(iotdata_decoder_t *dec, uint8_t *okta) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_clouds_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_CLOUDS, unpack_clouds, &v);
    if (okta)
        *okta = v.clouds;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_CLOUDS);
    if (okta)
        *okta = dec->clouds.clouds;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_clouds(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_clouds(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_clouds(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_clouds_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->clouds);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_clouds(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_CLOUDS_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 " okta", dequantise_clouds(r));
    n = dump_add(dump, n, s, IOTDATA_CLOUDS_BITS, r, dump->_dec_buf, "0..8 okta", "clouds");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_clouds(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_clouds_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 " okta\n", label, _padd(label), v->clouds);
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_clouds = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, clouds)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, clouds)) _IOTDATA_OP_NAME("clouds") _IOTDATA_OP_BITS(
    IOTDATA_CLOUDS_BITS) _IOTDATA_OP_PACK(pack_clouds) _IOTDATA_OP_UNPACK(unpack_clouds) _IOTDATA_OP_DUMP(dump_clouds) _IOTDATA_OP_PRINT(print_clouds) _IOTDATA_OP_JSON_SET(json_set_clouds) _IOTDATA_OP_JSON_GET(json_get_clouds) };
#endif

/* =========================================================================
 * Field AIR_QUALITY, AIR_QUALITY_INDEX, AIR_QUALITY_PM, AIR_QUALITY_GAS
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM) || defined(IOTDATA_ENABLE_AIR_QUALITY)
#if !defined(IOTDATA_NO_PRINT) || !defined(IOTDATA_NO_DUMP) || !defined(IOTDATA_NO_JSON)
static const char *_aq_pm_names[IOTDATA_AIR_QUALITY_PM_COUNT] = { "pm1", "pm25", "pm4", "pm10" };
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static const char *_aq_pm_labels[IOTDATA_AIR_QUALITY_PM_COUNT] = { "PM1", "PM2.5", "PM4", "PM10" };
#endif
#endif

#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS) || defined(IOTDATA_ENABLE_AIR_QUALITY)
static const uint8_t _aq_gas_bits[IOTDATA_AIR_QUALITY_GAS_COUNT] = {
    IOTDATA_AIR_QUALITY_GAS_BITS_VOC,  IOTDATA_AIR_QUALITY_GAS_BITS_NOX, IOTDATA_AIR_QUALITY_GAS_BITS_CO2,   IOTDATA_AIR_QUALITY_GAS_BITS_CO,
    IOTDATA_AIR_QUALITY_GAS_BITS_HCHO, IOTDATA_AIR_QUALITY_GAS_BITS_O3,  IOTDATA_AIR_QUALITY_GAS_BITS_RSVD6, IOTDATA_AIR_QUALITY_GAS_BITS_RSVD7,
};
static const uint16_t _aq_gas_res[IOTDATA_AIR_QUALITY_GAS_COUNT] = {
    IOTDATA_AIR_QUALITY_GAS_RES_VOC,  IOTDATA_AIR_QUALITY_GAS_RES_NOX, IOTDATA_AIR_QUALITY_GAS_RES_CO2,   IOTDATA_AIR_QUALITY_GAS_RES_CO,
    IOTDATA_AIR_QUALITY_GAS_RES_HCHO, IOTDATA_AIR_QUALITY_GAS_RES_O3,  IOTDATA_AIR_QUALITY_GAS_RES_RSVD6, IOTDATA_AIR_QUALITY_GAS_RES_RSVD7,
};
/* Already inside the GAS || AIR_QUALITY guard, so asking for GAS again was a NARROWING, and the
   wrong one: the composite's setter reads this table, so with AIR_QUALITY enabled and the part
   not, the table was missing while the code that indexes it was present. Its two siblings above
   never carried that extra condition. */
#if !defined(IOTDATA_NO_ENCODE) && !defined(IOTDATA_NO_CHECKS_TYPES)
static const uint16_t _aq_gas_max[IOTDATA_AIR_QUALITY_GAS_COUNT] = {
    IOTDATA_AIR_QUALITY_GAS_MAX_VOC,  IOTDATA_AIR_QUALITY_GAS_MAX_NOX, IOTDATA_AIR_QUALITY_GAS_MAX_CO2,   IOTDATA_AIR_QUALITY_GAS_MAX_CO,
    IOTDATA_AIR_QUALITY_GAS_MAX_HCHO, IOTDATA_AIR_QUALITY_GAS_MAX_O3,  IOTDATA_AIR_QUALITY_GAS_MAX_RSVD6, IOTDATA_AIR_QUALITY_GAS_MAX_RSVD7,
};
#endif
/* The JSON paths name the gas slots too, so this table is not print/dump's alone -- with JSON on
   and both of those off, the guard it used to carry left it undeclared. */
#if !defined(IOTDATA_NO_PRINT) || !defined(IOTDATA_NO_DUMP) || !defined(IOTDATA_NO_JSON)
static const char *_aq_gas_names[IOTDATA_AIR_QUALITY_GAS_COUNT] = {
    "voc", "nox", "co2", "co", "hcho", "o3", "rsvd6", "rsvd7",
};
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static const char *_aq_gas_labels[IOTDATA_AIR_QUALITY_GAS_COUNT] = {
    "VOC", "NOx", "CO2", "CO", "HCHO", "O3", "rsvd6", "rsvd7",
};
#endif
#if !defined(IOTDATA_NO_PRINT) || !defined(IOTDATA_NO_DUMP)
static const char *_aq_gas_units[IOTDATA_AIR_QUALITY_GAS_COUNT] = {
    "idx", "idx", "ppm", "ppm", "ppb", "ppb", "", "",
};
#endif
#if !defined(IOTDATA_NO_DUMP)
static const char *_aq_gas_range[IOTDATA_AIR_QUALITY_GAS_COUNT] = {
    "0..510, 2 idx", "0..510, 2 idx", "0..51150, 50 ppm", "0..1023, 1 ppm", "0..5115, 5 ppb", "0..1023, 1 ppb", "reserved", "reserved",
};
#endif
#endif

#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX) || defined(IOTDATA_ENABLE_AIR_QUALITY)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_air_quality_index(uint16_t v) {
    return (uint32_t)v;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint16_t dequantise_air_quality_index(uint32_t r) {
    return (uint16_t)r;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_air_quality_index(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_air_quality_index_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_air_quality_index(v->aq_index), IOTDATA_AIR_QUALITY_INDEX_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_air_quality_index_by_slot(iotdata_encoder_t *const enc, const int slot, uint16_t aq_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_AIR_QUALITY_INDEX, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (aq_index > IOTDATA_AIR_QUALITY_INDEX_MAX)
        return IOTDATA_ERR_AIR_QUALITY_INDEX_HIGH;
#endif
    iotdata_field_air_quality_index_t v;
    v.aq_index = aq_index;
    PACK_DIRECT(enc, pack_air_quality_index, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY_INDEX);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_air_quality_index(iotdata_encoder_t *const enc, uint16_t aq_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY_INDEX, si);
    return iotdata_encode_air_quality_index_by_slot(enc, si, aq_index);
}
#else
iotdata_status_t iotdata_encode_air_quality_index(iotdata_encoder_t *const enc, uint16_t aq_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY_INDEX, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (aq_index > IOTDATA_AIR_QUALITY_INDEX_MAX)
        return IOTDATA_ERR_AIR_QUALITY_INDEX_HIGH;
#endif
    enc->air_quality_index.aq_index = aq_index;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY_INDEX);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_air_quality_index(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_air_quality_index_t *const v = vv;
    if (*bp + IOTDATA_AIR_QUALITY_INDEX_BITS > bb)
        return false;
    v->aq_index = dequantise_air_quality_index(bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_INDEX_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_air_quality_index(iotdata_decoder_t *dec, uint16_t *aq_index) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_air_quality_index_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY_INDEX, unpack_air_quality_index, &v);
    if (aq_index)
        *aq_index = v.aq_index;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY_INDEX);
    if (aq_index)
        *aq_index = dec->air_quality_index.aq_index;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_air_quality_index(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_air_quality_index(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_air_quality_index(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_air_quality_index_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->aq_index);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_air_quality_index(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_INDEX_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " AQI", dequantise_air_quality_index(r));
    n = dump_add(dump, n, s, IOTDATA_AIR_QUALITY_INDEX_BITS, r, dump->_dec_buf, "0..500 AQI", "aq_index");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_air_quality_index(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_air_quality_index_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " AQI\n", label, _padd(label), v->aq_index);
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX)
static const iotdata_field_ops_t _iotdata_field_def_air_quality_index = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, air_quality_index)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, air_quality_index))
                                                                              _IOTDATA_OP_NAME("air_quality_index") _IOTDATA_OP_BITS(IOTDATA_AIR_QUALITY_INDEX_BITS) _IOTDATA_OP_PACK(pack_air_quality_index)
                                                                                  _IOTDATA_OP_UNPACK(unpack_air_quality_index) _IOTDATA_OP_DUMP(dump_air_quality_index) _IOTDATA_OP_PRINT(print_air_quality_index)
                                                                                      _IOTDATA_OP_JSON_SET(json_set_air_quality_index) _IOTDATA_OP_JSON_GET(json_get_air_quality_index) };
#endif
#endif

#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM) || defined(IOTDATA_ENABLE_AIR_QUALITY)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_air_quality_pm(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_air_quality_pm_t *const v = vv;
    if (!bits_write(buf, bb, bp, v->aq_pm_present, IOTDATA_AIR_QUALITY_PM_PRESENT_BITS))
        return false;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if (v->aq_pm_present & (1U << i))
            if (!bits_write(buf, bb, bp, v->aq_pm[i] / IOTDATA_AIR_QUALITY_PM_VALUE_RES, IOTDATA_AIR_QUALITY_PM_VALUE_BITS))
                return false;
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_air_quality_pm_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_AIR_QUALITY_PM, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if ((pm_present & (1U << i)) && pm[i] > IOTDATA_AIR_QUALITY_PM_VALUE_MAX)
            return IOTDATA_ERR_AIR_QUALITY_PM_VALUE_HIGH;
#endif
    iotdata_field_air_quality_pm_t v;
    v.aq_pm_present = pm_present & 0x0F;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        v.aq_pm[i] = pm[i];
    PACK_DIRECT(enc, pack_air_quality_pm, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY_PM);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_air_quality_pm(iotdata_encoder_t *const enc, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY_PM, si);
    return iotdata_encode_air_quality_pm_by_slot(enc, si, pm_present, pm);
}
#else
iotdata_status_t iotdata_encode_air_quality_pm(iotdata_encoder_t *const enc, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY_PM, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if ((pm_present & (1U << i)) && pm[i] > IOTDATA_AIR_QUALITY_PM_VALUE_MAX)
            return IOTDATA_ERR_AIR_QUALITY_PM_VALUE_HIGH;
#endif
    enc->air_quality_pm.aq_pm_present = pm_present & 0x0F;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        enc->air_quality_pm.aq_pm[i] = pm[i];
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY_PM);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_air_quality_pm(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_air_quality_pm_t *const v = vv;
    if (*bp + IOTDATA_AIR_QUALITY_PM_PRESENT_BITS > bb)
        return false;
    v->aq_pm_present = (uint8_t)bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_PM_PRESENT_BITS);
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++) {
        if (v->aq_pm_present & (1U << i) && (*bp + IOTDATA_AIR_QUALITY_PM_VALUE_BITS > bb))
            return false;
        v->aq_pm[i] = v->aq_pm_present & (1U << i) ? (uint16_t)(bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_PM_VALUE_BITS) * IOTDATA_AIR_QUALITY_PM_VALUE_RES) : 0;
    }
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_air_quality_pm(iotdata_decoder_t *dec, uint8_t *pm_present, uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_air_quality_pm_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY_PM, unpack_air_quality_pm, &v);
    if (pm_present)
        *pm_present = v.aq_pm_present;
    if (pm)
        memcpy(pm, v.aq_pm, sizeof(v.aq_pm));
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY_PM);
    if (pm_present)
        *pm_present = dec->air_quality_pm.aq_pm_present;
    if (pm)
        memcpy(pm, dec->air_quality_pm.aq_pm, sizeof(dec->air_quality_pm.aq_pm));
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_air_quality_pm(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    uint8_t present = 0;
    uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT] = { 0 };
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++) {
        cJSON *v = cJSON_GetObjectItem(j, _aq_pm_names[i]);
        if (v) {
            present |= (1U << i);
            pm[i] = (uint16_t)v->valueint;
        }
    }
    return iotdata_encode_air_quality_pm(enc, present, pm);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_air_quality_pm(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_air_quality_pm_t *const v = vv;
    (void)scratch;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if (v->aq_pm_present & (1U << i))
            cJSON_AddNumberToObject(obj, _aq_pm_names[i], v->aq_pm[i]);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_air_quality_pm(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t present = bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_PM_PRESENT_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "0x%" PRIX32, present);
    n = dump_add(dump, n, s, IOTDATA_AIR_QUALITY_PM_PRESENT_BITS, present, dump->_dec_buf, "4-bit mask", "aq_pm_present");
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if (present & (1U << i)) {
            s = *bp;
            uint32_t r = bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_PM_VALUE_BITS);
            snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu32 " ug/m3", (uint32_t)(r * IOTDATA_AIR_QUALITY_PM_VALUE_RES));
            n = dump_add(dump, n, s, IOTDATA_AIR_QUALITY_PM_VALUE_BITS, r, dump->_dec_buf, "0..1275, 5 ug/m3", _aq_pm_names[i]);
        }
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_air_quality_pm(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_air_quality_pm_t *const v = vv;
    bprintf(bp, "  %s:%s", label, _padd(label));
    for (int i = 0, first = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if (v->aq_pm_present & (1U << i))
            bprintf(bp, "%s %s=%" PRIu16, first++ ? "," : "", _aq_pm_labels[i], v->aq_pm[i]);
    bprintf(bp, "%s\n", v->aq_pm_present ? " ug/m3" : "");
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM)
static const iotdata_field_ops_t _iotdata_field_def_air_quality_pm = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, air_quality_pm)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, air_quality_pm)) _IOTDATA_OP_NAME("air_quality_pm")
                                                                           _IOTDATA_OP_BITS(0) _IOTDATA_OP_PACK(pack_air_quality_pm) _IOTDATA_OP_UNPACK(unpack_air_quality_pm) _IOTDATA_OP_DUMP(dump_air_quality_pm)
                                                                               _IOTDATA_OP_PRINT(print_air_quality_pm) _IOTDATA_OP_JSON_SET(json_set_air_quality_pm) _IOTDATA_OP_JSON_GET(json_get_air_quality_pm) };
#endif
#endif

#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS) || defined(IOTDATA_ENABLE_AIR_QUALITY)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_air_quality_gas(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_air_quality_gas_t *const v = vv;
    if (!bits_write(buf, bb, bp, v->aq_gas_present, IOTDATA_AIR_QUALITY_GAS_PRESENT_BITS))
        return false;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if (v->aq_gas_present & (1U << i))
            if (!bits_write(buf, bb, bp, v->aq_gas[i] / _aq_gas_res[i], _aq_gas_bits[i]))
                return false;
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_air_quality_gas_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_AIR_QUALITY_GAS, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if ((gas_present & (1U << i)) && gas[i] > _aq_gas_max[i])
            return IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH;
#endif
    iotdata_field_air_quality_gas_t v;
    v.aq_gas_present = gas_present;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        v.aq_gas[i] = gas[i];
    PACK_DIRECT(enc, pack_air_quality_gas, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY_GAS);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_air_quality_gas(iotdata_encoder_t *const enc, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY_GAS, si);
    return iotdata_encode_air_quality_gas_by_slot(enc, si, gas_present, gas);
}
#else
iotdata_status_t iotdata_encode_air_quality_gas(iotdata_encoder_t *const enc, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY_GAS, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if ((gas_present & (1U << i)) && gas[i] > _aq_gas_max[i])
            return IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH;
#endif
    enc->air_quality_gas.aq_gas_present = gas_present;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        enc->air_quality_gas.aq_gas[i] = gas[i];
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY_GAS);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_air_quality_gas(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_air_quality_gas_t *const v = vv;
    if (*bp + IOTDATA_AIR_QUALITY_GAS_PRESENT_BITS > bb)
        return false;
    v->aq_gas_present = (uint8_t)bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_GAS_PRESENT_BITS);
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++) {
        if (v->aq_gas_present & (1U << i) && (*bp + _aq_gas_bits[i] > bb))
            return false;
        v->aq_gas[i] = v->aq_gas_present & (1U << i) ? (uint16_t)(bits_read(buf, bb, bp, _aq_gas_bits[i]) * _aq_gas_res[i]) : 0;
    }
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_air_quality_gas(iotdata_decoder_t *dec, uint8_t *gas_present, uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_air_quality_gas_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY_GAS, unpack_air_quality_gas, &v);
    if (gas_present)
        *gas_present = v.aq_gas_present;
    if (gas)
        memcpy(gas, v.aq_gas, sizeof(v.aq_gas));
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY_GAS);
    if (gas_present)
        *gas_present = dec->air_quality_gas.aq_gas_present;
    if (gas)
        memcpy(gas, dec->air_quality_gas.aq_gas, sizeof(dec->air_quality_gas.aq_gas));
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_air_quality_gas(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    uint8_t present = 0;
    uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT] = { 0 };
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++) {
        cJSON *v = cJSON_GetObjectItem(j, _aq_gas_names[i]);
        if (v) {
            present |= (1U << i);
            gas[i] = (uint16_t)v->valueint;
        }
    }
    return iotdata_encode_air_quality_gas(enc, present, gas);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_air_quality_gas(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_air_quality_gas_t *const v = vv;
    (void)scratch;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if (v->aq_gas_present & (1U << i))
            cJSON_AddNumberToObject(obj, _aq_gas_names[i], v->aq_gas[i]);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_air_quality_gas(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t present = bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_GAS_PRESENT_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "0x%02" PRIX32, present);
    n = dump_add(dump, n, s, IOTDATA_AIR_QUALITY_GAS_PRESENT_BITS, present, dump->_dec_buf, "8-bit mask", "aq_gas_present");
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if (present & (1U << i)) {
            s = *bp;
            uint32_t r = bits_read(buf, bb, bp, _aq_gas_bits[i]);
            snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu32 " %s", r * _aq_gas_res[i], _aq_gas_units[i]);
            n = dump_add(dump, n, s, _aq_gas_bits[i], r, dump->_dec_buf, _aq_gas_range[i], _aq_gas_names[i]);
        }
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_air_quality_gas(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_air_quality_gas_t *const v = vv;
    bprintf(bp, "  %s:%s", label, _padd(label));
    for (int i = 0, first = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if (v->aq_gas_present & (1U << i))
            bprintf(bp, "%s %s=%" PRIu16 "%s%s", first++ ? "," : "", _aq_gas_labels[i], v->aq_gas[i], _aq_gas_units[i][0] ? " " : "", _aq_gas_units[i][0] ? _aq_gas_units[i] : "");
    bprintf(bp, "\n");
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS)
static const iotdata_field_ops_t _iotdata_field_def_air_quality_gas = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, air_quality_gas)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, air_quality_gas)) _IOTDATA_OP_NAME("air_quality_gas")
                                                                            _IOTDATA_OP_BITS(0) _IOTDATA_OP_PACK(pack_air_quality_gas) _IOTDATA_OP_UNPACK(unpack_air_quality_gas) _IOTDATA_OP_DUMP(dump_air_quality_gas)
                                                                                _IOTDATA_OP_PRINT(print_air_quality_gas) _IOTDATA_OP_JSON_SET(json_set_air_quality_gas) _IOTDATA_OP_JSON_GET(json_get_air_quality_gas) };
#endif
#endif

#if defined(IOTDATA_ENABLE_AIR_QUALITY)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_air_quality(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_air_quality_t *const v = vv;
    return pack_air_quality_index(buf, bb, bp, &v->air_quality_index) && pack_air_quality_pm(buf, bb, bp, &v->air_quality_pm) && pack_air_quality_gas(buf, bb, bp, &v->air_quality_gas);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_air_quality_by_slot(iotdata_encoder_t *const enc, const int slot, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present,
                                                    const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_AIR_QUALITY, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (aq_index > IOTDATA_AIR_QUALITY_INDEX_MAX)
        return IOTDATA_ERR_AIR_QUALITY_INDEX_HIGH;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if ((pm_present & (1U << i)) && pm[i] > IOTDATA_AIR_QUALITY_PM_VALUE_MAX)
            return IOTDATA_ERR_AIR_QUALITY_PM_VALUE_HIGH;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if ((gas_present & (1U << i)) && gas[i] > _aq_gas_max[i])
            return IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH;
#endif
    iotdata_field_air_quality_t v;
    v.air_quality_index.aq_index = aq_index;
    v.air_quality_pm.aq_pm_present = pm_present & 0x0F;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        v.air_quality_pm.aq_pm[i] = pm[i];
    v.air_quality_gas.aq_gas_present = gas_present;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        v.air_quality_gas.aq_gas[i] = gas[i];
    PACK_DIRECT(enc, pack_air_quality, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_air_quality(iotdata_encoder_t *const enc, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY, si);
    return iotdata_encode_air_quality_by_slot(enc, si, aq_index, pm_present, pm, gas_present, gas);
}
#else
iotdata_status_t iotdata_encode_air_quality(iotdata_encoder_t *const enc, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (aq_index > IOTDATA_AIR_QUALITY_INDEX_MAX)
        return IOTDATA_ERR_AIR_QUALITY_INDEX_HIGH;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if ((pm_present & (1U << i)) && pm[i] > IOTDATA_AIR_QUALITY_PM_VALUE_MAX)
            return IOTDATA_ERR_AIR_QUALITY_PM_VALUE_HIGH;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if ((gas_present & (1U << i)) && gas[i] > _aq_gas_max[i])
            return IOTDATA_ERR_AIR_QUALITY_GAS_VALUE_HIGH;
#endif
    enc->air_quality.air_quality_index.aq_index = aq_index;
    enc->air_quality.air_quality_pm.aq_pm_present = pm_present & 0x0F;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        enc->air_quality.air_quality_pm.aq_pm[i] = pm[i];
    enc->air_quality.air_quality_gas.aq_gas_present = gas_present;
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        enc->air_quality.air_quality_gas.aq_gas[i] = gas[i];
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_air_quality(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_air_quality_t *const v = vv;
    return unpack_air_quality_index(buf, bb, bp, &v->air_quality_index) && unpack_air_quality_pm(buf, bb, bp, &v->air_quality_pm) && unpack_air_quality_gas(buf, bb, bp, &v->air_quality_gas);
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_air_quality(iotdata_decoder_t *dec, uint16_t *aq_index, uint8_t *pm_present, uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t *gas_present, uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_air_quality_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY, unpack_air_quality, &v);
    if (aq_index)
        *aq_index = v.air_quality_index.aq_index;
    if (pm_present)
        *pm_present = v.air_quality_pm.aq_pm_present;
    if (pm)
        memcpy(pm, v.air_quality_pm.aq_pm, sizeof(v.air_quality_pm.aq_pm));
    if (gas_present)
        *gas_present = v.air_quality_gas.aq_gas_present;
    if (gas)
        memcpy(gas, v.air_quality_gas.aq_gas, sizeof(v.air_quality_gas.aq_gas));
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY);
    if (aq_index)
        *aq_index = dec->air_quality.air_quality_index.aq_index;
    if (pm_present)
        *pm_present = dec->air_quality.air_quality_pm.aq_pm_present;
    if (pm)
        memcpy(pm, dec->air_quality.air_quality_pm.aq_pm, sizeof(dec->air_quality.air_quality_pm.aq_pm));
    if (gas_present)
        *gas_present = dec->air_quality.air_quality_gas.aq_gas_present;
    if (gas)
        memcpy(gas, dec->air_quality.air_quality_gas.aq_gas, sizeof(dec->air_quality.air_quality_gas.aq_gas));
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_air_quality(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    /* Extract index */
    uint16_t idx = 0;
    cJSON *ji = cJSON_GetObjectItem(j, "index");
    if (ji)
        idx = (uint16_t)ji->valueint;
    /* Extract PM */
    uint8_t pm_present = 0;
    uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT] = { 0 };
    cJSON *jp = cJSON_GetObjectItem(j, "pm");
    if (jp)
        for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++) {
            cJSON *v = cJSON_GetObjectItem(jp, _aq_pm_names[i]);
            if (v) {
                pm_present |= (1U << i);
                pm[i] = (uint16_t)v->valueint;
            }
        }
    /* Extract gas */
    uint8_t gas_present = 0;
    uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT] = { 0 };
    cJSON *jg = cJSON_GetObjectItem(j, "gas");
    if (jg)
        for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++) {
            cJSON *v = cJSON_GetObjectItem(jg, _aq_gas_names[i]);
            if (v) {
                gas_present |= (1U << i);
                gas[i] = (uint16_t)v->valueint;
            }
        }
    return iotdata_encode_air_quality(enc, idx, pm_present, pm, gas_present, gas);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_air_quality(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_air_quality_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_air_quality_index(obj, &v->air_quality_index, "index", scratch);
    json_set_air_quality_pm(obj, &v->air_quality_pm, "pm", scratch);
    json_set_air_quality_gas(obj, &v->air_quality_gas, "gas", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_air_quality(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    n = dump_air_quality_index(buf, bb, bp, dump, n, label);
    n = dump_air_quality_pm(buf, bb, bp, dump, n, label);
    n = dump_air_quality_gas(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_air_quality(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_air_quality_t *const v = vv;
    print_air_quality_index(&v->air_quality_index, bp, label);
    print_air_quality_pm(&v->air_quality_pm, bp, label);
    print_air_quality_gas(&v->air_quality_gas, bp, label);
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_air_quality = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, air_quality)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, air_quality)) _IOTDATA_OP_NAME("air_quality")
                                                                        _IOTDATA_OP_BITS(0) _IOTDATA_OP_PACK(pack_air_quality) _IOTDATA_OP_UNPACK(unpack_air_quality) _IOTDATA_OP_DUMP(dump_air_quality) _IOTDATA_OP_PRINT(print_air_quality)
                                                                            _IOTDATA_OP_JSON_SET(json_set_air_quality) _IOTDATA_OP_JSON_GET(json_get_air_quality) };
#endif

/* =========================================================================
 * Field RADIATION, RADIATION_CPM, RADIATION_DOSE
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_RADIATION_CPM) || defined(IOTDATA_ENABLE_RADIATION)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_radiation_cpm(uint16_t cpm) {
    return (uint32_t)cpm;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint16_t dequantise_radiation_cpm(uint32_t raw) {
    return (uint16_t)raw;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_radiation_cpm(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_radiation_cpm_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_radiation_cpm(v->radiation_cpm), IOTDATA_RADIATION_CPM_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_CPM) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_radiation_cpm_by_slot(iotdata_encoder_t *const enc, const int slot, uint16_t cpm) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_RADIATION_CPM, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (cpm > IOTDATA_RADIATION_CPM_MAX)
        return IOTDATA_ERR_RADIATION_CPM_HIGH;
#endif
    iotdata_field_radiation_cpm_t v;
    v.radiation_cpm = cpm;
    PACK_DIRECT(enc, pack_radiation_cpm, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RADIATION_CPM);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_radiation_cpm(iotdata_encoder_t *const enc, uint16_t cpm) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION_CPM, si);
    return iotdata_encode_radiation_cpm_by_slot(enc, si, cpm);
}
#else
iotdata_status_t iotdata_encode_radiation_cpm(iotdata_encoder_t *const enc, uint16_t cpm) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION_CPM, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (cpm > IOTDATA_RADIATION_CPM_MAX)
        return IOTDATA_ERR_RADIATION_CPM_HIGH;
#endif
    enc->radiation_cpm.radiation_cpm = cpm;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RADIATION_CPM);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_radiation_cpm(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_radiation_cpm_t *const v = vv;
    if (*bp + IOTDATA_RADIATION_CPM_BITS > bb)
        return false;
    v->radiation_cpm = dequantise_radiation_cpm(bits_read(buf, bb, bp, IOTDATA_RADIATION_CPM_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_CPM) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_radiation_cpm(iotdata_decoder_t *dec, uint16_t *cpm) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_radiation_cpm_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_RADIATION_CPM, unpack_radiation_cpm, &v);
    if (cpm)
        *cpm = v.radiation_cpm;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_RADIATION_CPM);
    if (cpm)
        *cpm = dec->radiation_cpm.radiation_cpm;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_CPM) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_radiation_cpm(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_radiation_cpm(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_radiation_cpm(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_radiation_cpm_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->radiation_cpm);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_radiation_cpm(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_RADIATION_CPM_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " CPM", dequantise_radiation_cpm(r));
    n = dump_add(dump, n, s, IOTDATA_RADIATION_CPM_BITS, r, dump->_dec_buf, "0..65535 CPM", "radiation_cpm");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_CPM) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_radiation_cpm(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_radiation_cpm_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " CPM\n", label, _padd(label), v->radiation_cpm);
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_CPM)
static const iotdata_field_ops_t _iotdata_field_def_radiation_cpm = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, radiation_cpm)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, radiation_cpm)) _IOTDATA_OP_NAME("radiation_cpm")
                                                                          _IOTDATA_OP_BITS(IOTDATA_RADIATION_CPM_BITS) _IOTDATA_OP_PACK(pack_radiation_cpm) _IOTDATA_OP_UNPACK(unpack_radiation_cpm) _IOTDATA_OP_DUMP(dump_radiation_cpm)
                                                                              _IOTDATA_OP_PRINT(print_radiation_cpm) _IOTDATA_OP_JSON_SET(json_set_radiation_cpm) _IOTDATA_OP_JSON_GET(json_get_radiation_cpm) };
#endif
#endif

#if defined(IOTDATA_ENABLE_RADIATION_DOSE) || defined(IOTDATA_ENABLE_RADIATION)
#if !defined(IOTDATA_NO_FLOATING)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_radiation_dose(float dose) {
    return (uint32_t)roundf(dose / IOTDATA_RADIATION_DOSE_RES);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static float dequantise_radiation_dose(uint32_t raw) {
    return (float)raw * IOTDATA_RADIATION_DOSE_RES;
}
#endif
#else
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_radiation_dose(int32_t dose100) {
    return (uint32_t)dose100;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int32_t dequantise_radiation_dose(uint32_t raw) {
    return (int32_t)raw;
}
#endif
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_radiation_dose(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_radiation_dose_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_radiation_dose(v->radiation_dose), IOTDATA_RADIATION_DOSE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_DOSE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_radiation_dose_by_slot(iotdata_encoder_t *const enc, const int slot, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_RADIATION_DOSE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (usvh < 0 || usvh > IOTDATA_RADIATION_DOSE_MAX)
        return IOTDATA_ERR_RADIATION_DOSE_HIGH;
#endif
    iotdata_field_radiation_dose_t v;
    v.radiation_dose = usvh;
    PACK_DIRECT(enc, pack_radiation_dose, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RADIATION_DOSE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_radiation_dose(iotdata_encoder_t *const enc, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION_DOSE, si);
    return iotdata_encode_radiation_dose_by_slot(enc, si, usvh);
}
#else
iotdata_status_t iotdata_encode_radiation_dose(iotdata_encoder_t *const enc, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION_DOSE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (usvh < 0 || usvh > IOTDATA_RADIATION_DOSE_MAX)
        return IOTDATA_ERR_RADIATION_DOSE_HIGH;
#endif
    enc->radiation_dose.radiation_dose = usvh;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RADIATION_DOSE);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_radiation_dose(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_radiation_dose_t *const v = vv;
    if (*bp + IOTDATA_RADIATION_DOSE_BITS > bb)
        return false;
    v->radiation_dose = dequantise_radiation_dose(bits_read(buf, bb, bp, IOTDATA_RADIATION_DOSE_BITS));
    return true;
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_DOSE) && !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_radiation_dose(iotdata_decoder_t *dec, iotdata_float_t *usvh) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_radiation_dose_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_RADIATION_DOSE, unpack_radiation_dose, &v);
    if (usvh)
        *usvh = v.radiation_dose;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_RADIATION_DOSE);
    if (usvh)
        *usvh = dec->radiation_dose.radiation_dose;
#endif
    return IOTDATA_OK;
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_DOSE) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_radiation_dose(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_radiation_dose(enc, (iotdata_float_t)j->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_radiation_dose(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_radiation_dose_t *const v = vv;
    (void)scratch;
#if !defined(IOTDATA_NO_FLOATING)
    json_add_number_fixed(root, label, (double)v->radiation_dose, 2);
#else
    cJSON_AddNumberToObject(root, label, v->radiation_dose);
#endif
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_radiation_dose(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_RADIATION_DOSE_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.2f uSv/h", (double)dequantise_radiation_dose(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_radiation_dose(r), 100, "uSv/h");
#endif
    n = dump_add(dump, n, s, IOTDATA_RADIATION_DOSE_BITS, r, dump->_dec_buf, "0..163.83, 0.01", "radiation_dose");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_DOSE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_radiation_dose(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_radiation_dose_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.1f uSv/h\n", label, _padd(label), (double)v->radiation_dose);
#else
    bprintf(bp, "  %s:%s %" PRId32 ".%02" PRId32 " uSv/h\n", label, _padd(label), v->radiation_dose / 100, v->radiation_dose % 100);
#endif
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_DOSE)
static const iotdata_field_ops_t _iotdata_field_def_radiation_dose = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, radiation_dose)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, radiation_dose)) _IOTDATA_OP_NAME("radiation_dose")
                                                                           _IOTDATA_OP_BITS(IOTDATA_RADIATION_DOSE_BITS) _IOTDATA_OP_PACK(pack_radiation_dose) _IOTDATA_OP_UNPACK(unpack_radiation_dose) _IOTDATA_OP_DUMP(dump_radiation_dose)
                                                                               _IOTDATA_OP_PRINT(print_radiation_dose) _IOTDATA_OP_JSON_SET(json_set_radiation_dose) _IOTDATA_OP_JSON_GET(json_get_radiation_dose) };
#endif
#endif

#if defined(IOTDATA_ENABLE_RADIATION)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_radiation(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_radiation_t *const v = vv;
    return pack_radiation_cpm(buf, bb, bp, &v->radiation_cpm) && pack_radiation_dose(buf, bb, bp, &v->radiation_dose);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_radiation_by_slot(iotdata_encoder_t *const enc, const int slot, uint16_t cpm, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_RADIATION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (cpm > IOTDATA_RADIATION_CPM_MAX)
        return IOTDATA_ERR_RADIATION_CPM_HIGH;
    if (usvh < 0 || usvh > IOTDATA_RADIATION_DOSE_MAX)
        return IOTDATA_ERR_RADIATION_DOSE_HIGH;
#endif
    iotdata_field_radiation_t v;
    v.radiation_cpm.radiation_cpm = cpm;
    v.radiation_dose.radiation_dose = usvh;
    PACK_DIRECT(enc, pack_radiation, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RADIATION);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_radiation(iotdata_encoder_t *const enc, uint16_t cpm, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION, si);
    return iotdata_encode_radiation_by_slot(enc, si, cpm, usvh);
}
#else
iotdata_status_t iotdata_encode_radiation(iotdata_encoder_t *const enc, uint16_t cpm, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (cpm > IOTDATA_RADIATION_CPM_MAX)
        return IOTDATA_ERR_RADIATION_CPM_HIGH;
    if (usvh < 0 || usvh > IOTDATA_RADIATION_DOSE_MAX)
        return IOTDATA_ERR_RADIATION_DOSE_HIGH;
#endif
    enc->radiation.radiation_cpm.radiation_cpm = cpm;
    enc->radiation.radiation_dose.radiation_dose = usvh;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RADIATION);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_radiation(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_radiation_t *const v = vv;
    return unpack_radiation_cpm(buf, bb, bp, &v->radiation_cpm) && unpack_radiation_dose(buf, bb, bp, &v->radiation_dose);
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_radiation(iotdata_decoder_t *dec, uint16_t *cpm, iotdata_float_t *usvh) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_radiation_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_RADIATION, unpack_radiation, &v);
    if (cpm)
        *cpm = v.radiation_cpm.radiation_cpm;
    if (usvh)
        *usvh = v.radiation_dose.radiation_dose;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_RADIATION);
    if (cpm)
        *cpm = dec->radiation.radiation_cpm.radiation_cpm;
    if (usvh)
        *usvh = dec->radiation.radiation_dose.radiation_dose;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_radiation(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    cJSON *j_cpm = cJSON_GetObjectItem(j, "cpm"), *j_dose = cJSON_GetObjectItem(j, "dose");
    if (!j_cpm || !j_dose)
        return IOTDATA_OK;
    return iotdata_encode_radiation(enc, (uint16_t)j_cpm->valueint, (iotdata_float_t)j_dose->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_radiation(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_radiation_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_radiation_cpm(obj, &v->radiation_cpm, "cpm", scratch);
    json_set_radiation_dose(obj, &v->radiation_dose, "dose", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_radiation(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    n = dump_radiation_cpm(buf, bb, bp, dump, n, label);
    n = dump_radiation_dose(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_radiation(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_radiation_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %" PRIu16 " CPM, %.2f uSv/h\n", label, _padd(label), v->radiation_cpm.radiation_cpm, (double)v->radiation_dose.radiation_dose);
#else
    bprintf(bp, "  %s:%s %" PRIu16 " CPM, %" PRId32 ".%02" PRId32 " uSv/h\n", label, _padd(label), v->radiation_cpm.radiation_cpm, v->radiation_dose.radiation_dose / 100, v->radiation_dose.radiation_dose % 100);
#endif
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_radiation = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, radiation)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, radiation)) _IOTDATA_OP_NAME("radiation")
                                                                      _IOTDATA_OP_BITS(IOTDATA_RADIATION_CPM_BITS + IOTDATA_RADIATION_DOSE_BITS) _IOTDATA_OP_PACK(pack_radiation) _IOTDATA_OP_UNPACK(unpack_radiation)
                                                                          _IOTDATA_OP_DUMP(dump_radiation) _IOTDATA_OP_PRINT(print_radiation) _IOTDATA_OP_JSON_SET(json_set_radiation) _IOTDATA_OP_JSON_GET(json_get_radiation) };
#endif

/* =========================================================================
 * Field DEPTH
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_DEPTH)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_depth(uint16_t depth) {
    return (uint32_t)depth;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint16_t dequantise_depth(uint32_t raw) {
    return (uint16_t)raw;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_depth(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_depth_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_depth(v->depth), IOTDATA_DEPTH_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_depth_by_slot(iotdata_encoder_t *const enc, const int slot, uint16_t depth_cm) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_DEPTH, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (depth_cm > IOTDATA_DEPTH_MAX)
        return IOTDATA_ERR_DEPTH_HIGH;
#endif
    iotdata_field_depth_t v;
    v.depth = depth_cm;
    PACK_DIRECT(enc, pack_depth, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_DEPTH);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_depth(iotdata_encoder_t *const enc, uint16_t depth_cm) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_DEPTH, si);
    return iotdata_encode_depth_by_slot(enc, si, depth_cm);
}
#else
iotdata_status_t iotdata_encode_depth(iotdata_encoder_t *const enc, uint16_t depth_cm) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_DEPTH, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (depth_cm > IOTDATA_DEPTH_MAX)
        return IOTDATA_ERR_DEPTH_HIGH;
#endif
    enc->depth.depth = depth_cm;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_DEPTH);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_depth(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_depth_t *const v = vv;
    if (*bp + IOTDATA_DEPTH_BITS > bb)
        return false;
    v->depth = dequantise_depth(bits_read(buf, bb, bp, IOTDATA_DEPTH_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_depth(iotdata_decoder_t *dec, uint16_t *depth_cm) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_depth_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_DEPTH, unpack_depth, &v);
    if (depth_cm)
        *depth_cm = v.depth;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_DEPTH);
    if (depth_cm)
        *depth_cm = dec->depth.depth;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_depth(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_depth(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_depth(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_depth_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->depth);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_depth(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_DEPTH_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " cm", dequantise_depth(r));
    n = dump_add(dump, n, s, IOTDATA_DEPTH_BITS, r, dump->_dec_buf, "0..1023 cm", "depth");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_depth(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_depth_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " cm\n", label, _padd(label), v->depth);
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_depth = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, depth)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, depth)) _IOTDATA_OP_NAME("depth") _IOTDATA_OP_BITS(
    IOTDATA_DEPTH_BITS) _IOTDATA_OP_PACK(pack_depth) _IOTDATA_OP_UNPACK(unpack_depth) _IOTDATA_OP_DUMP(dump_depth) _IOTDATA_OP_PRINT(print_depth) _IOTDATA_OP_JSON_SET(json_set_depth) _IOTDATA_OP_JSON_GET(json_get_depth) };
#endif

/* =========================================================================
 * Field POSITION (LATITUDE, LONGITUDE)
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_POSITION)
#if !defined(IOTDATA_NO_FLOATING)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_position_lat(iotdata_double_t lat) {
#if !defined(IOTDATA_NO_FLOATING_DOUBLES)
    return (uint32_t)round((lat + (iotdata_double_t)IOTDATA_POSITION_LAT_OFFSET) / (iotdata_double_t)IOTDATA_POSITION_LAT_RANGE * (iotdata_double_t)IOTDATA_POSITION_SCALE);
#else
    return (uint32_t)roundf((lat + (iotdata_double_t)IOTDATA_POSITION_LAT_OFFSET) / (iotdata_double_t)IOTDATA_POSITION_LAT_RANGE * (iotdata_double_t)IOTDATA_POSITION_SCALE);
#endif
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static iotdata_double_t dequantise_position_lat(uint32_t raw) {
    return (iotdata_double_t)raw / (iotdata_double_t)IOTDATA_POSITION_SCALE * (iotdata_double_t)IOTDATA_POSITION_LAT_RANGE - (iotdata_double_t)IOTDATA_POSITION_LAT_OFFSET;
}
#endif
#else
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_position_lat(int32_t lat7) {
    return (uint32_t)((((int64_t)lat7 + IOTDATA_POSITION_LAT_OFFSET_I) * IOTDATA_POSITION_SCALE + IOTDATA_POSITION_LAT_OFFSET_I) / IOTDATA_POSITION_LAT_RANGE_I);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int32_t dequantise_position_lat(uint32_t raw) {
    return (int32_t)(((int64_t)raw * IOTDATA_POSITION_LAT_RANGE_I + IOTDATA_POSITION_SCALE / 2) / IOTDATA_POSITION_SCALE - IOTDATA_POSITION_LAT_OFFSET_I);
}
#endif
#endif
#if !defined(IOTDATA_NO_FLOATING)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_position_lon(iotdata_double_t lon) {
#if !defined(IOTDATA_NO_FLOATING_DOUBLES)
    return (uint32_t)round((lon + (iotdata_double_t)IOTDATA_POSITION_LON_OFFSET) / (iotdata_double_t)IOTDATA_POSITION_LON_RANGE * (iotdata_double_t)IOTDATA_POSITION_SCALE);
#else
    return (uint32_t)roundf((lon + (iotdata_double_t)IOTDATA_POSITION_LON_OFFSET) / (iotdata_double_t)IOTDATA_POSITION_LON_RANGE * (iotdata_double_t)IOTDATA_POSITION_SCALE);
#endif
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static iotdata_double_t dequantise_position_lon(uint32_t raw) {
    return (iotdata_double_t)raw / (iotdata_double_t)IOTDATA_POSITION_SCALE * (iotdata_double_t)IOTDATA_POSITION_LON_RANGE - (iotdata_double_t)IOTDATA_POSITION_LON_OFFSET;
}
#endif
#else
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_position_lon(int32_t lon7) {
    return (uint32_t)((((int64_t)lon7 + IOTDATA_POSITION_LON_OFFSET_I) * IOTDATA_POSITION_SCALE + IOTDATA_POSITION_LON_OFFSET_I) / IOTDATA_POSITION_LON_RANGE_I);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int32_t dequantise_position_lon(uint32_t raw) {
    return (int32_t)(((int64_t)raw * IOTDATA_POSITION_LON_RANGE_I + IOTDATA_POSITION_SCALE / 2) / IOTDATA_POSITION_SCALE - IOTDATA_POSITION_LON_OFFSET_I);
}
#endif
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_position(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_position_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_position_lat(v->position_lat), IOTDATA_POSITION_LAT_BITS) && bits_write(buf, bb, bp, quantise_position_lon(v->position_lon), IOTDATA_POSITION_LON_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_position_by_slot(iotdata_encoder_t *const enc, const int slot, iotdata_double_t latitude, iotdata_double_t longitude) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_POSITION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (latitude < (iotdata_double_t)IOTDATA_POSITION_LAT_LOW)
        return IOTDATA_ERR_POSITION_LAT_LOW;
    if (latitude > (iotdata_double_t)IOTDATA_POSITION_LAT_HIGH)
        return IOTDATA_ERR_POSITION_LAT_HIGH;
    if (longitude < (iotdata_double_t)IOTDATA_POSITION_LON_LOW)
        return IOTDATA_ERR_POSITION_LON_LOW;
    if (longitude > (iotdata_double_t)IOTDATA_POSITION_LON_HIGH)
        return IOTDATA_ERR_POSITION_LON_HIGH;
#endif
    iotdata_field_position_t v;
    v.position_lat = latitude;
    v.position_lon = longitude;
    PACK_DIRECT(enc, pack_position, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_POSITION);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_position(iotdata_encoder_t *const enc, iotdata_double_t latitude, iotdata_double_t longitude) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_POSITION, si);
    return iotdata_encode_position_by_slot(enc, si, latitude, longitude);
}
#else
iotdata_status_t iotdata_encode_position(iotdata_encoder_t *const enc, iotdata_double_t latitude, iotdata_double_t longitude) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_POSITION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (latitude < (iotdata_double_t)IOTDATA_POSITION_LAT_LOW)
        return IOTDATA_ERR_POSITION_LAT_LOW;
    if (latitude > (iotdata_double_t)IOTDATA_POSITION_LAT_HIGH)
        return IOTDATA_ERR_POSITION_LAT_HIGH;
    if (longitude < (iotdata_double_t)IOTDATA_POSITION_LON_LOW)
        return IOTDATA_ERR_POSITION_LON_LOW;
    if (longitude > (iotdata_double_t)IOTDATA_POSITION_LON_HIGH)
        return IOTDATA_ERR_POSITION_LON_HIGH;
#endif
    enc->position.position_lat = latitude;
    enc->position.position_lon = longitude;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_POSITION);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_position(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_position_t *const v = vv;
    if (*bp + IOTDATA_POSITION_LAT_BITS + IOTDATA_POSITION_LON_BITS > bb)
        return false;
    v->position_lat = dequantise_position_lat(bits_read(buf, bb, bp, IOTDATA_POSITION_LAT_BITS));
    v->position_lon = dequantise_position_lon(bits_read(buf, bb, bp, IOTDATA_POSITION_LON_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_position(iotdata_decoder_t *dec, iotdata_double_t *latitude, iotdata_double_t *longitude) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_position_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_POSITION, unpack_position, &v);
    if (latitude)
        *latitude = v.position_lat;
    if (longitude)
        *longitude = v.position_lon;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_POSITION);
    if (latitude)
        *latitude = dec->position.position_lat;
    if (longitude)
        *longitude = dec->position.position_lon;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_position(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    cJSON *j_latitude = cJSON_GetObjectItem(j, "latitude"), *j_longitude = cJSON_GetObjectItem(j, "longitude");
    if (!j_latitude || !j_longitude)
        return IOTDATA_OK;
    return iotdata_encode_position(enc, (iotdata_double_t)j_latitude->valuedouble, (iotdata_double_t)j_longitude->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_position(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_position_t *const v = vv;
    (void)scratch;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
#if !defined(IOTDATA_NO_FLOATING)
    json_add_number_fixed(obj, "latitude", (double)v->position_lat, 6);
    json_add_number_fixed(obj, "longitude", (double)v->position_lon, 6);
#else
    cJSON_AddNumberToObject(obj, "latitude", v->position_lat);
    cJSON_AddNumberToObject(obj, "longitude", v->position_lon);
#endif
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_position(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_POSITION_LAT_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.6f", (double)dequantise_position_lat(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_position_lat(r), 10000000, "");
#endif
    n = dump_add(dump, n, s, IOTDATA_POSITION_LAT_BITS, r, dump->_dec_buf, "-90..+90", "latitude");
    s = *bp;
    r = bits_read(buf, bb, bp, IOTDATA_POSITION_LON_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.6f", (double)dequantise_position_lon(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_position_lon(r), 10000000, "");
#endif
    n = dump_add(dump, n, s, IOTDATA_POSITION_LON_BITS, r, dump->_dec_buf, "-180..+180", "longitude");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_position(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_position_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.6f, %.6f\n", label, _padd(label), (double)v->position_lat, (double)v->position_lon);
#else
    const int32_t lat = v->position_lat, la = lat < 0 ? -lat : lat, lon = v->position_lon, lo = lon < 0 ? -lon : lon;
    bprintf(bp, "  %s:%s %s%d.%06d, %s%d.%06d\n", label, _padd(label), lat < 0 ? "-" : "", (int)(la / 10000000), (int)(la % 10000000), lon < 0 ? "-" : "", (int)(lo / 10000000), (int)(lo % 10000000));
#endif
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_position = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, position)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, position)) _IOTDATA_OP_NAME("position")
                                                                     _IOTDATA_OP_BITS(IOTDATA_POSITION_LAT_BITS + IOTDATA_POSITION_LON_BITS) _IOTDATA_OP_PACK(pack_position) _IOTDATA_OP_UNPACK(unpack_position) _IOTDATA_OP_DUMP(dump_position)
                                                                         _IOTDATA_OP_PRINT(print_position) _IOTDATA_OP_JSON_SET(json_set_position) _IOTDATA_OP_JSON_GET(json_get_position) };
#endif

/* =========================================================================
 * Field DATETIME
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_DATETIME)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_datetime(uint32_t datetime) {
    return (uint32_t)(datetime / IOTDATA_DATETIME_RES);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint32_t dequantise_datetime(uint32_t raw) {
    return (uint32_t)(raw * IOTDATA_DATETIME_RES);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_datetime(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_datetime_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_datetime(v->datetime_secs), IOTDATA_DATETIME_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_datetime_by_slot(iotdata_encoder_t *const enc, const int slot, uint32_t seconds_from_year_start) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_DATETIME, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if ((seconds_from_year_start / IOTDATA_DATETIME_RES) > IOTDATA_DATETIME_MAX)
        return IOTDATA_ERR_DATETIME_HIGH;
#endif
    iotdata_field_datetime_t v;
    v.datetime_secs = seconds_from_year_start;
    PACK_DIRECT(enc, pack_datetime, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_DATETIME);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_datetime(iotdata_encoder_t *const enc, uint32_t seconds_from_year_start) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_DATETIME, si);
    return iotdata_encode_datetime_by_slot(enc, si, seconds_from_year_start);
}
#else
iotdata_status_t iotdata_encode_datetime(iotdata_encoder_t *const enc, uint32_t seconds_from_year_start) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_DATETIME, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if ((seconds_from_year_start / IOTDATA_DATETIME_RES) > IOTDATA_DATETIME_MAX)
        return IOTDATA_ERR_DATETIME_HIGH;
#endif
    enc->datetime.datetime_secs = seconds_from_year_start;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_DATETIME);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_datetime(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_datetime_t *const v = vv;
    if (*bp + IOTDATA_DATETIME_BITS > bb)
        return false;
    v->datetime_secs = dequantise_datetime(bits_read(buf, bb, bp, IOTDATA_DATETIME_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_datetime(iotdata_decoder_t *dec, uint32_t *seconds_from_year_start) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_datetime_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_DATETIME, unpack_datetime, &v);
    if (seconds_from_year_start)
        *seconds_from_year_start = v.datetime_secs;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_DATETIME);
    if (seconds_from_year_start)
        *seconds_from_year_start = dec->datetime.datetime_secs;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_datetime(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_datetime(enc, (uint32_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_datetime(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_datetime_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->datetime_secs);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_datetime(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_DATETIME_BITS);
    const uint32_t secs = dequantise_datetime(r);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "day %" PRIu32 " %02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 " (%" PRIu32 "s)", secs / 86400, (secs % 86400) / 3600, (secs % 3600) / 60, secs % 60, secs);
    n = dump_add(dump, n, s, IOTDATA_DATETIME_BITS, r, dump->_dec_buf, "5s res", "datetime");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_datetime(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_datetime_t *const v = vv;
    bprintf(bp, "  %s:%s day %" PRIu32 " %02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 " (%" PRIu32 "s)\n", label, _padd(label), v->datetime_secs / 86400, (v->datetime_secs % 86400) / 3600, (v->datetime_secs % 3600) / 60, v->datetime_secs % 60,
            v->datetime_secs);
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_datetime = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, datetime)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, datetime)) _IOTDATA_OP_NAME("datetime")
                                                                     _IOTDATA_OP_BITS(IOTDATA_DATETIME_BITS) _IOTDATA_OP_PACK(pack_datetime) _IOTDATA_OP_UNPACK(unpack_datetime) _IOTDATA_OP_DUMP(dump_datetime)
                                                                         _IOTDATA_OP_PRINT(print_datetime) _IOTDATA_OP_JSON_SET(json_set_datetime) _IOTDATA_OP_JSON_GET(json_get_datetime) };
#endif

/* =========================================================================
 * Field FLAGS
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_FLAGS)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_flags(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_flags_t *const v = vv;
    return bits_write(buf, bb, bp, v->flags, IOTDATA_FLAGS_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_flags_by_slot(iotdata_encoder_t *const enc, const int slot, uint8_t flags) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_FLAGS, si);
    iotdata_field_flags_t v;
    v.flags = flags;
    PACK_DIRECT(enc, pack_flags, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_FLAGS);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_flags(iotdata_encoder_t *const enc, uint8_t flags) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_FLAGS, si);
    return iotdata_encode_flags_by_slot(enc, si, flags);
}
#else
iotdata_status_t iotdata_encode_flags(iotdata_encoder_t *const enc, uint8_t flags) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_FLAGS, si);
    enc->flags.flags = flags;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_FLAGS);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_flags(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_flags_t *const v = vv;
    if (*bp + IOTDATA_FLAGS_BITS > bb)
        return false;
    v->flags = (uint8_t)bits_read(buf, bb, bp, IOTDATA_FLAGS_BITS);
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_flags(iotdata_decoder_t *dec, uint8_t *flags) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_flags_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_FLAGS, unpack_flags, &v);
    if (flags)
        *flags = v.flags;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_FLAGS);
    if (flags)
        *flags = dec->flags.flags;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_flags(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_flags(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_flags(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_flags_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->flags);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_flags(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_FLAGS_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "0x%02" PRIX32, r);
    n = dump_add(dump, n, s, IOTDATA_FLAGS_BITS, r, dump->_dec_buf, "8-bit bitmask", "flags");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_flags(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_flags_t *const v = vv;
    bprintf(bp, "  %s:%s 0x%02" PRIX8 "\n", label, _padd(label), v->flags);
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_flags = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, flags)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, flags)) _IOTDATA_OP_NAME("flags") _IOTDATA_OP_BITS(
    IOTDATA_FLAGS_BITS) _IOTDATA_OP_PACK(pack_flags) _IOTDATA_OP_UNPACK(unpack_flags) _IOTDATA_OP_DUMP(dump_flags) _IOTDATA_OP_PRINT(print_flags) _IOTDATA_OP_JSON_SET(json_set_flags) _IOTDATA_OP_JSON_GET(json_get_flags) };
#endif

/* =========================================================================
 * Field BITS32
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_BITS32)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_bits32(uint8_t *const buf, const size_t bb, size_t *const bp, const void *const vv) {
    const iotdata_field_bits32_t *const v = vv;
    return bits_write(buf, bb, bp, v->bits32, IOTDATA_BITS32_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
iotdata_status_t iotdata_encode_bits32_by_slot(iotdata_encoder_t *const enc, const int slot, uint32_t bits32) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_BITS32, si);
    iotdata_field_bits32_t v;
    v.bits32 = bits32;
    PACK_DIRECT(enc, pack_bits32, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BITS32);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_bits32(iotdata_encoder_t *const enc, uint32_t bits32) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BITS32, si);
    return iotdata_encode_bits32_by_slot(enc, si, bits32);
}
#else
iotdata_status_t iotdata_encode_bits32(iotdata_encoder_t *const enc, uint32_t bits32) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BITS32, si);
    enc->bits32.bits32 = bits32;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BITS32);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_bits32(const uint8_t *const buf, const size_t bb, size_t *const bp, void *const vv) {
    iotdata_field_bits32_t *const v = vv;
    if (*bp + IOTDATA_BITS32_BITS > bb)
        return false;
    v->bits32 = (uint32_t)bits_read(buf, bb, bp, IOTDATA_BITS32_BITS);
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_bits32(iotdata_decoder_t *dec, uint32_t *bits32) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_bits32_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_BITS32, unpack_bits32, &v);
    if (bits32)
        *bits32 = v.bits32;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_BITS32);
    if (bits32)
        *bits32 = dec->bits32.bits32;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_bits32(cJSON *const root, iotdata_encoder_t *const enc, const char *const label, iotdata_encode_from_json_scratch_t *const scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_bits32(enc, (uint32_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_bits32(cJSON *const root, const void *const vv, const char *const label, iotdata_decode_to_json_scratch_t *const scratch) {
    const iotdata_field_bits32_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->bits32);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_bits32(const uint8_t *const buf, const size_t bb, size_t *const bp, iotdata_dump_t *const dump, int n, const char *const label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_BITS32_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "0x%08" PRIX32, r);
    n = dump_add(dump, n, s, IOTDATA_BITS32_BITS, r, dump->_dec_buf, "32-bit unit", "bits32");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_bits32(const void *const vv, iotdata_buf_t *const bp, const char *const label) {
    const iotdata_field_bits32_t *const v = vv;
    bprintf(bp, "  %s:%s 0x%08" PRIX32 "\n", label, _padd(label), v->bits32);
}
#endif
static const iotdata_field_ops_t _iotdata_field_def_bits32 = { _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, bits32)) _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, bits32)) _IOTDATA_OP_NAME("bits32") _IOTDATA_OP_BITS(
    IOTDATA_BITS32_BITS) _IOTDATA_OP_PACK(pack_bits32) _IOTDATA_OP_UNPACK(unpack_bits32) _IOTDATA_OP_DUMP(dump_bits32) _IOTDATA_OP_PRINT(print_bits32) _IOTDATA_OP_JSON_SET(json_set_bits32) _IOTDATA_OP_JSON_GET(json_get_bits32) };
#endif

#endif /* IOTDATA_FIELDS_IMPLEMENTATION */
