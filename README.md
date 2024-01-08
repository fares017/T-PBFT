# T-PBFT-Protocol-Framework



## How to use this code and what it is
This code is intended to be some skeleton code to simplify the implementation of stateful BFT protocols. This project handles
the message exchange via the [salticidae](https://github.com/Determinant/salticidae) library in an asynchronous way. 

We define a Node class that can be seen as a replica in a replicated scenario and member variables can be defined and updated to represent
the state of a given instance. Messages that are received will enter a callback linked to that message from a specific Node instance.

The Handler creates the defined number of replicas and connects the nodes together in a fully connected network.

The messages.h file defines the messages and the messages.cpp file implements the serialization and de-serialization of the messages that are sent over the network
as part of the protocol that is implemented on top.

The config.h file defines some configuration variables that are used throughout the project, for example the number of replicas that should
be used.

Since this code is not a library per-se, it should not be imported but instead be modified and built-upon to implement a certain protocol. For a basic
protocol implementation only the messages.h, messages.cpp, node.h and node.cpp files should need to be updated.

There is an accompanying client code that can be used for testing purposes. Its documentation is not as detailed as this code's since it follows many of the same paradigms,
just that it is only a single client trying to connect.


## Installation

### Dependencies

The skeleton code needs the following Linux (Debian/Ubuntu) packages installed:
 - build-essential
 - cmake
 - libuv1-dev
 - libssl-dev
 - libcrypto++-dev

The code was made under WSL2 with Ubuntu 22.04.2 LTS but should also run natively on Ubuntu and related Distros.

The following commands are necessary to install salticidae and other dependencies:
```
sudo apt update
sudo apt install -y build-essential cmake libuv1-dev libssl-dev
git clone https://github.com/Determinant/salticidae.git
cd salticidae/
cmake .
make
sudo make install
```


## Building
The project uses make to be built and the following make commands are available:
 - make
 -- Build the project.
 - make debug
 -- Build the project with debugging symbols and the DEBUG flag defined (Useful for <code>#ifdef DEBUG ... #endif</code>).
 - make clean
 -- Remove temporary build files.
 - make run
 -- Clears the terminal and executes the project via ./main.

## Debugging
The project was made in the Visual Studio Code IDE and the necessary settings are made to enable debugging integrated into VSCode (via F5 key).
Other dependencies (for VSCode development) are:
 - [C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
 - gdb (sudo apt install -y gdb)

## Documentation
The project's documentation is made using [Doxygen](https://www.doxygen.nl/index.html) and can be generated by installing Doxygen:
```
sudo apt install -y doxygen graphviz
```

Then building the documentation in the root of the server folder using:
```
doxygen Doxygen
```

The generated documentation of the current commit is also in this repository under "./server/docs/html/index.html".


