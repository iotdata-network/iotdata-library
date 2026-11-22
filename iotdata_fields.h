/*
 * GENERATED FILE -- DO NOT EDIT.
 *
 * Built by generators/iotdata-generator-fields.js from the field specification in
 * specification/iotdata-fields-*.xml through generators/templates/fields.c.h.tmpl.
 * `make generate` rewrites it and `make generate-check` proves it reproduces.
 *
 * AN EDIT HERE IS LOST AT THE NEXT GENERATE, and silently: the file still compiles
 * and the tests still pass, it just no longer says what you made it say. A change
 * belongs in the SPECIFICATION if the field has been migrated, and in the TEMPLATE
 * if it has not -- the template still carries the literal text of every field that
 * has yet to move. iotdata_fields.csv lists which are which.
 */

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
 * Field BATTERY
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_BATTERY)
#define IOTDATA_FIELDS_DAT_BATTERY \
    uint8_t battery_level; \
    bool battery_charging;
typedef struct {
    IOTDATA_FIELDS_DAT_BATTERY
} iotdata_field_battery_t;
#define IOTDATA_BATTERY_LEVEL_MAX   100
#define IOTDATA_BATTERY_LEVEL_BITS  5
#define IOTDATA_BATTERY_CHARGE_BITS 1
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_battery(struct iotdata_encoder_t_ *enc, uint8_t level_percent, bool charging);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_battery_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t level_percent, bool charging);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_battery(struct iotdata_decoder_t_ *dec, uint8_t *level_percent, bool *charging);
#endif
#define IOTDATA_FIELDS_MEM_BATTERY            iotdata_field_battery_t battery;
#define IOTDATA_FIELD_ID_BATTERY              0x0001u
#define IOTDATA_FIELDS_DEF_BATTERY(F)         F(BATTERY)
#define IOTDATA_FIELDS_OPS_BATTERY(E)         E(BATTERY, _iotdata_field_def_battery)
#define IOTDATA_FIELDS_ERR_IDENTS_BATTERY(S)  S(IOTDATA_ERR_BATTERY_LEVEL_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_BATTERY(D) D(IOTDATA_ERR_BATTERY_LEVEL_HIGH, "Battery level above 100%")
#else
#define IOTDATA_FIELDS_DAT_BATTERY
#define IOTDATA_FIELDS_MEM_BATTERY
#define IOTDATA_FIELDS_DEF_BATTERY(F)
#define IOTDATA_FIELDS_OPS_BATTERY(E)
#define IOTDATA_FIELDS_ERR_IDENTS_BATTERY(S)
#define IOTDATA_FIELDS_ERR_STRINGS_BATTERY(D)
#endif

