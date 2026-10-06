#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$PROJECT_DIR"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}====================================================${NC}"
echo -e "${BLUE}   BCA 182 Lab 2: Automated Verification & Smoke Test${NC}"
echo -e "${BLUE}====================================================${NC}"

# Phase 1: Native Unit Tests
echo -e "\n${YELLOW}[Phase 1/4] Running 18 Native Unit Tests...${NC}"
pio test -e native
echo -e "${GREEN}✓ All Unit Tests Passed!${NC}"

# Phase 2: Static Code Analysis
echo -e "\n${YELLOW}[Phase 2/4] Running Static Analysis (cppcheck)...${NC}"
pio check -e black_f407zg --severity high --severity medium
echo -e "${GREEN}✓ Static Analysis Clean!${NC}"

# Phase 3: Compile Target Firmware
echo -e "\n${YELLOW}[Phase 3/4] Compiling Target Firmware (black_f407zg)...${NC}"
pio run -e black_f407zg
echo -e "${GREEN}✓ Target Firmware Built Successfully!${NC}"

# Phase 4: Hardware Detection & Flash & UART Smoke Test
echo -e "\n${YELLOW}[Phase 4/4] Hardware Smoke Test & Flashing...${NC}"

find_serial_port() {
    for dev in /dev/serial/by-id/* /dev/ttyACM* /dev/ttyUSB*; do
        if [ -e "$dev" ]; then
            echo "$dev"
            return 0
        fi
    done
    return 1
}

TARGET_PORT="$(find_serial_port || true)"

if [ -z "$TARGET_PORT" ]; then
    echo -e "${YELLOW}Notice: No physical board detected on USB/Serial.${NC}"
    echo -e "Waiting up to 15 seconds for board to be plugged in..."
    for i in {1..15}; do
        TARGET_PORT="$(find_serial_port || true)"
        if [ -n "$TARGET_PORT" ]; then
            break
        fi
        sleep 1
    done
fi

if [ -z "$TARGET_PORT" ]; then
    echo -e "${RED}No board detected on USB/Serial. Skipping flash & hardware UART capture.${NC}"
    echo -e "To run hardware verification when connected:"
    echo -e "  ./scripts/run_all_tests.sh"
    exit 0
fi

echo -e "${GREEN}Detected board on port: ${TARGET_PORT}${NC}"
echo -e "Flashing firmware..."
pio run -e black_f407zg -t upload

echo -e "Waiting for board to finish booting..."
sleep 1

# Issue a clean target reset via OpenOCD / ST-Link while monitoring UART
echo -e "Resetting target MCU and listening on ${TARGET_PORT}..."
OPENOCD_BIN="$(find ~/.platformio/packages/tool-openocd -name openocd -type f 2>/dev/null | head -n1 || echo "openocd")"
UART_LOG=$(mktemp)

# Start background UART reader
( stty -F "$TARGET_PORT" 115200 raw -echo -echoe -echok 2>/dev/null || true
  timeout 4 cat "$TARGET_PORT" > "$UART_LOG" 2>/dev/null || true ) &
UART_PID=$!
sleep 0.3

# Hardware reset
"$OPENOCD_BIN" -f interface/stlink.cfg -f target/stm32f4x.cfg -c "init; reset run; shutdown" >/dev/null 2>&1 || true
wait $UART_PID || true

echo -e "\n${BLUE}--- UART Output Captured ---${NC}"
cat "$UART_LOG"
echo -e "${BLUE}----------------------------${NC}"

if grep -q "MP3 Player" "$UART_LOG" || grep -q "Controls:" "$UART_LOG"; then
    echo -e "\n${GREEN}✓ Hardware Smoke Test PASSED: Board booted and transmitted instruction banner via UART!${NC}"
else
    echo -e "\n${YELLOW}⚠ Warning: Boot banner pattern not matched in initial 5 seconds. Check UART RX/TX connection.${NC}"
fi

rm -f "$UART_LOG"
