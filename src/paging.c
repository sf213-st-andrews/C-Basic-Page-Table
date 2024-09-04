#include "paging.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Your implementation goes in here!
// My Functions
void get_binary(char* binary, uint16_t integer) {
	for (int bit = 0; bit < 16; bit++) {
        *(binary + (15-bit)) = ((integer & 0x1) + '0');
		integer = integer >> 1;
	}
}

void print_binary(uint16_t integer) {
	char* binary = (char*) malloc(sizeof(char) * 16);
	get_binary(binary, integer);
	printf("%s\n", binary);
	free(binary);
}

uint16_t get_entry_data(uint16_t entry, uint16_t section) {
	uint16_t bits = -1;
	switch (section) {
		case PAGE_MASK:
			bits = (entry & PAGE_MASK) >> (FRAME_BITS + META_BITS);
			break;
		case FRAME_MASK:
			bits = (entry & FRAME_MASK) >> META_BITS;
			break;
		case META_MASK:
			bits = (entry & META_MASK);
			break;
		default:
			printf("Error Uknown Mask\n");
			break;
	}
	return bits;
}

uint16_t generate_virtual_address(uint16_t page_number, uint16_t offset) {
	// Virtual_Address makeup 	[Page Number 6 Bits, Offset 10 Bits]
	return (page_number << OFFSET_BITS) | offset;
}

uint16_t get_page_number(uint16_t virtual_address) {
	// Get Page Number bits, in the 6 high order position bits. To get correct number as integer, it is bitwise shifted 10 to the right
	return (virtual_address & PAGE_MASK) >> OFFSET_BITS;
}

uint16_t get_page_entry(void* table, uint16_t virtual_address) {
	return *(uint16_t*) (table + (get_page_number(virtual_address)*PAGE_SIZE));
}

uint16_t get_frame_number(void* table, uint16_t virtual_address) {
	// Virtual_Address makeup 	[Page Number 6 Bits, Offset 10 Bits]
	// Page Table Entry makeup	[Page Number 6 Bits, Frame Number 6 Bits, Meta-Data 4 Bits]

	// Get Page entry
	uint16_t page_entry			= get_page_entry(table, virtual_address);

	// Get Frame Number, as the 6 low order bits
	uint16_t frame_number		= get_entry_data(page_entry, FRAME_MASK);
	return frame_number;
}



// Below are implementations of starter functions

void* pt_init() {
	uint16_t* pt = (uint16_t*) malloc(PAGETABLE_SIZE);
	return pt;
}

uint16_t virtual_to_physical(void* table, uint16_t virtual_address) {
	// Virtual_Address makeup 	[Page Number 6 Bits, Offset 10 Bits]
	// Page Table Entry makeup	[Page Number 6 Bits, Frame Number 6 Bits, Meta-Data 4 Bits]
	
	// Get Offset bits, in the 10 low order position bits. They will not need to be shifted to give the accurate number
	uint16_t offset_bits		= virtual_address & OFFSET_MASK;

	// Get Frame number
	uint16_t frame_number		= get_frame_number(table, virtual_address);

	// Get the Frame. Shift frame number to 6 higher order bits, with offset bits added back to the 10 low order bits
	uint16_t physical_address	= ((frame_number << OFFSET_BITS) | offset_bits);
	return physical_address;
}

void map_page_to_frame(void* table, uint16_t page_number, uint16_t frame_number, bool readonly, bool executable) {
	// Page Table Entry makeup	[Page Number 6 Bits, Frame Number 6 Bits, Meta-Data 4 Bits]

	uint16_t meta_data = OFFSET_MASK & DIRTY_BIT;
	if (readonly) {
		meta_data = meta_data | READ_ONLY_BIT;
	}
	if (executable) {
		meta_data = meta_data | EXECUTABLE_BIT;
	}
	uint16_t entry_data = (frame_number << META_BITS) | meta_data;

	memcpy((table + page_number*PAGE_SIZE), &entry_data, PAGE_SIZE);// Unsure about (&entry_data). This is referencing memory inside a function.
}

void print_table(void* table) {
	char* binary = (char*) malloc(sizeof(char) * 16);
	
	uint16_t page_entry;
	uint16_t physical_address;

	printf("Page\t| Frame\t| Phys.\t| Hex\t| Entry in Binary\n--------+-------+-------+-------+----------------\n");
	
	for (int i = 0; i < PAGETABLE_ROWS; i++) {
		page_entry = *(uint16_t*) (table + i*PAGE_SIZE);
		if ((page_entry & DIRTY_BIT) == 0x0000) {
			// Skip if: entry's Dirty bit is not true
			continue;
		}
		physical_address = virtual_to_physical(table, i << OFFSET_BITS);
		get_binary(binary, page_entry);

		printf("%d\t| %u\t| %u\t| %X\t| %s\n--------+-------+-------+-------+----------------\n", 
		i, 
		(page_entry & FRAME_MASK) >> META_BITS,
		physical_address, 
		physical_address,
		binary);
	}
	free(binary);
}

void unmap_page(void* table, uint16_t page_number) {
	memcpy((table + page_number*PAGE_SIZE), NULL, PAGE_SIZE);
}

void store_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length) {
	if (length > FRAME_SIZE) {
		printf("Error: Cannot store data larger than frame contains\n");
		return;
	}
	uint16_t meta_bits = get_entry_data(get_page_entry(table, virtual_address), META_MASK);
	if ((meta_bits & READ_ONLY_BIT) == READ_ONLY_BIT) {
		printf("Invalid Operation: Entry is READ_ONLY, cannot overwrite\n");
		return;
	}
	
	memcpy((store + virtual_to_physical(table, virtual_address)), buffer, length);
}

void read_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length) {
	if (length > FRAME_SIZE) {
		printf("Error: Cannot read data larger than frame contains\n");
		return;
	}
	memcpy(buffer, (store + virtual_to_physical(table, virtual_address)), length);
}
