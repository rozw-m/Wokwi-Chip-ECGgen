#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  pin_t ecgPin;
  uint32_t amplitude_attr;
  uint32_t frequency_attr;
  float frequencyHz;
  uint32_t offset_attr;
  int ecgValue;
  int counter;
  int state;
  uint8_t tick;
  bool firstRun;
} chip_state_t;

// Mimicking Arduino map() function
int map(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


static void chip_timer_event(void *user_data);

timer_t timer_id;
uint32_t time_int;

// Custom chip initialization
void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));
  chip->ecgPin = pin_init("OUT", ANALOG);  // Pin where ECG signal is generated
  chip->amplitude_attr = attr_init_float("amplitude", 1.0);
  chip->frequency_attr = attr_init_float("frequency", 1.0);
  chip->frequencyHz = attr_read_float(chip->frequency_attr);
  chip->offset_attr = attr_init_float("offset", 0);
  chip->counter = 0;
  chip->state = 0;
  chip->tick = 0;
  chip->firstRun = true;
  
  const timer_config_t timer_config = {
      .callback = chip_timer_event,
      .user_data = chip,
    };
  timer_id = timer_init(&timer_config);
  time_int = (uint32_t)1e6/(chip->frequencyHz*500);
  timer_start(timer_id, time_int, true);
  float BPM = chip->frequencyHz*60.0;
  printf("BPM: %.2f\n", BPM);
  printf("ECG Chip Initialized!\n");
}

void chip_timer_event(void *user_data) {
  uint32_t t;
  chip_state_t *chip = (chip_state_t*)user_data;
  if (chip->state == 0) {
    // P wave
    chip->ecgValue = 60;
    chip->counter++;
    if (chip->counter >= 245) {
      chip->state = 1;
      chip->counter = 0;
    }
  } else if (chip->state == 1) {
    // QRS Complex
    if (chip->counter < 3) chip->ecgValue = 120;
    else if (chip->counter < 6) chip->ecgValue = 255;   // Q wave
    else chip->ecgValue = 120;
    chip->counter++;
    if (chip->counter >= 10) {
      chip->state = 2;
      chip->counter = 0;
    }
  } else if (chip->state == 2) {
    // Baseline after T wave
    chip->ecgValue = 60;
    chip->counter++;
    if (chip->counter >= 245) {
      chip->state = 0;  // Restart the cycle
      chip->counter = 0;
      t = get_sim_nanos() / 1000000;
    }
  }
  float amplitude = attr_read_float(chip->amplitude_attr);
  float voltage = chip->ecgValue * amplitude/255;
  pin_dac_write(chip->ecgPin, voltage);

  chip->tick++;
  
  if (chip->tick > 100) {
    chip->tick = 0;
    if (chip->firstRun || (chip->frequencyHz != attr_read_float(chip->frequency_attr))) {
      chip->frequencyHz = attr_read_float(chip->frequency_attr);
      time_int = (uint32_t)1e6/(chip->frequencyHz*500);
      timer_start(timer_id, time_int, true);
      float BPM = chip->frequencyHz*60.0;
      printf("BPM: %.2f\n", BPM);
      if (chip->firstRun) chip->firstRun = false;
    }
  }
}

void chip_fini(void) {
  // Called when the simulation ends
}
