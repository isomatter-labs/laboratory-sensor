#include "lab-sensor.h"
#include "hardware/i2c.h"
#include "lib/Si7021/src/si7021.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <iostream>
#include <stdio.h>


//     A
//   F   B
//     G
//   E   C
//     D  DP

// 0: - - F E D C B A | 0b00111111 | 0b11000000 | 0xC0
// 1: - - - - - C B - | 0b00000110 | 0b11111001 | 0xF9
// 2: - G - E D - B A | 0b01011011 | 0b10100100 | 0xA4
// 3: - G - - D C B A | 0b01001111 | 0b10110000 | 0xB0
// 4: - G F - - C B - | 0b01100110 | 0b10011001 | 0x99
// 5: - G F - D C - A | 0b01101101 | 0b10010010 | 0x92
// 6: - G F E D C - - | 0b01111100 | 0b10000011 | 0x83
// 7: - - - - - C B A | 0b00000111 | 0b11111000 | 0xF8
// 8: - G F E D C B A | 0b01111111 | 0b10000000 | 0x80
// 9: - G F - - C B A | 0b01100111 | 0b10011000 | 0x98


const int digits[10] = {0x3F, 0x06, 0x5B, 0x4f, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67};
const int selector_pins[2][3] = {/* GRN ENABLE */
                                 {0b00111100,
                                  0b01011100,
                                  0b10011100},
                                 /* RED ENABLE */
                                 {0b00011000,
                                  0b00010100,
                                  0b00001100}};

static int readouts[2][3] = {{0, 0, 0}, {0, 0, 0}};

int main() {
  int progress_pins[] = {-1, PROG_1, PROG_2, PROG_3, PROG_4};
  stdio_init_all();

  // Initialize the port for 7-segment display
  for (int pin_num = SEL; pin_num < SEL+8; pin_num++) {
    gpio_init(pin_num);
    gpio_set_dir(pin_num, GPIO_OUT);
  }

  for (int pin_num = DATA; pin_num < DATA+8 + 1; pin_num++) {
    gpio_init(pin_num);
    gpio_set_dir(pin_num, GPIO_OUT);
  }

  gpio_init(LED);
  gpio_set_dir(LED, GPIO_OUT);

  gpio_init(ERR);
  gpio_set_dir(ERR, GPIO_OUT);

  gpio_init(BUZZ);
  gpio_set_dir(BUZZ, GPIO_OUT);

  for (int i = 1; i < 5; i++) {
    gpio_init(progress_pins[i]);
    gpio_set_dir(progress_pins[i], GPIO_OUT);
  }

  i2c_init(i2c0, 100000);
  gpio_set_function(SDA, GPIO_FUNC_I2C);
  gpio_set_function(SCL, GPIO_FUNC_I2C);
  gpio_pull_up(SDA);
  gpio_pull_up(SCL);

  Si7021::Si7021 si7021(i2c0);
  si7021.setResolution(Si7021::Resolution::RH11T11);

  multicore_launch_core1(display_digits);
  
  bool toggle = false;
  for (;;) {
    gpio_put(PROG_1, 0);
    gpio_put(PROG_2, 0);
    gpio_put(PROG_3, 0);
    gpio_put(PROG_4, 0);
    gpio_put(ERR, toggle);
    for (int progress = 0; progress < 5; progress++) {
      gpio_put(LED, 1);
      if (progress)
        gpio_put(progress_pins[progress], 1);
      float temp_adjusted = (si7021.askForTemperature() * 1.8) + 32;
      float humidity_adjusted = si7021.askForHumidity();
      gpio_put(ERR, (temp_adjusted >= 100));
      parse_digits(RED, temp_adjusted);
      parse_digits(GRN, humidity_adjusted);

      printf("-----------------------------------------------------\n");
      printf("Si7021 Humidity: %.2f[%%]\n", humidity_adjusted);
      printf("Si7021 Temperature: %.2f[°F]\n", temp_adjusted);
      printf("-----------------------------------------------------\n");

      gpio_put(LED, 0);

      // pause before polling again
      sleep_ms(1000);
    }
    toggle = !toggle;
  }
}

void parse_digits(display_t display, float raw_val) {
  int val = (int)(raw_val* 10);
  if (val >= 1000) {
    val = 990;
  }
  readouts[display][0] = (val / 100) % 10;
  readouts[display][1] = (val / 10) % 10;
  readouts[display][2] = val % 10;
}

void write_to_data(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    gpio_put(DATA + i, (byte >> i) & 1);
  }
}

// top two bits EXPECTED to be 0
void write_to_sel(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    gpio_put(SEL + i, (byte >> i) & 1);
  }
}

void err() {
  gpio_put(ERR, 1);
  for (;;) {
    gpio_put(BUZZ, 1);
    sleep_ms(500);
    gpio_put(BUZZ, 0);
    sleep_ms(1000);

  }
}

void display_digits() {
  for (;;) {

      for (int i = 0; i < 3; i++) {
        for (int d = 0; d < END; d++) {
          int digit = digits[readouts[d][i]];
          if (i == 1) {
            digit |= 0b10000000;
          }

          if (d == GRN) {
            digit = ~digit;
          }
          write_to_sel(selector_pins[d][i]);
          write_to_data(digit);
          sleep_ms(DELAY_TIME);
        }
      }

  }
}

