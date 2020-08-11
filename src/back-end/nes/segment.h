#ifndef ZNES_SEGMENT_H
#define ZNES_SEGMENT_H

typedef enum ZnesSegment {
    ZNES_SEGMENT_ZP,
    ZNES_SEGMENT_DATA,
    ZNES_SEGMENT_CODE,
    ZNES_SEGMENT_TEMP,
} ZnesSegment;

typedef struct ZnesDataSegment {
    uint8_t *bytes;
    uint8_t *slots;
    uint16_t base_address;
} ZnesDataSegment;

typedef struct ZnesCodeSegment {
    uint8_t *bytes;
    uint16_t pc;
} ZnesCodeSegment;

typedef struct ZnesZeroPageSegment {
    uint8_t slots[255];
} ZnesZeroPageSegment;

#endif /* ZNES_SEGMENT_H */
