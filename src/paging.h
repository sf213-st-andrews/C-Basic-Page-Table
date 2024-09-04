#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// You may find defining the following macros useful. Edit the values below to match your report.

// Number of Rows (Entries) in the Pagetable. 2^6 = 64
#define PAGETABLE_ROWS	64

// Total bits of entire pagetable. 2^16 (total memory) / 2^10 (frame size) = 2^6 pages. Each page takes 16 bits, so 2^6 * 2^4 = 2^10 = 1024
#define PAGETABLE_SIZE	1024

// The page size. My own addition.
#define PAGE_SIZE       16

// The frame size. My own addition.
#define FRAME_SIZE      1024

// 6 Bits to determine which of the 64 frames to reference.
#define FRAME_BITS 	    6

// 4 Bits for meta-data. My own addition.
#define META_BITS		4

// 10 Bits to determine offset of frames of size 2^10 = 1024
#define OFFSET_BITS 	10

/**
 * Mask for bitwise operations.  My own addition.
 * Bitwise & operation will remove the 6 non offset bits from a virtual address.
 * In Binary:	0000001111111111
 */
#define OFFSET_MASK 	0x03FF

// Hexidecimal value of true dirty bit. My own addition.        0000000000001000
#define DIRTY_BIT       0x0008

// Hexidecimal value of true read-only bit. My own addition.    0000000000000100
#define READ_ONLY_BIT   0x0004

// Hexidecimal value of true executalbe bit. My own addition.   0000000000000010
#define EXECUTABLE_BIT	0x0002

// Mask of page bits of a page table entry. My own addition.    1111110000000000
#define PAGE_MASK       0xFC00

// Mask of frame bits of a page table entry. My own addition.   0000001111110000
#define FRAME_MASK		0x03F0

// Mask of meta bits of a page table entry. My own addition.    0000000000001111
#define META_MASK		0x000F

/**
 * Determine the binary form of uint16_t, then load it into a given char array. My own addition.
 * Used for debugging.
 * 
 * @param   binary      The char array which the result will be loaded into.
 * @param   integer     The integer which will be decoded into binary.
*/
void get_binary(char* binary, uint16_t integer);

/**
 * Print a uint16_t in binary. Used for debugging.
 * 
 * @param   integer     The integer which will be printed in binary
*/
void print_binary(uint16_t integer);

/**
 * Get the bits of a chosen section of a page entry. My own addition.
 * Use _MASK definitions to choose which setction to get.
 * 
 * @param	page_entry	the page entry
 * @param	section		the section to be returned
 * 
 * @return				the selected data retrived, positioned in the lowest part.
*/
uint16_t get_entry_data(uint16_t entry, uint16_t section);

/**
 * Convert a CPU generated address ([Page Number], [Offset]) into a virtual address. My own addition
 * The virtual address is a 16-bit integer with the page number making up the 6 high order bits, 
 * and the offset making up the 10 low order bits.
 * 
 * @param   page_number The page number to be requested
 * @param   offset      The offset, to determine which part of the frame to begin reading information from.
 * 
 * @return              16-bit integer that is the virtual address.
*/
uint16_t generate_virtual_address(uint16_t page_number, uint16_t offset);

/**
 * Convert virtual memory address into a page number. My own addition
 * 
 * This function will use bitwise operations to obtain the 6 higher order bits to get the page number.
 * 
 * @param   virtual_address Virtual address to be converted to a page number
 * 
 * @return  page_number contained in the virtual address
 * 
*/
uint16_t get_page_number(uint16_t virtual_address);

/**
 * Convert virtual memory address into a page number. My own addition
 * 
 * @param	table           Pointer to the page table returned by pt_init()
 * @param	virtual_address	Virtual address to be converted to a page entry
 * 
 * @return					The entry in the table
*/
uint16_t get_page_entry(void* table, uint16_t virtual_address);

