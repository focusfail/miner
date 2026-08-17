
clean:
    rm -rf ./build/*

run mode="Debug": (build mode)
    LSAN_OPTIONS=suppressions=lsan.supp mangohud ./build/game

build mode="Debug":
    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE={{mode}} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    ninja -C build