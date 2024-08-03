// Function to create a delay in CPU cycles
void cpu_delay(unsigned int cycles) {
    volatile unsigned int count;
    for (count = 0; count < cycles; count++) {
        // Empty loop to waste CPU cycles
    }
}
