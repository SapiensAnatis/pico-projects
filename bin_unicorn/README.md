# bin_unicorn

Small program to fetch data from the Reading Borough Council API regarding the next bin collection. Can be used to power a Bindicator-style LED lamp.

## Building

Create a file called `address.cmake` in this folder, containing these contents: 

```cmake
SET(BIN_UNICORN_HOME_ADDRESS "your%20url%20encoded%20address")
```

and build as normal.