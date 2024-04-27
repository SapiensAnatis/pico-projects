# pico-projects

Monorepository of personal projects for the Raspberry Pi Pico / Pico W.

## Setup

Requires the [Pico SDK](https://github.com/raspberrypi/pico-sdk) and defining `PICO_SDK_PATH`. Some targets need the [Pimoroni Pico libraries](https://github.com/pimoroni/pimoroni-pico).

To use wireless functionality, create a file `wifi_credentials.cmake` in the repository root and populate it with:

```cmake
SET(WIFI_SSD, "Your Network")
SET(WIFI_PASSWORD, "Your Password")
```

If you are using a Pico and not a Pico W, replace `set(PICO_BOARD pico_w)` with `set(PICO_BOARD pico)` in the root `CMakeLists.txt`.