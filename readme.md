# Lud's C++ Util thingamathigs

This 'library' is meant for my own use and I don't promise code of any quality whatsoever, I don't even know if this compiles at any moment.

Its inception came from me being tired of using fstream and reading some dude in stack overflow refer to 
reading a whole file as slurping it.

Then assert and parser  came as I was doing Advent of Code 2023.

## Usage

The whole library is contained in ```Lud``` namespace, they are single header lil' guys.

```c++
#define LUD_SLURPER_IMPLEMENTATION // define implementation 
#import "lud_slurper.hpp" // import just slurper
```

```c++
#import "lud_parser.hpp" // import just parser
```

```c++
#import "lud_assert.hpp" // import just assert
```



---
c++20+ required btw
