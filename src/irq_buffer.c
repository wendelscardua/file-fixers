#pragma rodata-name ("RODATA")
#pragma code-name ("CODE")

char irq_array[32];
unsigned char double_buffer[32];

#pragma bss-name(push, "ZEROPAGE")
unsigned char double_buffer_index;
#pragma bss-name(pop)
