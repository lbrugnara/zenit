#ifndef ZENIT_NES_SEGMENT_H
#define ZENIT_NES_SEGMENT_H

typedef enum ZnesSegment {
    ZENIT_NES_SEGMENT_ZP,
    ZENIT_NES_SEGMENT_DATA,
    ZENIT_NES_SEGMENT_CODE,
    ZENIT_NES_SEGMENT_TEMP,
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

#endif /* ZENIT_NES_SEGMENT_H */
