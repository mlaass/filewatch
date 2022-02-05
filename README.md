# Filewatch
A utitlity too watch file folders and post changes to http


## building

```sh
git submodule update --init
mkdir build && cd build
cmake ..&&make
./filewatch <watch_path> --post <post_url> --ping <ping_url>
```

### cross compilation windows

requirements:
```
sudo apt install g++-mingw-w64
sudo apt install gcc-mingw-w64
```