Made and tested with Ubuntu 22.04.2 LTS using WSL2.

The code is documented using Doxygen. A generated version can be found under "./docs/html"
and can be viewed by opening the "index.html" file in that directory with a web-browser.
The remaining details of this README are also explained in the Doxygen documentation.


The following commands are necessary to install salticidae and other dependencies:
	- sudo apt update
	- sudo apt install -y build-essential cmake libuv1-dev libssl-dev
	- git clone https://github.com/Determinant/salticidae.git
	- cd salticidae/
	- cmake .
	- make
	- sudo make install

For debugging also install gdb
	- sudo apt install -y gdb

The salticidae folder can be deleted afterwards.
	- cd ..
	- rm -rf salticidae

One line command:
	sudo apt update && \
	sudo apt install -y build-essential cmake libuv1-dev libssl-dev && \
	git clone https://github.com/Determinant/salticidae.git && \
	cd salticidae/ && \
	cmake . && \
	make && \
	sudo make install

The following make commands are available:
	- make
		Build the project.
	- make debug
		Build the project with debugging symbols and the DEBUG flag defined (Useful for #ifdef DEBUG ... #endif).
	- make clean
		Remove temporary build files.
	- make run
		Clears the terminal and executes the project via ./main.

To generate documentation for the project install doxygen (and the optional deppendency graphviz):
	- sudo apt install -y doxygen graphviz
then generate the documentation with
	- doxygen Doxyfile
