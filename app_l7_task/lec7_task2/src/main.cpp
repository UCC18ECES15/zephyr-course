#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

extern "C" {
    #include "../../drivers/our_driver/our_driver.h"
}

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    LOG_INF("=== TASK 2: Custom Extension API Shell with Validation ===\n");
    
    const struct device* driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    
    if (!device_is_ready(driver)) {
        LOG_ERR("LED sensor driver not ready!");
        return -1;
    }
    
    LOG_INF("LED sensor driver is ready");
    LOG_INF("Shell is enabled. Available commands:");
    LOG_INF("  - our_driver set_rate <ms>  : Set blink rate (with validation)");
    LOG_INF("  - our_driver get_rate       : Get current blink rate");
    LOG_INF("  - our_driver blink_n <count>: Blink N times (with validation)");
    LOG_INF("  - our_driver info           : Show driver information\n");
    LOG_INF("Note: Commands use SHELL_CMD_ARG for argument validation");
    
    /* Display initial state */
    uint32_t rate_ms;
    int ret = our_driver_get_blink_rate(driver, &rate_ms);
    if (ret == 0) {
        LOG_INF("Initial blink rate: %d ms", rate_ms);
        LOG_INF("Valid range: %d - %d ms\n", 
                LED_BLINK_RATE_MIN_MS, LED_BLINK_RATE_MAX_MS);
    }
    
    /* Main loop - shell runs in background */
    while (1) {
        k_sleep(K_SECONDS(15));
        
        uint32_t count;
        our_driver_get_blink_count(driver, &count);
        our_driver_get_blink_rate(driver, &rate_ms);
        LOG_INF("Status: Total blinks = %d, Rate = %d ms", count, rate_ms);
    }
    
    return 0;
}
