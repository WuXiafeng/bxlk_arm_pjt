/*bram.c*/
/*Add by Wuxiafeng 20181202 first version */

#include <bram.h>
 
#define BASE_ADDRESS     0x40000000
#define GPIO_DATA_OFFSET     0
#define GPIO_DIRECTION_OFFSET     4
 
#define MAP_SIZE 8192UL
#define MAP_MASK (MAP_SIZE - 1)

void * mapped_base = NULL;
int memfd = -1;

static inline int range_invalid(UINT32 offset, int len)
{
	if((offset < BASE_ADDRESS) || (offset + len > BASE_ADDRESS + MAP_SIZE))
		return 1;
	return 0;
}

int bram_read(UINT32 offset, UINT8 * buf, int len)
{
    void *mapped_dev_base; 
    off_t dev_base = BASE_ADDRESS; 
	if(range_invalid(offset,len))
	{
		printf("bram_read Invalid address!\n");
		return -1;
	}

	mapped_dev_base = mapped_base + (dev_base & MAP_MASK);

	memcpy(buf, \
        (unsigned char *)(mapped_dev_base + (offset - BASE_ADDRESS)), len);

    return 0;
}

int bram_write(UINT32 offset, UINT8 * buf, int len)
{
    void *mapped_dev_base; 
    off_t dev_base = BASE_ADDRESS; 
	if(range_invalid(offset,len))
	{
		printf("bram_write Invalid address!\n");
		return -1;
	}

	mapped_dev_base = mapped_base + (dev_base & MAP_MASK);

	memcpy((unsigned char *)(mapped_dev_base + (offset - BASE_ADDRESS)), \
         buf,  len);
    return 0;
}

int bram_release(void)
{
    if (munmap(mapped_base, MAP_SIZE) == -1) 
	{
        printf("Can't unmap memory from user space.\n");
        exit(0);
    }
 
    close(memfd);
    return 0;
}

int bram_init(void)
{
    off_t dev_base = BASE_ADDRESS; 
 
    memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if (memfd == -1) 
    {
        printf("Can't open /dev/mem.\n");
        exit(0);
    }
    printf("/dev/mem opened.\n"); 
 
    // Map one page of memory into user space such that the device is in that page, but it may not
    // be at the start of the page.
 
    mapped_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, \
    					memfd, dev_base & ~MAP_MASK);
    if (mapped_base == (void *) -1) 
    {
        printf("Can't map the memory to user space.\n");
        exit(0);
    }
    printf("Memory mapped at address %p.\n", mapped_base); 
	return 0;
}


