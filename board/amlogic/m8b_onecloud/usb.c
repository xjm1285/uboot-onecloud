#include <common.h>

#include <asm/arch/usb.h>
#include <asm/arch/gpio.h>

#ifdef CONFIG_USB_DWC_OTG_HCD

struct amlogic_usb_config g_usb_a_config={
	USB_PHY_CLK_SEL_XTAL,
	1,			// PLL divider: (clock/12-1)
	0xC9040000,	// Base
	USB_ID_MODE_SW_HOST,
	NULL,		// set_vbus_power
	NULL,		// battery_charging_det_cb
};

struct amlogic_usb_config g_usb_b_config={
	USB_PHY_CLK_SEL_XTAL,
	1,			// PLL divider: (clock/12-1)
	0xC90C0000,	// Base
	USB_ID_MODE_SW_HOST,
	NULL,		// set_vbus_power
	NULL,		// battery_charging_det_cb
};

int onecloud_usb_init(void)
{
	board_usb_init(&g_usb_a_config, BOARD_USB_MODE_HOST);
	board_usb_init(&g_usb_b_config, BOARD_USB_MODE_HOST);
	return 0;
}

#endif /*CONFIG_USB_DWC_OTG_HCD*/
