# isomatter-labs :: Laboratory Sensor

## Software
### Building
To build the firmware for the Laboratory Sensor, execute the following:
```sh
cd build
./build.sh
```

### Flashing
After building, there should be a `.uf2` file that can be dragged-and-dropped into a Raspberry Pi Pico.
This is the entirety of the firmware, and requires no additional steps.

## Electronics
### Editing
KiCAD files can be found in `hardware/printed_circuit_board/kicad_files`,
and can be edited or modified to provide any changes you wish to the hardware or layout.

(note that changing the overall footprint of the PCB will likely require changes to the enclosure files)

### Printing
Many PCB-printing services require only `gbr` files, which can be found at
`hardware/printed_circuit_board/gbr_files`.

These files can also be re-generated using the above mentioned KiCAD files and a working copy of KiCAD.

## Enclosure
Included in `hardware/enclosure` are a series of `.step` files, with one file per part.
These files can be sliced by most, if not all, slicers, and are a bit more resilient to paywalling by various CAD editors.

Also included is a file `render.png`, which should give a good idea of what the parts should look like,
and how they should fit together to form the enclosure.
