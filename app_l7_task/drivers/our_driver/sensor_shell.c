#include <zephyr/shell/shell.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sensor_shell, LOG_LEVEL_INF);

/* Get the sensor device instance */
static const struct device *get_sensor_device(void)
{
    return DEVICE_DT_GET(DT_NODELABEL(our_driver0));
}

/* Shell command: fetch - calls sensor_sample_fetch() */
static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_sensor_device();
    int ret;
    
    if (!device_is_ready(dev)) {
        shell_error(sh, "Sensor device not ready");
        return -ENODEV;
    }
    
    ret = sensor_sample_fetch(dev);
    if (ret == 0) {
        shell_print(sh, "sensor_sample_fetch() called successfully");
        shell_print(sh, "LED turned ON");
    } else {
        shell_error(sh, "sensor_sample_fetch() failed: %d", ret);
    }
    
    return ret;
}

/* Shell command: read - calls sensor_channel_get() and prints result */
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_sensor_device();
    struct sensor_value val;
    int ret;
    
    if (!device_is_ready(dev)) {
        shell_error(sh, "Sensor device not ready");
        return -ENODEV;
    }
    
    ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
    if (ret == 0) {
        shell_print(sh, "sensor_channel_get() called successfully");
        shell_print(sh, "LED turned OFF");
        shell_print(sh, "LED state value: %d", val.val1);
    } else {
        shell_error(sh, "sensor_channel_get() failed: %d", ret);
    }
    
    return ret;
}

/* Shell command: info - prints device name and ready state */
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_sensor_device();
    
    shell_print(sh, "=== Sensor Device Information ===");
    shell_print(sh, "Device name: %s", dev->name);
    shell_print(sh, "Device ready: %s", device_is_ready(dev) ? "YES" : "NO");
    
    if (device_is_ready(dev)) {
        shell_print(sh, "Status: Operational");
    } else {
        shell_error(sh, "Status: Not ready");
    }
    
    return 0;
}

/* Define sensor subcommands */
SHELL_STATIC_SUBCMD_SET_CREATE(sensor_subcmds,
    SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch() - Turn LED ON", cmd_sensor_fetch),
    SHELL_CMD(read, NULL, "Call sensor_channel_get() - Turn LED OFF and print result", cmd_sensor_read),
    SHELL_CMD(info, NULL, "Print device name and ready state", cmd_sensor_info),
    SHELL_SUBCMD_SET_END
);

/* Register the sensor root command */
SHELL_CMD_REGISTER(sensor, &sensor_subcmds, "Sensor driver commands", NULL);
