
/* =========================================================================
 * Field BATTERY
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_BATTERY)
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
static bool pack_battery(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_battery_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_battery_level(v->battery_level), IOTDATA_BATTERY_LEVEL_BITS) && bits_write(buf, bb, bp, quantise_battery_state(v->battery_charging), IOTDATA_BATTERY_CHARGE_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_battery_by_slot(iotdata_encoder_t *enc, int slot, uint8_t level_percent, bool charging) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_BATTERY, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (level_percent > IOTDATA_BATTERY_LEVEL_MAX)
        return IOTDATA_ERR_BATTERY_LEVEL_HIGH;
#endif
    iotdata_field_battery_t v;
    v.battery_level = level_percent;
    v.battery_charging = charging;
    PACK_DIRECT(enc, pack_battery, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BATTERY);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_battery(iotdata_encoder_t *enc, uint8_t level_percent, bool charging) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BATTERY, si);
    return iotdata_encode_battery_by_slot(enc, si, level_percent, charging);
}
#else
iotdata_status_t iotdata_encode_battery(iotdata_encoder_t *enc, uint8_t level_percent, bool charging) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BATTERY, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (level_percent > IOTDATA_BATTERY_LEVEL_MAX)
        return IOTDATA_ERR_BATTERY_LEVEL_HIGH;
#endif
    enc->battery.battery_level = level_percent;
    enc->battery.battery_charging = charging;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BATTERY);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_battery(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_battery_t *const v = vv;
    if (*bp + IOTDATA_BATTERY_LEVEL_BITS + IOTDATA_BATTERY_CHARGE_BITS > bb)
        return false;
    v->battery_level = dequantise_battery_level(bits_read(buf, bb, bp, IOTDATA_BATTERY_LEVEL_BITS));
    v->battery_charging = dequantise_battery_state(bits_read(buf, bb, bp, IOTDATA_BATTERY_CHARGE_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_battery(iotdata_decoder_t *dec, uint8_t *level_percent, bool *charging) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_battery_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_BATTERY, unpack_battery, &v);
    if (level_percent)
        *level_percent = v.battery_level;
    if (charging)
        *charging = v.battery_charging;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_BATTERY);
    if (level_percent)
        *level_percent = dec->battery.battery_level;
    if (charging)
        *charging = dec->battery.battery_charging;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_battery(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
static void json_set_battery(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_battery_t *const v = vv;
    (void)scratch;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    cJSON_AddNumberToObject(obj, "level", v->battery_level);
    cJSON_AddBoolToObject(obj, "state", v->battery_charging);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_battery(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_BATTERY_LEVEL_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 "%%", dequantise_battery_level(r));
    n = dump_add(dump, n, s, IOTDATA_BATTERY_LEVEL_BITS, r, dump->_dec_buf, "0..100%, 5b quant", "battery_level");
    s = *bp;
    r = bits_read(buf, bb, bp, IOTDATA_BATTERY_CHARGE_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%s", dequantise_battery_state(r) ? "charging" : "discharging");
    n = dump_add(dump, n, s, IOTDATA_BATTERY_CHARGE_BITS, r, dump->_dec_buf, "0/1", "battery_state");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_battery(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_battery_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 "%% %s\n", label, _padd(label), v->battery_level, v->battery_charging ? "(charging)" : "(discharging)");
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_battery = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, battery))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, battery))
    _IOTDATA_OP_NAME("battery")
    _IOTDATA_OP_BITS(IOTDATA_BATTERY_LEVEL_BITS + IOTDATA_BATTERY_CHARGE_BITS)
    _IOTDATA_OP_PACK(pack_battery)
    _IOTDATA_OP_UNPACK(unpack_battery)
    _IOTDATA_OP_DUMP(dump_battery)
    _IOTDATA_OP_PRINT(print_battery)
    _IOTDATA_OP_JSON_SET(json_set_battery)
    _IOTDATA_OP_JSON_GET(json_get_battery)
};
#endif
// clang-format on

/* =========================================================================
 * Field LINK
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_LINK)
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
static bool pack_link(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_link_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_link_rssi(v->link_rssi), IOTDATA_LINK_RSSI_BITS) && bits_write(buf, bb, bp, quantise_link_snr(v->link_snr), IOTDATA_LINK_SNR_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_link_by_slot(iotdata_encoder_t *enc, int slot, int8_t rssi_dbm, iotdata_float_t snr_db) {
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
    v.link_rssi = rssi_dbm;
    v.link_snr = snr_db;
    PACK_DIRECT(enc, pack_link, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_LINK);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_link(iotdata_encoder_t *enc, int8_t rssi_dbm, iotdata_float_t snr_db) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_LINK, si);
    return iotdata_encode_link_by_slot(enc, si, rssi_dbm, snr_db);
}
#else
iotdata_status_t iotdata_encode_link(iotdata_encoder_t *enc, int8_t rssi_dbm, iotdata_float_t snr_db) {
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
    enc->link.link_rssi = rssi_dbm;
    enc->link.link_snr = snr_db;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_LINK);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_link(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_link_t *const v = vv;
    if (*bp + IOTDATA_LINK_RSSI_BITS + IOTDATA_LINK_SNR_BITS > bb)
        return false;
    v->link_rssi = dequantise_link_rssi(bits_read(buf, bb, bp, IOTDATA_LINK_RSSI_BITS));
    v->link_snr = dequantise_link_snr(bits_read(buf, bb, bp, IOTDATA_LINK_SNR_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_link(iotdata_decoder_t *dec, int8_t *rssi_dbm, iotdata_float_t *snr_db) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_link_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_LINK, unpack_link, &v);
    if (rssi_dbm)
        *rssi_dbm = v.link_rssi;
    if (snr_db)
        *snr_db = v.link_snr;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_LINK);
    if (rssi_dbm)
        *rssi_dbm = dec->link.link_rssi;
    if (snr_db)
        *snr_db = dec->link.link_snr;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_link(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
static void json_set_link(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_link_t *const v = vv;
    (void)scratch;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    cJSON_AddNumberToObject(obj, "rssi", v->link_rssi);
#if !defined(IOTDATA_NO_FLOATING)
    json_add_number_fixed(obj, "snr", (double)v->link_snr, 0);
#else
    cJSON_AddNumberToObject(obj, "snr", v->link_snr);
#endif
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_link(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_LINK_RSSI_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIi8 " dBm", dequantise_link_rssi(r));
    n = dump_add(dump, n, s, IOTDATA_LINK_RSSI_BITS, r, dump->_dec_buf, "-120..-60, 4dBm", "link_rssi");
    s = *bp;
    r = bits_read(buf, bb, bp, IOTDATA_LINK_SNR_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.0f dB", (double)dequantise_link_snr(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_link_snr(r), 10, "dB");
#endif
    n = dump_add(dump, n, s, IOTDATA_LINK_SNR_BITS, r, dump->_dec_buf, "-20..+10, 10dB", "link_snr");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_link(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_link_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %" PRIi8 " dBm RSSI, %.0f dB SNR\n", label, _padd(label), v->link_rssi, (double)v->link_snr);
#else
    bprintf(bp, "  %s:%s %" PRIi8 " dBm RSSI, %d.%d dB SNR\n", label, _padd(label), v->link_rssi, v->link_snr / 10, v->link_snr % 10);
#endif
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_link = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, link))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, link))
    _IOTDATA_OP_NAME("link")
    _IOTDATA_OP_BITS(IOTDATA_LINK_RSSI_BITS + IOTDATA_LINK_SNR_BITS)
    _IOTDATA_OP_PACK(pack_link)
    _IOTDATA_OP_UNPACK(unpack_link)
    _IOTDATA_OP_DUMP(dump_link)
    _IOTDATA_OP_PRINT(print_link)
    _IOTDATA_OP_JSON_SET(json_set_link)
    _IOTDATA_OP_JSON_GET(json_get_link)
};
#endif
// clang-format on

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
static bool pack_temperature(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_temperature_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_temperature(v->temperature), IOTDATA_TEMPERATURE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_TEMPERATURE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_temperature_by_slot(iotdata_encoder_t *enc, int slot, iotdata_float_t temperature_c) {
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
iotdata_status_t iotdata_encode_temperature(iotdata_encoder_t *enc, iotdata_float_t temperature_c) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_TEMPERATURE, si);
    return iotdata_encode_temperature_by_slot(enc, si, temperature_c);
}
#else
iotdata_status_t iotdata_encode_temperature(iotdata_encoder_t *enc, iotdata_float_t temperature_c) {
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
static bool unpack_temperature(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_temperature_t *const v = vv;
    if (*bp + IOTDATA_TEMPERATURE_BITS > bb)
        return false;
    v->temperature = dequantise_temperature(bits_read(buf, bb, bp, IOTDATA_TEMPERATURE_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_temperature(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_temperature(enc, (iotdata_float_t)j->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_temperature(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
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
static int dump_temperature(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
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
static void print_temperature(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_temperature_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.2f C\n", label, _padd(label), (double)v->temperature);
#else
    const int32_t ta = v->temperature < 0 ? -(v->temperature) : v->temperature;
    bprintf(bp, "  %s:%s %s%d.%02d C\n", label, _padd(label), v->temperature < 0 ? "-" : "", ta / 100, ta % 100);
#endif
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_TEMPERATURE)
static const iotdata_field_ops_t _iotdata_field_def_temperature = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, temperature))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, temperature))
    _IOTDATA_OP_NAME("temperature")
    _IOTDATA_OP_BITS(IOTDATA_TEMPERATURE_BITS)
    _IOTDATA_OP_PACK(pack_temperature)
    _IOTDATA_OP_UNPACK(unpack_temperature)
    _IOTDATA_OP_DUMP(dump_temperature)
    _IOTDATA_OP_PRINT(print_temperature)
    _IOTDATA_OP_JSON_SET(json_set_temperature)
    _IOTDATA_OP_JSON_GET(json_get_temperature)
};
#endif
#endif
// clang-format on

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
static bool pack_pressure(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_pressure_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_pressure(v->pressure), IOTDATA_PRESSURE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_PRESSURE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_pressure_by_slot(iotdata_encoder_t *enc, int slot, uint16_t pressure_hpa) {
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
iotdata_status_t iotdata_encode_pressure(iotdata_encoder_t *enc, uint16_t pressure_hpa) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_PRESSURE, si);
    return iotdata_encode_pressure_by_slot(enc, si, pressure_hpa);
}
#else
iotdata_status_t iotdata_encode_pressure(iotdata_encoder_t *enc, uint16_t pressure_hpa) {
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
static bool unpack_pressure(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_pressure_t *const v = vv;
    if (*bp + IOTDATA_PRESSURE_BITS > bb)
        return false;
    v->pressure = dequantise_pressure(bits_read(buf, bb, bp, IOTDATA_PRESSURE_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_pressure(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_pressure(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_pressure(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_pressure_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->pressure);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_pressure(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_PRESSURE_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " hPa", dequantise_pressure(r));
    n = dump_add(dump, n, s, IOTDATA_PRESSURE_BITS, r, dump->_dec_buf, "850..1105 hPa", "pressure");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_PRESSURE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_pressure(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_pressure_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " hPa\n", label, _padd(label), v->pressure);
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_PRESSURE)
static const iotdata_field_ops_t _iotdata_field_def_pressure = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, pressure))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, pressure))
    _IOTDATA_OP_NAME("pressure")
    _IOTDATA_OP_BITS(IOTDATA_PRESSURE_BITS)
    _IOTDATA_OP_PACK(pack_pressure)
    _IOTDATA_OP_UNPACK(unpack_pressure)
    _IOTDATA_OP_DUMP(dump_pressure)
    _IOTDATA_OP_PRINT(print_pressure)
    _IOTDATA_OP_JSON_SET(json_set_pressure)
    _IOTDATA_OP_JSON_GET(json_get_pressure)
};
#endif
#endif
// clang-format on

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
static bool pack_humidity(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_humidity_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_humidity(v->humidity), IOTDATA_HUMIDITY_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_HUMIDITY) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_humidity_by_slot(iotdata_encoder_t *enc, int slot, uint8_t humidity_pct) {
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
iotdata_status_t iotdata_encode_humidity(iotdata_encoder_t *enc, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_HUMIDITY, si);
    return iotdata_encode_humidity_by_slot(enc, si, humidity_pct);
}
#else
iotdata_status_t iotdata_encode_humidity(iotdata_encoder_t *enc, uint8_t humidity_pct) {
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
static bool unpack_humidity(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_humidity_t *const v = vv;
    if (*bp + IOTDATA_HUMIDITY_BITS > bb)
        return false;
    v->humidity = dequantise_humidity(bits_read(buf, bb, bp, IOTDATA_HUMIDITY_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_humidity(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_humidity(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_humidity(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_humidity_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->humidity);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_humidity(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_HUMIDITY_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 "%%", dequantise_humidity(r));
    n = dump_add(dump, n, s, IOTDATA_HUMIDITY_BITS, r, dump->_dec_buf, "0..100%", "humidity");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_HUMIDITY) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_humidity(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_humidity_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 "%%\n", label, _padd(label), v->humidity);
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_HUMIDITY)
static const iotdata_field_ops_t _iotdata_field_def_humidity = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, humidity))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, humidity))
    _IOTDATA_OP_NAME("humidity")
    _IOTDATA_OP_BITS(IOTDATA_HUMIDITY_BITS)
    _IOTDATA_OP_PACK(pack_humidity)
    _IOTDATA_OP_UNPACK(unpack_humidity)
    _IOTDATA_OP_DUMP(dump_humidity)
    _IOTDATA_OP_PRINT(print_humidity)
    _IOTDATA_OP_JSON_SET(json_set_humidity)
    _IOTDATA_OP_JSON_GET(json_get_humidity)
};
#endif
#endif
// clang-format on

#if defined(IOTDATA_ENABLE_ENVIRONMENT)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_environment(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_environment_t *const v = vv;
    return pack_temperature(buf, bb, bp, &v->temperature) && pack_pressure(buf, bb, bp, &v->pressure) && pack_humidity(buf, bb, bp, &v->humidity);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_environment_by_slot(iotdata_encoder_t *enc, int slot, iotdata_float_t temperature_c, uint16_t pressure_hpa, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_ENVIRONMENT, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (temperature_c < IOTDATA_TEMPERATURE_MIN)
        return IOTDATA_ERR_TEMPERATURE_LOW;
    if (temperature_c > IOTDATA_TEMPERATURE_MAX)
        return IOTDATA_ERR_TEMPERATURE_HIGH;
    if (pressure_hpa < IOTDATA_PRESSURE_MIN)
        return IOTDATA_ERR_PRESSURE_LOW;
    if (pressure_hpa > IOTDATA_PRESSURE_MAX)
        return IOTDATA_ERR_PRESSURE_HIGH;
    if (humidity_pct > IOTDATA_HUMIDITY_MAX)
        return IOTDATA_ERR_HUMIDITY_HIGH;
#endif
    iotdata_field_environment_t v;
    v.temperature.temperature = temperature_c;
    v.pressure.pressure = pressure_hpa;
    v.humidity.humidity = humidity_pct;
    PACK_DIRECT(enc, pack_environment, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_ENVIRONMENT);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_environment(iotdata_encoder_t *enc, iotdata_float_t temperature_c, uint16_t pressure_hpa, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_ENVIRONMENT, si);
    return iotdata_encode_environment_by_slot(enc, si, temperature_c, pressure_hpa, humidity_pct);
}
#else
iotdata_status_t iotdata_encode_environment(iotdata_encoder_t *enc, iotdata_float_t temperature_c, uint16_t pressure_hpa, uint8_t humidity_pct) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_ENVIRONMENT, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (temperature_c < IOTDATA_TEMPERATURE_MIN)
        return IOTDATA_ERR_TEMPERATURE_LOW;
    if (temperature_c > IOTDATA_TEMPERATURE_MAX)
        return IOTDATA_ERR_TEMPERATURE_HIGH;
    if (pressure_hpa < IOTDATA_PRESSURE_MIN)
        return IOTDATA_ERR_PRESSURE_LOW;
    if (pressure_hpa > IOTDATA_PRESSURE_MAX)
        return IOTDATA_ERR_PRESSURE_HIGH;
    if (humidity_pct > IOTDATA_HUMIDITY_MAX)
        return IOTDATA_ERR_HUMIDITY_HIGH;
#endif
    enc->environment.temperature.temperature = temperature_c;
    enc->environment.pressure.pressure = pressure_hpa;
    enc->environment.humidity.humidity = humidity_pct;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_ENVIRONMENT);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_environment(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
static iotdata_status_t json_get_environment(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
static void json_set_environment(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_environment_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_temperature(obj, &v->temperature, "temperature", scratch);
    json_set_pressure(obj, &v->pressure, "pressure", scratch);
    json_set_humidity(obj, &v->humidity, "humidity", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_environment(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    n = dump_temperature(buf, bb, bp, dump, n, label);
    n = dump_pressure(buf, bb, bp, dump, n, label);
    n = dump_humidity(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_environment(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_environment_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.2f C, %" PRIu16 " hPa, %" PRIu8 "%%\n", label, _padd(label), (double)v->temperature.temperature, v->pressure.pressure, v->humidity.humidity);
#else
    const int32_t ta = v->temperature.temperature < 0 ? -(v->temperature.temperature) : v->temperature.temperature;
    bprintf(bp, "  %s:%s %s%d.%02d C, %" PRIu16 " hPa, %" PRIu8 "%%\n", label, _padd(label), v->temperature.temperature < 0 ? "-" : "", ta / 100, ta % 100, v->pressure.pressure, v->humidity.humidity);
#endif
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_environment = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, environment))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, environment))
    _IOTDATA_OP_NAME("environment")
    _IOTDATA_OP_BITS(IOTDATA_TEMPERATURE_BITS + IOTDATA_PRESSURE_BITS + IOTDATA_HUMIDITY_BITS)
    _IOTDATA_OP_PACK(pack_environment)
    _IOTDATA_OP_UNPACK(unpack_environment)
    _IOTDATA_OP_DUMP(dump_environment)
    _IOTDATA_OP_PRINT(print_environment)
    _IOTDATA_OP_JSON_SET(json_set_environment)
    _IOTDATA_OP_JSON_GET(json_get_environment)
};
#endif
// clang-format on

/* =========================================================================
 * Field WIND, WIND_SPEED, WIND_DIRECTION, WIND_GUST
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_WIND_SPEED) || defined(IOTDATA_ENABLE_WIND_GUST) || defined(IOTDATA_ENABLE_WIND)
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
static bool pack_wind_speed(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_wind_speed_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_wind_speed(v->wind_speed), IOTDATA_WIND_SPEED_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_WIND_SPEED) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_wind_speed_by_slot(iotdata_encoder_t *enc, int slot, iotdata_float_t speed_ms) {
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
iotdata_status_t iotdata_encode_wind_speed(iotdata_encoder_t *enc, iotdata_float_t speed_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_SPEED, si);
    return iotdata_encode_wind_speed_by_slot(enc, si, speed_ms);
}
#else
iotdata_status_t iotdata_encode_wind_speed(iotdata_encoder_t *enc, iotdata_float_t speed_ms) {
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
static bool unpack_wind_speed(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_wind_speed_t *const v = vv;
    if (*bp + IOTDATA_WIND_SPEED_BITS > bb)
        return false;
    v->wind_speed = dequantise_wind_speed(bits_read(buf, bb, bp, IOTDATA_WIND_SPEED_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_wind_speed(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_wind_speed(enc, (iotdata_float_t)j->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_wind_speed(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
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
static int dump_wind_speed(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
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
static void print_wind_speed(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_wind_speed_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.1f m/s\n", label, _padd(label), (double)v->wind_speed);
#else
    bprintf(bp, "  %s:%s %d.%02d m/s\n", label, _padd(label), v->wind_speed / 100, v->wind_speed % 100);
#endif
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_WIND_SPEED) 
static const iotdata_field_ops_t _iotdata_field_def_wind_speed = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, wind_speed))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, wind_speed))
    _IOTDATA_OP_NAME("wind_speed")
    _IOTDATA_OP_BITS(IOTDATA_WIND_SPEED_BITS)
    _IOTDATA_OP_PACK(pack_wind_speed)
    _IOTDATA_OP_UNPACK(unpack_wind_speed)
    _IOTDATA_OP_DUMP(dump_wind_speed)
    _IOTDATA_OP_PRINT(print_wind_speed)
    _IOTDATA_OP_JSON_SET(json_set_wind_speed)
    _IOTDATA_OP_JSON_GET(json_get_wind_speed)
};
#endif
#endif
// clang-format on

#if defined(IOTDATA_ENABLE_WIND_DIRECTION) || defined(IOTDATA_ENABLE_WIND)
#if !defined(IOTDATA_NO_FLOATING)
#define IOTDATA_WIND_DIRECTION_SCALE ((float)(IOTDATA_WIND_DIRECTION_MAX + 1) / (float)(1 << IOTDATA_WIND_DIRECTION_BITS))
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
static bool pack_wind_direction(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_wind_direction_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_wind_direction(v->wind_direction), IOTDATA_WIND_DIRECTION_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_WIND_DIRECTION) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_wind_direction_by_slot(iotdata_encoder_t *enc, int slot, uint16_t direction_deg) {
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
iotdata_status_t iotdata_encode_wind_direction(iotdata_encoder_t *enc, uint16_t direction_deg) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_DIRECTION, si);
    return iotdata_encode_wind_direction_by_slot(enc, si, direction_deg);
}
#else
iotdata_status_t iotdata_encode_wind_direction(iotdata_encoder_t *enc, uint16_t direction_deg) {
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
static bool unpack_wind_direction(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_wind_direction_t *const v = vv;
    if (*bp + IOTDATA_WIND_DIRECTION_BITS > bb)
        return false;
    v->wind_direction = dequantise_wind_direction(bits_read(buf, bb, bp, IOTDATA_WIND_DIRECTION_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_wind_direction(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_wind_direction(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_wind_direction(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_wind_direction_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->wind_direction);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_wind_direction(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_WIND_DIRECTION_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " deg", dequantise_wind_direction(r));
    n = dump_add(dump, n, s, IOTDATA_WIND_DIRECTION_BITS, r, dump->_dec_buf, "0..355, ~1.4deg", "wind_direction");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_DIRECTION) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_wind_direction(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_wind_direction_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " deg\n", label, _padd(label), v->wind_direction);
}
#endif
// clang-format off
#if defined (IOTDATA_ENABLE_WIND_DIRECTION)
static const iotdata_field_ops_t _iotdata_field_def_wind_direction = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, wind_direction))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, wind_direction))
    _IOTDATA_OP_NAME("wind_direction")
    _IOTDATA_OP_BITS(IOTDATA_WIND_DIRECTION_BITS)
    _IOTDATA_OP_PACK(pack_wind_direction)
    _IOTDATA_OP_UNPACK(unpack_wind_direction)
    _IOTDATA_OP_DUMP(dump_wind_direction)
    _IOTDATA_OP_PRINT(print_wind_direction)
    _IOTDATA_OP_JSON_SET(json_set_wind_direction)
    _IOTDATA_OP_JSON_GET(json_get_wind_direction)
};
#endif
#endif
// clang-format on

#if defined(IOTDATA_ENABLE_WIND_GUST) || defined(IOTDATA_ENABLE_WIND)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_wind_gust(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_wind_gust_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_wind_speed(v->wind_gust), IOTDATA_WIND_GUST_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_WIND_GUST) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_wind_gust_by_slot(iotdata_encoder_t *enc, int slot, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_WIND_GUST, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (gust_ms < 0 || gust_ms > IOTDATA_WIND_SPEED_MAX)
        return IOTDATA_ERR_WIND_GUST_HIGH;
#endif
    iotdata_field_wind_gust_t v;
    v.wind_gust = gust_ms;
    PACK_DIRECT(enc, pack_wind_gust, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND_GUST);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_wind_gust(iotdata_encoder_t *enc, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_GUST, si);
    return iotdata_encode_wind_gust_by_slot(enc, si, gust_ms);
}
#else
iotdata_status_t iotdata_encode_wind_gust(iotdata_encoder_t *enc, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND_GUST, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (gust_ms < 0 || gust_ms > IOTDATA_WIND_SPEED_MAX)
        return IOTDATA_ERR_WIND_GUST_HIGH;
#endif
    enc->wind_gust.wind_gust = gust_ms;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_WIND_GUST);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_wind_gust(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_wind_gust_t *const v = vv;
    if (*bp + IOTDATA_WIND_GUST_BITS > bb)
        return false;
    v->wind_gust = dequantise_wind_speed(bits_read(buf, bb, bp, IOTDATA_WIND_GUST_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_wind_gust(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_wind_gust(enc, (iotdata_float_t)j->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_wind_gust(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
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
static int dump_wind_gust(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_WIND_GUST_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.1f m/s", (double)dequantise_wind_speed(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_wind_speed(r), 100, "m/s");
#endif
    n = dump_add(dump, n, s, IOTDATA_WIND_GUST_BITS, r, dump->_dec_buf, "0..63.5, 0.5m/s", "wind_gust");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_WIND_GUST) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_wind_gust(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_wind_gust_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.1f m/s\n", label, _padd(label), (double)v->wind_gust);
#else
    bprintf(bp, "  %s:%s %d.%02d m/s\n", label, _padd(label), v->wind_gust / 100, v->wind_gust % 100);
#endif
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_WIND_GUST) 
static const iotdata_field_ops_t _iotdata_field_def_wind_gust = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, wind_gust))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, wind_gust))
    _IOTDATA_OP_NAME("wind_gust")
    _IOTDATA_OP_BITS(IOTDATA_WIND_GUST_BITS)
    _IOTDATA_OP_PACK(pack_wind_gust)
    _IOTDATA_OP_UNPACK(unpack_wind_gust)
    _IOTDATA_OP_DUMP(dump_wind_gust)
    _IOTDATA_OP_PRINT(print_wind_gust)
    _IOTDATA_OP_JSON_SET(json_set_wind_gust)
    _IOTDATA_OP_JSON_GET(json_get_wind_gust)
};
#endif
#endif
// clang-format on

#if defined(IOTDATA_ENABLE_WIND)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_wind(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_wind_t *const v = vv;
    return pack_wind_speed(buf, bb, bp, &v->wind_speed) && pack_wind_direction(buf, bb, bp, &v->wind_direction) && pack_wind_gust(buf, bb, bp, &v->wind_gust);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_wind_by_slot(iotdata_encoder_t *enc, int slot, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_WIND, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (speed_ms < 0 || speed_ms > IOTDATA_WIND_SPEED_MAX)
        return IOTDATA_ERR_WIND_SPEED_HIGH;
    if (direction_deg > IOTDATA_WIND_DIRECTION_MAX)
        return IOTDATA_ERR_WIND_DIRECTION_HIGH;
    if (gust_ms < 0 || gust_ms > IOTDATA_WIND_SPEED_MAX)
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
iotdata_status_t iotdata_encode_wind(iotdata_encoder_t *enc, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND, si);
    return iotdata_encode_wind_by_slot(enc, si, speed_ms, direction_deg, gust_ms);
}
#else
iotdata_status_t iotdata_encode_wind(iotdata_encoder_t *enc, iotdata_float_t speed_ms, uint16_t direction_deg, iotdata_float_t gust_ms) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_WIND, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (speed_ms < 0 || speed_ms > IOTDATA_WIND_SPEED_MAX)
        return IOTDATA_ERR_WIND_SPEED_HIGH;
    if (direction_deg > IOTDATA_WIND_DIRECTION_MAX)
        return IOTDATA_ERR_WIND_DIRECTION_HIGH;
    if (gust_ms < 0 || gust_ms > IOTDATA_WIND_SPEED_MAX)
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
static bool unpack_wind(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
static iotdata_status_t json_get_wind(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
static void json_set_wind(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_wind_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_wind_speed(obj, &v->wind_speed, "speed", scratch);
    json_set_wind_direction(obj, &v->wind_direction, "direction", scratch);
    json_set_wind_gust(obj, &v->wind_gust, "gust", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_wind(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    n = dump_wind_speed(buf, bb, bp, dump, n, label);
    n = dump_wind_direction(buf, bb, bp, dump, n, label);
    n = dump_wind_gust(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_wind(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_wind_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.1f m/s, %" PRIu16 " deg, gust %.1f m/s\n", label, _padd(label), (double)v->wind_speed.wind_speed, v->wind_direction.wind_direction, (double)v->wind_gust.wind_gust);
#else
    bprintf(bp, "  %s:%s %d.%02d m/s, %" PRIu16 " deg, gust %d.%02d m/s\n", label, _padd(label), v->wind_speed.wind_speed / 100, v->wind_speed.wind_speed % 100, v->wind_direction.wind_direction, v->wind_gust.wind_gust / 100,
            v->wind_gust.wind_gust % 100);
#endif
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_wind = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, wind))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, wind))
    _IOTDATA_OP_NAME("wind")
    _IOTDATA_OP_BITS(IOTDATA_WIND_SPEED_BITS + IOTDATA_WIND_DIRECTION_BITS + IOTDATA_WIND_GUST_BITS)
    _IOTDATA_OP_PACK(pack_wind)
    _IOTDATA_OP_UNPACK(unpack_wind)
    _IOTDATA_OP_DUMP(dump_wind)
    _IOTDATA_OP_PRINT(print_wind)
    _IOTDATA_OP_JSON_SET(json_set_wind)
    _IOTDATA_OP_JSON_GET(json_get_wind)
};
#endif
// clang-format on

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
static bool pack_rain_rate(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_rain_rate_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_rain_rate(v->rain_rate), IOTDATA_RAIN_RATE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_RATE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_rain_rate_by_slot(iotdata_encoder_t *enc, int slot, uint8_t rate_mmhr) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_RAIN_RATE, si);
    iotdata_field_rain_rate_t v;
    v.rain_rate = rate_mmhr;
    PACK_DIRECT(enc, pack_rain_rate, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_RAIN_RATE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_rain_rate(iotdata_encoder_t *enc, uint8_t rate_mmhr) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RAIN_RATE, si);
    return iotdata_encode_rain_rate_by_slot(enc, si, rate_mmhr);
}
#else
iotdata_status_t iotdata_encode_rain_rate(iotdata_encoder_t *enc, uint8_t rate_mmhr) {
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
static bool unpack_rain_rate(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_rain_rate_t *const v = vv;
    if (*bp + IOTDATA_RAIN_RATE_BITS > bb)
        return false;
    v->rain_rate = dequantise_rain_rate(bits_read(buf, bb, bp, IOTDATA_RAIN_RATE_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_rain_rate(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_rain_rate(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_rain_rate(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_rain_rate_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->rain_rate);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_rain_rate(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_RAIN_RATE_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 " mm/hr", dequantise_rain_rate(r));
    n = dump_add(dump, n, s, IOTDATA_RAIN_RATE_BITS, r, dump->_dec_buf, "0..255 mm/hr", "rain_rate");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_RATE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_rain_rate(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_rain_rate_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 " mm/hr\n", label, _padd(label), v->rain_rate);
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_RAIN_RATE)
static const iotdata_field_ops_t _iotdata_field_def_rain_rate = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, rain_rate))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, rain_rate))
    _IOTDATA_OP_NAME("rain_rate")
    _IOTDATA_OP_BITS(IOTDATA_RAIN_RATE_BITS)
    _IOTDATA_OP_PACK(pack_rain_rate)
    _IOTDATA_OP_UNPACK(unpack_rain_rate)
    _IOTDATA_OP_DUMP(dump_rain_rate)
    _IOTDATA_OP_PRINT(print_rain_rate)
    _IOTDATA_OP_JSON_SET(json_set_rain_rate)
    _IOTDATA_OP_JSON_GET(json_get_rain_rate)
};
#endif
#endif
// clang-format on

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
static bool pack_rain_size(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_rain_size_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_rain_size(v->rain_size10), IOTDATA_RAIN_SIZE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_SIZE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_rain_size_by_slot(iotdata_encoder_t *enc, int slot, uint8_t size10_mmd) {
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
iotdata_status_t iotdata_encode_rain_size(iotdata_encoder_t *enc, uint8_t size10_mmd) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RAIN_SIZE, si);
    return iotdata_encode_rain_size_by_slot(enc, si, size10_mmd);
}
#else
iotdata_status_t iotdata_encode_rain_size(iotdata_encoder_t *enc, uint8_t size10_mmd) {
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
static bool unpack_rain_size(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_rain_size_t *const v = vv;
    if (*bp + IOTDATA_RAIN_SIZE_BITS > bb)
        return false;
    v->rain_size10 = dequantise_rain_size(bits_read(buf, bb, bp, IOTDATA_RAIN_SIZE_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_rain_size(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_rain_size(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_rain_size(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_rain_size_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->rain_size10);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_rain_size(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_RAIN_SIZE_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 ".%" PRIu8 " mm/d", dequantise_rain_size(r) / 10, dequantise_rain_size(r) % 10);
    n = dump_add(dump, n, s, IOTDATA_RAIN_SIZE_BITS, r, dump->_dec_buf, "0..6.3 mm/d", "rain_size");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_RAIN_SIZE) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_rain_size(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_rain_size_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 ".%" PRIu8 " mm/d\n", label, _padd(label), v->rain_size10 / 10, v->rain_size10 % 10);
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_RAIN_SIZE)
static const iotdata_field_ops_t _iotdata_field_def_rain_size = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, rain_size))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, rain_size))
    _IOTDATA_OP_NAME("rain_size")
    _IOTDATA_OP_BITS(IOTDATA_RAIN_SIZE_BITS)
    _IOTDATA_OP_PACK(pack_rain_size)
    _IOTDATA_OP_UNPACK(unpack_rain_size)
    _IOTDATA_OP_DUMP(dump_rain_size)
    _IOTDATA_OP_PRINT(print_rain_size)
    _IOTDATA_OP_JSON_SET(json_set_rain_size)
    _IOTDATA_OP_JSON_GET(json_get_rain_size)
};
#endif
#endif
// clang-format on

#if defined(IOTDATA_ENABLE_RAIN)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_rain(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_rain_t *const v = vv;
    return pack_rain_rate(buf, bb, bp, &v->rain_rate) && pack_rain_size(buf, bb, bp, &v->rain_size);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_rain_by_slot(iotdata_encoder_t *enc, int slot, uint8_t rate_mmhr, uint8_t size10_mmd) {
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
iotdata_status_t iotdata_encode_rain(iotdata_encoder_t *enc, uint8_t rate_mmhr, uint8_t size10_mmd) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RAIN, si);
    return iotdata_encode_rain_by_slot(enc, si, rate_mmhr, size10_mmd);
}
#else
iotdata_status_t iotdata_encode_rain(iotdata_encoder_t *enc, uint8_t rate_mmhr, uint8_t size10_mmd) {
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
static bool unpack_rain(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
static iotdata_status_t json_get_rain(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
static void json_set_rain(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_rain_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_rain_rate(obj, &v->rain_rate, "rate", scratch);
    json_set_rain_size(obj, &v->rain_size, "size", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_rain(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    n = dump_rain_rate(buf, bb, bp, dump, n, label);
    n = dump_rain_size(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_rain(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_rain_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 " mm/hr, %" PRIu8 ".%" PRIu8 " mm/d\n", label, _padd(label), v->rain_rate.rain_rate, v->rain_size.rain_size10 / 10, v->rain_size.rain_size10 % 10);
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_rain = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, rain))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, rain))
    _IOTDATA_OP_NAME("rain")
    _IOTDATA_OP_BITS(IOTDATA_RAIN_RATE_BITS + IOTDATA_RAIN_SIZE_BITS)
    _IOTDATA_OP_PACK(pack_rain)
    _IOTDATA_OP_UNPACK(unpack_rain)
    _IOTDATA_OP_DUMP(dump_rain)
    _IOTDATA_OP_PRINT(print_rain)
    _IOTDATA_OP_JSON_SET(json_set_rain)
    _IOTDATA_OP_JSON_GET(json_get_rain)
};
#endif
// clang-format on

/* =========================================================================
 * Field SOLAR, SOLAR_IRRADIANCE, SOLAR_ULTRAVIOLET
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_SOLAR)
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
static bool pack_solar(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_solar_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_solar_irradiance(v->solar_irradiance), IOTDATA_SOLAR_IRRADIATION_BITS) && bits_write(buf, bb, bp, quantise_solar_ultraviolet(v->solar_ultraviolet), IOTDATA_SOLAR_ULTRAVIOLET_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_solar_by_slot(iotdata_encoder_t *enc, int slot, uint16_t irradiance_wm2, uint8_t ultraviolet_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_SOLAR, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (irradiance_wm2 > IOTDATA_SOLAR_IRRADIATION_MAX)
        return IOTDATA_ERR_SOLAR_IRRADIATION_HIGH;
    if (ultraviolet_index > IOTDATA_SOLAR_ULTRAVIOLET_MAX)
        return IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH;
#endif
    iotdata_field_solar_t v;
    v.solar_irradiance = irradiance_wm2;
    v.solar_ultraviolet = ultraviolet_index;
    PACK_DIRECT(enc, pack_solar, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_SOLAR);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_solar(iotdata_encoder_t *enc, uint16_t irradiance_wm2, uint8_t ultraviolet_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_SOLAR, si);
    return iotdata_encode_solar_by_slot(enc, si, irradiance_wm2, ultraviolet_index);
}
#else
iotdata_status_t iotdata_encode_solar(iotdata_encoder_t *enc, uint16_t irradiance_wm2, uint8_t ultraviolet_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_SOLAR, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (irradiance_wm2 > IOTDATA_SOLAR_IRRADIATION_MAX)
        return IOTDATA_ERR_SOLAR_IRRADIATION_HIGH;
    if (ultraviolet_index > IOTDATA_SOLAR_ULTRAVIOLET_MAX)
        return IOTDATA_ERR_SOLAR_ULTRAVIOLET_HIGH;
#endif
    enc->solar.solar_irradiance = irradiance_wm2;
    enc->solar.solar_ultraviolet = ultraviolet_index;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_SOLAR);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_solar(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_solar_t *const v = vv;
    if (*bp + IOTDATA_SOLAR_IRRADIATION_BITS + IOTDATA_SOLAR_ULTRAVIOLET_BITS > bb)
        return false;
    v->solar_irradiance = dequantise_solar_irradiance(bits_read(buf, bb, bp, IOTDATA_SOLAR_IRRADIATION_BITS));
    v->solar_ultraviolet = dequantise_solar_ultraviolet(bits_read(buf, bb, bp, IOTDATA_SOLAR_ULTRAVIOLET_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_solar(iotdata_decoder_t *dec, uint16_t *irradiance_wm2, uint8_t *ultraviolet_index) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_solar_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_SOLAR, unpack_solar, &v);
    if (irradiance_wm2)
        *irradiance_wm2 = v.solar_irradiance;
    if (ultraviolet_index)
        *ultraviolet_index = v.solar_ultraviolet;
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_SOLAR);
    if (irradiance_wm2)
        *irradiance_wm2 = dec->solar.solar_irradiance;
    if (ultraviolet_index)
        *ultraviolet_index = dec->solar.solar_ultraviolet;
#endif
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_solar(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
static void json_set_solar(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_solar_t *const v = vv;
    (void)scratch;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    cJSON_AddNumberToObject(obj, "irradiance", v->solar_irradiance);
    cJSON_AddNumberToObject(obj, "ultraviolet", v->solar_ultraviolet);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_solar(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_SOLAR_IRRADIATION_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " W/m2", dequantise_solar_irradiance(r));
    n = dump_add(dump, n, s, IOTDATA_SOLAR_IRRADIATION_BITS, r, dump->_dec_buf, "0..1023 W/m2", "solar_ir");
    s = *bp;
    r = bits_read(buf, bb, bp, IOTDATA_SOLAR_ULTRAVIOLET_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8, dequantise_solar_ultraviolet(r));
    n = dump_add(dump, n, s, IOTDATA_SOLAR_ULTRAVIOLET_BITS, r, dump->_dec_buf, "0..15", "solar_uv");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_solar(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_solar_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " W/m2, UV %" PRIu8 "\n", label, _padd(label), v->solar_irradiance, v->solar_ultraviolet);
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_solar = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, solar))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, solar))
    _IOTDATA_OP_NAME("solar")
    _IOTDATA_OP_BITS(IOTDATA_SOLAR_IRRADIATION_BITS + IOTDATA_SOLAR_ULTRAVIOLET_BITS)
    _IOTDATA_OP_PACK(pack_solar)
    _IOTDATA_OP_UNPACK(unpack_solar)
    _IOTDATA_OP_DUMP(dump_solar)
    _IOTDATA_OP_PRINT(print_solar)
    _IOTDATA_OP_JSON_SET(json_set_solar)
    _IOTDATA_OP_JSON_GET(json_get_solar)
};
#endif
// clang-format on

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
static bool pack_clouds(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_clouds_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_clouds(v->clouds), IOTDATA_CLOUDS_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_clouds_by_slot(iotdata_encoder_t *enc, int slot, uint8_t okta) {
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
iotdata_status_t iotdata_encode_clouds(iotdata_encoder_t *enc, uint8_t okta) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_CLOUDS, si);
    return iotdata_encode_clouds_by_slot(enc, si, okta);
}
#else
iotdata_status_t iotdata_encode_clouds(iotdata_encoder_t *enc, uint8_t okta) {
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
static bool unpack_clouds(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
static iotdata_status_t json_get_clouds(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_clouds(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_clouds(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_clouds_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->clouds);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_clouds(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_CLOUDS_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 " okta", dequantise_clouds(r));
    n = dump_add(dump, n, s, IOTDATA_CLOUDS_BITS, r, dump->_dec_buf, "0..8 okta", "clouds");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_clouds(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_clouds_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu8 " okta\n", label, _padd(label), v->clouds);
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_clouds = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, clouds))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, clouds))
    _IOTDATA_OP_NAME("clouds")
    _IOTDATA_OP_BITS(IOTDATA_CLOUDS_BITS)
    _IOTDATA_OP_PACK(pack_clouds)
    _IOTDATA_OP_UNPACK(unpack_clouds)
    _IOTDATA_OP_DUMP(dump_clouds)
    _IOTDATA_OP_PRINT(print_clouds)
    _IOTDATA_OP_JSON_SET(json_set_clouds)
_IOTDATA_OP_JSON_GET(json_get_clouds)
};
#endif
// clang-format on

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
#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS) && !defined(IOTDATA_NO_ENCODE) && !defined(IOTDATA_NO_CHECKS_TYPES)
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
static uint32_t quantise_aq_index(uint16_t v) {
    return (uint32_t)v;
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static uint16_t dequantise_aq_index(uint32_t r) {
    return (uint16_t)r;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_aq_index(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_air_quality_index_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_aq_index(v->aq_index), IOTDATA_AIR_QUALITY_INDEX_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_air_quality_index_by_slot(iotdata_encoder_t *enc, int slot, uint16_t aq_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_AIR_QUALITY_INDEX, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (aq_index > IOTDATA_AIR_QUALITY_INDEX_MAX)
        return IOTDATA_ERR_AIR_QUALITY_INDEX_HIGH;
#endif
    iotdata_field_air_quality_index_t v;
    v.aq_index = aq_index;
    PACK_DIRECT(enc, pack_aq_index, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY_INDEX);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_air_quality_index(iotdata_encoder_t *enc, uint16_t aq_index) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY_INDEX, si);
    return iotdata_encode_air_quality_index_by_slot(enc, si, aq_index);
}
#else
iotdata_status_t iotdata_encode_air_quality_index(iotdata_encoder_t *enc, uint16_t aq_index) {
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
static bool unpack_aq_index(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_air_quality_index_t *const v = vv;
    if (*bp + IOTDATA_AIR_QUALITY_INDEX_BITS > bb)
        return false;
    v->aq_index = dequantise_aq_index(bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_INDEX_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_air_quality_index(iotdata_decoder_t *dec, uint16_t *aq_index) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_air_quality_index_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY_INDEX, unpack_aq_index, &v);
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
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_aq_index(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_air_quality_index_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->aq_index);
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_aq_index(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_air_quality_index(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_aq_index(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_AIR_QUALITY_INDEX_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " AQI", dequantise_aq_index(r));
    n = dump_add(dump, n, s, IOTDATA_AIR_QUALITY_INDEX_BITS, r, dump->_dec_buf, "0..500 AQI", "aq_index");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_aq_index(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_air_quality_index_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " AQI\n", label, _padd(label), v->aq_index);
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_AIR_QUALITY_INDEX)
static const iotdata_field_ops_t _iotdata_field_def_aq_index = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, air_quality_index))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, air_quality_index))
    _IOTDATA_OP_NAME("air_quality_index")
    _IOTDATA_OP_BITS(IOTDATA_AIR_QUALITY_INDEX_BITS)
    _IOTDATA_OP_PACK(pack_aq_index)
    _IOTDATA_OP_UNPACK(unpack_aq_index)
    _IOTDATA_OP_DUMP(dump_aq_index)
    _IOTDATA_OP_PRINT(print_aq_index)
    _IOTDATA_OP_JSON_SET(json_set_aq_index)
    _IOTDATA_OP_JSON_GET(json_get_aq_index)
};
#endif
#endif
// clang-format on

#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM) || defined(IOTDATA_ENABLE_AIR_QUALITY)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_aq_pm(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
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
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_air_quality_pm_by_slot(iotdata_encoder_t *enc, int slot, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]) {
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
    PACK_DIRECT(enc, pack_aq_pm, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY_PM);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_air_quality_pm(iotdata_encoder_t *enc, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY_PM, si);
    return iotdata_encode_air_quality_pm_by_slot(enc, si, pm_present, pm);
}
#else
iotdata_status_t iotdata_encode_air_quality_pm(iotdata_encoder_t *enc, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]) {
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
static bool unpack_aq_pm(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_air_quality_pm(iotdata_decoder_t *dec, uint8_t *pm_present, uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT]) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_air_quality_pm_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY_PM, unpack_aq_pm, &v);
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
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_aq_pm(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_air_quality_pm_t *const v = vv;
    (void)scratch;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    for (int i = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if (v->aq_pm_present & (1U << i))
            cJSON_AddNumberToObject(obj, _aq_pm_names[i], v->aq_pm[i]);
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_aq_pm(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
#if !defined(IOTDATA_NO_DUMP)
static int dump_aq_pm(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
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
static void print_aq_pm(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_air_quality_pm_t *const v = vv;
    bprintf(bp, "  %s:%s", label, _padd(label));
    for (int i = 0, first = 0; i < IOTDATA_AIR_QUALITY_PM_COUNT; i++)
        if (v->aq_pm_present & (1U << i))
            bprintf(bp, "%s %s=%" PRIu16, first++ ? "," : "", _aq_pm_labels[i], v->aq_pm[i]);
    bprintf(bp, "%s\n", v->aq_pm_present ? " ug/m3" : "");
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_AIR_QUALITY_PM)
static const iotdata_field_ops_t _iotdata_field_def_aq_pm = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, air_quality_pm))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, air_quality_pm))
    _IOTDATA_OP_NAME("air_quality_pm")
    _IOTDATA_OP_BITS(0)
    _IOTDATA_OP_PACK(pack_aq_pm)
    _IOTDATA_OP_UNPACK(unpack_aq_pm)
    _IOTDATA_OP_DUMP(dump_aq_pm)
    _IOTDATA_OP_PRINT(print_aq_pm)
    _IOTDATA_OP_JSON_SET(json_set_aq_pm)
    _IOTDATA_OP_JSON_GET(json_get_aq_pm)
};
#endif
#endif
// clang-format on

#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS) || defined(IOTDATA_ENABLE_AIR_QUALITY)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_aq_gas(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
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
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_air_quality_gas_by_slot(iotdata_encoder_t *enc, int slot, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
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
    PACK_DIRECT(enc, pack_aq_gas, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_AIR_QUALITY_GAS);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_air_quality_gas(iotdata_encoder_t *enc, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY_GAS, si);
    return iotdata_encode_air_quality_gas_by_slot(enc, si, gas_present, gas);
}
#else
iotdata_status_t iotdata_encode_air_quality_gas(iotdata_encoder_t *enc, uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
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
static bool unpack_aq_gas(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_air_quality_gas(iotdata_decoder_t *dec, uint8_t *gas_present, uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
#if defined(IOTDATA_OPERATE_ORDERED)
    iotdata_field_air_quality_gas_t v;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_AIR_QUALITY_GAS, unpack_aq_gas, &v);
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
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_aq_gas(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_air_quality_gas_t *const v = vv;
    (void)scratch;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    for (int i = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if (v->aq_gas_present & (1U << i))
            cJSON_AddNumberToObject(obj, _aq_gas_names[i], v->aq_gas[i]);
}
#endif
#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS) && !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_aq_gas(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
#if !defined(IOTDATA_NO_DUMP)
static int dump_aq_gas(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
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
static void print_aq_gas(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_air_quality_gas_t *const v = vv;
    bprintf(bp, "  %s:%s", label, _padd(label));
    for (int i = 0, first = 0; i < IOTDATA_AIR_QUALITY_GAS_COUNT; i++)
        if (v->aq_gas_present & (1U << i))
            bprintf(bp, "%s %s=%" PRIu16 "%s%s", first++ ? "," : "", _aq_gas_labels[i], v->aq_gas[i], _aq_gas_units[i][0] ? " " : "", _aq_gas_units[i][0] ? _aq_gas_units[i] : "");
    bprintf(bp, "\n");
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_AIR_QUALITY_GAS)
static const iotdata_field_ops_t _iotdata_field_def_aq_gas = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, air_quality_gas))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, air_quality_gas))
    _IOTDATA_OP_NAME("air_quality_gas")
    _IOTDATA_OP_BITS(0)
    _IOTDATA_OP_PACK(pack_aq_gas)
    _IOTDATA_OP_UNPACK(unpack_aq_gas)
    _IOTDATA_OP_DUMP(dump_aq_gas)
    _IOTDATA_OP_PRINT(print_aq_gas)
    _IOTDATA_OP_JSON_SET(json_set_aq_gas)
    _IOTDATA_OP_JSON_GET(json_get_aq_gas)
};
#endif
#endif
// clang-format on

#if defined(IOTDATA_ENABLE_AIR_QUALITY)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_air_quality(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_air_quality_t *const v = vv;
    return pack_aq_index(buf, bb, bp, &v->air_quality_index) && pack_aq_pm(buf, bb, bp, &v->air_quality_pm) && pack_aq_gas(buf, bb, bp, &v->air_quality_gas);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_air_quality_by_slot(iotdata_encoder_t *enc, int slot, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present,
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
iotdata_status_t iotdata_encode_air_quality(iotdata_encoder_t *enc, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_AIR_QUALITY, si);
    return iotdata_encode_air_quality_by_slot(enc, si, aq_index, pm_present, pm, gas_present, gas);
}
#else
iotdata_status_t iotdata_encode_air_quality(iotdata_encoder_t *enc, uint16_t aq_index, uint8_t pm_present, const uint16_t pm[IOTDATA_AIR_QUALITY_PM_COUNT], uint8_t gas_present, const uint16_t gas[IOTDATA_AIR_QUALITY_GAS_COUNT]) {
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
static bool unpack_air_quality(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_air_quality_t *const v = vv;
    return unpack_aq_index(buf, bb, bp, &v->air_quality_index) && unpack_aq_pm(buf, bb, bp, &v->air_quality_pm) && unpack_aq_gas(buf, bb, bp, &v->air_quality_gas);
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
static iotdata_status_t json_get_air_quality(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
static void json_set_air_quality(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_air_quality_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_aq_index(obj, &v->air_quality_index, "index", scratch);
    json_set_aq_pm(obj, &v->air_quality_pm, "pm", scratch);
    json_set_aq_gas(obj, &v->air_quality_gas, "gas", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_air_quality(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    n = dump_aq_index(buf, bb, bp, dump, n, label);
    n = dump_aq_pm(buf, bb, bp, dump, n, label);
    n = dump_aq_gas(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_air_quality(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_air_quality_t *const v = vv;
    print_aq_index(&v->air_quality_index, bp, label);
    print_aq_pm(&v->air_quality_pm, bp, label);
    print_aq_gas(&v->air_quality_gas, bp, label);
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_air_quality = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, air_quality))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, air_quality))
    _IOTDATA_OP_NAME("air_quality")
    _IOTDATA_OP_BITS(0)
    _IOTDATA_OP_PACK(pack_air_quality)
    _IOTDATA_OP_UNPACK(unpack_air_quality)
    _IOTDATA_OP_DUMP(dump_air_quality)
    _IOTDATA_OP_PRINT(print_air_quality)
    _IOTDATA_OP_JSON_SET(json_set_air_quality)
    _IOTDATA_OP_JSON_GET(json_get_air_quality)
};
#endif
// clang-format on

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
static bool pack_radiation_cpm(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_radiation_cpm_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_radiation_cpm(v->radiation_cpm), IOTDATA_RADIATION_CPM_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_CPM) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_radiation_cpm_by_slot(iotdata_encoder_t *enc, int slot, uint16_t cpm) {
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
iotdata_status_t iotdata_encode_radiation_cpm(iotdata_encoder_t *enc, uint16_t cpm) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION_CPM, si);
    return iotdata_encode_radiation_cpm_by_slot(enc, si, cpm);
}
#else
iotdata_status_t iotdata_encode_radiation_cpm(iotdata_encoder_t *enc, uint16_t cpm) {
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
static bool unpack_radiation_cpm(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_radiation_cpm_t *const v = vv;
    if (*bp + IOTDATA_RADIATION_CPM_BITS > bb)
        return false;
    v->radiation_cpm = dequantise_radiation_cpm(bits_read(buf, bb, bp, IOTDATA_RADIATION_CPM_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_radiation_cpm(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_radiation_cpm(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_radiation_cpm(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_radiation_cpm_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->radiation_cpm);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_radiation_cpm(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_RADIATION_CPM_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " CPM", dequantise_radiation_cpm(r));
    n = dump_add(dump, n, s, IOTDATA_RADIATION_CPM_BITS, r, dump->_dec_buf, "0..65535 CPM", "radiation_cpm");
    return n;
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_CPM) && !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_radiation_cpm(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_radiation_cpm_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " CPM\n", label, _padd(label), v->radiation_cpm);
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_RADIATION_CPM) 
static const iotdata_field_ops_t _iotdata_field_def_radiation_cpm = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, radiation_cpm))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, radiation_cpm))
    _IOTDATA_OP_NAME("radiation_cpm")
    _IOTDATA_OP_BITS(IOTDATA_RADIATION_CPM_BITS)
    _IOTDATA_OP_PACK(pack_radiation_cpm)
    _IOTDATA_OP_UNPACK(unpack_radiation_cpm)
    _IOTDATA_OP_DUMP(dump_radiation_cpm)
    _IOTDATA_OP_PRINT(print_radiation_cpm)
    _IOTDATA_OP_JSON_SET(json_set_radiation_cpm)
    _IOTDATA_OP_JSON_GET(json_get_radiation_cpm)
};
#endif
#endif
// clang-format on

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
static bool pack_radiation_dose(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_radiation_dose_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_radiation_dose(v->radiation_dose), IOTDATA_RADIATION_DOSE_BITS);
}
#endif
#if defined(IOTDATA_ENABLE_RADIATION_DOSE) && !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_radiation_dose_by_slot(iotdata_encoder_t *enc, int slot, iotdata_float_t usvh) {
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
iotdata_status_t iotdata_encode_radiation_dose(iotdata_encoder_t *enc, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION_DOSE, si);
    return iotdata_encode_radiation_dose_by_slot(enc, si, usvh);
}
#else
iotdata_status_t iotdata_encode_radiation_dose(iotdata_encoder_t *enc, iotdata_float_t usvh) {
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
static bool unpack_radiation_dose(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_radiation_dose_t *const v = vv;
    if (*bp + IOTDATA_RADIATION_DOSE_BITS > bb)
        return false;
    v->radiation_dose = dequantise_radiation_dose(bits_read(buf, bb, bp, IOTDATA_RADIATION_DOSE_BITS));
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
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
static iotdata_status_t json_get_radiation_dose(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_radiation_dose(enc, (iotdata_float_t)j->valuedouble);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_radiation_dose(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
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
static int dump_radiation_dose(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
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
static void print_radiation_dose(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_radiation_dose_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.1f uSv/h\n", label, _padd(label), (double)v->radiation_dose);
#else
    bprintf(bp, "  %s:%s %d.%02d uSv/h\n", label, _padd(label), v->radiation_dose / 100, v->radiation_dose % 100);
#endif
}
#endif
// clang-format off
#if defined(IOTDATA_ENABLE_RADIATION_DOSE)
static const iotdata_field_ops_t _iotdata_field_def_radiation_dose = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, radiation_dose))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, radiation_dose))
    _IOTDATA_OP_NAME("radiation_dose")
    _IOTDATA_OP_BITS(IOTDATA_RADIATION_DOSE_BITS)
    _IOTDATA_OP_PACK(pack_radiation_dose)
    _IOTDATA_OP_UNPACK(unpack_radiation_dose)
    _IOTDATA_OP_DUMP(dump_radiation_dose)
    _IOTDATA_OP_PRINT(print_radiation_dose)
    _IOTDATA_OP_JSON_SET(json_set_radiation_dose)
    _IOTDATA_OP_JSON_GET(json_get_radiation_dose)
};
#endif
#endif
// clang-format on

#if defined(IOTDATA_ENABLE_RADIATION)
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_radiation(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_radiation_t *const v = vv;
    return pack_radiation_cpm(buf, bb, bp, &v->radiation_cpm) && pack_radiation_dose(buf, bb, bp, &v->radiation_dose);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_radiation_by_slot(iotdata_encoder_t *enc, int slot, uint16_t cpm, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_RADIATION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
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
iotdata_status_t iotdata_encode_radiation(iotdata_encoder_t *enc, uint16_t cpm, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION, si);
    return iotdata_encode_radiation_by_slot(enc, si, cpm, usvh);
}
#else
iotdata_status_t iotdata_encode_radiation(iotdata_encoder_t *enc, uint16_t cpm, iotdata_float_t usvh) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_RADIATION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
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
static bool unpack_radiation(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
static iotdata_status_t json_get_radiation(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
static void json_set_radiation(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_radiation_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    json_set_radiation_cpm(obj, &v->radiation_cpm, "cpm", scratch);
    json_set_radiation_dose(obj, &v->radiation_dose, "dose", scratch);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_radiation(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    n = dump_radiation_cpm(buf, bb, bp, dump, n, label);
    n = dump_radiation_dose(buf, bb, bp, dump, n, label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_radiation(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_radiation_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %" PRIu16 " CPM, %.2f uSv/h\n", label, _padd(label), v->radiation_cpm.radiation_cpm, (double)v->radiation_dose.radiation_dose);
#else
    bprintf(bp, "  %s:%s %" PRIu16 " CPM, %d.%02d uSv/h\n", label, _padd(label), v->radiation_cpm.radiation_cpm, v->radiation_dose.radiation_dose / 100, v->radiation_dose.radiation_dose % 100);
#endif
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_radiation = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, radiation))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, radiation))
    _IOTDATA_OP_NAME("radiation")
    _IOTDATA_OP_BITS(IOTDATA_RADIATION_CPM_BITS + IOTDATA_RADIATION_DOSE_BITS)
    _IOTDATA_OP_PACK(pack_radiation)
    _IOTDATA_OP_UNPACK(unpack_radiation)
    _IOTDATA_OP_DUMP(dump_radiation)
    _IOTDATA_OP_PRINT(print_radiation)
    _IOTDATA_OP_JSON_SET(json_set_radiation)
    _IOTDATA_OP_JSON_GET(json_get_radiation)
};
#endif
// clang-format on

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
static bool pack_depth(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_depth_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_depth(v->depth), IOTDATA_DEPTH_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_depth_by_slot(iotdata_encoder_t *enc, int slot, uint16_t depth_cm) {
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
iotdata_status_t iotdata_encode_depth(iotdata_encoder_t *enc, uint16_t depth_cm) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_DEPTH, si);
    return iotdata_encode_depth_by_slot(enc, si, depth_cm);
}
#else
iotdata_status_t iotdata_encode_depth(iotdata_encoder_t *enc, uint16_t depth_cm) {
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
static bool unpack_depth(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
static iotdata_status_t json_get_depth(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_depth(enc, (uint16_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_depth(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_depth_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->depth);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_depth(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_DEPTH_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu16 " cm", dequantise_depth(r));
    n = dump_add(dump, n, s, IOTDATA_DEPTH_BITS, r, dump->_dec_buf, "0..1023 cm", label);
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_depth(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_depth_t *const v = vv;
    bprintf(bp, "  %s:%s %" PRIu16 " cm\n", label, _padd(label), v->depth);
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_depth = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, depth))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, depth))
    _IOTDATA_OP_NAME("depth")
    _IOTDATA_OP_BITS(IOTDATA_DEPTH_BITS)
    _IOTDATA_OP_PACK(pack_depth)
    _IOTDATA_OP_UNPACK(unpack_depth)
    _IOTDATA_OP_DUMP(dump_depth)
    _IOTDATA_OP_PRINT(print_depth)
    _IOTDATA_OP_JSON_SET(json_set_depth)
    _IOTDATA_OP_JSON_GET(json_get_depth)
};
#endif
// clang-format on

/* =========================================================================
 * Field POSITION (LATITUDE, LONGITUDE)
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_POSITION)
#if !defined(IOTDATA_NO_FLOATING)
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_position_lat(iotdata_double_t lat) {
#if !defined(IOTDATA_NO_FLOATING_DOUBLES)
    return (uint32_t)round((lat + (iotdata_double_t)IOTDATA_POS_LAT_OFFSET) / (iotdata_double_t)IOTDATA_POS_LAT_RANGE * (iotdata_double_t)IOTDATA_POS_SCALE);
#else
    return (uint32_t)roundf((lat + (iotdata_double_t)IOTDATA_POS_LAT_OFFSET) / (iotdata_double_t)IOTDATA_POS_LAT_RANGE * (iotdata_double_t)IOTDATA_POS_SCALE);
#endif
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static iotdata_double_t dequantise_position_lat(uint32_t raw) {
    return (iotdata_double_t)raw / (iotdata_double_t)IOTDATA_POS_SCALE * (iotdata_double_t)IOTDATA_POS_LAT_RANGE - (iotdata_double_t)IOTDATA_POS_LAT_OFFSET;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_position_lon(iotdata_double_t lon) {
#if !defined(IOTDATA_NO_FLOATING_DOUBLES)
    return (uint32_t)round((lon + (iotdata_double_t)IOTDATA_POS_LON_OFFSET) / (iotdata_double_t)IOTDATA_POS_LON_RANGE * (iotdata_double_t)IOTDATA_POS_SCALE);
#else
    return (uint32_t)roundf((lon + (iotdata_double_t)IOTDATA_POS_LON_OFFSET) / (iotdata_double_t)IOTDATA_POS_LON_RANGE * (iotdata_double_t)IOTDATA_POS_SCALE);
#endif
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static iotdata_double_t dequantise_position_lon(uint32_t raw) {
    return (iotdata_double_t)raw / (iotdata_double_t)IOTDATA_POS_SCALE * (iotdata_double_t)IOTDATA_POS_LON_RANGE - (iotdata_double_t)IOTDATA_POS_LON_OFFSET;
}
#endif
#else
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_position_lat(int32_t lat7) {
    return (uint32_t)((((int64_t)lat7 + IOTDATA_POS_LAT_OFFSET_I) * IOTDATA_POS_SCALE + IOTDATA_POS_LAT_OFFSET_I) / IOTDATA_POS_LAT_RANGE_I);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int32_t dequantise_position_lat(uint32_t raw) {
    return (int32_t)(((int64_t)raw * IOTDATA_POS_LAT_RANGE_I + IOTDATA_POS_SCALE / 2) / IOTDATA_POS_SCALE - IOTDATA_POS_LAT_OFFSET_I);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
static uint32_t quantise_position_lon(int32_t lon7) {
    return (uint32_t)((((int64_t)lon7 + IOTDATA_POS_LON_OFFSET_I) * IOTDATA_POS_SCALE + IOTDATA_POS_LON_OFFSET_I) / IOTDATA_POS_LON_RANGE_I);
}
#endif
#if !defined(IOTDATA_NO_DECODE) || !defined(IOTDATA_NO_DUMP)
static int32_t dequantise_position_lon(uint32_t raw) {
    return (int32_t)(((int64_t)raw * IOTDATA_POS_LON_RANGE_I + IOTDATA_POS_SCALE / 2) / IOTDATA_POS_SCALE - IOTDATA_POS_LON_OFFSET_I);
}
#endif
#endif
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_position(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_position_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_position_lat(v->position_lat), IOTDATA_POS_LAT_BITS) && bits_write(buf, bb, bp, quantise_position_lon(v->position_lon), IOTDATA_POS_LON_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_position_by_slot(iotdata_encoder_t *enc, int slot, iotdata_double_t latitude, iotdata_double_t longitude) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_POSITION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (latitude < (iotdata_double_t)IOTDATA_POS_LAT_LOW)
        return IOTDATA_ERR_POSITION_LAT_LOW;
    if (latitude > (iotdata_double_t)IOTDATA_POS_LAT_HIGH)
        return IOTDATA_ERR_POSITION_LAT_HIGH;
    if (longitude < (iotdata_double_t)IOTDATA_POS_LON_LOW)
        return IOTDATA_ERR_POSITION_LON_LOW;
    if (longitude > (iotdata_double_t)IOTDATA_POS_LON_HIGH)
        return IOTDATA_ERR_POSITION_LON_HIGH;
#endif
    iotdata_field_position_t v;
    v.position_lat = latitude;
    v.position_lon = longitude;
    PACK_DIRECT(enc, pack_position, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_POSITION);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_position(iotdata_encoder_t *enc, iotdata_double_t latitude, iotdata_double_t longitude) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_POSITION, si);
    return iotdata_encode_position_by_slot(enc, si, latitude, longitude);
}
#else
iotdata_status_t iotdata_encode_position(iotdata_encoder_t *enc, iotdata_double_t latitude, iotdata_double_t longitude) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_POSITION, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (latitude < (iotdata_double_t)IOTDATA_POS_LAT_LOW)
        return IOTDATA_ERR_POSITION_LAT_LOW;
    if (latitude > (iotdata_double_t)IOTDATA_POS_LAT_HIGH)
        return IOTDATA_ERR_POSITION_LAT_HIGH;
    if (longitude < (iotdata_double_t)IOTDATA_POS_LON_LOW)
        return IOTDATA_ERR_POSITION_LON_LOW;
    if (longitude > (iotdata_double_t)IOTDATA_POS_LON_HIGH)
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
static bool unpack_position(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_position_t *const v = vv;
    if (*bp + IOTDATA_POS_LAT_BITS + IOTDATA_POS_LON_BITS > bb)
        return false;
    v->position_lat = dequantise_position_lat(bits_read(buf, bb, bp, IOTDATA_POS_LAT_BITS));
    v->position_lon = dequantise_position_lon(bits_read(buf, bb, bp, IOTDATA_POS_LON_BITS));
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
static iotdata_status_t json_get_position(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
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
static void json_set_position(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
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
static int dump_position(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_POS_LAT_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.6f", (double)dequantise_position_lat(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_position_lat(r), 10000000, "");
#endif
    n = dump_add(dump, n, s, IOTDATA_POS_LAT_BITS, r, dump->_dec_buf, "-90..+90", "latitude");
    s = *bp;
    r = bits_read(buf, bb, bp, IOTDATA_POS_LON_BITS);
#if !defined(IOTDATA_NO_FLOATING)
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%.6f", (double)dequantise_position_lon(r));
#else
    fmt_scaled(dump->_dec_buf, sizeof(dump->_dec_buf), dequantise_position_lon(r), 10000000, "");
#endif
    n = dump_add(dump, n, s, IOTDATA_POS_LON_BITS, r, dump->_dec_buf, "-180..+180", "longitude");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_position(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_position_t *const v = vv;
#if !defined(IOTDATA_NO_FLOATING)
    bprintf(bp, "  %s:%s %.6f, %.6f\n", label, _padd(label), (double)v->position_lat, (double)v->position_lon);
#else
    const int32_t lat = v->position_lat, la = lat < 0 ? -lat : lat, lon = v->position_lon, lo = lon < 0 ? -lon : lon;
    bprintf(bp, "  %s:%s %s%d.%06d, %s%d.%06d\n", label, _padd(label), lat < 0 ? "-" : "", (int)(la / 10000000), (int)(la % 10000000), lon < 0 ? "-" : "", (int)(lo / 10000000), (int)(lo % 10000000));
#endif
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_position = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, position))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, position))
    _IOTDATA_OP_NAME("position")
    _IOTDATA_OP_BITS(IOTDATA_POS_LAT_BITS + IOTDATA_POS_LON_BITS)
    _IOTDATA_OP_PACK(pack_position)
    _IOTDATA_OP_UNPACK(unpack_position)
    _IOTDATA_OP_DUMP(dump_position)
    _IOTDATA_OP_PRINT(print_position)
    _IOTDATA_OP_JSON_SET(json_set_position)
    _IOTDATA_OP_JSON_GET(json_get_position)
};
#endif
// clang-format on

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
static bool pack_datetime(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_datetime_t *const v = vv;
    return bits_write(buf, bb, bp, quantise_datetime(v->datetime_secs), IOTDATA_DATETIME_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_datetime_by_slot(iotdata_encoder_t *enc, int slot, uint32_t seconds_from_year_start) {
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
iotdata_status_t iotdata_encode_datetime(iotdata_encoder_t *enc, uint32_t seconds_from_year_start) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_DATETIME, si);
    return iotdata_encode_datetime_by_slot(enc, si, seconds_from_year_start);
}
#else
iotdata_status_t iotdata_encode_datetime(iotdata_encoder_t *enc, uint32_t seconds_from_year_start) {
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
static bool unpack_datetime(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
static iotdata_status_t json_get_datetime(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_datetime(enc, (uint32_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_datetime(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_datetime_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->datetime_secs);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_datetime(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
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
static void print_datetime(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_datetime_t *const v = vv;
    bprintf(bp, "  %s:%s day %" PRIu32 " %02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 " (%" PRIu32 "s)\n", label, _padd(label), v->datetime_secs / 86400, (v->datetime_secs % 86400) / 3600, (v->datetime_secs % 3600) / 60, v->datetime_secs % 60,
            v->datetime_secs);
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_datetime = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, datetime))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, datetime))
    _IOTDATA_OP_NAME("datetime")
    _IOTDATA_OP_BITS(IOTDATA_DATETIME_BITS)
    _IOTDATA_OP_PACK(pack_datetime)
    _IOTDATA_OP_UNPACK(unpack_datetime)
    _IOTDATA_OP_DUMP(dump_datetime)
    _IOTDATA_OP_PRINT(print_datetime)
    _IOTDATA_OP_JSON_SET(json_set_datetime)
    _IOTDATA_OP_JSON_GET(json_get_datetime)
};
#endif
// clang-format on

/* =========================================================================
 * Field IMAGE
 *
 * Variable-length field: 8-bit length prefix + control byte + pixel data.
 * First variable-length data field in the protocol.
 *
 * Wire layout:
 *   [Length:8] [Control:8] [PixelData: Length-1 bytes]
 *
 * Length = number of bytes following (control + pixel data).
 * Total field size = 1 + Length bytes = (1 + Length) * 8 bits.
 *
 * Control byte:
 *   bits 7-6: pixel format (0=bilevel/1bpp, 1=grey4/2bpp, 2=grey16/4bpp)
 *   bits 5-4: size tier (0=24x18, 1=32x24, 2=48x36, 3=64x48)
 *   bits 3-2: compression (0=raw, 1=RLE, 2=heatshrink)
 *   bits 1-0: flags (bit1=fragment, bit0=invert)
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_IMAGE)
static const uint8_t _image_widths[] = { 24, 32, 48, 64 }, _image_heights[] = { 18, 24, 36, 48 }, _image_bits[] = { 1, 2, 4 };
size_t iotdata_image_pixel_count(uint8_t size_tier) {
    return size_tier < sizeof(_image_widths) / sizeof(_image_widths[0]) ? (size_t)_image_widths[size_tier] * (size_t)_image_heights[size_tier] : 0;
}
uint8_t iotdata_image_bpp(uint8_t pixel_format) {
    return pixel_format < sizeof(_image_bits) / sizeof(_image_bits[0]) ? _image_bits[pixel_format] : 0;
}
size_t iotdata_image_bytes(uint8_t pixel_format, uint8_t size_tier) {
    return (iotdata_image_pixel_count(size_tier) * (size_t)iotdata_image_bpp(pixel_format) + 7) / 8;
}
static uint8_t _pixel_get(const uint8_t *buf, size_t idx, uint8_t bpp) {
    if (bpp == 1)
        return (buf[idx / 8] >> (7 - (idx % 8))) & 1;
    else if (bpp == 2)
        return (buf[idx / 4] >> (6 - (idx % 4) * 2)) & 3;
    else if (bpp == 4)
        return (idx & 1) ? buf[idx / 2] & 0x0F : buf[idx / 2] >> 4;
    return 0;
}
static void _pixel_set(uint8_t *buf, size_t idx, uint8_t val, uint8_t bpp) {
    if (bpp == 1)
        buf[idx / 8] = (uint8_t)((buf[idx / 8] & ~(1U << (7 - (idx % 8)))) | ((val & 1) << (7 - (idx % 8))));
    else if (bpp == 2)
        buf[idx / 4] = (uint8_t)((buf[idx / 4] & ~(3U << (6 - (idx % 4) * 2))) | ((val & 3) << (6 - (idx % 4) * 2)));
    else if (bpp == 4)
        buf[idx / 2] = (uint8_t)(idx & 1 ? (buf[idx / 2] & 0xF0) | (val & 0x0F) : (buf[idx / 2] & 0x0F) | ((val & 0x0F) << 4));
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
size_t iotdata_image_rle_compress(const uint8_t *pixels, size_t pixel_count, uint8_t bpp, uint8_t *out, size_t out_max) {
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
size_t iotdata_image_rle_decompress(const uint8_t *compressed, size_t comp_len, uint8_t bpp, uint8_t *pixels, size_t pixel_buf_bytes) {
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
static void _hs_bw_init(_hs_bw_t *bw, uint8_t *buf, size_t max) {
    bw->buf = buf;
    bw->max = max;
    bw->byte_idx = 0;
    bw->bit_idx = 7;
    bw->overflow = false;
    if (max > 0)
        buf[0] = 0;
}
static void _hs_bw_put(_hs_bw_t *bw, uint32_t value, uint8_t nbits) {
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
static size_t _hs_bw_bytes(const _hs_bw_t *bw) {
    return bw->bit_idx == 7 ? bw->byte_idx : bw->byte_idx + 1;
}
typedef struct {
    const uint8_t *buf;
    size_t len;
    size_t byte_idx;
    uint8_t bit_idx;
} _hs_br_t;
static void _hs_br_init(_hs_br_t *br, const uint8_t *buf, size_t len) {
    br->buf = buf;
    br->len = len;
    br->byte_idx = 0;
    br->bit_idx = 7;
}
static int _hs_br_get(_hs_br_t *br, uint8_t nbits) {
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
static bool _hs_br_done(const _hs_br_t *br) {
    return br->byte_idx >= br->len;
}
size_t iotdata_image_hs_compress(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_max) {
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
size_t iotdata_image_hs_decompress(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_max) {
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

#if !defined(IOTDATA_NO_ENCODE)
static bool pack_image(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_image_enc_t *const v = vv;
    /* Length = 1 (control byte) + pixel data bytes */
    if (!bits_write(buf, bb, bp, (uint8_t)(1 + v->image_data_len), 8))
        return false;
    /* Control byte: format(2) | size(2) | compression(2) | flags(2) */
    if (!bits_write(buf, bb, bp, (uint8_t)((v->image_pixel_format << 6) | (v->image_size_tier << 4) | (v->image_compression << 2) | (v->image_flags & 0x03)), 8))
        return false;
    /* Pixel data (compressed or raw, as provided by caller) */
    for (int i = 0; i < v->image_data_len; i++)
        if (!bits_write(buf, bb, bp, v->image_data[i], 8))
            return false;
    return true;
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_image_by_slot(iotdata_encoder_t *enc, int slot, uint8_t pixel_format, uint8_t size_tier, uint8_t compression, uint8_t flags, const uint8_t *data, uint8_t data_len) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_IMAGE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (pixel_format > 2)
        return IOTDATA_ERR_IMAGE_FORMAT_HIGH;
    if (size_tier > 3)
        return IOTDATA_ERR_IMAGE_SIZE_HIGH;
    if (compression > 2)
        return IOTDATA_ERR_IMAGE_COMPRESSION_HIGH;
    if (!data && data_len > 0)
        return IOTDATA_ERR_IMAGE_DATA_NULL;
    if (data_len > IOTDATA_IMAGE_DATA_MAX)
        return IOTDATA_ERR_IMAGE_DATA_HIGH;
