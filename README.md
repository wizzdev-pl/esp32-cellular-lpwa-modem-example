# <project_name>

<project_description>

Target board - ESP32

Build process is aimed for Ubuntu/Debian based Linux distributions.
 

# 0. Prerequisites
To compile you need to get the following packages:
```
sudo apt-get install git wget flex bison gperf python python-pip python-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util
```
Add the user to `dialout` group:
```
sudo usermod -a -G dialout $USER
```

# 1. Development
In this README, the following variables are assumed to be defined: `$PROJECT_DIR`, `$BUILD_DIR`, `BUILD_TEST_DIR`.
E.g.: 
```
PROJECT_DIR=`pwd`  # source root - directory of this README
BUILD_DIR=$(realpath $PROJECT_DIR/../build-<project_name>)
BUILD_TEST_DIR=$(realpath $PROJECT_DIR/../build-<project_name>-test)
```

## 1.1. Build environment preparation
Before first build, some one-time preparation is required

### 1.1.1. Don't forget to update the submodules first
```
git submodule init && git submodule update
git submodule update --init --recursive
```
To update the submodules to the tracked branch (set in `.gitmodules` file):
```
git submodule update --remote  
```
It will update the submodules in local repo. To commit them, call `git add path/to/submodule` and commit.

### 1.1.2. Build the esp compiler
```
cd $PROJECT_DIR
cd build
./install_esp32_compiler.sh 
```

## 1.2. Activate the environment in the terminal
Environment needs to be activated for all commands related to building, flashing and monitoring firmware. 
Needs to be run in every terminal.
```
. $PROJECT_DIR/externals/esp-idf/export.sh  
```

## 1.3. Building
To build the firmware, issue the following commands:
```
mkdir $BUILD_DIR
cd $BUILD_DIR
idf.py -B . -C $PROJECT_DIR/ build  
```

## 1.4. IDE editing
At this point you should be able to open the environment in the QtCreator.
 - Start QtCreator (e.g. run `/opt/Qt/Tools/QtCreator/bin/qtcreator`) from a terminal with **active environment**
 - Open the main CMakeLists.txt from the `$PROJECT_DIR/`
 - Import the automatically generated kit (the one that points to your `$BUILD_DIR`) and disable the desktop kit if active.

## 1.5. Flashing
To flash: 

(if not in `$BUILD_DIR` change `./` to appropriate folder) 
```
idf.py -B ./ -C $PROJECT_DIR/ --baud 921600 flash 
```

One can also start the terminal monitor afterwards:
```
idf.py -B ./ -C $PROJECT_DIR/ --baud 921600 flash monitor
```
Other commands to be combined: `build`, `erase_flash`, e.g.:
```
idf.py -B ./ -C ../pockethernet-esp32/ --baud 921600 erase_flash build flash monitor
```

If the board and serial port converter doesn't support automatic control of RESET and BOOT pins control, one may need to drive them manually, e.g. with buttons.

## 1.6. Testing
To prepare the testing environment, build and run tests, issue the following commands: 
```
# Do NOT activate idf.py environment!
mkdir $BUILD_TEST_DIR
cd $BUILD_TEST_DIR
${PROJECT_DIR}/test/build_tests.sh  
```

Now to rebuild and rerun tests issue:
```
make && ./esp32_test_on_pc
```
Tests can be edited and run in QtCreator

## 1.7. Menuconfig
To configure the ESP32 build - run:
```
# Remember to activate the environment
cd $BUILD_DIR
idf.py -B ./ -C $PROJECT_DIR/ menuconfig
```

# 2. Hardware requirements

- [Pico SIM7080G Cat-M/NB-IoT module](https://www.waveshare.com/wiki/Pico-SIM7080G-Cat-M/NB-IoT)
- [ESP32-DevKitC-32E](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html)
- 3.7 - 4.2V power supply

# 3. SIM7080G connection

- VSYS needs to be connected to 3.7 - 4.2 V power supply. 3.3 V seems not to be efficient enough to provide stable power supply for the module
- GND from power supply and and SIM7080G connected with ESP32 GND pin (next to pin 23)
- DTR connected to GND (pin on SIM7080G next to DTR can be used)
- PWR connected to ESP32 GPIO14
- RXD connected to ESP32 pin 4
- TXD connected to ESP32 pin 5
- 3V3 (OUT) connected to ESP32 3V3 pin
 
...
