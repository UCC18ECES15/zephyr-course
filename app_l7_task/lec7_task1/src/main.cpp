#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

extern "C" {
    #include "../../drivers/our_driver/our_driver.h"
}

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    LOG_INF("=== TASK 1: Sensor Shell Commands ===\n");
    
    const struct device* driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    
    if (!device_is_ready(driver)) {
        LOG_ERR("LED sensor driver not ready!");
        return -1;
    }
    
    LOG_INF("LED sensor driver is ready");
    LOG_INF("Shell is enabled. Available sensor commands:");
    LOG_INF("  - sensor fetch   : Call sensor_sample_fetch() - Turn LED ON");
    LOG_INF("  - sensor read    : Call sensor_channel_get() - Turn LED OFF");
    LOG_INF("  - sensor info    : Print device information\n");
    
    /* Main loop - shell runs in background */
    while (1) {
        k_sleep(K_SECONDS(10));
        LOG_INF("Main loop running... Use shell for interaction");
    }
    
    return 0;
}
