# Three-Port EV Charging Cabinet

A PIC18F8722 controller for a three-port EV charging cabinet, developed with two teammates as a group assignment for **CENG336 - Introduction to Embedded Systems Development** at METU. The firmware tracks port connections, limits current according to a temperature input, and reports its state to a Python supervisor.

Our work covers the cabinet firmware in `src/`. The Python supervisor, sample scenario, and assets in `simulator/` were provided by the course staff and are included to run and test the firmware. The firmware builds on the course-provided starter code and device configuration.

## Features

- Interrupt-driven serial communication at **115200 baud, 8N1**.
- A **100 ms** control cycle with command acknowledgements and status reports.
- Temperature input sampled every **500 ms**, with normal, derated, and overheat modes.
- A cabinet-wide current limit of **0, 8, 16, or 24 A**, constrained by the thermal mode.
- A four-digit seven-segment display with two pages, selected by releasing **RB6**.

## Hardware

The firmware targets a **PIC18F8722 running at 40 MHz**.

| Connection | Purpose |
|---|---|
| RC6 / RC7 | EUSART TX / RX |
| RH4 / AN12 | Potentiometer input representing temperature |
| RB6 | Display page selection |
| PORTJ[0:6] | Seven-segment outputs a-g |
| PORTH[0:3] | Four display digit selects |

The effective current limit is the lower of the requested limit and the thermal cap:

| ADC reading | Mode | Current cap |
|---|---|---|
| 0-699 | Normal (`N`) | 24 A |
| 700-899 | Derated (`D`) | 8 A |
| 900-1023 | Overheat (`H`) | 0 A |

Display page 0 shows the ADC reading with leading zeroes, such as `0480`. Page 1 shows the effective current limit, a blank digit, and the connected-port bitmask: `24 5` means a 24 A limit with ports 0 and 2 connected.

## Build and Run

1. Install **MPLAB X IDE 5.45**, **XC8 2.30**, and the PIC18F8722 device support pack. These are the versions specified for the assignment.
2. Open `the3.X` through **File > Open Project**. This folder contains the MPLAB project; the source files are in `src/`.
3. In Project Properties, select your installed XC8 compiler. To use a physical board, select the connected programmer/debugger instead of Simulator.
4. Select **Clean and Build Project**, then **Make and Program Device** to upload the firmware.

### Python Supervisor

The course-provided supervisor communicates with a programmed board over serial. It displays the controller's reports and can send commands interactively or play a sample scenario. It does not emulate the microcontroller.

Use **Python 3.10 or newer**:

```sh
python -m pip install -r simulator/requirements.txt
python simulator/evCabinetSimulator.py --port COM3 simulator/sample_scenario.json
```

Replace `COM3` with your board's serial port, such as `/dev/ttyUSB0` on Linux or `/dev/cu.usbserial-...` on macOS. Reset the board before each run, then press **S** in the supervisor window to start.

Omit the scenario path for interactive control. Add `--log-only` for an automatically started text trace.

## Serial Commands

| Command | Action |
|---|---|
| `$GO#` | Start a run from the waiting state |
| `$CON0#` / `$DIS0#` | Connect or disconnect port 0; the same format applies to ports 1 and 2 |
| `$LIM24#` | Set the requested current limit; valid values are `00`, `08`, `16`, and `24` |
| `$END#` | End the run and turn off the display |

Each 100 ms slot during a run sends one acknowledgement (`$ACKcc#`) or status frame (`$STSmxxxxcee#`). Status reports contain the thermal mode, ADC reading, connected-port bitmask, and effective current limit. Reset the board to start another run after `$END#`.

## Testing

With the board programmed and the supervisor closed:

```sh
python tests/board_test.py --port COM3
```

The script checks serial protocol behavior and timing. Temperature thresholds, display output, and RB6 require physical checks; see the [manual checklist](tests/manual_checklist.md).

## Project Structure

- `src/` - Firmware, thermal control, display driver, and device configuration.
- `the3.X/` - MPLAB X project and build configuration.
- `simulator/` - Course-provided Python supervisor, assets, and sample scenario.
- `tests/` - Serial test script and manual hardware checklist.
- `THE3.pdf` - Original assignment specification.
