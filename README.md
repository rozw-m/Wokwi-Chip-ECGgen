# Wokwi-Chip-ECGgen
## Description
Simulates an ECG output as an analogue voltage between GND and VCC - assumed to be 3.3V as written for Rapsberry Pi Pico device

2 inputs: VCC, GND

1 output: OUT - the analogue output value for signal.

3 controls: 

- Amplitude of signal: the peak amplitude between 0 and 3.3V
- Voltage Offset: offset of baseline of signal - between 0 and 3.3V
- Frequency of beat: frequency of peak - between 0 and 5Hz

## Usage
To use this chip in your project, include it as a dependency in your `diagram.json` file:

```json
"dependencies": { "chip-ecggen": "github:rozw-m/Wokwi-Chip-ECGgen@2.0.0" }
```

Then, add the chip to your circuit by adding a `chip-ecggen` item to the `parts` section of `diagram.json`:

```json
  "parts": {
    ...,
    "type": "chip-ecggen", "id": "chip2", "top": 105.6, "left": 192, "attrs": {},
```

The actual source code for the chip lives in [src/main.c](https://github.com/rozw-m/Wokwi-Chip-ECGgen/blob/main/src/main.c), and the pins are described in [chip.json](https://github.com/rozw-m/Wokwi-Chip-ECGgen/blob/main/chip.json).

## Example of usage
An example [WokWi project](https://wokwi.com/projects/436649333354782721) using this chip.


