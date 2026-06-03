#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

extern "C" {
    #include "../../drivers/our_driver/our_driver.h"
}

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    LOG_INF("=== TASK 2: Custom Extension API Test ===\n");
    
    const struct device* driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    
    if (!device_is_ready(driver)) {
        LOG_ERR("LED sensor driver not ready!");
        return -1;
    }
    
    LOG_INF("LED sensor driver is ready\n");
    
    uint32_t rate_ms;
    uint32_t count;
    int ret;
    
    /* Test 1: Get initial blink rate using custom API */
    LOG_INF("--- Test 1: Get Initial Blink Rate (Custom API) ---");
    ret = our_driver_get_blink_rate(driver, &rate_ms);
    LOG_INF("Initial blink rate: %d ms (ret: %d)\n", rate_ms, ret);
    
    /* Test 2: Set different blink rates using custom API */
    uint32_t test_rates[] = {500, 1000, 2000};
    
    for (size_t i = 0; i < ARRAY_SIZE(test_rates); i++) {
        LOG_INF("--- Test %zu: Set Blink Rate to %d ms (Custom API) ---", 
                i + 2, test_rates[i]);
        
        /* Set new blink rate using custom API */
        ret = our_driver_set_blink_rate(driver, test_rates[i]);
        LOG_INF("Set blink rate returned: %d", ret);
        
        /* Verify it was set using custom API */
        ret = our_driver_get_blink_rate(driver, &rate_ms);
        LOG_INF("Verified blink rate: %d ms", rate_ms);
        
        /* Blink LED 3 times with this rate */
        struct sensor_value val;
        for (int j = 0; j < 3; j++) {
            LOG_INF("  Blink %d/3", j + 1);
            
            /* Turn LED ON using sensor API */
            sensor_sample_fetch(driver);
            k_sleep(K_MSEC(rate_ms));
            
            /* Turn LED OFF using sensor API */
            sensor_channel_get(driver, SENSOR_CHAN_ALL, &val);
            k_sleep(K_MSEC(rate_ms));
        }
        
        /* Get blink count using custom API */
        ret = our_driver_get_blink_count(driver, &count);
        LOG_INF("Total blinks so far: %d (ret: %d)\n", count, ret);
    }
    
    /* Test 3: Invalid blink rate (too low) */
    LOG_INF("--- Test: Invalid Blink Rate - Too Low (Custom API) ---");
    ret = our_driver_set_blink_rate(driver, 50);
    LOG_INF("Set invalid rate (50ms) returned: %d (expected error)\n", ret);
    
    /* Test 4: Invalid blink rate (too high) */
    LOG_INF("--- Test: Invalid Blink Rate - Too High (Custom API) ---");
    ret = our_driver_set_blink_rate(driver, 6000);
    LOG_INF("Set invalid rate (6000ms) returned: %d (expected error)\n", ret);
    
    /* Verify blink rate unchanged after invalid attempts */
    ret = our_driver_get_blink_rate(driver, &rate_ms);
    LOG_INF("Current blink rate after invalid attempts: %d ms\n", rate_ms);
    
    LOG_INF("=== Task 2 Complete ===\n");
    
    while (1) {
        k_sleep(K_MSEC(5000));
        LOG_INF("Main loop running...");
    }
    
    return 0;
}