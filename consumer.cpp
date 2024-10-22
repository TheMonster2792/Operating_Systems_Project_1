#include <iostream>
#include <semaphore.h>
#include <thread>
#include <ctime>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>

constexpr int memorySize = 2;

struct SharedMemory {
    int buffer[memorySize];
    sem_t empty;
    sem_t full;
    sem_t mutex;
};

void consume(SharedMemory* shm) {
    while (true) {
        sem_wait(&shm->full);   // Wait for an item to be produced
        sem_wait(&shm->mutex);  // Ensure exclusive access to the buffer

        // Find and consume an item from the buffer
        int consumedItem = 0;
        for (int i = 0; i < memorySize; ++i) {
            if (shm->buffer[i] != 0) {
                consumedItem = shm->buffer[i];
                shm->buffer[i] = 0;
                break;
            }
        }

        std::cout << "Consumer consumed: " << consumedItem << std::endl;

        sem_post(&shm->mutex);  // Release exclusive access
        sem_post(&shm->empty);  // Signal that an item has been consumed

        sleep(rand() % 4 + 1);  // Simulate variable consumption time
    }
}

int main(){
    srand(static_cast<unsigned>(time(nullptr)));  // Seed random number generator

    // Open the shared memory object
    int shm_fd = shm_open("/sharedBuffer", O_RDWR, 0666);
    if(shm_fd == -1) {
        std::cerr << "Producer Failed to open Shared Memory" << std::endl;
        return 1;
    }

    // Map the shared memory object
    SharedMemory* shm = static_cast<SharedMemory*>(mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shm == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        return 1;
    }

    std::thread consumerThread(consume, shm);
    consumerThread.join();  // Wait for the consumer thread to finish (it never will)

    return 0;
}