#endif
    iotdata_field_image_enc_t v;
    v.image_pixel_format = pixel_format;
    v.image_size_tier = size_tier;
    v.image_compression = compression;
    v.image_flags = flags & 0x03;
    v.image_data = data;
    v.image_data_len = data_len;
    PACK_DIRECT(enc, pack_image, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_IMAGE);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_image(iotdata_encoder_t *enc, uint8_t pixel_format, uint8_t size_tier, uint8_t compression, uint8_t flags, const uint8_t *data, uint8_t data_len) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_IMAGE, si);
    return iotdata_encode_image_by_slot(enc, si, pixel_format, size_tier, compression, flags, data, data_len);
}
#else
iotdata_status_t iotdata_encode_image(iotdata_encoder_t *enc, uint8_t pixel_format, uint8_t size_tier, uint8_t compression, uint8_t flags, const uint8_t *data, uint8_t data_len) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_IMAGE, si);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (pixel_format > 2)
        return IOTDATA_ERR_IMAGE_FORMAT_HIGH;
    if (size_tier > 3)
        return IOTDATA_ERR_IMAGE_SIZE_HIGH;
    if (compression > 2)
        return IOTDATA_ERR_IMAGE_COMPRESSION_HIGH;
    if (!data && data_len > 0)
        return IOTDATA_ERR_IMAGE_DATA_NULL;
    if (data_len > IOTDATA_IMAGE_DATA_MAX)
        return IOTDATA_ERR_IMAGE_DATA_HIGH;
