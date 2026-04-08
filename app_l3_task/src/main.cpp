#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>


/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool led_state = true;

    if (!gpio_is_ready_dt(&led)) return 0;

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    #ifdef CONFIG_LED_ENABLE_DEBUGGING
        LOG_INF("LED Subsystem initialized");
        LOG_INF("Blink sleep time: %d ms", CONFIG_BLINK_SLEEP_TIME_MS);
        #ifdef CONFIG_LED_ADVANCED
        LOG_INF("LED Brightness: %d%%", CONFIG_LED_BRIGHTNESS);
        LOG_INF("LED Fade duration: %d ms", CONFIG_LED_FADE_DURATION);
        #endif
        #ifdef CONFIG_LED_CUSTOM_BLINK_PATTERN
        LOG_INF("Custom blink pattern enabled");
        LOG_INF("Pattern ON time: %d ms", CONFIG_LED_PATTERN_ON_TIME);
        LOG_INF("Pattern OFF time: %d ms", CONFIG_LED_PATTERN_OFF_TIME);
        #endif
    #endif

    while (1) {
        if (gpio_pin_toggle_dt(&led) < 0) return 0;

        led_state = !led_state;
    #ifdef CONFIG_LED_ENABLE_DEBUGGING
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
    #endif
    #ifdef LED_CUSTOM_BLINK_PATTERN
        if (led_state) 
        {
            k_msleep(CONFIG_LED_PATTERN_ON_TIME);
        } 
        else 
        {
            k_msleep(CONFIG_LED_PATTERN_OFF_TIME);
        }
    #else
        k_msleep(CONFIG_BLINK_SLEEP_TIME_MS);
    #endif
    }
    return 0;
}
