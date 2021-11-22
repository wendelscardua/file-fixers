#ifndef _TEMP_H
extern unsigned char arg1;
extern unsigned char arg2;
extern unsigned char pad1;
extern unsigned char pad1_new;
extern unsigned char temp, i, temp_x, temp_y;
extern unsigned int temp_int, temp_int_x, temp_int_y;

#pragma zpsym("arg1")
#pragma zpsym("arg2")
#pragma zpsym("pad1")
#pragma zpsym("pad1_new")
#pragma zpsym("temp")
#pragma zpsym("i")
#pragma zpsym("temp_x")
#pragma zpsym("temp_y")
#pragma zpsym("temp_int")
#pragma zpsym("temp_int_x")
#pragma zpsym("temp_int_y")
#endif