#endif
    enc->image.image_pixel_format = pixel_format;
    enc->image.image_size_tier = size_tier;
    enc->image.image_compression = compression;
    enc->image.image_flags = flags & 0x03;
    enc->image.image_data = data;
    enc->image.image_data_len = data_len;
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_IMAGE);
    return IOTDATA_OK;
}
#endif
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_image(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
    iotdata_field_image_dec_t *const v = vv;
    if (*bp + 16 > bb)
        return false; /* need at least length + control */
    const uint8_t length = (uint8_t)bits_read(buf, bb, bp, 8);
    if (length < 1)
        return false; /* control byte required */
    if (*bp + (length * 8) > bb)
        return false; /* short data */
    const uint8_t control = (uint8_t)bits_read(buf, bb, bp, 8);
    v->image_pixel_format = (control >> 6) & 0x03;
    v->image_size_tier = (control >> 4) & 0x03;
    v->image_compression = (control >> 2) & 0x03;
    v->image_flags = control & 0x03;
    v->image_data_len = 0;
    for (int i = 0; i < (length - 1); i++) {
        const uint8_t pixel = (uint8_t)bits_read(buf, bb, bp, 8);
        if (i < IOTDATA_IMAGE_DATA_MAX)
            v->image_data[v->image_data_len++] = pixel;
    }
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
iotdata_status_t iotdata_decode_image(iotdata_decoder_t *dec, uint8_t *pixel_format, uint8_t *size_tier, uint8_t *compression, uint8_t *flags, const uint8_t **data, uint8_t *data_len, iotdata_field_image_scratch_t *scratch) {
    if (!scratch)
        return IOTDATA_ERR_BUF_NULL;
#if defined(IOTDATA_OPERATE_ORDERED)
    /* THE PIXELS LAND IN THE CALLER'S SCRATCH, which is the whole reason it is a parameter: there
       is no decoder storage here, and a 254-byte local would die on return with *data still
       pointing into it. */
    iotdata_field_image_dec_t *const v = &scratch->_image;
    RESOLVE_DEC_FIELD(dec, IOTDATA_FIELD_IMAGE, unpack_image, v);
#else
    CHECK_DEC_FIELD(dec, IOTDATA_FIELD_IMAGE);
    /* The pixels are already in the decoder, so the scratch goes unwritten and *data is the
       decoder's own buffer. It is still demanded, so that a caller cannot pass NULL in this mode
       and discover on the day it switches to ORDERED that there was nowhere to decode into. */
    const iotdata_field_image_dec_t *const v = &dec->image;
#endif
    if (pixel_format)
        *pixel_format = v->image_pixel_format;
    if (size_tier)
        *size_tier = v->image_size_tier;
    if (compression)
        *compression = v->image_compression;
    if (flags)
        *flags = v->image_flags;
    /* Good until the next decode on this decoder, or until the scratch dies -- whichever first. */
    if (data)
        *data = v->image_data;
    if (data_len)
        *data_len = v->image_data_len;
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_JSON) || !defined(IOTDATA_NO_DUMP) || !defined(IOTDATA_NO_PRINT)
static const char *_image_fmt_names[] = { "bilevel", "grey4", "grey16", "reserved" };
static const char *_image_size_names[] = { "24x18", "32x24", "48x36", "64x48" };
static const char *_image_comp_names[] = { "raw", "rle", "heatshrink", "reserved" };
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_image(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_image_dec_t *const v = vv;
    cJSON *obj = cJSON_AddObjectToObject(root, label);
    cJSON_AddStringToObject(obj, "format", _image_fmt_names[v->image_pixel_format & 0x03]);
    cJSON_AddStringToObject(obj, "size", _image_size_names[v->image_size_tier & 0x03]);
    cJSON_AddStringToObject(obj, "compression", _image_comp_names[v->image_compression & 0x03]);
    cJSON_AddBoolToObject(obj, "fragment", (v->image_flags & IOTDATA_IMAGE_FLAG_FRAGMENT) != 0);
    cJSON_AddBoolToObject(obj, "invert", (v->image_flags & IOTDATA_IMAGE_FLAG_INVERT) != 0);
    if (v->image_data_len > 0)
        cJSON_AddStringToObject(obj, "pixels", _b64_encode(v->image_data, v->image_data_len, scratch->image.b64));
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t json_get_image(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    /* Parse pixel_format */
    uint8_t fmt = 0;
    cJSON *jf = cJSON_GetObjectItem(j, "format");
    if (jf && jf->valuestring)
        for (int i = 0; i < (int)(sizeof(_image_fmt_names) / sizeof(_image_fmt_names[0])); i++)
            if (strcmp(jf->valuestring, _image_fmt_names[i]) == 0) {
                fmt = (uint8_t)i;
                break;
            }
    /* Parse size */
    uint8_t sz = 0;
    cJSON *js = cJSON_GetObjectItem(j, "size");
    if (js && js->valuestring)
        for (int i = 0; i < (int)(sizeof(_image_size_names) / sizeof(_image_size_names[0])); i++)
            if (strcmp(js->valuestring, _image_size_names[i]) == 0) {
                sz = (uint8_t)i;
                break;
            }
    /* Parse compression */
    uint8_t comp = 0;
    cJSON *jc = cJSON_GetObjectItem(j, "compression");
    if (jc && jc->valuestring)
        for (int i = 0; i < (int)(sizeof(_image_comp_names) / sizeof(_image_comp_names[0])); i++)
            if (strcmp(jc->valuestring, _image_comp_names[i]) == 0) {
                comp = (uint8_t)i;
                break;
            }
    /* Parse flags */
    uint8_t flags = 0;
    if (cJSON_IsTrue(cJSON_GetObjectItem(j, "fragment")))
        flags |= IOTDATA_IMAGE_FLAG_FRAGMENT;
    if (cJSON_IsTrue(cJSON_GetObjectItem(j, "invert")))
        flags |= IOTDATA_IMAGE_FLAG_INVERT;
    /* Parse pixels (base64 → raw bytes) */
    cJSON *jp = cJSON_GetObjectItem(j, "pixels");
    return iotdata_encode_image(enc, fmt, sz, comp, flags, scratch->image.data, jp && jp->valuestring ? (uint8_t)_b64_decode(jp->valuestring, scratch->image.data, sizeof(scratch->image.data)) : 0);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_image(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    if (*bp + 16 > bb)
        return n;
    /* Length byte */
    size_t s = *bp;
    const uint8_t length = (uint8_t)bits_read(buf, bb, bp, 8);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 " (%" PRIu8 " total)", length, 1 + length);
    n = dump_add(dump, n, s, 8, length, dump->_dec_buf, "1..255", "image_length");
    /* Control byte */
    s = *bp;
    const uint8_t control = (uint8_t)bits_read(buf, bb, bp, 8);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%s %s %s%s%s", _image_fmt_names[(control >> 6) & 3], _image_size_names[(control >> 4) & 3], _image_comp_names[(control >> 2) & 3], (control & IOTDATA_IMAGE_FLAG_FRAGMENT) ? " frag" : "",
             (control & IOTDATA_IMAGE_FLAG_INVERT) ? " inv" : "");
    n = dump_add(dump, n, s, 8, control, dump->_dec_buf, "fmt|sz|comp|flg", "image_control");
    /* Pixel data (show as single span) */
    const uint8_t data_len = (length > 1) ? (uint8_t)(length - 1) : 0;
    if (data_len > 0) {
        s = *bp;
        const size_t data_bits = (size_t)data_len * 8;
        if (*bp + data_bits <= bb)
            *bp += data_bits;
        else
            *bp = bb;
        snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8 " bytes", data_len);
        n = dump_add(dump, n, s, data_bits, 0, dump->_dec_buf, "pixel data", "image_pixels");
    }
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_image(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_image_dec_t *const v = vv;
    bprintf(bp, "  %s:%s %s %s %s, %" PRIu8 " bytes%s%s\n", label, _padd(label), _image_fmt_names[v->image_pixel_format & 3], _image_size_names[v->image_size_tier & 3], _image_comp_names[v->image_compression & 3], v->image_data_len,
            (v->image_flags & IOTDATA_IMAGE_FLAG_FRAGMENT) ? " [fragment]" : "", (v->image_flags & IOTDATA_IMAGE_FLAG_INVERT) ? " [inverted]" : "");
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_image = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, image))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, image))
    _IOTDATA_OP_NAME("image")
    _IOTDATA_OP_BITS(0) /* variable length */
    _IOTDATA_OP_PACK(pack_image)
    _IOTDATA_OP_UNPACK(unpack_image)
    _IOTDATA_OP_DUMP(dump_image)
    _IOTDATA_OP_PRINT(print_image)
    _IOTDATA_OP_JSON_SET(json_set_image)
    _IOTDATA_OP_JSON_GET(json_get_image)
};
#endif
// clang-format on

