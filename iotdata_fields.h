
/*
 * GENERATED FILE -- DO NOT EDIT.
 *
 * Built by generators/iotdata-generator-fields.js from the field specifiers in
 * specifiers/iotdata-fields-*.xml through generators/templates/fields.c.all.tmpl.
 * `make generate` rewrites it and `make generate-check` proves it reproduces.
 */

#if !defined(IOTDATA_FIELDS_H)
#define IOTDATA_FIELDS_H

#if !defined(IOTDATA_ENABLE_SELECTIVE)
#define IOTDATA_ENABLE_TLV
#define IOTDATA_ENABLE_BATTERY_LEVEL
#define IOTDATA_ENABLE_BATTERY_CHARGE
#define IOTDATA_ENABLE_LINK_RSSI
#define IOTDATA_ENABLE_LINK_SNR
#define IOTDATA_ENABLE_BATTERY
#define IOTDATA_ENABLE_LINK
#define IOTDATA_ENABLE_TEMPERATURE
#define IOTDATA_ENABLE_PRESSURE
#define IOTDATA_ENABLE_HUMIDITY
#define IOTDATA_ENABLE_ENVIRONMENT
#define IOTDATA_ENABLE_WIND_SPEED
#define IOTDATA_ENABLE_WIND_DIRECTION
#define IOTDATA_ENABLE_WIND_GUST
#define IOTDATA_ENABLE_WIND
#define IOTDATA_ENABLE_RAIN_RATE
#define IOTDATA_ENABLE_RAIN_SIZE
#define IOTDATA_ENABLE_RAIN
#define IOTDATA_ENABLE_SOLAR_IRRADIANCE
#define IOTDATA_ENABLE_SOLAR_ULTRAVIOLET
#define IOTDATA_ENABLE_CLOUDS
#define IOTDATA_ENABLE_SOLAR
#define IOTDATA_ENABLE_AIR_QUALITY_INDEX
#define IOTDATA_ENABLE_AIR_QUALITY_PM
#define IOTDATA_ENABLE_AIR_QUALITY_GAS
#define IOTDATA_ENABLE_AIR_QUALITY
#define IOTDATA_ENABLE_RADIATION_CPM
#define IOTDATA_ENABLE_RADIATION_DOSE
#define IOTDATA_ENABLE_RADIATION
#define IOTDATA_ENABLE_DEPTH
#define IOTDATA_ENABLE_POSITION
#define IOTDATA_ENABLE_DATETIME
#define IOTDATA_ENABLE_IMAGE
#define IOTDATA_ENABLE_FLAGS
#define IOTDATA_ENABLE_BITS32
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
enum iotdata_status_t_;
struct iotdata_encoder_t_;
struct iotdata_decoder_t_;
#pragma GCC diagnostic pop

#endif /* IOTDATA_FIELDS_H */

#if defined(IOTDATA_FIELDS_IMPLEMENTATION)
#define IOTDATA_FIELDS_TLV_IMPLEMENTATION
#define IOTDATA_FIELDS_GENERATED_IMPLEMENTATION
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "iotdata_fields_tlv.h"
#include "iotdata_fields_generated.h"
#if defined(IOTDATA_FIELDS_IMPLEMENTATION)
#include "iotdata_fields_impl_image.h"
#endif
#pragma GCC diagnostic pop
