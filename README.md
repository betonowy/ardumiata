# Engine health monitoring for Mazda MX-5 NB1

Firmware for custom Arduino Nano powered device. Mainly for precise engine health monitoring on track days. Features:

- Invisible, non-invasive piggyback off of factory wiring harness if needed at all.
- Modifications are reversible.
- Cleanly integrates with car's interior. 16x2 green display can replace smoking ashtray on NB1. I made a "mounting bracket" for it with a matte painted piece of wood.
- Monitoring engine health and more:
    - (ECT) Coolant temperature - piggybacked and calibrated against factory sensor and ECU measurement circuit. May alternatively use temp gauge circuit, but you're on your own to deal with that.
    - (EOT) Oil temperatoure - from Ecumaster Temperature Sensor.
    - (EOP) Oil pressure - from Ecumaster Pressure Sensor.
    - (BAT) Battery voltage (from ignition).
- Alarm when any measurement exceeds safe limits so you know when to cooldown or make an emergency shutdown.
- If your ECT ever breaks, you will know.

## Important notes

- ECT approximation function might need to be recalibrated/curve fitted for any other car than mine, especially if any part of PCB design is altered. Measuring temperatures/ECT voltage at the back of the engine with device attached and compensating for measured voltages when fan switches on and off at 97C and 93C is good enough, especially if you just need a relative guide for how hot the engine is. If outside EU, check your regional specs specifically.
- Despite the efforts, cabin fan can offset measurements up to 4 degrees upwards when on and it can't be fixed, take that into account or don't turn it.
- ECT relay is required to ensure device does not affect ECU measurements when device is in an inconsistent state i.e. during reset, or engine startup.
- Cars with no AC have a very convenient rubber cap that can feed wires through passenger side firewall.

# Building project

- PCB project initially made in KiCad 8.
- Source compilled with `avr-gcc` toolchain with Arduino package. For guide check out `Arduino-CMake-NG` project. Initially developed on a Linux system.

Assuming basic KiCad and CMake knowledge, you should already know how to build the project.

## Project layout

- `.` - miscelaneous files and root `CMakeLists.txt`.
- `src` - Firmware source code.
- `kicad` - KiCad PCB project.