/* ---------------------------------------------------------------------------
 * Field LINK
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_LINK)
#define IOTDATA_FIELDS_DAT_LINK \
    int8_t link_rssi; \
    iotdata_float_t link_snr;
typedef struct {
    IOTDATA_FIELDS_DAT_LINK
} iotdata_field_link_t;
#define IOTDATA_LINK_RSSI_MIN  (-120)
#define IOTDATA_LINK_RSSI_MAX  (-60)
#define IOTDATA_LINK_RSSI_STEP (4)
#define IOTDATA_LINK_RSSI_BITS (4)
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
enum iotdata_status_t_ iotdata_encode_link(struct iotdata_encoder_t_ *enc, int8_t rssi_dbm, iotdata_float_t snr_db);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_link_by_slot(struct iotdata_encoder_t_ *enc, int slot, int8_t rssi_dbm, iotdata_float_t snr_db);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_link(struct iotdata_decoder_t_ *dec, int8_t *rssi_dbm, iotdata_float_t *snr_db);
#endif
#define IOTDATA_FIELDS_MEM_LINK    iotdata_field_link_t link;
#define IOTDATA_FIELD_ID_LINK      0x0002u
#define IOTDATA_FIELDS_DEF_LINK(F) F(LINK)
#define IOTDATA_FIELDS_OPS_LINK(E) E(LINK, _iotdata_field_def_link)
#define IOTDATA_FIELDS_ERR_IDENTS_LINK(S) \
    S(IOTDATA_ERR_LINK_RSSI_LOW) \
    S(IOTDATA_ERR_LINK_RSSI_HIGH) \
    S(IOTDATA_ERR_LINK_SNR_LOW) \
    S(IOTDATA_ERR_LINK_SNR_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_LINK(D) \
    D(IOTDATA_ERR_LINK_RSSI_LOW, "RSSI below -120 dBm") \
    D(IOTDATA_ERR_LINK_RSSI_HIGH, "RSSI above -60 dBm") \
    D(IOTDATA_ERR_LINK_SNR_LOW, "SNR below -20 dB") \
    D(IOTDATA_ERR_LINK_SNR_HIGH, "SNR above +10 dB")
#else
#define IOTDATA_FIELDS_DAT_LINK
#define IOTDATA_FIELDS_MEM_LINK
#define IOTDATA_FIELDS_DEF_LINK(F)
#define IOTDATA_FIELDS_OPS_LINK(E)
#define IOTDATA_FIELDS_ERR_IDENTS_LINK(S)
#define IOTDATA_FIELDS_ERR_STRINGS_LINK(D)
#endif

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
enum iotdata_status_t_ iotdata_encode_temperature(struct iotdata_encoder_t_ *enc, iotdata_float_t temperature_c);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_temperature_by_slot(struct iotdata_encoder_t_ *enc, int slot, iotdata_float_t temperature_c);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_temperature(struct iotdata_decoder_t_ *dec, iotdata_float_t *temperature_c);
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
enum iotdata_status_t_ iotdata_encode_pressure(struct iotdata_encoder_t_ *enc, uint16_t pressure_hpa);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_pressure_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint16_t pressure_hpa);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_pressure(struct iotdata_decoder_t_ *dec, uint16_t *pressure_hpa);
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
enum iotdata_status_t_ iotdata_encode_humidity(struct iotdata_encoder_t_ *enc, uint8_t humidity_pct);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_humidity_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t humidity_pct);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_humidity(struct iotdata_decoder_t_ *dec, uint8_t *humidity_pct);
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
enum iotdata_status_t_ iotdata_encode_environment(struct iotdata_encoder_t_ *enc, iotdata_float_t temp_c, uint16_t pressure_hpa, uint8_t humidity_pct);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_environment_by_slot(struct iotdata_encoder_t_ *enc, int slot, iotdata_float_t temp_c, uint16_t pressure_hpa, uint8_t humidity_pct);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_environment(struct iotdata_decoder_t_ *dec, iotdata_float_t *temp_c, uint16_t *pressure_hpa, uint8_t *humidity_pct);
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
enum iotdata_status_t_ iotdata_encode_wind_speed(struct iotdata_encoder_t_ *enc, iotdata_float_t speed_ms);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_wind_speed_by_slot(struct iotdata_encoder_t_ *enc, int slot, iotdata_float_t speed_ms);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_wind_speed(struct iotdata_decoder_t_ *dec, iotdata_float_t *speed_ms);
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
enum iotdata_status_t_ iotdata_encode_wind_direction(struct iotdata_encoder_t_ *enc, uint16_t direction_deg);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_wind_direction_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint16_t direction_deg);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_wind_direction(struct iotdata_decoder_t_ *dec, uint16_t *direction_deg);
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
enum iotdata_status_t_ iotdata_encode_wind_gust(struct iotdata_encoder_t_ *enc, iotdata_float_t gust_ms);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_wind_gust_by_slot(struct iotdata_encoder_t_ *enc, int slot, iotdata_float_t gust_ms);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_wind_gust(struct iotdata_decoder_t_ *dec, iotdata_float_t *gust_ms);
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
enum iotdata_status_t_ iotdata_encode_wind(struct iotdata_encoder_t_ *enc, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_wind_by_slot(struct iotdata_encoder_t_ *enc, int slot, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_wind(struct iotdata_decoder_t_ *dec, iotdata_float_t *speed_ms, uint16_t *direction_deg, iotdata_float_t *gust_ms);
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
enum iotdata_status_t_ iotdata_encode_rain_rate(struct iotdata_encoder_t_ *enc, uint8_t rate_mmhr);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_rain_rate_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t rate_mmhr);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_rain_rate(struct iotdata_decoder_t_ *dec, uint8_t *rate_mmhr);
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
enum iotdata_status_t_ iotdata_encode_rain_size(struct iotdata_encoder_t_ *enc, uint8_t size10_mmd);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_rain_size_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t size10_mmd);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_rain_size(struct iotdata_decoder_t_ *dec, uint8_t *size10_mmd);
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
enum iotdata_status_t_ iotdata_encode_rain(struct iotdata_encoder_t_ *enc, uint8_t rate_mmhr, uint8_t size10_mmd);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_rain_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t rate_mmhr, uint8_t size10_mmd);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_rain(struct iotdata_decoder_t_ *dec, uint8_t *rate_mmhr, uint8_t *size10_mmd);
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
 * Field SOLAR, SOLAR_IRRADIATION, SOLAR_ULTRAVIOLET
 * -------------------------------------------------------------------------*/

