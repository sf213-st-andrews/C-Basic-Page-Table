// Imports from paging.c
#include "paging.h"

// Imports from test.c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void* store;
static void* table;

// Setup the test. Initialize and/or malloc store and table
void setup() {
    store = malloc(1<<15);
    table = pt_init();
}

// Teardown pointers. Free the pointers at the end of test.
void teardown() {
    free(table);
    free(store);
}

void test1() {
    for (int i = 0; i < PAGETABLE_ROWS; i++) {
        map_page_to_frame(table, i, i, false, false);
    }
    print_table(table);
}

void test2() {
    map_page_to_frame(table, 0, 4, false, false);
	map_page_to_frame(table, 1, 3, true, false);
	map_page_to_frame(table, 2, 0, false, true);
	map_page_to_frame(table, 3, 2, true, true);
	map_page_to_frame(table, 4, 1, false, false);
    print_table(table);
}

void test3() {
    print_table(table);
}

// Test meta-data bits. Ignoring 'Clean' Bit entries.
void test4() {
    map_page_to_frame(table, 0, 0, false, false);
	map_page_to_frame(table, 1, 1, false, false);
	map_page_to_frame(table, 2, 2, false, false);
	map_page_to_frame(table, 3, 3, false, false);

	map_page_to_frame(table, 60, 60, true, true);
    map_page_to_frame(table, 61, 61, true, true);
    map_page_to_frame(table, 62, 62, true, true);
    map_page_to_frame(table, 63, 63, true, true);

    print_table(table);
}

/**
 * Shamelessly stolen from ./test.c
 * Now try to write something to our store. Our implementation should transparently use the 
 * page table to store the virtual pages into correct physical frames. This example should fit 
 * into a single page
*/
void test5() {
	map_page_to_frame(table, 0, 4, false, false);

    char text[]  = "It was the best of times, it was the worst of times";
	size_t length = strlen(text);
	char* buffer = malloc(length+1);

    store_data(table, store, text, 0, length);
	read_data(table, store, buffer, 0, length);
	printf("%s\n", buffer);

	print_table(table);// My own addition to the stolen code.

	free(buffer);
}

// Now try something longer and see what happens. The data read out should be exactly the same as what was put in. Also stolen from ./test.c
void test6() {
	map_page_to_frame(table, 0, 0, false, false);

    char text2[] = "It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, it was the epoch of belief, it was the epoch of incredulity, it was the season of light, it was the season of darkness, it was the spring of hope, it was the winter of despair, we had everything before us, we had nothing before us, we were all going direct to heaven, we were all going direct the other way–in short, the period was so far like the present period, that some of its noisiest authorities insisted on its being received, for good or for evil, in the superlative degree of comparison only.";
	size_t length = strlen(text2);
	char* buffer = malloc(length+1);

	store_data(table, store, text2, 0, length);
	read_data(table, store, buffer, 0, length);
	printf("%s\n", buffer);

	print_table(table);// My own addition to the stolen code.

	free(buffer);
}

// Each frame is 128 bytes = 1024 bits. Let's try something we know is too big for it. This string is of size: 1232. Nothing seems to break? Odd
void test7() {
	map_page_to_frame(table, 0, 0, false, false);
    
    char text3[] = "It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, it was the epoch of belief, it was the epoch of incredulity, it was the season of light, it was the season of darkness, it was the spring of hope, it was the winter of despair, we had everything before us, we had nothing before us, we were all going direct to heaven, we were all going direct the other way–in short, the period was so far like the present period, that some of its noisiest authorities insisted on its being received, for good or for evil, in the superlative degree of comparison only. I wonder what this quote could mean. It implies tensions, there are good things, and there are bad things, and they are in a contest with each other. Normal oppression is just bad. Such as WWI, the Trench Warfare (Old World Squabbles, New World Arms), WWII, Facism, the Holocaust (People were not having a good time). Those times were just bad. So what makes this time different? The good? What is it? The Enlightenment? In case you are wondering, yes I did think about and write about this quote. I will have you know I tried to escape literature analysis. I am agog that I find myself back in this train of thought.";
	size_t length = strlen(text3);
	char* buffer = malloc(length+1);

	store_data(table, store, text3, 0, length);
	read_data(table, store, buffer, 0, length);
	printf("%s\n", buffer);

	print_table(table);// My own addition to the stolen code.

	free(buffer);
}