/* =========================================================================
 * Field FLAGS
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_FLAGS)
// quantise
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_flags(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_flags_t *const v = vv;
    return bits_write(buf, bb, bp, v->flags, IOTDATA_FLAGS_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_flags_by_slot(iotdata_encoder_t *enc, int slot, uint8_t flags) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_FLAGS, si);
    iotdata_field_flags_t v;
    v.flags = flags;
    PACK_DIRECT(enc, pack_flags, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_FLAGS);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_flags(iotdata_encoder_t *enc, uint8_t flags) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_FLAGS, si);
    return iotdata_encode_flags_by_slot(enc, si, flags);
}
#else
iotdata_status_t iotdata_encode_flags(iotdata_encoder_t *enc, uint8_t flags) {
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
static bool unpack_flags(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
static iotdata_status_t json_get_flags(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_flags(enc, (uint8_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_flags(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_flags_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->flags);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_flags(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_FLAGS_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "0x%02" PRIX32, r);
    n = dump_add(dump, n, s, IOTDATA_FLAGS_BITS, r, dump->_dec_buf, "8-bit bitmask", "flags");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_flags(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_flags_t *const v = vv;
    bprintf(bp, "  %s:%s 0x%02" PRIX8 "\n", label, _padd(label), v->flags);
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_flags = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, flags))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, flags))
    _IOTDATA_OP_NAME("flags")
    _IOTDATA_OP_BITS(IOTDATA_FLAGS_BITS)
    _IOTDATA_OP_PACK(pack_flags)
    _IOTDATA_OP_UNPACK(unpack_flags)
    _IOTDATA_OP_DUMP(dump_flags)
    _IOTDATA_OP_PRINT(print_flags)
    _IOTDATA_OP_JSON_SET(json_set_flags)
    _IOTDATA_OP_JSON_GET(json_get_flags)
};
#endif
// clang-format on

/* =========================================================================
 * Field BITS32
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_BITS32)
// quantise
#if !defined(IOTDATA_NO_ENCODE)
static bool pack_bits32(uint8_t *buf, size_t bb, size_t *bp, const void *vv) {
    const iotdata_field_bits32_t *const v = vv;
    return bits_write(buf, bb, bp, v->bits32, IOTDATA_BITS32_BITS);
}
#endif
#if !defined(IOTDATA_NO_ENCODE)
#if defined(IOTDATA_OPERATE_ORDERED)
/* The precise form: this slot, not "the next one of this type". Slots between the cursor
   and here are left absent. */
