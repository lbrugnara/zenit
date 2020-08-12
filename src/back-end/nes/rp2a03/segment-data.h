#ifndef RP2A03_DATA_SEGMENT_H
#define RP2A03_DATA_SEGMENT_H

#include <stddef.h>
#include <stdint.h>

typedef struct Rp2a03DataSegment {
    uint8_t *bytes;
    uint8_t *slots;
    uint16_t base_address;
} Rp2a03DataSegment;

Rp2a03DataSegment* rp2a03_data_segment_new(uint16_t base_address, size_t size);
void rp2a03_data_segment_free(Rp2a03DataSegment *data);
char* rp2a03_data_segment_disassemble(Rp2a03DataSegment *data, bool as_code, char *output);

#endif /* RP2A03_DATA_SEGMENT_H */
