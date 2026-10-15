import os
import sys
import time
import re
import threading
import serial  # Requires: pip install pyserial
import subprocess
from junit_xml import TestSuite, TestCase

# --- Path setups ---  
BINARY_PATH = "/home/gitlab-runner/builds/jdCX-l3P4/0/mcusys/rs-smooth-lighting-controller/test/Smooth_LED_Lighting_FPB_RA4E2/build/Debug/Smooth_LED_Lighting_FPB_RA4E2_cmake.hex"
COM_PORT = "/dev/ttyACM0"
BAUD_RATE = int(os.getenv("BAUD_RATE", "115200"))
LOG_PATH = "serial_output.log"
TEST_DURATION_SECS = 20

stop_event = threading.Event()
def serial_listener():
    print(f"Init UART COM port {COM_PORT} ({BAUD_RATE} baud rate)...")
    try:
        # Open serial port with one second timeout
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=1.0) as ser, \
             open(LOG_PATH, "a", encoding="utf-8") as log_file:
            
            while not stop_event.is_set():
                if ser.in_waiting > 0:
                    line = ser.readline().decode("utf-8", errors="replace")
                    if line:
                        log_file.write(line)
                        log_file.flush()
                else:
                    time.sleep(0.05)  # Sleep 50ms
                    
    except Exception as e:
        print(f"Serial Listener Error: {e}", file=sys.stderr)

def main():
    # Start UART port
    listener_thread = threading.Thread(target=serial_listener, daemon=True)
    listener_thread.start()
    time.sleep(1)

    # Flash project using rfp-cli
    cmd = [
        "rfp-cli",
        "-d",
        "RA",
        "-tool",
        "jlink",
        "-file",
        BINARY_PATH,
        "-auto",  # Performs erase, program, and verify sequence
    ]

    print(f"Flashing RA device witth command: {' '.join(cmd)}")

    try:
        # Run the command
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        print("Flashing Output:")
        print(result.stdout)
        print("Flashed successfully!")
    except subprocess.CalledProcessError as e:
        print("Flashing failed!", file=sys.stderr)
        print(e.stdout, file=sys.stderr)
        print(e.stderr, file=sys.stderr)
        sys.exit(1)

    # Wait for execution of the tests
    print(f"Waiting for execution of the tests ({TEST_DURATION_SECS} seconds)...")
    time.sleep(TEST_DURATION_SECS)

    # Stop COM port listener
    print("Stopping UART-COM Port listener...")
    stop_event.set()
    listener_thread.join(timeout=3)

    # Parse the Unity test results and create JUnit XML
    print("\n--- Parsing Unity UART Log for JUnit XML ---")
    if not os.path.exists(LOG_PATH):
        print(f"Error: No UART log recorded at '{LOG_PATH}'!", file=sys.stderr)
        sys.exit(1)

    test_cases = []
    has_failures = False
    
    # Matching cases: path/file.c : line_num : test_name : STATUS (PASS, FAIL, or IGNORE)
    unity_pattern = re.compile(
        r"(?P<file>[^\s:]+\.[ch]):(?P<line>\d+):(?P<name>\w+):(?P<status>PASS|FAIL|IGNORE)", 
        re.IGNORECASE
    )
    
    with open(LOG_PATH, "r", encoding="utf-8") as log_file:
        for line in log_file:
            print(line, end="")
            for match in unity_pattern.finditer(line):
                gd = match.groupdict()
                file_name = os.path.basename(gd["file"])
                test_name = gd["name"]
                status = gd["status"].upper()
                
                # Derive classname from the test file name
                classname = f"hardware.{file_name.replace('.', '_')}"
                tc = TestCase(name=test_name, classname=classname)
                if status == "FAIL":
                    has_failures = True
                    tc.add_failure_info(message=f"Failed at line {gd['line']} in {file_name}")
                elif status == "IGNORE":
                    tc.add_skipped_info(message=f"Ignored at line {gd['line']} in {file_name}")
                test_cases.append(tc)
                
    # Test result validation
    if not test_cases:
        print("\nWarning: No Unity test parsed.", file=sys.stderr)
        with open(LOG_PATH, "r", encoding="utf-8") as log_file:
            raw_content = log_file.read().upper()
            if "FAIL" in raw_content or "ERR" in raw_content:
                has_failures = True
                tc = TestCase(name="Parsing Error", classname="hardware.error")
                tc.add_failure_info(message="Failures found.")
                test_cases.append(tc)

    # Generate JUnit XML
    ts = TestSuite("HIL-Unity test", test_cases)
    with open("junit_report.xml", "w", encoding="utf-8") as f:
        TestSuite.to_file(f, [ts], prettyprint=True)
    print(f"\nGenerated JUnit XML with {len(test_cases)} test case results.")

    # Check test result
    if has_failures:
        print("\nHIL-Unity tests failed!", file=sys.stderr)
        sys.exit(1)
    else:
        print("\nHIL-Unity tests passed successfully!")
        sys.exit(0)

if __name__ == "__main__":
    main()

