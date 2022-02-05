# Filewatch

A utitlity too watch file folders and post changes and pings to http host

You can edit the `connect.sh` and `connect.bat` files to configure your urls or use thhe commandline directly.

```
$ bin/filewatch.x64 --help
bin/filewatch.x64 takes between 0 and 1 arguments, plus these options:
-v      --verbose        Verbose output
-r      --recursive      Watch files recursively
-i      --interval       Update interval in milliseconds
-h      --host   Host URL including port
-p      --post   Host URL to post updated files to
-g      --ping   Host URL to ping every intervall

```

## building

preparations:

```sh
git submodule update --init
```

building

```sh
mkdir build && cd build
cmake ..&&make
./filewatch <watch_path> --post <post_url> --ping <ping_url>
```

### cross compilation for windows

requirements:

```sh
sudo apt install g++-mingw-w64
sudo apt install gcc-mingw-w64
```
