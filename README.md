# CuteSynth

Operator API for embedded synthesis, courtesy of CuteLab 🧀 🍷 🥖

## Building

Clone the repo and partake in some good old fashioned cmake fun

```
mkdir build
cd build
cmake ..
```

## Testing

```
cd build/test
make
```

After that, all you have to do is `#include <cutesynth.h>` in your project for DSP/API building blocks, and `#include <cutemodules.h>` for full instruments and plugins.
