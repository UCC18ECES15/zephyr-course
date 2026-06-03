#include <zephyr/shell/shell.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdlib.h>
#include "our_driver.h"

/* Get the driver device instance */
static const struct device *get_our_driver_device(void)
{
    return DEVICE_DT_GET(DT_NODELABEL(our_driver0));
}

/* Shell command: Get blink rate */
static int cmd_get_blink_rate(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_our_driver_device();
    uint32_t rate_ms;
    int ret;
    
    if (!device_is_ready(dev)) {
        shell_error(sh, "Device not ready");
        return -ENODEV;
    }
    
    ret = our_driver_get_blink_rate(dev, &rate_ms);
    if (ret == 0) {
        shell_print(sh, "Current blink rate: %u ms", rate_ms);
    } else {
        shell_error(sh, "Failed to get blink rate: %d", ret);
    }
    
    return ret;
}

/* Shell command: Set blink rate - WITH VALIDATION using SHELL_CMD_ARG */
static int cmd_set_blink_rate(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_our_driver_device();
    uint32_t rate_ms;
    int ret;
    
    if (!device_is_ready(dev)) {
        shell_error(sh, "Device not ready");
        return -ENODEV;
    }
    
    /* Argument validation - argc is enforced by SHELL_CMD_ARG */
    if (argc != 2) {
        shell_error(sh, "Error: Missing argument");
        shell_print(sh, "Usage: set_rate <rate_ms>");
        shell_print(sh, "  Valid range: %d - %d ms", 
                    LED_BLINK_RATE_MIN_MS, LED_BLINK_RATE_MAX_MS);
        return -EINVAL;
    }
    
    /* Convert argument to integer */
    rate_ms = (uint32_t)atoi(argv[1]);
    
    /* Range validation */
    if (rate_ms < LED_BLINK_RATE_MIN_MS || rate_ms > LED_BLINK_RATE_MAX_MS) {
        shell_error(sh, "Error: Rate %u ms is out of range", rate_ms);
        shell_print(sh, "Valid range: %d - %d ms", 
                    LED_BLINK_RATE_MIN_MS, LED_BLINK_RATE_MAX_MS);
        return -EINVAL;
    }
    
    /* Call the extension API */
    ret = our_driver_set_blink_rate(dev, rate_ms);
    if (ret == 0) {
        shell_print(sh, "Blink rate set to: %u ms", rate_ms);
    } else {
        shell_error(sh, "Failed to set blink rate: %d", ret);
    }
    
    return ret;
}

/* Shell command: Get blink count */
static int cmd_get_blink_count(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_our_driver_device();
    uint32_t count;
    int ret;
    
    if (!device_is_ready(dev)) {
        shell_error(sh, "Device not ready");
        return -ENODEV;
    }
    
    ret = our_driver_get_blink_count(dev, &count);
    if (ret == 0) {
        shell_print(sh, "Total blink count: %u", count);
    } else {
        shell_error(sh, "Failed to get blink count: %d", ret);
    }
    
    return ret;
}

/* Shell command: Blink LED once */
static int cmd_blink_once(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_our_driver_device();
    uint32_t rate_ms;
    struct sensor_value val;
    int ret;
    
    if (!device_is_ready(dev)) {
        shell_error(sh, "Device not ready");
        return -ENODEV;
    }
    
    /* Get current blink rate */
    ret = our_driver_get_blink_rate(dev, &rate_ms);
    if (ret != 0) {
        shell_error(sh, "Failed to get blink rate: %d", ret);
        return ret;
    }
    
    shell_print(sh, "Blinking LED (rate: %u ms)...", rate_ms);
    
    /* Turn LED ON */
    ret = sensor_sample_fetch(dev);
    if (ret != 0) {
        shell_error(sh, "Failed to turn LED ON: %d", ret);
        return ret;
    }
    
    k_sleep(K_MSEC(rate_ms));
    
    /* Turn LED OFF */
    ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
    if (ret != 0) {
        shell_error(sh, "Failed to turn LED OFF: %d", ret);
        return ret;
    }
    
    shell_print(sh, "Done! LED state: %d", val.val1);
    
    return 0;
}

