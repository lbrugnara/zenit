#ifndef ZNES_SEGMENT_H
#define ZNES_SEGMENT_H

typedef enum ZnesSegmentKind {
    ZNES_SEGMENT_ZP,
    ZNES_SEGMENT_DATA,
    ZNES_SEGMENT_TEXT,
    ZNES_SEGMENT_TEMP,
} ZnesSegmentKind;

static const char *znes_segment_kind_str[] = {
    [ZNES_SEGMENT_ZP]   = "ZERO PAGE",
    [ZNES_SEGMENT_DATA] = "DATA",
    [ZNES_SEGMENT_TEXT] = "TEXT",
    [ZNES_SEGMENT_TEMP] = "TEMP",
};

#endif /* ZNES_SEGMENT_H */
