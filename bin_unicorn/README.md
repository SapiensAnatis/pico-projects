# bin_unicorn

Small program to fetch data from the Reading Borough Council API regarding the next bin collection. Can be used to power a Bindicator-style LED lamp.

## Building

Create a file called `address.cmake` in this folder, containing these contents: 

```cmake
SET(BIN_UNICORN_HOME_ADDRESS "your address")
```

The address should not be URL-encoded (the program does this) and have a comma before the postcode

Example:

```
40 Caversham Road Reading, RG17EB
```