iotdata_status_t iotdata_encode_bits32_by_slot(iotdata_encoder_t *enc, int slot, uint32_t bits32) {
    CHECK_CTX_ACTIVE(enc);
    int si = slot;
    RESOLVE_SLOT_EXACT(enc, IOTDATA_FIELD_BITS32, si);
    iotdata_field_bits32_t v;
    v.bits32 = bits32;
    PACK_DIRECT(enc, pack_bits32, &v);
    COMMIT_SLOT(enc, si, IOTDATA_FIELD_BITS32);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_bits32(iotdata_encoder_t *enc, uint32_t bits32) {
    CHECK_CTX_ACTIVE(enc);
    int si = IOTDATA_SLOT_NONE;
    RESOLVE_SLOT(enc, IOTDATA_FIELD_BITS32, si);
    return iotdata_encode_bits32_by_slot(enc, si, bits32);
}
#else
iotdata_status_t iotdata_encode_bits32(iotdata_encoder_t *enc, uint32_t bits32) {
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
static bool unpack_bits32(const uint8_t *buf, size_t bb, size_t *bp, void *vv) {
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
static iotdata_status_t json_get_bits32(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)scratch;
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j)
        return IOTDATA_OK;
    return iotdata_encode_bits32(enc, (uint32_t)j->valueint);
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void json_set_bits32(cJSON *root, const void *vv, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    const iotdata_field_bits32_t *const v = vv;
    (void)scratch;
    cJSON_AddNumberToObject(root, label, v->bits32);
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int dump_bits32(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    size_t s = *bp;
    uint32_t r = bits_read(buf, bb, bp, IOTDATA_BITS32_BITS);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "0x%08" PRIX32, r);
    n = dump_add(dump, n, s, IOTDATA_BITS32_BITS, r, dump->_dec_buf, "32-bit unit", "bits32");
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void print_bits32(const void *vv, iotdata_buf_t *bp, const char *label) {
    const iotdata_field_bits32_t *const v = vv;
    bprintf(bp, "  %s:%s 0x%08" PRIX32 "\n", label, _padd(label), v->bits32);
}
#endif
// clang-format off
static const iotdata_field_ops_t _iotdata_field_def_bits32 = {
    _IOTDATA_OP_ENC_OFF(offsetof(iotdata_encoder_t, bits32))
    _IOTDATA_OP_DEC_OFF(offsetof(iotdata_decoder_t, bits32))
    _IOTDATA_OP_NAME("bits32")
    _IOTDATA_OP_BITS(IOTDATA_BITS32_BITS)
    _IOTDATA_OP_PACK(pack_bits32)
    _IOTDATA_OP_UNPACK(unpack_bits32)
    _IOTDATA_OP_DUMP(dump_bits32)
    _IOTDATA_OP_PRINT(print_bits32)
    _IOTDATA_OP_JSON_SET(json_set_bits32)
    _IOTDATA_OP_JSON_GET(json_get_bits32)
};
#endif
// clang-format on

/* =========================================================================
 * Field TLV
 * ========================================================================= */

#if defined(IOTDATA_ENABLE_TLV)
#if !defined(IOTDATA_NO_ENCODE)
iotdata_status_t iotdata_encode_tlv(iotdata_encoder_t *enc, uint8_t type, const uint8_t *data, uint8_t length) {
    CHECK_CTX_ACTIVE(enc);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (type > IOTDATA_TLV_TYPE_MAX)
        return IOTDATA_ERR_TLV_TYPE_HIGH;
    /* a zero-length TLV is a legitimate message -- an empty RECEIVE says "listening, for anything"
       -- and the packer never dereferences data when length is 0, so NULL is allowed there */
    if (!data && length > 0)
        return IOTDATA_ERR_TLV_DATA_NULL;
    /* length is uint8_t, and IOTDATA_TLV_LENGTH_MAX is what that field holds, so always in range */
#endif
    if (enc->tlv_count >= IOTDATA_TLV_MAX)
        return IOTDATA_ERR_TLV_FULL;
    const int idx = enc->tlv_count++;
    enc->tlv[idx].format = IOTDATA_TLV_FMT_RAW;
    enc->tlv[idx].type = type;
    enc->tlv[idx].length = length;
    enc->tlv[idx].data = data;
    IOTDATA_FIELD_SET(enc->fields, IOTDATA_FIELD_TLV);
    return IOTDATA_OK;
}
iotdata_status_t iotdata_encode_tlv_string(iotdata_encoder_t *enc, uint8_t type, const char *str) {
    CHECK_CTX_ACTIVE(enc);
#if !defined(IOTDATA_NO_CHECKS_TYPES)
    if (type > IOTDATA_TLV_TYPE_MAX)
        return IOTDATA_ERR_TLV_TYPE_HIGH;
    if (!str)
        return IOTDATA_ERR_TLV_STR_NULL;
    const size_t slen = strlen(str);
    if (slen > IOTDATA_TLV_STR_LEN_MAX)
        return IOTDATA_ERR_TLV_STR_LEN_HIGH;
    for (size_t i = 0; i < slen; i++)
        if (char_to_sixbit(str[i]) < 0)
            return IOTDATA_ERR_TLV_STR_CHAR_INVALID;
#else
    size_t slen = strlen(str);
    if (slen > IOTDATA_TLV_STR_LEN_MAX)
        slen = IOTDATA_TLV_STR_LEN_MAX;
#endif
    if (enc->tlv_count >= IOTDATA_TLV_MAX)
        return IOTDATA_ERR_TLV_FULL;
    const int idx = enc->tlv_count++;
    enc->tlv[idx].format = IOTDATA_TLV_FMT_STRING;
    enc->tlv[idx].type = type;
    enc->tlv[idx].length = (uint8_t)slen;
    enc->tlv[idx].str = str;
    IOTDATA_FIELD_SET(enc->fields, IOTDATA_FIELD_TLV);
    return IOTDATA_OK;
}
static bool pack_tlv(uint8_t *buf, size_t bb, size_t *bp, const iotdata_encoder_t *enc) {
    for (int i = 0; i < enc->tlv_count; i++) {
        if (!bits_write(buf, bb, bp, enc->tlv[i].format, IOTDATA_TLV_FMT_BITS))
            return false;
        if (!bits_write(buf, bb, bp, enc->tlv[i].type, IOTDATA_TLV_TYPE_BITS))
            return false;
        if (!bits_write(buf, bb, bp, (i < enc->tlv_count - 1) ? 1 : 0, IOTDATA_TLV_MORE_BITS))
            return false;
        if (!bits_write(buf, bb, bp, enc->tlv[i].length, IOTDATA_TLV_LENGTH_BITS))
            return false;
        for (int j = 0, l = enc->tlv[i].format == IOTDATA_TLV_FMT_STRING ? IOTDATA_TLV_CHAR_BITS : 8; j < enc->tlv[i].length; j++)
            if (!bits_write(buf, bb, bp, enc->tlv[i].format == IOTDATA_TLV_FMT_STRING ? (uint32_t)char_to_sixbit(enc->tlv[i].str[j]) : enc->tlv[i].data[j], (uint8_t)l))
                return false;
    }
    return true;
}
#endif
#if !defined(IOTDATA_NO_DECODE)
static bool unpack_tlv(const uint8_t *buf, size_t bb, size_t *bp, iotdata_decoder_t *dec) {
    bool more = true;
    while (more) {
        if (*bp + IOTDATA_TLV_HEADER_BITS > bb)
            return false;
        const uint8_t format = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_FMT_BITS);
        const uint8_t type = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_TYPE_BITS);
        more = bits_read(buf, bb, bp, IOTDATA_TLV_MORE_BITS) != 0;
        const uint8_t length = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_LENGTH_BITS);
        const uint8_t bpv = format == IOTDATA_TLV_FMT_STRING ? IOTDATA_TLV_CHAR_BITS : 8;
        if (*bp + (size_t)bpv * length > bb)
            return false;
        if (dec->tlv_count >= IOTDATA_TLV_MAX) {
            *bp += (size_t)bpv * length;
        } else {
            const int idx = dec->tlv_count++;
            dec->tlv[idx].format = format;
            dec->tlv[idx].type = type;
            dec->tlv[idx].length = length;
            if (format == IOTDATA_TLV_FMT_STRING) {
                for (int j = 0; j < length; j++)
                    dec->tlv[idx].str[j] = sixbit_to_char((uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_CHAR_BITS));
                dec->tlv[idx].str[length] = '\0';
            } else {
                for (int j = 0; j < length; j++)
                    dec->tlv[idx].raw[j] = (uint8_t)bits_read(buf, bb, bp, 8);
            }
        }
    }
    return true;
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void _json_set_tlv_data(cJSON *obj, const iotdata_decoder_tlv_t *t, iotdata_decode_to_json_scratch_t *scratch) {
    cJSON_AddStringToObject(obj, "format", t->format == IOTDATA_TLV_FMT_STRING ? "string" : "raw");
    if (t->format == IOTDATA_TLV_FMT_STRING)
        cJSON_AddStringToObject(obj, "data", t->str);
    else
        cJSON_AddStringToObject(obj, "data", _b64_encode(t->raw, t->length, scratch->tlv.b64));
}
static void json_set_tlv(cJSON *root, const iotdata_decoder_t *dec, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    cJSON *arr = cJSON_AddArrayToObject(root, label);
    for (int i = 0; i < dec->tlv_count; i++) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "type", dec->tlv[i].type);
        _json_set_tlv_data(obj, &dec->tlv[i], scratch);
        cJSON_AddItemToArray(arr, obj);
    }
}
#endif
#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t _json_get_tlv_generic(cJSON *item, iotdata_encoder_t *enc, int tidx, uint8_t type, iotdata_encode_from_json_scratch_tlv_t *scratch) {
    cJSON *format = cJSON_GetObjectItem(item, "format");
    cJSON *data = cJSON_GetObjectItem(item, "data");
    if (!data || !cJSON_IsString(data))
        return IOTDATA_OK; /* skip malformed */
    if (strcmp(format ? format->valuestring : "raw", "string") == 0) {
        snprintf(scratch->str[tidx], IOTDATA_TLV_STR_LEN_MAX + 1, "%s", data->valuestring);
        return iotdata_encode_tlv_string(enc, type, scratch->str[tidx]);
    } else
        return iotdata_encode_tlv(enc, type, scratch->raw[tidx], (uint8_t)_b64_decode(data->valuestring, scratch->raw[tidx], IOTDATA_TLV_LENGTH_MAX));
}
static iotdata_status_t json_get_tlv(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_tlv_t *scratch) {
    cJSON *j = cJSON_GetObjectItem(root, label);
    if (!j || !cJSON_IsArray(j))
        return IOTDATA_OK;
    cJSON *item;
    int tidx = 0;
    cJSON_ArrayForEach(item, j) {
        if (tidx >= IOTDATA_TLV_MAX)
            break;
        cJSON *j_type = cJSON_GetObjectItem(item, "type");
        if (j_type) {
            const uint8_t type = (uint8_t)j_type->valueint;
            iotdata_status_t rc;
            rc = _json_get_tlv_generic(item, enc, tidx, type, scratch);
            if (rc != IOTDATA_OK)
                return rc;
            tidx++;
        }
    }
    return IOTDATA_OK;
}
#endif
#if !defined(IOTDATA_NO_DUMP)
static int _dump_tlv_data(size_t *bp, iotdata_dump_t *dump, int n, uint8_t format, uint8_t length, int tlv_idx, const char *name) {
    const size_t data_bits = (format == IOTDATA_TLV_FMT_STRING) ? (size_t)length * IOTDATA_TLV_CHAR_BITS : (size_t)length * 8;
    snprintf(dump->_name_buf, sizeof(dump->_name_buf), "tlv[%d].%s", tlv_idx, name);
    snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "(%" PRIu32 " bits)", (uint32_t)data_bits); // XXX string
    n = dump_add(dump, n, *bp, data_bits, 0, dump->_dec_buf, format == IOTDATA_TLV_FMT_STRING ? "6-bit chars" : "raw bytes", dump->_name_buf);
    *bp += data_bits;
    return n;
}
static int dump_tlv(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label) {
    (void)label;
    bool more = true;
    int tlv_idx = 0;
    while (more && *bp + IOTDATA_TLV_HEADER_BITS <= bb) {
        size_t s = *bp;
        const uint8_t format = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_FMT_BITS);
        const uint8_t type = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_TYPE_BITS);
        more = bits_read(buf, bb, bp, IOTDATA_TLV_MORE_BITS) != 0;
        snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%s type=0x%02" PRIX8 " more=%d", format == IOTDATA_TLV_FMT_STRING ? "str" : "raw", type, more ? 1 : 0);
        snprintf(dump->_name_buf, sizeof(dump->_name_buf), "tlv[%d].hdr", tlv_idx);
        n = dump_add(dump, n, s, IOTDATA_TLV_FMT_BITS + IOTDATA_TLV_TYPE_BITS + IOTDATA_TLV_MORE_BITS, 0, dump->_dec_buf, "format+type+more", dump->_name_buf);
        s = *bp;
        const uint8_t length = (uint8_t)bits_read(buf, bb, bp, IOTDATA_TLV_LENGTH_BITS);
        snprintf(dump->_dec_buf, sizeof(dump->_dec_buf), "%" PRIu8, length);
        snprintf(dump->_name_buf, sizeof(dump->_name_buf), "tlv[%d].len", tlv_idx);
        n = dump_add(dump, n, s, IOTDATA_TLV_LENGTH_BITS, length, dump->_dec_buf, "0..255", dump->_name_buf);
        if (length > 0) {
            n = _dump_tlv_data(bp, dump, n, format, length, tlv_idx, "data");
        }
        tlv_idx++;
    }
    return n;
}
#endif
#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void _print_tlv_data(const iotdata_decoder_tlv_t *t, iotdata_buf_t *bp, int i) {
    /* Application-defined TLVs (0x20+) */
    if (t->format == IOTDATA_TLV_FMT_STRING) {
        bprintf(bp, "    [%d] type=%" PRIu8 " str(%" PRIu8 ")=\"%s\"\n", i, t->type, t->length, t->str);
    } else {
        bprintf(bp, "    [%d] type=%" PRIu8 " raw(%" PRIu8 ")=", i, t->type, t->length);
        for (int j = 0; j < t->length && j < 16; j++)
            bprintf(bp, "%02" PRIX8, t->raw[j]);
        if (t->length > 16)
            bprintf(bp, "...");
        bprintf(bp, "\n");
    }
}
static void print_tlv(const iotdata_decoder_t *dec, iotdata_buf_t *bp, const char *label) {
    bprintf(bp, "  %s: %" PRIu8 " TLV entries\n", label, dec->tlv_count);
    for (int i = 0; i < dec->tlv_count; i++)
        _print_tlv_data(&dec->tlv[i], bp, i);
}
#endif

