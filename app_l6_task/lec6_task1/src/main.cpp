#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    LOG_INF("=== TASK 1: Standard Sensor API Test ===\n");
    
    const struct device* driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    
    if (!device_is_ready(driver)) {
        LOG_ERR("LED sensor driver not ready!");
        return -1;
    }
    
    LOG_INF("LED sensor driver is ready\n");
    
    struct sensor_value val;
    int ret;
    
    /* Test 1: Basic LED control using sample_fetch and channel_get */
    LOG_INF("--- Test 1: Basic LED Control ---");
    for (int i = 0; i < 5; i++) {
        LOG_INF("Blink %d/5", i + 1);
        
        /* Turn LED ON */
        ret = sensor_sample_fetch(driver);
        LOG_INF("  sample_fetch returned: %d", ret);
        k_sleep(K_MSEC(1000));
        
        /* Turn LED OFF */
        ret = sensor_channel_get(driver, SENSOR_CHAN_ALL, &val);
        LOG_INF("  channel_get returned: %d, LED state: %d", ret, val.val1);
        k_sleep(K_MSEC(1000));
    }
    
    LOG_INF("\n=== Task 1 Complete ===\n");
    
    while (1) {
        k_sleep(K_MSEC(5000));
    }
    
    return 0;
}
