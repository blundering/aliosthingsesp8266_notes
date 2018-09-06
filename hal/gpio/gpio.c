/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 * 从esp32复制...
 */
#include <aos/aos.h>

#define gpio_dev_t aos_gpio_dev_t
#define gpio_config_t aos_gpio_config_t
#include <hal/soc/gpio.h>
#undef gpio_dev_t
#undef gpio_config_t

#define gpio_config_t GPIO_ConfigTypeDef
#include "driver/gpio.h"

#define ESP_INTR_FLAG_DEFAULT 0

static enum gpio_state {
    disable,
    enable
} state;

int32_t hal_gpio_init(aos_gpio_dev_t *gpio) {
    if (gpio == NULL)
    {
        return EIO;
    }

    if (gpio->port == 16) {
        gpio16_output_conf();
        return 0;
    }
    
    gpio_config_t io_conf;
    /* disable interrupt */
    io_conf.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
    /* set mode */
    if (gpio->config == OUTPUT_PUSH_PULL)
    {
        io_conf.GPIO_Mode = GPIO_Mode_Output;
    }
    else if ((gpio->config == OUTPUT_OPEN_DRAIN_NO_PULL) || 
            (gpio->config == OUTPUT_OPEN_DRAIN_PULL_UP))
    {
        io_conf.GPIO_Mode = GPIO_Mode_Out_OD;
    }
    else if ((gpio->config == INPUT_PULL_UP) || 
             (gpio->config == INPUT_PULL_DOWN) || 
             (gpio->config == INPUT_HIGH_IMPEDANCE))
    {
        io_conf.GPIO_Mode = GPIO_Mode_Input;
    }

    /* bit mask of the pins */
    io_conf.GPIO_Pin = BIT(gpio->port);
    /* set pull-up mode */
    io_conf.GPIO_Pullup = ((gpio->config == INPUT_PULL_UP) || 
                          (gpio->config == IRQ_MODE) ||
                          (gpio->config == OUTPUT_OPEN_DRAIN_PULL_UP)) ? 1 : 0;
    /* configure GPIO with the given settings */
    gpio_config(&io_conf);

    return 0;
}

int32_t hal_gpio_output_high(aos_gpio_dev_t *gpio)
{
    if (gpio == NULL)
    {
        return EIO;
    }

    if (gpio->port==16) {
        gpio16_output_set(enable);
        uint32_t ret = gpio16_input_get();
        if (ret != enable) {
            return EIO;
        }
        return 0;
    }
    GPIO_OUTPUT_SET(gpio->port, enable);
    uint32_t ret = GPIO_INPUT_GET(gpio->port);
    if (ret != enable) {
        return EIO;
    }
    
    return 0;
}

int32_t hal_gpio_output_low(aos_gpio_dev_t *gpio)
{
    
    if (gpio == NULL)
    {
        return EIO;
    }
    if (gpio->port==16) {
        gpio16_output_set(disable);
        int ret = gpio16_input_get();
        if (ret != disable) {
            return EIO;
        }
        return 0;
    }
    GPIO_OUTPUT_SET(gpio->port, disable);
    int ret = GPIO_INPUT_GET(gpio->port);
    if (ret != disable) {
        return EIO;
    }
    
    return 0;
}

int32_t hal_gpio_output_toggle(aos_gpio_dev_t *gpio)
{
    if (gpio == NULL)
    {
        return EIO;
    }

    if (gpio->port==16) {
        uint32_t bit,new_bit;
        bit = gpio16_input_get();
        gpio16_output_set(bit ^ 0x0000001);
        new_bit = gpio16_input_get();
        if (new_bit != bit ^ 0x0000001) {
            return EIO;
        }
        return 0;
    }
    
    uint32_t bit,new_bit;
    bit = GPIO_INPUT_GET(gpio->port);
    GPIO_OUTPUT_SET(gpio->port, bit ^ 0x0000001 );
    new_bit = GPIO_INPUT_GET(gpio->port);
    if (new_bit != bit ^ 0x0000001) {
        return EIO;
    }

    return 0;
}

int32_t hal_gpio_input_get(aos_gpio_dev_t *gpio, uint32_t *value)
{
    if (gpio == NULL || value == NULL)
    {
        return EIO;
    }
    if (gpio->port==16) {
        *value = gpio16_input_get();
    }
    *value = GPIO_INPUT_GET(gpio->port);

    return 0;
}

int32_t hal_gpio_enable_irq(aos_gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                                     gpio_irq_handler_t handler, void *arg)
{
    int32_t ret = EIO;
    if (gpio == NULL || arg == NULL)
    {
        return EIO;
    }
    gpio_pin_intr_state_set(gpio->port, (GPIO_INT_TYPE)trigger);
    // @TODO esp32是注册给某个pin?
    gpio_intr_handler_register(handler, arg);
    return 0;
}

int32_t hal_gpio_disable_irq(aos_gpio_dev_t *gpio)
{
    if (gpio == NULL)
    {
        return EIO;
    }
    gpio_pin_intr_state_set(gpio->port, GPIO_PIN_INTR_DISABLE);
    return 0;
}

int32_t hal_gpio_clear_irq(aos_gpio_dev_t *gpio)
{
    if (gpio == NULL)
    {
        return EIO;
    }
    gpio_pin_intr_state_set(gpio->port, GPIO_PIN_INTR_DISABLE);
    // @TODO 8266应该做什么?
    // rgpio_isr_handler_remove(gpio->port);
    return 0;
}

int32_t hal_gpio_finalize(aos_gpio_dev_t *gpio)
{
    gpio_config_t io_conf;

    if (gpio == NULL)
    {
        return EIO;
    }

    /* disable interrupt */
    io_conf.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
    /* disable output */
    io_conf.GPIO_Mode = GPIO_Mode_Input;
    /* bit mask of the pins */
    io_conf.GPIO_Pin = BIT(gpio->port);
    /* disable pull-down mode */
    // io_conf.pull_down_en = 0;
    /* disable pull-up mode */
    io_conf.GPIO_Pullup = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    // uninstall isr service
    // @TODO 8266应该做什么?
    // gpio_uninstall_isr_service();
    return 0;
}