// Continuation of 7. Demonstrates how simple (1) for index does not work.
void test8() {
    map_page_to_frame(table, 0, 0, false, false);
	map_page_to_frame(table, 1, 1, false, false);
    
    char text3[] = "It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, it was the epoch of belief, it was the epoch of incredulity, it was the season of light, it was the season of darkness, it was the spring of hope, it was the winter of despair, we had everything before us, we had nothing before us, we were all going direct to heaven, we were all going direct the other way–in short, the period was so far like the present period, that some of its noisiest authorities insisted on its being received, for good or for evil, in the superlative degree of comparison only. I wonder what this quote could mean. It implies tensions, there are good things, and there are bad things, and they are in a contest with each other. Normal oppression is just bad. Such as WWI, the Trench Warfare (Old World Squabbles, New World Arms), WWII, Facism, the Holocaust (People were not having a good time). Those times were just bad. So what makes this time different? The good? What is it? The Enlightenment? In case you are wondering, yes I did think about and write about this quote. I will have you know I tried to escape literature analysis. I am agog that I find myself back in this train of thought.";
	size_t length = strlen(text3);
	char* buffer = malloc(length+1);

	store_data(table, store, text3, 0, length);
	read_data(table, store, buffer, 1, length);
	printf("%s\n", buffer);

	print_table(table);// My own addition to the stolen code.

	free(buffer);
}

/**
 * Can text be read if it is not overflowing? Yes, but the first character appears to be ommitted.
 * Need to malloc in map function? No? It does not seem to work that way
 * AHHH IT'S THE FORMATTING OF THE VIRTUAL ADDRESS! I need to fix that.
 */
void test9() {
    map_page_to_frame(table, 0, 0, false, false);
	map_page_to_frame(table, 1, 1, false, false);

    char text4[] = "Nothing Important Here";
    size_t length = strlen(text4);
	char* buffer = malloc(length+1);

    store_data(table, store, text4, 0, length);
	read_data(table, store, buffer, 1, length);//0x0400
	printf("%s\n", buffer);

	print_table(table);

	free(buffer);
}

// Multiple entries, using generate_virtual_address to correctly format the address
void test10() {
	// Generate virtual (logical) addresses
	uint16_t v_address1 = generate_virtual_address(0, 0);
	uint16_t v_address2 = generate_virtual_address(1, 0);
	uint16_t v_address3 = generate_virtual_address(2, 0);

    map_page_to_frame(table, 0, 0, false, false);
	map_page_to_frame(table, 1, 1, false, false);
	map_page_to_frame(table, 2, 2, false, false);

	print_table(table);

	char* buffer1;
	char* buffer2;
	char* buffer3;

	char text1[] 	= "The One Piece";
	size_t len1		= strlen(text1);
	buffer1 		= malloc(len1+1);

	char text2[]	= "! ";
	size_t len2		= strlen(text2);
	buffer2 		= malloc(len2+1);

	char text3[]	= " Is Real";
	size_t len3		= strlen(text3);
	buffer3 		= malloc(len3+1);

	store_data(table, store, text1, v_address1, len1);
	store_data(table, store, text2, v_address2, len2);
	store_data(table, store, text3, v_address3, len3);

	read_data(table, store, buffer1, v_address1, len1);
	read_data(table, store, buffer2, v_address2, len2);
	read_data(table, store, buffer3, v_address3, len3);
	
	printf("%s%s%s%s%s\n", buffer1, buffer2, buffer1, buffer3, buffer2);

	free(buffer1);
	free(buffer2);
	free(buffer3);
}

/**
 * Using offset to read different parts of one frame.
 * Demonstrates making multiple sentences from one frame which contains a single sentence.
*/
void test11() {
	// Generate virtual (logical) addresses
	uint16_t v_address1 = generate_virtual_address(0, 0);// 	start:			The
	uint16_t v_address2 = generate_virtual_address(0, 14);// 	mid:			Is
	uint16_t v_address3 = generate_virtual_address(0, 21);//	end:			!
	uint16_t v_address4 = generate_virtual_address(0, 3);//		first space:	" "

    map_page_to_frame(table, 0, 0, false, false);
	
	print_table(table);

	char text[] 	= "The One Piece Is Real!";// We want to print "The One Piece! The One Piece Is Real!"
	size_t len		= strlen(text);
	size_t len1		= 13;
	size_t len2		= 7;
	size_t len3		= 1;
	size_t len4		= 1;
	
	char* buffer1	= malloc(len1+1);//	The One Piece
	char* buffer2	= malloc(len2+1);//	Is Real
	char* buffer3	= malloc(len3+1);//	!
	char* buffer4	= malloc(len4+1);//	" "

	store_data(table, store, text, v_address1, len);

	read_data(table, store, buffer1, v_address1, len1);
	read_data(table, store, buffer2, v_address2, len2);
	read_data(table, store, buffer3, v_address3, len3);
	read_data(table, store, buffer4, v_address4, len4);

	printf("%s|%s|%s|%s|%s|%s|%s\n", buffer1, buffer3, buffer4, buffer1, buffer4, buffer2, buffer3);
	printf("%s%s%s%s%s%s%s\n", buffer1, buffer3, buffer4, buffer1, buffer4, buffer2, buffer3);

	free(buffer1);
	free(buffer2);
	free(buffer3);
	free(buffer4);
}

