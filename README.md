# Filesystem 'open()' benchmark

Benchmark of the filesystems performing opening files.

I was going to use 'open()' to eliminate the stdio overhead, but it's actually comparatively tiny
for the case of opening a file, and it made the code slightly more portable (lookin' at you win32).

# Building

Configure the build directory:
```sh
# Generally
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
# or if toolchain not detected
# cmake -DCMAKE_BUILD_TYPE=Release -S . -B build -G Ninja
```

Build the program
```
cmake --build build --config Release
```

and then run the `./build/filebench` or `./build/Release/filebench.exe` executable.

This will create ~1000 one-line text files in the current directory and then repeatedly open them,
capturing the timing, and then deleting them.

# Outputs

Output on a high-end 2019 i7 on Samsung nvme drive/Windows:
```
Iteration Stats (4096 files): min=56ms, avg=59ms, p90=62ms, p95=63ms, p99=68ms, max=77ms
Per File: 14411ns
```

Output from an Ubuntu 18.04 VM on an Intel NUC with a SATA 2 SSD:
```
Iteration Stats (4096 files): min=10ms, avg=11ms, p90=12ms, p95=13ms, p99=15ms, max=18ms
Per File: 2786ns
```

Output from an Ubuntu 19.04 Linux machine on an EVO SSD:
```
Iteration Stats (4096 files): min=2ms, avg=4ms, p90=4ms, p95=5ms, p99=6ms, max=7ms
Per File: 914ns
```
