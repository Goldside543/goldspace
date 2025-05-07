#include <stdint.h>
#include <stddef.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void init_paging() {

   unsigned int i;
   for(i = 0; i < 1024; i++)
   {
      // This sets the following flags to the pages:
      //   Supervisor: Only kernel-mode can access them
      //   Write Enabled: It can be both read from and written to
      //   Not Present: The page table is not present
      page_directory[i] = 0x00000002;
   }

   for(i = 0; i < 1024; i++)
   {
       // As the address is page aligned, it will always leave 12 bits zeroed.
       // Those bits are used by the attributes ;)
       first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
   }

   page_directory[0] = ((unsigned int)first_page_table) | 3;

   loadPageDirectory(page_directory);

}

void loadPageDirectory(uint32_t* page_directory) {
    asm volatile (
        "push %%ebp;"              // Save the current base pointer
        "mov %%esp, %%ebp;"        // Set up the new base pointer
        "mov 8(%%esp), %%eax;"     // Load the page directory pointer from the function argument
        "mov %%eax, %%cr3;"        // Load the page directory into CR3
        "mov %%ebp, %%esp;"        // Restore the original stack pointer
        "pop %%ebp;"               // Restore the base pointer
        "ret;"                     // Return from function
        :
        : "m"(page_directory)      // Input: the page_directory pointer
        : "%eax"                   // Clobber: we're modifying %eax
    );
}

void enable_paging() {
    asm volatile (
        "push %%ebp;"              // Save the current base pointer
        "mov %%esp, %%ebp;"        // Set up the new base pointer
        "mov %%cr0, %%eax;"        // Move the value of CR0 into %eax
        "or $0x80000000, %%eax;"   // Set the PG (bit 31) in %eax
        "mov %%eax, %%cr0;"        // Write the new value of %eax back to CR0
        "mov %%ebp, %%esp;"        // Restore the original stack pointer
        "pop %%ebp;"               // Restore the base pointer
        "ret;"                     // Return from function
        :
        :
        : "%eax"                   // Clobber: we're modifying %eax
    );
}
