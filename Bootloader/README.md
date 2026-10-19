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
```

Application A changes LD2 every second.

Application B changes LD2 every 200 ms.

The bootloader starts application A after three seconds without a command.
