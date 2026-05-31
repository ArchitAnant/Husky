#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint64_t left_over_start;
    uint32_t free_size;
} m_lfov;

typedef struct {
    uint32_t size; // How many bytes were allocated here
} MallocHeader;

void* kmalloc(size_t size);
void kfree(void* ptr);