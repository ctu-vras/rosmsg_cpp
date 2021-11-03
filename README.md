# rosmsg_cpp

C++ library for getting full message definition, MD5 sum and more given just the message type as string.

This package provides both C++ library `librosmsg_cpp.so` and executable `rosmsg++`.

## Usage

### Library

For usage of the library, see the documentation in [include/rosmsg_cpp/rosmsg_cpp.h]().

### Executable

`rosmsg++ show|md5|fields|field-types|file|md5-text package/MessageType`

- `show`: Prints the full message definition. It corresponds exactly to the one generated by `genpy`.
- `md5`: Prints MD5 sum of the message.
- `fields`: Prints the names of the fields in the message (non-recursively). Each field on one line.
- `field-types`: Prints the types of the fields in the message (non-recursively). Each field on one line.
- `file`: Prints path to the `.msg` file corresponding to the message type.
- `md5-text`: Prints the exact text from which the MD5 sum of the message is computed.

## Why this package

For long, there hasn't been any way of dynamically getting the full message definitions in C++ ROS nodes.
The only easily accessible way are the definitions reported by `rosmsg show` and `rosmsg show --raw`,
but neither of these corresponds to the one that is stored in bag files and reported by publishers
(rosmsg output has either stripped comments or is non-recursive). Moreover, calling `rosmsg show`
using `exec()` and similar methods from C++ is highly inefficient and unsafe.

This package provides a library that can finally provide the correct full message definition,
which contains comments and is recursive. This definition corresponds to the definitions in
bag files and those reported by publishers.

The `rosmsg_cpp` library internally also uses Python, but not via the `exec()` calls. It uses the C API
of Python to directly call the required interpreter functions. Moreover, the interpreter is initialized
only once when loading this library, and not in every call. Due to this fact, binaries that link to
this library may have a non-negligible memory footprint. If you care about the footprint,
call `Py_Finalize()` after you are done using this library.