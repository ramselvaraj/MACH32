#!/usr/bin/env python3
"""Upload one MACH32 application image through the bootloader UART."""

import argparse
import os
import select
import sys
import termios
import time
import zlib
from pathlib import Path


BAUD_RATE = termios.B115200


def configure_serial(fd: int) -> None:
    attributes = termios.tcgetattr(fd)
    attributes[0] = 0
    attributes[1] = 0
    attributes[2] = termios.CS8 | termios.CREAD | termios.CLOCAL
    attributes[3] = 0
    attributes[4] = BAUD_RATE
    attributes[5] = BAUD_RATE
    attributes[6][termios.VMIN] = 0
    attributes[6][termios.VTIME] = 0
    termios.tcsetattr(fd, termios.TCSAFLUSH, attributes)


def read_until(fd: int, marker: bytes, timeout: float) -> bytes:
    received = bytearray()
    deadline = time.monotonic() + timeout

    while marker not in received:
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            raise TimeoutError(f"Timed out waiting for {marker!r}")

        readable, _, _ = select.select([fd], [], [], remaining)
        if readable:
            block = os.read(fd, 256)
            received.extend(block)
            sys.stdout.buffer.write(block)
            sys.stdout.buffer.flush()

    return bytes(received)


def write_all(fd: int, data: bytes) -> None:
    view = memoryview(data)

    while view:
        _, writable, _ = select.select([], [fd], [], 2.0)
        if not writable:
            raise TimeoutError("Timed out while writing firmware data")
        count = os.write(fd, view)
        view = view[count:]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("port", help="Serial device, for example /dev/ttyACM0")
    parser.add_argument("image", type=Path, help="Application .bin file")
    parser.add_argument("slot", choices=("A", "B"), help="Destination slot")
    args = parser.parse_args()

    image = args.image.read_bytes()
    if not image:
        parser.error("the image is empty")

    crc = zlib.crc32(image) & 0xFFFFFFFF
    command = f"upload {args.slot} {len(image)} {crc:08X}\r".encode("ascii")

    fd = os.open(args.port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    try:
        configure_serial(fd)
        time.sleep(0.5)
        termios.tcflush(fd, termios.TCIFLUSH)
        print("Press and release the board's black RESET button.", flush=True)
        read_until(
            fd,
            b"Type a command within ten seconds.\r\n> ",
            30.0,
        )
        write_all(fd, command)
        read_until(fd, b"READY\r\n", 15.0)

        print(f"Sending {len(image)} bytes to slot {args.slot}...", flush=True)
        write_all(fd, image)
        response = read_until(fd, b"\r\n", 10.0)

        if b"OK\r\n" not in response:
            raise RuntimeError("The bootloader rejected the image")

        print(f"Upload complete. CRC32={crc:08X}")
        return 0
    except (OSError, TimeoutError, RuntimeError) as error:
        print(f"Upload failed: {error}", file=sys.stderr)
        return 1
    finally:
        os.close(fd)


if __name__ == "__main__":
    raise SystemExit(main())
