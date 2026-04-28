#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#ifdef CONFIG_RGB_LED_ENABLE
/* RGB LED definitions */
#define RED_LED_NODE DT_ALIAS(led_red)
#define GREEN_LED_NODE DT_ALIAS(led_green)
#define BLUE_LED_NODE DT_ALIAS(led_blue)

static const struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(RED_LED_NODE, gpios);
static const struct gpio_dt_spec green_led = GPIO_DT_SPEC_GET(GREEN_LED_NODE, gpios);
static const struct gpio_dt_spec blue_led = GPIO_DT_SPEC_GET(BLUE_LED_NODE, gpios);

void set_rgb_color(bool red, bool green, bool blue)
{
    gpio_pin_set_dt(&red_led, red);
    gpio_pin_set_dt(&green_led, green);
    gpio_pin_set_dt(&blue_led, blue);
}
#else
/* Single LED definition - Homework requirement */
#define LED_NODE DT_ALIAS(app_led)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
#endif

int main(void)
{
#ifdef CONFIG_RGB_LED_ENABLE
    /* Initialize RGB LEDs */
    if (!gpio_is_ready_dt(&red_led) || !gpio_is_ready_dt(&green_led) || !gpio_is_ready_dt(&blue_led)) {
        LOG_ERR("RGB LEDs not ready");
        return 0;
    }

    if (gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_INACTIVE) < 0) {
        LOG_ERR("Failed to configure Red LED");
        return 0;
    }
    if (gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_INACTIVE) < 0) {
        LOG_ERR("Failed to configure Green LED");
        return 0;
    }
    if (gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT_INACTIVE) < 0) {
        LOG_ERR("Failed to configure Blue LED");
        return 0;
    }

    LOG_INF("RGB LED Application started");
    LOG_INF("Heartbeat period: %d ms", CONFIG_APP_HEARTBEAT_PERIOD_MS);

    #ifdef CONFIG_LED_ENABLE_DEBUGGING
        LOG_INF("RGB LED debugging enabled");
    #endif

    int color_state = 0;

    while (1) {
        switch(color_state) {
            case 0: /* Red */
                set_rgb_color(true, false, false);
                #ifdef CONFIG_LED_ENABLE_DEBUGGING
                LOG_INF("Color: RED");
                #endif
                break;
            case 1: /* Green */
                set_rgb_color(false, true, false);
                #ifdef CONFIG_LED_ENABLE_DEBUGGING
                LOG_INF("Color: GREEN");
                #endif
                break;
            case 2: /* Blue */
                set_rgb_color(false, false, true);
                #ifdef CONFIG_LED_ENABLE_DEBUGGING
                LOG_INF("Color: BLUE");
                #endif
                break;
            case 3: /* Yellow (Red + Green) */
                set_rgb_color(true, true, false);
                #ifdef CONFIG_LED_ENABLE_DEBUGGING
                LOG_INF("Color: YELLOW");
                #endif
                break;
            case 4: /* Cyan (Green + Blue) */
                set_rgb_color(false, true, true);
                #ifdef CONFIG_LED_ENABLE_DEBUGGING
                LOG_INF("Color: CYAN");
                #endif
                break;
            case 5: /* Magenta (Red + Blue) */
                set_rgb_color(true, false, true);
                #ifdef CONFIG_LED_ENABLE_DEBUGGING
                LOG_INF("Color: MAGENTA");
                #endif
                break;
            case 6: /* White (All on) */
                set_rgb_color(true, true, true);
                #ifdef CONFIG_LED_ENABLE_DEBUGGING
                LOG_INF("Color: WHITE");
                #endif
                break;
            case 7: /* Off */
                set_rgb_color(false, false, false);
                #ifdef CONFIG_LED_ENABLE_DEBUGGING
                LOG_INF("Color: OFF");
                #endif
                break;
        }

        color_state = (color_state + 1) % 8;
        
        /* HOMEWORK REQUIREMENT: Use CONFIG_APP_HEARTBEAT_PERIOD_MS */
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }

#else
    /* Single LED mode - Homework requirement */
    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("LED device not ready");
        return 0;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) {
        LOG_ERR("Failed to configure LED pin");
        return 0;
    }

    LOG_INF("Single LED Application started");
    LOG_INF("Heartbeat period: %d ms", CONFIG_APP_HEARTBEAT_PERIOD_MS);

    #ifdef CONFIG_LED_ENABLE_DEBUGGING
        LOG_INF("LED debugging enabled");
    #endif

    bool led_state = false;

    while (1) {
        gpio_pin_toggle_dt(&led);
        led_state = !led_state;
        
        #ifdef CONFIG_LED_ENABLE_DEBUGGING
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        #endif
        
        /* HOMEWORK REQUIREMENT: Use CONFIG_APP_HEARTBEAT_PERIOD_MS */
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
#endif

    return 0;
}