#if defined(IOTDATA_ENABLE_SOLAR)
#define IOTDATA_FIELDS_DAT_SOLAR \
    uint16_t solar_irradiance; \
    uint8_t solar_ultraviolet;
typedef struct {
    IOTDATA_FIELDS_DAT_SOLAR
} iotdata_field_solar_t;
#define IOTDATA_SOLAR_IRRADIATION_MAX  1023
#define IOTDATA_SOLAR_IRRADIATION_BITS 10
#define IOTDATA_SOLAR_ULTRAVIOLET_MAX  15
#define IOTDATA_SOLAR_ULTRAVIOLET_BITS 4
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_solar(struct iotdata_encoder_t_ *enc, uint16_t irradiance_wm2, uint8_t ultraviolet_index);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_solar_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint16_t irradiance_wm2, uint8_t ultraviolet_index);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_solar(struct iotdata_decoder_t_ *dec, uint16_t *irradiance_wm2, uint8_t *ultraviolet_index);
#endif
#define IOTDATA_FIELDS_MEM_SOLAR    iotdata_field_solar_t solar;
#define IOTDATA_FIELD_ID_SOLAR      0x0040u
#define IOTDATA_FIELDS_DEF_SOLAR(F) F(SOLAR)
#define IOTDATA_FIELDS_OPS_SOLAR(E) E(SOLAR, _iotdata_field_def_solar)
#define IOTDATA_FIELDS_ERR_IDENTS_SOLAR(S) \
    S(IOTDATA_ERR_SOLAR_IRRADIATION_HIGH) \
    S(IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH)
#define IOTDATA_FIELDS_ERR_STRINGS_SOLAR(D) \
    D(IOTDATA_ERR_SOLAR_IRRADIATION_HIGH, "Solar irradiance above 1023 W/m2") \
    D(IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH, "Solar ultraviolet index above 15")