/* TLV wrappers matching generic field-op signatures (close as practical) so
 * that the generic encode/decode/dump/print/json paths in iotdata.c stay
 * field-agnostic. When IOTDATA_ENABLE_TLV is unset the #else branch below
 * provides empty stubs with the same signatures, so call sites compile
 * unconditionally. */

#if !defined(IOTDATA_NO_ENCODE)
static void iotdata_encode_tlv_begin(iotdata_encoder_t *enc) {
    enc->tlv_count = 0;
}
static void iotdata_encode_tlv_pres(const iotdata_encoder_t *enc, uint8_t *pres) {
    if (IOTDATA_FIELD_PRESENT(enc->fields, IOTDATA_FIELD_TLV))
        pres[0] |= IOTDATA_PRES_TLV;
}
static bool iotdata_encode_tlv_data(uint8_t *buf, size_t bb, size_t *bp, const iotdata_encoder_t *enc) {
    if (IOTDATA_FIELD_PRESENT(enc->fields, IOTDATA_FIELD_TLV))
        return pack_tlv(buf, bb, bp, enc);
    return true;
}
#endif

#if !defined(IOTDATA_NO_DECODE)
static bool iotdata_decode_tlv_data(const uint8_t *buf, size_t bb, size_t *bp, iotdata_decoder_t *dec, uint8_t pres0) {
    dec->tlv_count = 0;
    if ((pres0 & IOTDATA_PRES_TLV) != 0) {
        IOTDATA_FIELD_SET(dec->fields, IOTDATA_FIELD_TLV);
        return unpack_tlv(buf, bb, bp, dec);
    }
    return true;
}
#endif

