#include <stdint.h>

// Структура GPIO (как в лабе)
typedef struct {
    uint32_t MODER;
    uint32_t OTYPER;
    uint32_t OSPEEDR;
    uint32_t PUPDR;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t LCKR;
    uint32_t AFR[2];
} GPIO_TypeDef;

// Базовые адреса портов (как в лабе)
#define GPIOC ((GPIO_TypeDef*)0x40020800)
#define GPIOD ((GPIO_TypeDef*)0x40020C00)

// Функция задержки (грубая, как в лабе)
void delay(void) {
    for (volatile int i = 0; i < 500000; i++);
}

int main(void) {
    // Включаем тактирование портов C и D
    *((uint32_t*)0x40023830) |= 0xF;

    // Настраиваем PC4-PC11 как выходы (MODER4-MODER11 = 01)
    GPIOC->MODER |= (21845 << 8); // 0b101010101010101 << 8

    // Подтяжка для кнопок: PC13 и PD2 — Pull-up
    GPIOC->PUPDR |= (1 << 26); // PC13: Pull-up
    GPIOD->PUPDR |= (1 << 4);  // PD2: Pull-up

    // Счётчик зажжённых светодиодов (от 0 до 8)
    uint8_t led_count = 0;

    // Бесконечный цикл
    while (1) {

        // Если нажата СИНИЯ кнопка (SW1, PC13 — активный низкий)
        if (!(GPIOC->IDR & (1 << 13))) {
            delay(); // антидребезг

            // Если ещё не все 8 — добавляем один
            if (led_count < 8) {
                led_count++;
            }

            // Генерируем паттерн: младшие led_count битов = 1
            uint8_t pattern = (led_count == 0) ? 0x00 : (0xFF >> (8 - led_count));

            // Обновляем порт
            GPIOC->ODR = (GPIOC->ODR & ~(0xFF << 4)) | (pattern << 4);
        }

        // Если нажата КРАСНАЯ кнопка (SW2, PD2 — активный низкий)
        if (!(GPIOD->IDR & (1 << 2))) {
            delay(); // антидребезг

            // Если есть что гасить — убираем один
            if (led_count > 0) {
                led_count--;
            }

            // Генерируем паттерн
            uint8_t pattern = (led_count == 0) ? 0x00 : (0xFF >> (8 - led_count));

            // Обновляем порт
            GPIOC->ODR = (GPIOC->ODR & ~(0xFF << 4)) | (pattern << 4);
        }

    } // end while(1)
}
