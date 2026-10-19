# MACH32 learning bootloader

The bootloader uses three fixed Flash regions:

| Program | Start address | Maximum size |
|---|---:|---:|
| Bootloader | `0x08000000` | 64 KB |
| Application A | `0x08010000` | 192 KB |
| Application B | `0x08040000` | 192 KB |

Build all three programs:

```bash
make boot-demo
```

Build and flash the complete demonstration:

```bash
make flash-demo
```

Open the ST-LINK serial port:

```bash
screen /dev/ttyACM0 115200
```

The bootloader accepts these commands:

```text
status
boot A
boot B
upload A <byte-count> <CRC32>
upload B <byte-count> <CRC32>
```

Use the host tool instead of typing an `upload` command manually:

```bash
make app-b
python3 tools/uart_upload.py /dev/ttyACM0 build/app_b/firmware.bin B
```

Press and release the black RESET button when the tool asks.

The uploader calculates the byte count and CRC32. The bootloader erases the
selected slot, receives the binary data, programs Flash, and calculates CRC32
again from Flash. It accepts the image only when both CRC32 values match and
the vector table is valid.

The CRC32 check validates the upload operation. The bootloader does not store
update metadata or calculate CRC32 again during later boots.

CRC32 is a checksum that detects accidental changes in the received data.

Do not run `screen` while the upload tool uses the serial port.

Application A changes LD2 every second.

Application B changes LD2 every 200 ms.

The bootloader starts application A after ten seconds without UART activity.
