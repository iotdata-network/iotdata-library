/*
 * GENERATED FILE -- DO NOT EDIT.
 *
 * Built by generators/iotdata-generator-fields.js from the field specification in
 * specification/iotdata-fields-*.xml through generators/templates/fields.c.all.tmpl.
 * `make generate` rewrites it and `make generate-check` proves it reproduces.
 *
 * EVERYTHING FIELD-RELATED, IN ONE INCLUDE. A caller takes this header and has every field the
 * build knows: the generated ones, and the two whose code is hand-written for reasons their own
 * files explain. iotdata.h takes it and nothing else, and keeps only the choice of variant maps.
 *
 * The enable list below is generated, which is the point of this file existing. It used to live
 * in iotdata.h and be maintained by hand, so every new field needed an edit there that nothing
 * checked -- and when the three bundled fields were split, six were duly forgotten until the
 * build failed. A list derived from the specification cannot fall behind it.
 *
 * TLV IS LISTED BY HAND, and only TLV: it is not a field. It has no identifier, no presence slot,
 * no enum member and no row in the registry, so the specification does not describe it and this
 * list cannot derive it.
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
