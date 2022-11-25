/*
 * aml6236-m nand spl
 * @author jerry.yu
 *
 */
/* These define will be replace with */
#include <config.h>
#include <version.h>
#include <asm/arch/romboot.h>

#define PARTATION(active, type, start, size)	\
	.word						\
		active | 0xFFFFFE00,	\
		type | 0xFFFFFE00,		\
		start,					\
		size

#define ACTIVE		0x80
#define INACTIVE	0x80

#define FAT16LBA	0x0E
#define LINUX		0x83

.global __magic_word
__magic_word:
	.word	MAGIC_WORD1
	.word	MAGIC_WORD2
	.word	0
	.short	0

	// Partition 1
	PARTATION(ACTIVE, FAT16LBA, 32768, 524288)
	PARTATION(ACTIVE, LINUX, 557056, 14712832)

	.space	(0x1FE - 0x1B0) - (. - __magic_word)

	// Boot signature
	.byte	0x55, 0xAA
.global __end_magic
__end_magic:
