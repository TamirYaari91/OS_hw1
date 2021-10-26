#include <stdio.h>
#include "os.h"


uint64_t getLevelAddress(uint64_t vpn, int level) {
    return vpn >> (36 - 9 * level) & 0x1FF;
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) {
    uint64_t *pageTable = phys_to_virt(pt << 12);
    uint64_t levelAddress;
    uint64_t nextLevelPage = 0;
    int valid;
    int i;

    for (i = 0; i < 4; ++i) {
        levelAddress = getLevelAddress(vpn, i);
        valid = pageTable[levelAddress] & 1;
        if (!valid) {
            if (ppn == NO_MAPPING) {
                return;
            } else {
                nextLevelPage = alloc_page_frame() << 12;
                pageTable[levelAddress] = nextLevelPage + 1;
            }
        }
        pageTable = phys_to_virt(nextLevelPage);
    }
    levelAddress = getLevelAddress(vpn, 4);
    if (ppn == NO_MAPPING) {
        pageTable[levelAddress] = 0;
    } else {
        pageTable[levelAddress] = (ppn << 12) + 1;
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn) {
    uint64_t *pageTable = phys_to_virt(pt << 12);
    uint64_t levelAddress;
    int valid;
    int i;

    for (i = 0; i < 5; ++i) {
        levelAddress = getLevelAddress(vpn, i);
        valid = pageTable[levelAddress] & 1;
        if (!valid) {
            return NO_MAPPING;
        }
        if (i < 4) {
            pageTable = phys_to_virt(pageTable[levelAddress] - 1);
        }
    }
    return pageTable[levelAddress] >> 12;
}

