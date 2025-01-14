#ifndef SENSOR_PORT_SCHEMA_H
#define SENSOR_PORT_SCHEMA_H

/**
 * @file PortSchema.h
 * @author Kalina Knight (kalina.knight77@gmail.com)
 * @brief Sensor port schema definitions as descibed the README.
 * Schema's include the functions for encoding the data to the LoRaWAN payload as well.
 *
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <math.h>
#include <stdint.h>
#include "Logging.h"        /**< Go here to change the logging level for the entire application. */


/**
 * @brief Struct with data from sensors and their validity.
 * Data can be invalid for a variety of reasons e.g. sensor experienced an error taking a reading, the GPS may not have
 * a fix, etc.
 */
struct sensorData {
    struct {
        float value;
        bool is_valid;
    } battery_mv; /**< Battery mV. */
    struct {
        float value;
        bool is_valid;
    } temperature; /**< Temperature: degrees C. */
    struct {
        float value;
        bool is_valid;
    } humidity; /**< Relative humidity: %). */
    struct {
        uint32_t value;
        bool is_valid;
    } pressure; /**< Air pressure: Pa. */
    struct {
        uint32_t value;
        bool is_valid;
    } gas_resist; /**< Gas Resistance doesn't have units. */
    struct {
        float latitude;
        float longitude;
        bool is_valid;
    } location; /**< Location latitude & longitude in degrees. */
     struct {
        float value;
        bool is_valid;
        float ADCval;
    } current_A; /**< Current sensor A. */
};

/** @brief sensorPortSchema describes how each sensors data should be encoded. */
class sensorPortSchema {
  public:
    uint8_t n_bytes;    /**< Total length in payload - assumed to be split equally amongst n_values. */
    uint8_t n_values;   /**< Number of values sent for sensor data. */
    float scale_factor; /**< Only int values are encoded. To send a float value, mulitply by scale_factor to encode;
                             then divide by scale_factor to decode. */
    bool is_signed;     /**< Value has a sign and hence can be negative. */

    /**
     * @brief Byte encodes the given sensor data into the payload according to the sensor port schema.
     * @details Calls a template function defined in PortSchema.cpp that can take in sensor_data of various types.
     * Feel free to add a new sensor_data type overload of encodeData() if necessary.
     * If the sensor data is not valid, for whatever reason, a value close to max (for the number of bytes) will be
     * encoded instead. The decoder then knows to ignore the data as it is invalid. If the data is invalid a segment of
     * 0x7F7F7F7F (signed) or 0xFFFFFFFF (unsigned) will be encoded and sent instead of the invalid data. E.g. For an
     * invalid 2 byte signed the value will be 0x7f7f.
     * @param sensor_data Sensor data to encode (valid data types: int, float, uint8_t, uint16_t, uint32_t).
     * @param valid Validity of given sensor data.
     * @param payload_buffer Payload buffer for data to be written into.
     * @param current_buffer_len Length of current data in the buffer, used to avoid overwriting data.
     * @return Total length of data encoded to payload_buffer.
     */
    uint8_t encodeData(int sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const;
    uint8_t encodeData(float sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const;
    uint8_t encodeData(uint8_t sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const;
    uint8_t encodeData(uint16_t sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const;
    uint8_t encodeData(uint32_t sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const;

    /**
     * @brief Byte decodes the given buffer into the sensor data according to the given sensor port schema.
     * @details Calls a template function defined in PortSchema.cpp that can return sensor_data of various types.
     * Feel free to add a new sensor_data type overload of decodeData() if necessary.
     * If the sensor data is not valid, for whatever reason, the valid flag will be set to false and no data will be
     * decoded to sensor_data.
     * @param sensor_data Resulting decoded sensor data (valid data types: int, float, uint8_t, uint16_t, uint32_t).
     * @param valid Validity of sensor data.
     * @param buffer Buffer that data will be decoded from.
     * @param buf_pos Start decoding from this byte, used to avoid header data.
     * @return Total length of data decoded from buffer.
     */
    uint8_t decodeData(int *sensor_data, bool *valid, uint8_t *buffer, uint8_t buff_pos) const;
    uint8_t decodeData(float *sensor_data, bool *valid, uint8_t *buffer, uint8_t buff_pos) const;
    uint8_t decodeData(uint8_t *sensor_data, bool *valid, uint8_t *buffer, uint8_t buff_pos) const;
    uint8_t decodeData(uint16_t *sensor_data, bool *valid, uint8_t *buffer, uint8_t buff_pos) const;
    uint8_t decodeData(uint32_t *sensor_data, bool *valid, uint8_t *buffer, uint8_t buff_pos) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SCHEMA DEFINITIONS: See readme for definitions in tabular format.

static const sensorPortSchema timestampSchema = { // units: s
    .n_bytes = 4,
    .n_values = 1,
    .scale_factor = 1,
    .is_signed = false
};

static const sensorPortSchema batteryVoltageSchema = { // units: mV
    .n_bytes = 2,
    .n_values = 1,
    .scale_factor = 1,
    .is_signed = false
};

static const sensorPortSchema temperatureSchema = { // units: degrees C
    .n_bytes = 2,
    .n_values = 1,
    .scale_factor = pow(10.0, 2), // 2 decimal places
    .is_signed = true
};

/** NOTE: relativeHumidity could instead have the same schema as temperature if more resolution is desired. */
static const sensorPortSchema relativeHumiditySchema = { // units: %
    .n_bytes = 1,
    .n_values = 1,
    .scale_factor = (float)(UINT8_MAX / 100.0), // percentage (0->100) is scaled to a byte (0->255)
    .is_signed = false
};

static const sensorPortSchema airPressureSchema = { // units: Pa
    .n_bytes = 4,
    .n_values = 1,
    .scale_factor = 1,
    .is_signed = false
};

static const sensorPortSchema gasResistanceSchema = { // units: ??
    .n_bytes = 4,
    .n_values = 1,
    .scale_factor = 1,
    .is_signed = false
};

static const sensorPortSchema locationSchema = { // units: degrees
    .n_bytes = 8,                                // split equally: 4 bytes lat, 4 bytes lng
    .n_values = 2,                               // lat and lng
    .scale_factor = pow(10.0, 4),                // 4 decimal places
    .is_signed = true
};

static const sensorPortSchema currentSensorSchema = { // units: A
    // chaned vals to 2 and bytes to 4
    .n_bytes = 6,
    .n_values = 2,      // could try changing this for future iterations (add more values)
    .scale_factor = pow(10.0, 2), // 2 decimal places
    .is_signed = true
};

/* An example of a new sensor:
static const sensorPortSchema newSensorSchema = {
    .n_bytes = 1,
    .n_values = 1,
    .scale_factor = 1,
    .is_signed = false
};
*/

#endif // SENSOR_PORT_SCHEMA_H