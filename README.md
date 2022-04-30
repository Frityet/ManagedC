# ManagedC

Library to add a rederence-counter GC to C. See `tests/src/` for examples

## Usage

| :warning: WARNING                                                                                                     |
|:----------------------------------------------------------------------------------------------------------------------|
| Only works on **clang** currently, but the top goal is to add GCC support. MSVC/ISO-C is not planned to be supported. |

To use, copy the file `managed.h` from `src/` into your source files. You may also include `src/mstring.h` if you would like a "managed" string.
