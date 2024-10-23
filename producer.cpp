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

void produce(SharedMemory* shm) {
    while (true) {
        int producedItem = rand() % 1000 + 1;  // Generate a random number between 1 and 1000

        sem_wait(&shm->empty);   // Wait if the buffer is full
        sem_wait(&shm->mutex);   // Ensure exclusive access to the buffer

        // Place the produced item into the buffer
        for (int i = 0; i < memorySize; ++i) {
            if (shm->buffer[i] == 0) {
                shm->buffer[i] = producedItem;
                break;
            }
        }

        std::cout << "Producer produced: " << producedItem << std::endl;

        sem_post(&shm->mutex);   // Release exclusive access
        sem_post(&shm->full);    // Signal that an item has been produced

        sleep(rand() % 4 + 1);   // Simulate variable production time
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));  // Seed random number generator

    // Create and open the shared memory object
    int shm_fd = shm_open("/sharedBuffer", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory" << std::endl;
        return EXIT_FAILURE;
    }

    // Set the size of the shared memory object
    if (ftruncate(shm_fd, sizeof(SharedMemory)) == -1) {
        std::cerr << "Failed to set shared memory size" << std::endl;
        return EXIT_FAILURE;
    }

    // Map the shared memory object
    SharedMemory* shm = static_cast<SharedMemory*>(mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shm == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        return EXIT_FAILURE;
    }

    // Initialize semaphores
    sem_init(&shm->empty, 1, memorySize);  // Start with all slots empty
    sem_init(&shm->full, 1, 0);            // No items to consume at the start
    sem_init(&shm->mutex, 1, 1);           // Ensure mutual exclusion

    std::thread producerThread(produce, shm);
    producerThread.join();  // Wait for the producer thread to finish (it never will)

    return 0;
}