#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static void iotdata_decode_tlv_json_set(cJSON *root, const iotdata_decoder_t *dec, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    if (IOTDATA_FIELD_PRESENT(dec->fields, IOTDATA_FIELD_TLV))
        json_set_tlv(root, dec, label, scratch);
}
#endif

#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static iotdata_status_t iotdata_encode_tlv_json_get(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    return json_get_tlv(root, enc, label, &scratch->tlv);
}
#endif

#if !defined(IOTDATA_NO_DUMP)
static int iotdata_dump_tlv(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label, uint8_t pres0) {
    if ((pres0 & IOTDATA_PRES_TLV) != 0)
        return dump_tlv(buf, bb, bp, dump, n, label);
    return n;
}
#endif

#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static void iotdata_print_tlv(const iotdata_decoder_t *dec, iotdata_buf_t *bp, const char *label) {
    if (IOTDATA_FIELD_PRESENT(dec->fields, IOTDATA_FIELD_TLV))
        print_tlv(dec, bp, label);
}
#endif

#else /* !IOTDATA_ENABLE_TLV: empty stubs with matching signatures */

#if !defined(IOTDATA_NO_ENCODE)
static inline void iotdata_encode_tlv_begin(iotdata_encoder_t *enc) {
    (void)enc;
}
static inline void iotdata_encode_tlv_pres(const iotdata_encoder_t *enc, uint8_t *pres) {
    (void)enc;
    (void)pres;
}
static inline bool iotdata_encode_tlv_data(uint8_t *buf, size_t bb, size_t *bp, const iotdata_encoder_t *enc) {
    (void)buf;
    (void)bb;
    (void)bp;
    (void)enc;
    return true;
}
#endif

