#ifndef C39DB7A2_8972_4280_8422_E274BA968BAA
#define C39DB7A2_8972_4280_8422_E274BA968BAA

#include "driver/gpio.h"

namespace hardware
{
    constexpr int LPWA_MODULE_UART_RXD_PIN = 4;
    constexpr int LPWA_MODULE_UART_TXD_PIN = 5;
    constexpr gpio_num_t LPWA_MODULE_POWER_KEY = GPIO_NUM_14;
}; // namespace hardware

#endif /* C39DB7A2_8972_4280_8422_E274BA968BAA */