#else
#define IOTDATA_FIELDS_DAT_SOLAR
#define IOTDATA_FIELDS_MEM_SOLAR
#define IOTDATA_FIELDS_DEF_SOLAR(F)
#define IOTDATA_FIELDS_OPS_SOLAR(E)
#define IOTDATA_FIELDS_ERR_IDENTS_SOLAR(S)
#define IOTDATA_FIELDS_ERR_STRINGS_SOLAR(D)
#endif

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
enum iotdata_status_t_ iotdata_encode_clouds(struct iotdata_encoder_t_ *enc, uint8_t okta);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_clouds_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t okta);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_clouds(struct iotdata_decoder_t_ *dec, uint8_t *okta);
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
enum iotdata_status_t_ iotdata_encode_air_quality_index(struct iotdata_encoder_t_ *enc, uint16_t aq_index);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_air_quality_index_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint16_t aq_index);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_air_quality_index(struct iotdata_decoder_t_ *dec, uint16_t *aq_index);
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
enum iotdata_status_t_ iotdata_encode_air_quality_pm(struct iotdata_encoder_t_ *enc, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_air_quality_pm_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_air_quality_pm(struct iotdata_decoder_t_ *dec, uint8_t *pm_present, uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]);
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
enum iotdata_status_t_ iotdata_encode_air_quality_gas(struct iotdata_encoder_t_ *enc, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_air_quality_gas_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_air_quality_gas(struct iotdata_decoder_t_ *dec, uint8_t *gas_present, uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
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
enum iotdata_status_t_ iotdata_encode_air_quality(struct iotdata_encoder_t_ *enc, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present,
                                                  const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_air_quality_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present,
                                                          const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_air_quality(struct iotdata_decoder_t_ *dec, uint16_t *aq_index, uint8_t *pm_present, uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t *gas_present, uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]);
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
enum iotdata_status_t_ iotdata_encode_radiation_cpm(struct iotdata_encoder_t_ *enc, uint16_t cpm);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_radiation_cpm_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint16_t cpm);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_radiation_cpm(struct iotdata_decoder_t_ *dec, uint16_t *cpm);
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
enum iotdata_status_t_ iotdata_encode_radiation_dose(struct iotdata_encoder_t_ *enc, iotdata_float_t usvh);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_radiation_dose_by_slot(struct iotdata_encoder_t_ *enc, int slot, iotdata_float_t usvh);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_radiation_dose(struct iotdata_decoder_t_ *dec, iotdata_float_t *usvh);
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
enum iotdata_status_t_ iotdata_encode_radiation(struct iotdata_encoder_t_ *enc, uint16_t cpm, iotdata_float_t usvh);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_radiation_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint16_t cpm, iotdata_float_t usvh);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_radiation(struct iotdata_decoder_t_ *dec, uint16_t *cpm, iotdata_float_t *usvh);
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
enum iotdata_status_t_ iotdata_encode_depth(struct iotdata_encoder_t_ *enc, uint16_t depth_cm);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_depth_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint16_t depth_cm);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_depth(struct iotdata_decoder_t_ *dec, uint16_t *depth_cm);
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
#define IOTDATA_POS_LAT_OFFSET   90.0
#define IOTDATA_POS_LAT_RANGE    180.0
#define IOTDATA_POS_LON_OFFSET   180.0
#define IOTDATA_POS_LON_RANGE    360.0
#define IOTDATA_POS_LAT_OFFSET_I 900000000LL
#define IOTDATA_POS_LAT_RANGE_I  1800000000LL
#define IOTDATA_POS_LON_OFFSET_I 1800000000LL
#define IOTDATA_POS_LON_RANGE_I  3600000000LL
#define IOTDATA_FIELDS_DAT_POSITION \
    iotdata_double_t position_lat; \
    iotdata_double_t position_lon;
typedef struct {
    IOTDATA_FIELDS_DAT_POSITION
} iotdata_field_position_t;
#if !defined(IOTDATA_NO_FLOATING)
#define IOTDATA_POS_LAT_LOW  (-90.0)
#define IOTDATA_POS_LAT_HIGH (90.0)
#define IOTDATA_POS_LON_LOW  (-180.0)
#define IOTDATA_POS_LON_HIGH (180.0)
#else
#define IOTDATA_POS_LAT_LOW  (-900000000)
#define IOTDATA_POS_LAT_HIGH (900000000)
#define IOTDATA_POS_LON_LOW  (-1800000000)
#define IOTDATA_POS_LON_HIGH (1800000000)
#endif
#define IOTDATA_POS_LAT_BITS 24
#define IOTDATA_POS_LON_BITS 24
#define IOTDATA_POS_SCALE    16777215 /* (1 << 24) - 1 */
#define IOTDATA_POS_BITS     (IOTDATA_POS_LAT_BITS + IOTDATA_POS_LON_BITS)
#if !defined(IOTDATA_NO_ENCODE)
enum iotdata_status_t_ iotdata_encode_position(struct iotdata_encoder_t_ *enc, iotdata_double_t latitude, iotdata_double_t longitude);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_position_by_slot(struct iotdata_encoder_t_ *enc, int slot, iotdata_double_t latitude, iotdata_double_t longitude);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_position(struct iotdata_decoder_t_ *dec, iotdata_double_t *latitude, iotdata_double_t *longitude);
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
enum iotdata_status_t_ iotdata_encode_datetime(struct iotdata_encoder_t_ *enc, uint32_t seconds_from_year_start);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_datetime_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint32_t seconds_from_year_start);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_datetime(struct iotdata_decoder_t_ *dec, uint32_t *seconds_from_year_start);
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

