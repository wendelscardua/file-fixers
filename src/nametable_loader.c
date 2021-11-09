#define SLICE_SIZE 64
#define NUM_SLICES (1024/(SLICE_SIZE))

#include "lib/nesdoug.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")
unsigned char * nametable_buffer;
unsigned int current_nametable_address;
unsigned char nametable_slice_counter;
#pragma bss-name(pop)

void set_nametable_loader_buffer(unsigned char * buffer) {
  nametable_buffer = buffer;
}

void start_nametable_loader(unsigned int nametable_address) {
  current_nametable_address = nametable_address;
  nametable_slice_counter = 0;
}

unsigned char yield_nametable_loader () {
  if (nametable_slice_counter >= NUM_SLICES) return 0;

  multi_vram_buffer_horz((char *) nametable_buffer, SLICE_SIZE, current_nametable_address);

  nametable_buffer += SLICE_SIZE;
  current_nametable_address += SLICE_SIZE;

  ++nametable_slice_counter;
  return 1;
}
