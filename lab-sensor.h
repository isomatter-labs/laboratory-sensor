#ifndef LAB_SENSOR_H
#define LAB_SENSOR_H

#include <inttypes.h>

#define DELAY_TIME 3

/* PINS */
#define LED 25
#define ERR 6
#define BUZZ 7
#define SDA 0
#define SCL 1
#define PROG_1 2
#define PROG_2 3
#define PROG_3 4
#define PROG_4 5
/* END PINS */

/* PORTS */
#define DATA 15

#define SEL 6
/* END PORTS */

enum display_t {
  GRN,
  RED,
  END,
};

void err();
void write_to_data(uint8_t byte);
void write_to_sel(uint8_t byte);
void parse_digits(display_t display, float raw_val);
void display_digits();

#endif