static inline uint32_t iotdata_datetime_convert_utc_to_seconds_from_year_start(int64_t utc) {
    if (utc <= 0)
        return 0;
    // Days and seconds since Unix epoch
    int d = (int)(utc / 86400), rem = (int)(utc % 86400);
    // Leap days from 1970 to approximate year (every 4y, minus 100y, plus 400y)
    // Shift to March-based year so leap day falls at end, then shift back
    // Using: days since epoch -> civil date (Howard Hinnant algorithm, simplified)
    int y = (int)((10000LL * (long long)d + 14780) / 3652425) + 1970;
    // Day-of-year for Jan 1 of year y (relative to epoch)
    int j = y * 365 + y / 4 - y / 100 + y / 400 - 719527;
    if (j > d) {
        y--;
        j = y * 365 + y / 4 - y / 100 + y / 400 - 719527;
    }
    return (uint32_t)((d - j) * 86400 + rem); // 0-based day of year
}

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
enum iotdata_status_t_ iotdata_encode_flags(struct iotdata_encoder_t_ *enc, uint8_t flags);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_flags_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint8_t flags);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_flags(struct iotdata_decoder_t_ *dec, uint8_t *flags);
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
enum iotdata_status_t_ iotdata_encode_bits32(struct iotdata_encoder_t_ *enc, uint32_t bits32);
#if defined(IOTDATA_OPERATE_ORDERED)
enum iotdata_status_t_ iotdata_encode_bits32_by_slot(struct iotdata_encoder_t_ *enc, int slot, uint32_t bits32);
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
enum iotdata_status_t_ iotdata_decode_bits32(struct iotdata_decoder_t_ *dec, uint32_t *bits32);
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
    IOTDATA_FIELDS_DEF_BATTERY(F) \
    IOTDATA_FIELDS_DEF_LINK(F) \
    IOTDATA_FIELDS_DEF_TEMPERATURE(F) \
    IOTDATA_FIELDS_DEF_PRESSURE(F) \
    IOTDATA_FIELDS_DEF_HUMIDITY(F) \
    IOTDATA_FIELDS_DEF_ENVIRONMENT(F) \
    IOTDATA_FIELDS_DEF_WIND(F) \
    IOTDATA_FIELDS_DEF_WIND_SPEED(F) \
    IOTDATA_FIELDS_DEF_WIND_DIRECTION(F) \
    IOTDATA_FIELDS_DEF_WIND_GUST(F) \
    IOTDATA_FIELDS_DEF_RAIN(F) \
    IOTDATA_FIELDS_DEF_RAIN_RATE(F) \
    IOTDATA_FIELDS_DEF_RAIN_SIZE(F) \
    IOTDATA_FIELDS_DEF_SOLAR(F) \
    IOTDATA_FIELDS_DEF_CLOUDS(F) \
    IOTDATA_FIELDS_DEF_AIR_QUALITY(F) \
    IOTDATA_FIELDS_DEF_AIR_QUALITY_INDEX(F) \
    IOTDATA_FIELDS_DEF_AIR_QUALITY_PM(F) \
    IOTDATA_FIELDS_DEF_AIR_QUALITY_GAS(F) \
    IOTDATA_FIELDS_DEF_RADIATION(F) \
    IOTDATA_FIELDS_DEF_RADIATION_CPM(F) \
    IOTDATA_FIELDS_DEF_RADIATION_DOSE(F) \
    IOTDATA_FIELDS_DEF_DEPTH(F) \
    IOTDATA_FIELDS_DEF_POSITION(F) \
    IOTDATA_FIELDS_DEF_DATETIME(F) \
    IOTDATA_FIELDS_DEF_IMAGE(F) \
    IOTDATA_FIELDS_DEF_FLAGS(F) \
    IOTDATA_FIELDS_DEF_BITS32(F)

#define IOTDATA_FIELDS_RESERVED(F) IOTDATA_FIELDS_DEF_TLV(F)

/* THE FIELD MEMBERS, split out from the aggregate so an ORDERED build can leave them out. TLV is
   not among them: it is a trailing list rather than a slot, the node/relay/gateway layers walk
   dec->tlv[] after decode returns, and so it stays buffered in BOTH modes. */
#define IOTDATA_FIELDS_MEM_ENCODER_FIELDS \
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
    IOTDATA_FIELDS_MEM_SOLAR \
    IOTDATA_FIELDS_MEM_CLOUDS \
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
    IOTDATA_FIELDS_MEM_FLAGS \
    IOTDATA_FIELDS_MEM_BITS32 \
    IOTDATA_FIELDS_MEM_IMAGE_ENCODE