#if !defined(IOTDATA_NO_DECODE)
static inline bool iotdata_decode_tlv_data(const uint8_t *buf, size_t bb, size_t *bp, iotdata_decoder_t *dec, uint8_t pres0) {
    (void)buf;
    (void)bb;
    (void)bp;
    (void)dec;
    (void)pres0;
    return true;
}
#endif

#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_DECODE)
static inline void iotdata_decode_tlv_json_set(cJSON *root, const iotdata_decoder_t *dec, const char *label, iotdata_decode_to_json_scratch_t *scratch) {
    (void)root;
    (void)dec;
    (void)label;
    (void)scratch;
}
#endif

#if !defined(IOTDATA_NO_JSON) && !defined(IOTDATA_NO_ENCODE)
static inline iotdata_status_t iotdata_encode_tlv_json_get(cJSON *root, iotdata_encoder_t *enc, const char *label, iotdata_encode_from_json_scratch_t *scratch) {
    (void)root;
    (void)enc;
    (void)label;
    (void)scratch;
    return IOTDATA_OK;
}
#endif

#if !defined(IOTDATA_NO_DUMP)
static inline int iotdata_dump_tlv(const uint8_t *buf, size_t bb, size_t *bp, iotdata_dump_t *dump, int n, const char *label, uint8_t pres0) {
    (void)buf;
    (void)bb;
    (void)bp;
    (void)dump;
    (void)label;
    (void)pres0;
    return n;
}
#endif

#if !defined(IOTDATA_NO_PRINT) && !defined(IOTDATA_NO_DECODE)
static inline void iotdata_print_tlv(const iotdata_decoder_t *dec, iotdata_buf_t *bp, const char *label) {
    (void)dec;
    (void)bp;
    (void)label;
}
#endif

#endif