/**
 * Part 1 test: Convert virtual memory address into a physical memory address
*/
void test12() {
	for (int i = 0; i < PAGETABLE_ROWS; i++) {
        map_page_to_frame(table, i, i, false, false);
    }

	uint16_t v_address1	= 0xBEEF;
	uint16_t v_address2	= 0xC001;
	uint16_t v_address3	= 0x2441;// 09281 converts to this

	char text1[]			= "v_address1 = 0xBEEF. This spells Beef in human talk.";
	char text2[]			= "v_address2 = 0xC001. This spells Cool in standard vocabulary.";
	char text3[]			= "v_address3 = 0x2441. This spells nothing, other than a number. It's my number.";

	size_t len1				= strlen(text1);
	size_t len2				= strlen(text2);
	size_t len3				= strlen(text3);

	char* buffer1			= malloc(len1+1);
	char* buffer2			= malloc(len2+1);
	char* buffer3			= malloc(len3+1);

	char* binary_p_1		= (char*) malloc(sizeof(char) * 16);
	char* binary_p_2		= (char*) malloc(sizeof(char) * 16);
	char* binary_p_3		= (char*) malloc(sizeof(char) * 16);


	char* binary_a_1		= (char*) malloc(sizeof(char) * 16);
	char* binary_a_2		= (char*) malloc(sizeof(char) * 16);
	char* binary_a_3		= (char*) malloc(sizeof(char) * 16);

	store_data(table, store, text1, v_address1, len1);
	store_data(table, store, text2, v_address2, len2);
	store_data(table, store, text3, v_address3, len3);

	read_data(table, store, buffer1, v_address1, len1);
	read_data(table, store, buffer2, v_address2, len2);
	read_data(table, store, buffer3, v_address3, len3);

	get_binary(binary_p_1, get_page_entry(table, v_address1));
	get_binary(binary_p_2, get_page_entry(table, v_address2));
	get_binary(binary_p_3, get_page_entry(table, v_address3));

	get_binary(binary_a_1, v_address1);
	get_binary(binary_a_2, v_address2);
	get_binary(binary_a_3, v_address3);

	printf("%s\n%s\n%s\n", buffer1, buffer2, buffer3);
	
	printf("Virtual Address Info-\n");
	
	printf("\tHex: %X\tBinary: %s\tDeci: %i\tPage Number: %i\tOffset: %i\n",
	v_address1, binary_a_1, v_address1, get_page_number(v_address1), (v_address1 & OFFSET_MASK));
	
	printf("\tHex: %X\tBinary: %s\tDeci: %i\tPage Number: %i\tOffset: %i\n",
	v_address2, binary_a_2, v_address2, get_page_number(v_address2), (v_address2 & OFFSET_MASK));
	
	printf("\tHex: %X\tBinary: %s\tDeci: %i\tPage Number: %i\tOffset: %i\n",
	v_address3, binary_a_3, v_address3, get_page_number(v_address3), (v_address3 & OFFSET_MASK));
	

	printf("Page Entries-\n");
	printf("\tAddress: %X\tEntry: %s\tPage Number: %i\tFrame Number: %i  \tMeta Data: %X\n",
	v_address1,
	binary_p_1,
	get_page_number(v_address1),
	get_frame_number(table, v_address1),
	get_entry_data(get_page_entry(table, v_address1), META_MASK));

	printf("\tAddress: %X\tEntry: %s\tPage Number: %i\tFrame Number: %i  \tMeta Data: %X\n",
	v_address2,
	binary_p_2,
	get_page_number(v_address2),
	get_frame_number(table, v_address2),
	get_entry_data(get_page_entry(table, v_address2), META_MASK));

	printf("\tAddress: %X\tEntry: %s\tPage Number: %i\tFrame Number: %i  \tMeta Data: %X\n",
	v_address3,
	binary_p_3,
	get_page_number(v_address3),
	get_frame_number(table, v_address3),
	get_entry_data(get_page_entry(table, v_address3), META_MASK));

	free(buffer1);
	free(buffer2);
	free(buffer3);

	free(binary_p_1);
	free(binary_p_2);
	free(binary_p_3);

	free(binary_a_1);
	free(binary_a_2);
	free(binary_a_3);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Error: Expected argv[1] to be present");
        return 1;
    }
    
    switch (atoi(argv[1])) {
        case 12:
            setup();
            test12();
            teardown();
            break;

		case 11:
            setup();
            test11();
            teardown();
            break;
			
		case 10:
            setup();
            test10();
            teardown();
            break;

		case 9:
            setup();
            test9();
            teardown();
            break;

        case 8:
            setup();
            test8();
            teardown;
            break;

        case 1:
            setup();
            test1();
            teardown();
            break;

        case 2:
            setup();
            test2();
            teardown();
            break;

        case 3:
            setup();
            test3();
            teardown();
            break;

        case 4:
            setup();
            test4();
            teardown();
            break;

        case 5:
            setup();
            test5();
            teardown();
            break;

        case 6:
            setup();
            test6();
            teardown();
            break;

        case 7:
            setup();
            test7();
            teardown();
            break;

        default:
            printf("Error: Test requested does not exist\n");
            return 1;
    }

    return 0;
}