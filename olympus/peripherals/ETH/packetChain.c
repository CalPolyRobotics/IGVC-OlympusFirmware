#include <stddef.h>
#include <string.h>

typedef struct packetChain packetChain_t;
struct packetChain {
    packetChain_t *next;
    char *packet;
    size_t len;
};

void pchain_add(packetChain_t **head) {
    if(*head = NULL){
        *head = mem_alloc(size
    }

}
