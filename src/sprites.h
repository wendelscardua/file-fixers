#include "constants.h"

const unsigned char default_cursor_sprite[]={
                                             0,  0,0x00,0,
                                             8,  0,0x01,0,
                                             0,  8,0x10,0,
                                             8,  8,0x11,0,
                                             0, 16,0x20,0,
                                             8, 16,0x21,0,
                                             0x80
};

const unsigned char clicking_cursor_sprite[]={
                                              0,-16,0x02,0,
                                              8,-16,0x03,0,
                                              0,- 8,0x12,0,
                                              8,- 8,0x13,0,
                                              0,  0,0x22,0,
                                              8,  0,0x23,0,
                                              0x80
};

const unsigned char loading_cursor_sprite[]={
                                             - 8,- 8,0x04,0,
                                             0,- 8,0x05,0,
                                             - 8,  0,0x14,0,
                                             0,  0,0x15,0,
                                             0x80
};
