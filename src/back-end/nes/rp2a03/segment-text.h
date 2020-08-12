#ifndef RP2A03_TEXT_SEGMENT_H
#define RP2A03_TEXT_SEGMENT_H

#include <stdint.h>

typedef struct Rp2a03TextSegment {
    uint8_t *bytes;
    uint16_t pc;
    uint16_t base_address;
} Rp2a03TextSegment;

Rp2a03TextSegment* rp2a03_text_segment_new(size_t base_address);
void rp2a03_text_segment_free(Rp2a03TextSegment *text);
char* rp2a03_text_segment_disassemble(Rp2a03TextSegment *text, char *title, char *output);

#endif /* RP2A03_TEXT_SEGMENT_H */