#if defined(IOTDATA_OPERATE_ORDERED)
/* ORDERED: no per-field storage at all -- a setter packs straight into the buffer and an accessor
   unpacks straight out of it, so there is nothing to hold between the two. */
#define IOTDATA_FIELDS_MEM_ENCODER IOTDATA_FIELDS_DAT_TLV_ENCODE
#else
#define IOTDATA_FIELDS_MEM_ENCODER \
    IOTDATA_FIELDS_MEM_ENCODER_FIELDS \
    IOTDATA_FIELDS_DAT_TLV_ENCODE
#endif

/* THE FIELD MEMBERS, split out from the aggregate so an ORDERED build can leave them out. TLV is
   not among them: it is a trailing list rather than a slot, the node/relay/gateway layers walk
   dec->tlv[] after decode returns, and so it stays buffered in BOTH modes. */
#define IOTDATA_FIELDS_MEM_DECODER_FIELDS \
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
    IOTDATA_FIELDS_MEM_SOLAR \
    IOTDATA_FIELDS_MEM_CLOUDS \
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
    IOTDATA_FIELDS_MEM_FLAGS \
    IOTDATA_FIELDS_MEM_BITS32 \
    IOTDATA_FIELDS_MEM_IMAGE_DECODE

#if defined(IOTDATA_OPERATE_ORDERED)
/* ORDERED: no per-field storage at all -- a setter packs straight into the buffer and an accessor
   unpacks straight out of it, so there is nothing to hold between the two. */
#define IOTDATA_FIELDS_MEM_DECODER IOTDATA_FIELDS_DAT_TLV_DECODE
#else
#define IOTDATA_FIELDS_MEM_DECODER \
    IOTDATA_FIELDS_MEM_DECODER_FIELDS \
    IOTDATA_FIELDS_DAT_TLV_DECODE
#endif

#define IOTDATA_FIELDS_OPS(E) \
    IOTDATA_FIELDS_OPS_BATTERY(E) \
    IOTDATA_FIELDS_OPS_LINK(E) \
    IOTDATA_FIELDS_OPS_TEMPERATURE(E) \
    IOTDATA_FIELDS_OPS_PRESSURE(E) \
    IOTDATA_FIELDS_OPS_HUMIDITY(E) \
    IOTDATA_FIELDS_OPS_ENVIRONMENT(E) \
    IOTDATA_FIELDS_OPS_WIND(E) \
    IOTDATA_FIELDS_OPS_WIND_SPEED(E) \
    IOTDATA_FIELDS_OPS_WIND_DIRECTION(E) \
    IOTDATA_FIELDS_OPS_WIND_GUST(E) \
    IOTDATA_FIELDS_OPS_RAIN(E) \
    IOTDATA_FIELDS_OPS_RAIN_RATE(E) \
    IOTDATA_FIELDS_OPS_RAIN_SIZE(E) \
    IOTDATA_FIELDS_OPS_SOLAR(E) \
    IOTDATA_FIELDS_OPS_CLOUDS(E) \
    IOTDATA_FIELDS_OPS_AIR_QUALITY(E) \
    IOTDATA_FIELDS_OPS_AIR_QUALITY_INDEX(E) \
    IOTDATA_FIELDS_OPS_AIR_QUALITY_PM(E) \
    IOTDATA_FIELDS_OPS_AIR_QUALITY_GAS(E) \
    IOTDATA_FIELDS_OPS_RADIATION(E) \
    IOTDATA_FIELDS_OPS_RADIATION_CPM(E) \
    IOTDATA_FIELDS_OPS_RADIATION_DOSE(E) \
    IOTDATA_FIELDS_OPS_DEPTH(E) \
    IOTDATA_FIELDS_OPS_POSITION(E) \
    IOTDATA_FIELDS_OPS_DATETIME(E) \
    IOTDATA_FIELDS_OPS_IMAGE(E) \
    IOTDATA_FIELDS_OPS_FLAGS(E) \
    IOTDATA_FIELDS_OPS_BITS32(E)

