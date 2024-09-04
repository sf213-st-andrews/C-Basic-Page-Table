// Imports from paging.c
#include "paging.h"

// Imports from test.c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void* store;
static void* table;

void setup() {
    store = malloc(1<<15);
    table = pt_init();
}

void teardown() {
    free(table);
    free(store);
}

void test1() {
	uint16_t o = 0x0001;
    uint16_t ot = 1;
    printf("o: %i\n", o);
    print_binary(o);
    printf("ot: %i\n", ot);
    print_binary(ot);
}

void test2() {
	uint16_t x = 0xFF00;
    uint16_t y = 0x00FF;
    
	printf("x: %i\n", x);
    print_binary(x);
    printf("y: %i\n", y);
    print_binary(y);

    uint16_t or = x | y;
    printf("or = x | y: %i, %X\n", or, or);
    print_binary(or);
}

void test3() {
	uint16_t z = 0x137F;
	printf("z: %i\n", z);
    print_binary(z);
}

void test4() {
	uint16_t m = 0xFC00;
    
    printf("m: %i\n", m);
    print_binary(m);
    printf("!m: %i\n", ~m);
    print_binary(~m);
}

void test5() {
	char* binary = (char*) malloc(sizeof(char) * 16);

	uint16_t dirty_bit = 0x0008;
	uint16_t readonly_bit = 0x0004;
	uint16_t exec_bit = 0x0002;

	uint16_t temp;

	temp = OFFSET_MASK;
	get_binary(binary, temp);
	printf("OFFSET_MASK:\t\t\t%X:\t%s\n", temp, binary);

	temp = OFFSET_MASK & dirty_bit;
	get_binary(binary, temp);
	printf("OFFSET_MASK & d:\t\t%X:\t%s\n", temp, binary);
    
	temp = OFFSET_MASK & (dirty_bit | readonly_bit);
	get_binary(binary, temp);
	printf("OFFSET_MASK & (d | r):\t\t%X:\t%s\n", temp, binary);

	temp = OFFSET_MASK & (dirty_bit | readonly_bit | exec_bit);
	get_binary(binary, temp);
	printf("OFFSET_MASK & (d | r | e):\t%X:\t%s\n", temp, binary);

	free(binary);
}

void test6() {
	uint16_t frame_num		= 0x000F;// 16:0x000F
	
	printf("%X\t:\t", frame_num);
	print_binary(frame_num);

	frame_num = frame_num * FRAME_SIZE;

	printf("%X\t:\t", frame_num);
	print_binary(frame_num);

	frame_num = frame_num << 6;

	printf("%X\t:\t", frame_num);
	print_binary(frame_num);

	frame_num = frame_num >> 6;

	printf("%X\t:\t", frame_num);
	print_binary(frame_num);
}

void test7() {
	uint16_t value = 0x0018;
	uint16_t test = value & DIRTY_BIT;
	if (test == DIRTY_BIT) {
		printf("works\n");
	}
	printf("%u\n", test);
	if ((value & DIRTY_BIT) == DIRTY_BIT) {
		printf("works\n");
	}
	printf("%u\n", test);
}

void test8() {
	uint16_t v_address1	= 0xBEEF;
	char text1[]		= "v_address1 = 0xBEEF. This spells BEEF.";
	size_t len1			= strlen(text1);
	char* buffer1		= malloc(len1+1);
	memcpy(buffer1, text1, len1);
	printf("%s\n", buffer1);
}
int main(void) {
	test8();
    return 0;
}