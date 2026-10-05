# Laboratory Activity 2: Personal MP3 Player

## Course Information
- **Course:** BCA 182 – Embedded Systems Programming
- **Institution:** Mindanao State University – Iligan Institute of Technology (MSU-IIT)
- **Target Microcontroller:** STM32F407ZGT6 (RT-Thread RT-Spark Board / ARM Cortex-M4 @ 168 MHz)
- **RTOS:** FreeRTOS (Native preemptive/cooperative multitasking)
- **Framework:** STM32Cube HAL via PlatformIO

---

## 1. Overview & Architecture

This project implements a multi-threaded embedded MP3/audio player adhering strictly to all requirements in **Laboratory Activity 2: Personal MP3 Player**:

- **Song Selection:** 8 distinct musical pieces playable via external binary push buttons.
- **Binary Input:** Buttons 2, 3, and 4 represent a 3-bit binary number ($2^2, 2^1, 2^0$) indexing songs 0 through 7.
- **Selection & Confirmation:** Button 1 selects the candidate song. A 5-second countdown window is displayed; pressing Button 1 again confirms the choice. If left untouched for 5 seconds, the countdown times out and the player reverts to normal operation.
- **Playback Control:** On-board `USER_BUTTON` toggles between Play and Pause.
- **RGB LED Feedback:**
  - **Blue LED:** Song is actively playing.
  - **Red LED:** Player is paused.
  - **Green LED:** Changing/selecting a song (5-second confirmation window active).
- **LCD Display (Protected via Mutex):**
  - Displays the active song title and composer during playback.
  - Displays interactive confirmation messages and a 5-second countdown when changing songs.
- **Volume Control:** 12-bit ADC reads an external potentiometer (pin `A0` / `PA1`) to dynamically modulate output volume (0% to 100%).
- **UART Interface:** Transmits interactive operating instructions and status updates over USART1 (115200 baud).
- **Concurrency & RTOS:** Implemented using 3 dedicated FreeRTOS threads:
  1. `update_lcd_leds_thread`: Manages display updates under mutex lock and drives RGB LEDs.
  2. `polling_buttons`: Debounces push buttons, handles 3-bit binary selection, Button 1 confirmation, and playback toggles.
  3. `adjust_volume`: Samples potentiometer ADC and adjusts sound volume.
  - Uses FreeRTOS Software Timers to implement the required `Ticker` (tone rhythm) and `Timeout` (5s selection timeout) interfaces.

---

## 2. Hardware Pin Mapping

| Peripheral | Board Pin / Net | MCU Pin | Configuration / Notes |
| :--- | :--- | :--- | :--- |
| **Button 1 (Select/Confirm)** | External Header | `PB0` | Active-LOW, internal pull-up / external 330 $\Omega$ pull-up |
| **Button 2 (Bit 2 - MSB)** | External Header | `PB1` | Active-LOW, internal pull-up |
| **Button 3 (Bit 1)** | External Header | `PB2` | Active-LOW, internal pull-up |
| **Button 4 (Bit 0 - LSB)** | External Header | `PB3` | Active-LOW, internal pull-up |
| **User Button (Play/Pause)** | On-board KEY0 | `PA0` | Active-HIGH (internal pull-down) |
| **Potentiometer (Volume)** | Header `A0` | `PA1` | ADC1 Channel 1 (12-bit resolution: 0–4095) |
| **Red LED** | On-board / Ext | `PF11` | Active-HIGH indicator (Song Paused) |
| **Green LED** | On-board / Ext | `PF14` | Active-HIGH indicator (Changing Song) |
| **Blue LED** | On-board / Ext | `PF12` | Active-HIGH indicator (Song Playing) |
| **Audio Output** | PWM Audio Pin | `PB8` | TIM4 Channel 3 PWM / Tone Generator |
| **LCD Parallel Port** | FSMC 8080 Bus | `PD/PE/PF/PG` | 240x240 ST7789 IPS LCD display |
| **UART TX / RX** | USART1 | `PA9` / `PA10` | 115200 baud, 8-N-1 |

---

## 3. Song Library

All 8 songs defined in `song_def.h` are implemented:
1. `[000]` **Fur Elise** — *Beethoven*
2. `[001]` **Canon In D** — *Pachelbel*
3. `[010]` **Minuet in G major** — *Bach*
4. `[011]` **Turkish March** — *Mozart*
5. `[100]` **Nocturne in E-flat** — *Chopin*
6. `[101]` **Waltz No. 2** — *Shostakovich*
7. `[110]` **Nocturne in C-sharp** — *Chopin*
8. `[111]` **Symphony No. 40** — *Mozart*

---

## 4. Building and Verification

### Firmware Build
```bash
pio run -e black_f407zg
```
- **Target:** STM32F407ZGT6
- **Result:** Successfully compiles with 0 errors and 0 warnings.
- **Footprint:** ~35 KB Flash (3.4%), ~41 KB RAM (32.0%).

### Native Unit Tests
```bash
pio test -e native
```
- **Test Suites:**
  1. `test_song_selection`: Verifies array counts, valid notes/beats, titles, and boundary behavior.
  2. `test_binary_button_decoder`: Verifies active-low and raw 3-bit binary decoding for all 8 button states.
  3. `test_volume_scaling`: Verifies 12-bit ADC quantization to 0–100% volume and gain scaling.
  4. `test_state_machine`: Verifies all player states (`STOPPED`, `PLAYING`, `PAUSED`, `CONFIRMING`), Button 1 confirmation, countdown decrements, and 5-second timeout auto-reverts.
- **Test Results:** 18/18 test cases passing.
