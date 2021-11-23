#define DICE(amount,sides) ((sides << 3) | amount)

unsigned char roll_dice(unsigned char dice_spec);
