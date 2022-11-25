#include <asm/arch/storage.h>

#ifdef CONFIG_ACS

//partition tables
struct partitions partition_table[]={
	{
		.name = "resource",
		.size = 4 * SZ_1M,
		.mask_flags = STORE_DATA,
	},
	{
		.name = "boot",
		.size = 256 * SZ_1M,
		.mask_flags = STORE_DATA,
	},
	{
		.name = "rootfs",
		.size = NAND_PART_SIZE_FULL,
		.mask_flags = STORE_DATA,
	},
};

struct store_config store_configs ={
	.store_device_flag = EMMC_BOOT_FLAG,
	.mmc_configs = {
		.type = ((PORT_B_CARD_TYPE << 4) | (PORT_C_CARD_TYPE << 8)),
		.port = 0,
	},
};

#endif
