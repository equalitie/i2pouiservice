# How to build

## Build boost

If your system has Boost version lower than 1.58 or higher than 1.65,
you'll need to use a custom built version.

```
wget https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.bz2
tar xf boost_1_65_1.tar.bz2
cd boost_1_65_1
./bootstrap.sh
./b2 toolset=gcc -j`nproc`
```

## Buld i2pouiservice
```
mkdir build
cd build
cmake .. # Or cmake .. -DBOOST_ROOT=/path/to/boost_1_65_1
make -j `nproc`
```

# How to debug 

```
cd src
gdb -i=mi --args ./get_slash 127.0.0.1 /
```

