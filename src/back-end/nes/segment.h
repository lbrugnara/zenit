#ifndef ZENIT_NES_SEGMENT_H
#define ZENIT_NES_SEGMENT_H

enum ZenitNesSegment {
    ZENIT_NES_SEGMENT_ZP,
    ZENIT_NES_SEGMENT_DATA,
    ZENIT_NES_SEGMENT_CODE,
    ZENIT_NES_SEGMENT_TEMP,
};

struct ZenitNesDataSegment {
    uint8_t *bytes;
    uint8_t *slots;
    uint16_t base_address;
};

struct ZenitNesCodeSegment {
    uint8_t *bytes;
    uint16_t pc;
};

struct ZenitNesZeroPageSegment {
    uint8_t slots[255];
};

#endif /* ZENIT_NES_SEGMENT_H */
