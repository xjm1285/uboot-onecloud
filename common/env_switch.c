#include <common.h>
#include <environment.h>
#include <malloc.h>
#include <spi_flash.h>
#include <search.h>
#include <errno.h>
#include <command.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <nand.h>
#include <asm/arch/nand.h>
#include <linux/err.h>
#include <asm/arch/poc.h>
#include<partition_table.h>
#if defined(CONFIG_STORE_COMPATIBLE) || defined (CONFIG_SPI_NAND_COMPATIBLE) || defined (CONFIG_SPI_NAND_EMMC_COMPATIBLE)

DECLARE_GLOBAL_DATA_PTR;

/* references to names in env_common.c */
extern uchar default_environment[];

uchar env_get_char_spec(int index)
{
	return *((uchar *)(gd->env_addr + index));
}

#ifdef ENV_IS_EMBEDDED
extern uchar environment[];
env_t *env_ptr = (env_t *)(&environment[0]);
#else /* ! ENV_IS_EMBEDDED */
env_t *env_ptr = 0;
#endif /* ENV_IS_EMBEDDED */

char * env_name_spec;

extern void mdelay(unsigned long msec);

int env_init(void)
{
	//printk("env_init %s %d\n",__func__,__LINE__);
	gd->env_addr  = (ulong)&default_environment[0];
	gd->env_valid = 1;
	return 0;
}

void env_relocate_spec(void)
{
#ifdef CONFIG_CMD_NAND
#ifdef CONFIG_STORE_COMPATIBLE
	if(device_boot_flag == NAND_BOOT_FLAG) {
#else
	if(POR_NAND_BOOT()) {
#endif
		printk("NAND BOOT,nand_env_relocate_spec : %s %d \n",__func__,__LINE__);
		env_name_spec = "NAND";
		extern void nand_env_relocate_spec(void);
		nand_env_relocate_spec();
		return;
	}
#endif // CONFIG_CMD_NAND

#ifdef CONFIG_CMD_SF
#ifdef CONFIG_STORE_COMPATIBLE
	if((device_boot_flag == SPI_NAND_FLAG) || (device_boot_flag == SPI_EMMC_FLAG)) {
#else
	if(POR_SPI_BOOT()){
#endif
		printk("SPI BOOT,spi_env_relocate_spec : %s %d \n",__func__,__LINE__);
		env_name_spec = "SPI Flash";
		extern void spi_env_relocate_spec(void);
		spi_env_relocate_spec();
		return;
	}
#endif // CONFIG_CMD_SF

#ifdef CONFIG_CMD_MMC
#ifdef CONFIG_STORE_COMPATIBLE
	if(device_boot_flag == EMMC_BOOT_FLAG) {
#else
	if(POR_EMMC_BOOT()){
#endif
		printk("MMC BOOT, emmc_env_relocate_spec : %s %d \n",__func__,__LINE__);
		env_name_spec = "eMMC";
		extern void emmc_env_relocate_spec(void);
		emmc_env_relocate_spec();
		return;
	}
#endif // CONFIG_CMD_MMC

	printk("BOOT FROM CARD? env_relocate_spec\n");

#ifdef CONFIG_CMD_SF
	if(!run_command("sf probe 2", 0)) {
		printk("SPI BOOT, spi_env_relocate_spec %s %d \n",__func__,__LINE__);
		env_name_spec = "SPI Flash";
		extern void spi_env_relocate_spec(void);
		spi_env_relocate_spec();
		return;
	}
#endif // CONFIG_CMD_SF

#ifdef CONFIG_CMD_NAND
	if(!run_command("nand exist", 0)) {
		printk("NAND BOOT, nand_env_relocate_spec %s %d \n",__func__,__LINE__);
		env_name_spec = "NAND";
		extern void nand_env_relocate_spec(void);
		nand_env_relocate_spec();
		return;
	}
#endif // CONFIG_CMD_NAND

#ifdef CONFIG_CMD_MMC
	if(!run_command("mmcinfo 1", 0)){
		printk("MMC BOOT, emmc_env_relocate_spec %s %d \n",__func__,__LINE__);
		env_name_spec = "eMMC";
		extern void emmc_env_relocate_spec(void);
		emmc_env_relocate_spec();
		return;
	}
#endif // CONFIG_CMD_MMC

	env_name_spec = "None";
	set_default_env("error init device");
}

int saveenv(void)
{
#ifdef CONFIG_CMD_NAND
#ifdef CONFIG_STORE_COMPATIBLE
	if(device_boot_flag == NAND_BOOT_FLAG) {
#else
	if(POR_NAND_BOOT()) {
#endif
		printk("NAND BOOT,nand_saveenv :%s %d \n",__func__,__LINE__);
		extern int nand_saveenv(void);
		return nand_saveenv();
	}
#endif // CONFIG_CMD_NAND

#ifdef CONFIG_CMD_SF
#ifdef CONFIG_STORE_COMPATIBLE
	if((device_boot_flag == SPI_EMMC_FLAG) || (device_boot_flag == SPI_NAND_FLAG)){
#else
	if(POR_SPI_BOOT()){
#endif
		printk("SPI BOOT,spi_saveenv : %s %d \n",__func__,__LINE__);
		extern int spi_saveenv(void);
		return spi_saveenv();
	}
#endif // CONFIG_CMD_SF

#ifdef CONFIG_CMD_MMC
#ifdef CONFIG_STORE_COMPATIBLE
	if(device_boot_flag == EMMC_BOOT_FLAG) {
#else
	if(POR_EMMC_BOOT()){
#endif
		printk("MMC BOOT,emmc_saveenv : %s %d \n",__func__,__LINE__);
		extern int emmc_saveenv(void);
		return emmc_saveenv();
	}
#endif // CONFIG_CMD_MMC

#ifdef CONFIG_STORE_COMPATIBLE
	if (device_boot_flag == CARD_BOOT_FLAG){
#else
	if(POR_CARD_BOOT()){
#endif
		printk("BOOT FROM CARD?\n");

#ifdef CONFIG_CMD_SF
		if(!run_command("sf probe 2", 0)){
			printk("SPI BOOT, spi_saveenv %s %d \n",__func__,__LINE__);
			env_name_spec = "SPI Flash";
			extern int spi_saveenv(void);
			return spi_saveenv();
		}
#endif // CONFIG_CMD_SF
		
#ifdef CONFIG_CMD_NAND
		if(!run_command("nand exist", 0)){
			printk("NAND BOOT, nand_saveenv %s %d \n",__func__,__LINE__);
			env_name_spec = "NAND";
			extern int nand_saveenv(void);
			return nand_saveenv();
		}
#endif // CONFIG_CMD_NAND

#ifdef CONFIG_CMD_MMC
		if(!run_command("mmcinfo 1", 0)){
			printk("MMC BOOT, emmc_saveenv %s %d \n",__func__,__LINE__);
			env_name_spec = "eMMC";
			extern int emmc_saveenv(void);
			return emmc_saveenv();
		}
#endif // CONFIG_CMD_MMC
	}
	
	printk("error init devices, saveenv fail\n");
	return -1;
}

#endif