#define IOTDATA_FIELDS_ERR_IDENTS(S) \
    IOTDATA_FIELDS_ERR_IDENTS_TLV(S) \
    IOTDATA_FIELDS_ERR_IDENTS_BATTERY(S) \
    IOTDATA_FIELDS_ERR_IDENTS_LINK(S) \
    IOTDATA_FIELDS_ERR_IDENTS_TEMPERATURE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_PRESSURE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_HUMIDITY(S) \
    IOTDATA_FIELDS_ERR_IDENTS_ENVIRONMENT(S) \
    IOTDATA_FIELDS_ERR_IDENTS_WIND(S) \
    IOTDATA_FIELDS_ERR_IDENTS_WIND_SPEED(S) \
    IOTDATA_FIELDS_ERR_IDENTS_WIND_DIRECTION(S) \
    IOTDATA_FIELDS_ERR_IDENTS_WIND_GUST(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RAIN(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RAIN_RATE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RAIN_SIZE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_SOLAR(S) \
    IOTDATA_FIELDS_ERR_IDENTS_CLOUDS(S) \
    IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY(S) \
    IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_INDEX(S) \
    IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_PM(S) \
    IOTDATA_FIELDS_ERR_IDENTS_AIR_QUALITY_GAS(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RADIATION(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RADIATION_CPM(S) \
    IOTDATA_FIELDS_ERR_IDENTS_RADIATION_DOSE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_DEPTH(S) \
    IOTDATA_FIELDS_ERR_IDENTS_POSITION(S) \
    IOTDATA_FIELDS_ERR_IDENTS_DATETIME(S) \
    IOTDATA_FIELDS_ERR_IDENTS_IMAGE(S) \
    IOTDATA_FIELDS_ERR_IDENTS_FLAGS(S) \
    IOTDATA_FIELDS_ERR_IDENTS_BITS32(S)

#define IOTDATA_FIELDS_ERR_STRINGS(D) \
    IOTDATA_FIELDS_ERR_STRINGS_TLV(D) \
    IOTDATA_FIELDS_ERR_STRINGS_BATTERY(D) \
    IOTDATA_FIELDS_ERR_STRINGS_LINK(D) \
    IOTDATA_FIELDS_ERR_STRINGS_TEMPERATURE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_PRESSURE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_HUMIDITY(D) \
    IOTDATA_FIELDS_ERR_STRINGS_ENVIRONMENT(D) \
    IOTDATA_FIELDS_ERR_STRINGS_WIND(D) \
    IOTDATA_FIELDS_ERR_STRINGS_WIND_SPEED(D) \
    IOTDATA_FIELDS_ERR_STRINGS_WIND_DIRECTION(D) \
    IOTDATA_FIELDS_ERR_STRINGS_WIND_GUST(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RAIN(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RAIN_RATE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RAIN_SIZE(D) \
    IOTDATA_FIELDS_ERR_STRINGS_SOLAR(D) \
    IOTDATA_FIELDS_ERR_STRINGS_CLOUDS(D) \
    IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY(D) \
    IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_INDEX(D) \
    IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_PM(D) \
    IOTDATA_FIELDS_ERR_STRINGS_AIR_QUALITY_GAS(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RADIATION(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RADIATION_CPM(D) \
    IOTDATA_FIELDS_ERR_STRINGS_RADIATION_DOSE(D) \
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
/* ONE FIELD'S WORTH OF ROOM, for the library's own generic walkers. print and decode_to_json
   iterate the slots of a packet whose types are not known until the loop runs, so they cannot
   declare a local of the right type; in the default build they read the decoder's member through
   the ops table's dec_off, and under ORDERED there is no such member to read. A single union on
   the stack covers both: 264 bytes with IMAGE enabled (the 254-byte pixel buffer plus its header)
   and 32 without, against the 2536-byte decoder it stands in for. This is NOT the rejected
   "union per slot" -- that was 27 of these, 6993 bytes, and persistent. */
typedef union {
    uint32_t _any; /* so the union is never empty, however few fields are enabled */
    IOTDATA_FIELDS_MEM_DECODER_FIELDS
} iotdata_field_any_t;
#endif

/* ---------------------------------------------------------------------------
 * End
 * -------------------------------------------------------------------------*/
