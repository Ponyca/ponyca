ponyca
======

Dependencies
------------

On Debian:

```
sudo aptitude install python3 python3-yaml zlib1g-dev libgl1-mesa-swx11-dev libx11-dev libxext-dev libfreetype6-dev libopenal-dev freeglut3-dev libxmu-dev libxi-dev cxxtest libasio-dev
```

How to download the source code
-------------------------------

```
git clone git://github.com/Ponyca/ponyca.git
cd ponyca
git submodule init
git submodule update
```

How to build
------------

```
cd build/
cmake ..
make
```
