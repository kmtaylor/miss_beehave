#include <stdio.h>
#include <stdlib.h>

/* type (includes rotation), chain quadrant, chain channel */
int map[10*8] = {
    0x420, 0x430, 0x460, 0x470, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
    0x400, 0x410, 0x440, 0x450, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
    0x031, 0x032, 0x033, 0x034, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
    0x021, 0x022, 0x023, 0x024, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
    0x011, 0x012, 0x013, 0x014, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
    0x001, 0x002, 0x003, 0x004, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
    0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
    0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
};

int main(int argc, char **argv) {
    int x, y;
    int quant_x, quant_y;
    int map_addr, map_val;
    int chain_idx, chain_quad, chain_type;
    int mem_addr;

    if (argc < 3) {
        printf("Must give coordinates.\n");
        return 1;
    }

    x = atoi(argv[1]);
    y = atoi(argv[2]);

    if (x > 80 || y > 64) {
        printf("Out of range.\n");
        return 1;
    }

    printf("Location: %i,%i\n", x, y);

    quant_x = x / 8;
    quant_y = y / 8;
    map_addr = quant_y*10 + quant_x;

    printf("Quant location: %i,%i\n", quant_x, quant_y);
    printf("Map address: %i\n", map_addr);

    map_val = map[map_addr];
    
    printf("Map val %03X\n", map_val);

    chain_idx = (map_val >> 0) & 0xf;
    chain_quad = (map_val >> 4) & 0xf;
    chain_type = (map_val >> 8) & 0xf;

    mem_addr = (chain_idx * 512);

    switch (chain_type) {
        case 0:
            mem_addr += (chain_quad * 64);
            mem_addr += 64 - (y % 8);
            mem_addr += (y % 2) ? (x % 8) : (8 - (x % 8)); 
            break;
        case 4:
            mem_addr += 128;
            mem_addr += (chain_quad / 2) * 128;
            mem_addr -= (y % 8) * 16;
            if (y % 2) {
                mem_addr += (chain_quad % 2) * 8;
                mem_addr += (x % 8);
                mem_addr -= 16;
            } else {
                mem_addr -= (chain_quad % 2) * 8;
                mem_addr -= 1 + (x % 8);
            }
            break;
    }

    printf("Mem addr %i\n", mem_addr);

    return 0;
}
