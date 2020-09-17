#ifndef RP2A03_TEXT_SEGMENT_H
#define RP2A03_TEXT_SEGMENT_H

#include <stdint.h>
#include <fllib/containers/List.h>

typedef FlList Rp2a03PendingJumpList;
typedef struct FlListNode Rp2a03PendingJumpListNode;

typedef struct Rp2a03PendingJump {
    uint16_t base_jump_pc;
    uint16_t byte_index;
    uint16_t ir_offset;
    bool absolute;
} Rp2a03PendingJump;

typedef struct Rp2a03TextSegment {
    Rp2a03PendingJumpList *pending_jumps;
    uint8_t *bytes;
    uint16_t pc;
    uint16_t base_address;
} Rp2a03TextSegment;

Rp2a03TextSegment* rp2a03_text_segment_new(size_t base_address);
void rp2a03_text_segment_free(Rp2a03TextSegment *text);
void rp2a03_text_segment_add_pending_jump(Rp2a03TextSegment *text, Rp2a03PendingJump *pending_jump);
void rp2a03_text_segment_backpatch_jumps(Rp2a03TextSegment *text);
void rp2a03_text_segment_backpatch_absolute_jumps(Rp2a03TextSegment *text);
char* rp2a03_text_segment_disassemble(Rp2a03TextSegment *text, char *title, char *output);

#endif /* RP2A03_TEXT_SEGMENT_H */
