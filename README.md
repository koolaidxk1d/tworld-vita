# Tile World Vita

This is a port of Tile World for the PS Vita.

## Adding levelpacks / custom content

All of the games files are stored in the same directory as the eboot:

```
ux0:app/TWKK00001/
           |
            --- data/       # custom datasets (.dat)
            --- res/        # resources used for sounds and graphics (level independent)
            --- sets/       # level-sets (.dac)
            --- sce_sys/
            --- eboot.bin

```

## Build Instructions

Should build on any debian-based system.

### Requirements:

- VitaSDK installed and environment variable VITASDK set properly

- vdpm packages installed and up to date  

### Steps:

1. Make desired changes to CMakeLists.txt (if any).

2. From the root of the repo:

```
cd build
cmake -S .. -B .
make clean && make
```

3. Install the .vpk file using VitaShell or similar.