/* Shell command: Blink LED N times - WITH VALIDATION */
static int cmd_blink_n_times(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_our_driver_device();
    uint32_t rate_ms;
    uint32_t count;
    struct sensor_value val;
    int ret;
    
    if (!device_is_ready(dev)) {
        shell_error(sh, "Device not ready");
        return -ENODEV;
    }
    
    /* Argument validation */
    if (argc != 2) {
        shell_error(sh, "Error: Missing argument");
        shell_print(sh, "Usage: blink_n <count>");
        shell_print(sh, "  Valid range: 1 - 100");
        return -EINVAL;
    }
    
    count = (uint32_t)atoi(argv[1]);
    
    /* Range validation */
    if (count == 0 || count > 100) {
        shell_error(sh, "Error: Count %u is out of range", count);
        shell_print(sh, "Valid range: 1 - 100");
        return -EINVAL;
    }
    
    /* Get current blink rate */
    ret = our_driver_get_blink_rate(dev, &rate_ms);
    if (ret != 0) {
        shell_error(sh, "Failed to get blink rate: %d", ret);
        return ret;
    }
    
    shell_print(sh, "Blinking LED %u times (rate: %u ms)...", count, rate_ms);
    
    for (uint32_t i = 0; i < count; i++) {
        /* Turn LED ON */
        sensor_sample_fetch(dev);
        k_sleep(K_MSEC(rate_ms));
        
        /* Turn LED OFF */
        sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
        k_sleep(K_MSEC(rate_ms));
        
        if ((i + 1) % 10 == 0) {
            shell_print(sh, "  Completed %u/%u blinks", i + 1, count);
        }
    }
    
    shell_print(sh, "Done! Total blinks: %u", count);
    
    return 0;
}

/* Shell command: Show driver info */
static int cmd_driver_info(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_our_driver_device();
    uint32_t rate_ms;  
    uint32_t count;
    
    if (!device_is_ready(dev)) {
        shell_error(sh, "Device not ready");
        return -ENODEV;
    }
    
    shell_print(sh, "=== Our Driver Information ===");
    shell_print(sh, "Device name: %s", dev->name);
    shell_print(sh, "Device ready: %s", device_is_ready(dev) ? "YES" : "NO");
    
    our_driver_get_blink_rate(dev, &rate_ms);
    shell_print(sh, "Current blink rate: %u ms", rate_ms);
    
    our_driver_get_blink_count(dev, &count);
    shell_print(sh, "Total blink count: %u", count);
    
    shell_print(sh, "Valid rate range: %d - %d ms", 
                LED_BLINK_RATE_MIN_MS, LED_BLINK_RATE_MAX_MS);
    
    return 0;
}

/* Define subcommands with SHELL_CMD_ARG for argument enforcement */
SHELL_STATIC_SUBCMD_SET_CREATE(our_driver_subcmds,
    SHELL_CMD(get_rate, NULL, "Get current blink rate", cmd_get_blink_rate),
    SHELL_CMD_ARG(set_rate, NULL, "Set blink rate <ms>", cmd_set_blink_rate, 2, 0),
    SHELL_CMD(get_count, NULL, "Get total blink count", cmd_get_blink_count),
    SHELL_CMD(blink, NULL, "Blink LED once", cmd_blink_once),
    SHELL_CMD_ARG(blink_n, NULL, "Blink LED N times <count>", cmd_blink_n_times, 2, 0),
    SHELL_CMD(info, NULL, "Show driver information", cmd_driver_info),
    SHELL_SUBCMD_SET_END
);

/* Register the main command */
SHELL_CMD_REGISTER(our_driver, &our_driver_subcmds, "Our driver commands", NULL);
