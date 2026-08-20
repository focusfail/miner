# miner 
I am just making up a game as I go. 

## Building

**1. Clone the repo**
```sh
git clone --depth=1 https://github.com/focusfail/miner.git
cd ./miner/
git submodules update --init --recursive --depth=1
```

**2. Build**
```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

**3. Enjoy (unlikely)**
```sh
# run binary
./build/game
```