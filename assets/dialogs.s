.include "../src/charmap.inc"
.segment "RODATA"
.export _dialog_beginning
_dialog_beginning: .byte 1, "Welcome, Heroes of the System! I'm Queen Amda...", 2, "... and I'm Queen Intella. Please save our system from crashing.", 1, "Our Kingdom OS relies on four drivers: GPU.DLL, Disk.DLL, IO.DLL and RAM.DLL", 2, "Now the drivers are full of bugs. Dive into each driver, removing them. Cleaning the last sector will yield its crypto key. Return with it and the driver will be finally safe.", 1, "Now go, heroes! You're our last hope!", 0
.export _dialog_gpu_dll
_dialog_gpu_dll: .byte 1, "Thanks for fixing GPU.DLL!", 0
.export _dialog_disk_dll
_dialog_disk_dll: .byte 2, "Thanks for fixing Disk.DLL!", 0
.export _dialog_io_dll
_dialog_io_dll: .byte 1, "Thanks for fixing IO.DLL!", 0
.export _dialog_ram_dll
_dialog_ram_dll: .byte 2, "Thanks for fixing RAM.DLL!", 0
.export _dialog_the_end
_dialog_the_end: .byte 1, "The system is finally safe! Thanks heroes!", 0
