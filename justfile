PORT:='/dev/ttyUSB0'

default: build flash monitor

[no-cd]
build:
    idf.py build

[no-cd]
clean:
    idf.py clean

[no-cd]
format:
    find main -name "*.c" -o -name "*.h" -exec clang-format -i {} +

[no-cd]
size:
    idf.py size

[no-cd]
flash:
    idf.py --port {{PORT}} flash

[no-cd]
monitor:
    echo -e '\n\n\nCtrl+] to stop\n\n\n'
    idf.py --port {{PORT}} monitor

[no-cd]
config:
    reset ; idf.py menuconfig

[no-cd]
save-defconfig:
    idf.py save-defconfig
