# Operating_Systems_Project_1
# Operating Systems Producer-Consumer Problem

This program consists of two C++ files: one for the **producer** and one for the **consumer**. These programs use shared memory and semaphores to allow the two processes to interact with a shared table. The **producer** generates a random number between 1 and 1000 and places it in the table, while the **consumer** consumes the numbers in the table and replaces them with `0` to signify that the slot is empty.

# Compilation Instructions
To compile both the producer and consumer programs at the simultaneously yse the following Makfile commands in the terminal:

* make test

Both the producer and consumer programs separately can be compiled using the following commands in a terminal:

* g++ producer.cpp -pthread -lrt -o producer
* g++ consumer.cpp -pthread -lrt -o consumer

After compiling both programs, you can run the producer and consumer processes in the background using: 

* ./producer & ./consumer &

To remove the executable simply run the following Makefile command in the terminal:

* make clean

## Author

Conner Brunner
