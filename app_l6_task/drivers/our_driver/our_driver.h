#ifndef OUR_DRIVER_H
#define OUR_DRIVER_H

#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>

/* Custom sensor attribute for LED blink rate */
enum our_driver_attribute {
    /* Custom attribute: LED blink rate in milliseconds */
    SENSOR_ATTR_LED_BLINK_RATE = SENSOR_ATTR_PRIV_START,
};

/* Valid blink rate range */
#define LED_BLINK_RATE_MIN_MS 100
#define LED_BLINK_RATE_MAX_MS 5000
#define LED_BLINK_RATE_DEFAULT_MS 1000

/**
 * @brief Custom API: Set LED blink rate
 * 
 * This is a custom extension function that directly sets the blink rate
 * parameter in the driver's dynamic data structure.
 * 
 * @param dev Pointer to the device structure
 * @param rate_ms Blink rate in milliseconds (100-5000)
 * @return 0 on success, negative errno on failure
 */
int our_driver_set_blink_rate(const struct device *dev, uint32_t rate_ms);

/**
 * @brief Custom API: Get LED blink rate
 * 
 * This is a custom extension function that directly retrieves the blink rate
 * parameter from the driver's dynamic data structure.
 * 
 * @param dev Pointer to the device structure
 * @param rate_ms Pointer to store the current blink rate
 * @return 0 on success, negative errno on failure
 */
int our_driver_get_blink_rate(const struct device *dev, uint32_t *rate_ms);


/**
 * @brief Custom API: Get total blink count
 * 
 * @param dev Pointer to the device structure
 * @param count Pointer to store the blink count
 * @return 0 on success, negative errno on failure
 */
int our_driver_get_blink_count(const struct device *dev, uint32_t *count);

#endif /* OUR_DRIVER_H */
