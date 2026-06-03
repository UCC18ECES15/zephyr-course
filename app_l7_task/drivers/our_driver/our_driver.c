#define DT_DRV_COMPAT our_driver

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "our_driver.h"

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

/* Driver data structure (dynamic data) */
struct our_driver_data {
    uint32_t blink_rate_ms;  /* LED blink rate in milliseconds */
    bool led_state;          /* Current LED state (ON/OFF) */
    uint32_t blink_count;    /* Total number of blinks performed */
};

/* Driver configuration structure (static data) */
struct our_driver_config {
    struct gpio_dt_spec led;
};

/* ========================================================================
 * TASK 2: CUSTOM EXTENSION API FUNCTIONS
 * ======================================================================== */

int our_driver_set_blink_rate(const struct device *dev, uint32_t rate_ms)
{
    if (dev == NULL) {
        LOG_ERR("Custom API: Device pointer is NULL");
        return -EINVAL;
    }
    
    struct our_driver_data *data = dev->data;
    
    if (rate_ms < LED_BLINK_RATE_MIN_MS || rate_ms > LED_BLINK_RATE_MAX_MS) {
        LOG_ERR("Custom API: Invalid blink rate: %d ms (valid range: %d-%d)",
                rate_ms, LED_BLINK_RATE_MIN_MS, LED_BLINK_RATE_MAX_MS);
        return -EINVAL;
    }
    
    data->blink_rate_ms = rate_ms;
    LOG_INF("Custom API: Blink rate set to %d ms", data->blink_rate_ms);
    
    return 0;
}

int our_driver_get_blink_rate(const struct device *dev, uint32_t *rate_ms)
{
    if (dev == NULL || rate_ms == NULL) {
        LOG_ERR("Custom API: Invalid parameters");
        return -EINVAL;
    }
    
    struct our_driver_data *data = dev->data;
    *rate_ms = data->blink_rate_ms;
    LOG_INF("Custom API: Current blink rate is %d ms", *rate_ms);
    
    return 0;
}

int our_driver_get_blink_count(const struct device *dev, uint32_t *count)
{
    if (dev == NULL || count == NULL) {
        LOG_ERR("Custom API: Invalid parameters");
        return -EINVAL;
    }
    
    struct our_driver_data *data = dev->data;
    *count = data->blink_count;
    LOG_INF("Custom API: Total blink count is %d", *count);
    
    return 0;
}

/* ========================================================================
 * TASK 1: STANDARD SENSOR API FUNCTIONS
 * ======================================================================== */

static int attr_set_my_impl(const struct device *dev,
                            enum sensor_channel chan,
                            enum sensor_attribute attr,
                            const struct sensor_value *val)
{
    struct our_driver_data *data = dev->data;
    
    if (attr == (enum sensor_attribute)SENSOR_ATTR_LED_BLINK_RATE) {
        uint32_t new_rate = val->val1;
        
        if (new_rate < LED_BLINK_RATE_MIN_MS || new_rate > LED_BLINK_RATE_MAX_MS) {
            LOG_ERR("Sensor API: Invalid blink rate: %d ms", new_rate);
            return -EINVAL;
        }
        
        data->blink_rate_ms = new_rate;
        LOG_INF("Sensor API (attr_set): Blink rate set to %d ms", data->blink_rate_ms);
        return 0;
    }
    
    return -ENOTSUP;
}

static int attr_get_my_impl(const struct device *dev,
                            enum sensor_channel chan,
                            enum sensor_attribute attr,
                            struct sensor_value *val)
{
    struct our_driver_data *data = dev->data;
    
    if (attr == (enum sensor_attribute)SENSOR_ATTR_LED_BLINK_RATE) {
        val->val1 = data->blink_rate_ms;
        val->val2 = 0;
        LOG_INF("Sensor API (attr_get): Current blink rate is %d ms", data->blink_rate_ms);
        return 0;
    }
    
    return -ENOTSUP;
}

static int sample_fetch_my_impl(const struct device *dev,
                                enum sensor_channel chan)
{
    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;
    
    int ret = gpio_pin_set_dt(&config->led, 1);
    if (ret < 0) {
        LOG_ERR("Failed to turn LED ON: %d", ret);
        return ret;
    }
    
    data->led_state = true;
    data->blink_count++;
    LOG_INF("sample_fetch: LED ON (blink count: %d)", data->blink_count);
    
    return 0;
}

static int channel_get_my_impl(const struct device *dev,
                               enum sensor_channel chan,
                               struct sensor_value *val)
{
    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;
    
    int ret = gpio_pin_set_dt(&config->led, 0);
    if (ret < 0) {
        LOG_ERR("Failed to turn LED OFF: %d", ret);
        return ret;
    }
    
    data->led_state = false;
    LOG_INF("channel_get: LED OFF");
    
    if (val != NULL) {
        val->val1 = data->led_state ? 1 : 0;
        val->val2 = 0;
    }
    
    return 0;
}

static const struct sensor_driver_api api_l5_lect = {
    .attr_set = attr_set_my_impl,
    .attr_get = attr_get_my_impl,
    .sample_fetch = sample_fetch_my_impl,
    .channel_get = channel_get_my_impl,
};

static int init(const struct device *dev)
{
    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;
    
    LOG_INF("Initializing our_driver LED sensor");
    
    if (!gpio_is_ready_dt(&config->led)) {
        LOG_ERR("LED GPIO device not ready");
        return -ENODEV;
    }
    
    int ret = gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure LED pin: %d", ret);
        return ret;
    }
    
    data->blink_rate_ms = LED_BLINK_RATE_DEFAULT_MS;
    data->led_state = false;
    data->blink_count = 0;
    
    LOG_INF("LED sensor initialized (default blink rate: %d ms)", data->blink_rate_ms);
    
    return 0;
}

static struct our_driver_data our_driver_data_0;

static const struct our_driver_config our_driver_config_0 = {
    .led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
};

DEVICE_DT_INST_DEFINE(0, init, NULL, 
                      &our_driver_data_0, 
                      &our_driver_config_0, 
                      POST_KERNEL, 80, 
                      &api_l5_lect);
