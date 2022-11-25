#include <common.h>

#include <asm/arch/io.h>
#include <amlogic/aml_tv.h>
#include <amlogic/vinfo.h>

vidinfo_t tv_info = {};

static void tv_power_on(void)
{
	printf("%s\n", __FUNCTION__);
}

static void tv_power_off(void)
{
	printf("%s\n", __FUNCTION__);
}

static void tv_enable(void)
{
	printf("%s\n", __FUNCTION__);

	tv_info.vd_base = (void*)simple_strtoul(getenv("fb_addr"), NULL, 0);

	tv_info.vl_col = simple_strtoul(getenv("display_width"), NULL, 0);
	tv_info.vl_row = simple_strtoul(getenv("display_height"), NULL, 0);
	tv_info.vl_bpix = simple_strtoul(getenv("display_bpp"), NULL, 10);

	tv_info.vd_color_fg = simple_strtoul(getenv("display_color_fg"), NULL, 0);
	tv_info.vd_color_bg = simple_strtoul(getenv("display_color_bg"), NULL, 0);
}

void tv_disable(void)
{
	printf("%s\n", __FUNCTION__);
}

struct tv_operations tv_oper =
{
	.power_on	 = tv_power_on,
	.power_off	 = tv_power_off,
	.enable		 = tv_enable,
	.disable	 = tv_disable,
};

void hdmi_tx_power_init(void)
{
	printf("%s\n", __FUNCTION__);
}