/**
 * Convert virtual memory address into a frame number. My own addition.
 * 
 * This function will use the supplied page table to convert a virtual address (16-bit integer)
 * to a frame number another 16-bit integer. The output 16-bit integer will only use the 6 lower bits.
 * 
 * @param	table           Pointer to the page table returned by pt_init()
 * @param	virtual_address	Virtual address to be converted to a physical address
 * 
 * @return					The frame number
*/
uint16_t get_frame_number(void* table, uint16_t virtual_address);



/**
 * Initialise the page table. This should allocate the memory for the page table and return the pointer.
 * It is entirely up to you how you organise the data at this memory address!
 */
void* pt_init();

/**
 * Convert virtual memory address to a physical address
 *
 * This function will use the supplied page table to convert a virtual address (16-bit integer)
 * to a physical address (another 16-bit integer). The latter can be used to index straight into an
 * in-memory store that simulates our physical memory
 *
 * @param	table		    Pointer to the page table returned by pt_init()
 * @param	virtual_address	Virtual address to be converted to a physical address
 *
 * @return					The physical address corresponding to the virtual address
 */
uint16_t virtual_to_physical(void* table, uint16_t virtual_address);

/**
 * Add a page to frame mapping to the page table
 *
 * This function will add an entry to the page table which maps a logical (virtual) page to a 
 * physical frame. In our implementation, physical memory is modelled by an in-memory store that 
 * we can index into. This operation does not touch the store, only updates the page table 
 * at the supplied memory address.
 *
 * @param	table		    Pointer to the page table returned by pt_init()
 * @param	page_number	    Page number 
 * @param	frame_number	The corresponding frame number to be entered into the table
 * @param	readonly	    Read-only bit that determines whether this frame can be written to
 * @param	executable	    Executable bit that determines whether this frame can contain code
 */
void map_page_to_frame(void* table, uint16_t page_number, uint16_t frame_number, bool readonly, bool executable);

/**
 * Remove a page to frame mapping to the page table
 *
 * This function will remove an entry from the page table which maps a logical (virtual) page to a 
 * physical frame. 
 *
 * @param	table		Pointer to the page table returned by pt_init()
 * @param	page_number	Page number whose mapping we want to remove
 */
void unmap_page(void* table, uint16_t page_number);

/**
 * Stores a chunk of data into our store
 *
 * This function will take data of a given length and copy it into the store. It will do this 
 * by using the page table to make sure that the virtual addresses are correctly translated to
 * physical addresses (which are used to index into our store).
 *
 * @param	table		        Pointer to the page table returned by pt_init()
 * @param	store		        Pointer to the in-memory store which simulates physical memory
 * @param	buffer		        Pointer to the start of the data we want to store
 * @param	virtual_address	    The 16-bit virtual address describing where to store the data
 * @param	length		        The number of bytes to copy
 */
void store_data(void *table, void* store, void* buffer, uint16_t virtual_address, size_t length);

/**
 * Reads a chunk of data from our store
 *
 * This function is the opposite of store_data. It will read a chunk of data from a supplied
 * virtual memory address. It will make use of the page table to map virtual pages to physical
 * frames in order to correctly index into the in-memory store.
 *
 * @param	table		        Pointer to the page table returned by pt_init()
 * @param	store		        Pointer to the in-memory store which simulates physical memory
 * @param	buffer		        Pointer to a pre-allocated buffer our function will fill
 * @param	virtual_address	    The 16-bit virtual address describing where to read the data
 * @param	length		        The number of bytes to copy
 */
void read_data(void *table, void *store, void* buffer, uint16_t virtual_address, size_t length);

/**
 * Print the contents of the table
 *
 * This function is useful for debugging. Here you can write code to print all the valid 
 * mappings from pages to frames in your page table to make sure that you are storing the
 * table correctly. Hint: make use of the valid bit to only print the entries that have a valid
 * mapping defined.
 *
 * @param	table		Pointer to the page table returned by pt_init()
 */
void print_table(void* table);

#endif
