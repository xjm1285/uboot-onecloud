#include <common.h>

#include <asm/arch/sdio.h>
#include <malloc.h>
#include <mmc.h>

#ifdef CONFIG_CMD_MMC

static int onecloud_sdio_init(unsigned port)
{
	switch (port) {
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			SET_CBUS_REG_MASK(PREG_PAD_GPIO0_EN_N, 1 << 28); // CARD_6 input
			break;
		case SDIO_PORT_C:
			// enable pull up CARD_0 - CARD_6
			// CLEAR_CBUS_REG_MASK(PAD_PULL_UP_REG1, 0x7f << 20);
			break;
		case SDIO_PORT_XC_A:
		case SDIO_PORT_XC_B:
		case SDIO_PORT_XC_C:
		default:
			break;
	}

	return cpu_sdio_init(port);
}

static void onecloud_sdio_pwr_prepare(unsigned port)
{
	cpu_sdio_pwr_prepare(port);
}

static void onecloud_sdio_pwr_on(unsigned port)
{
	return;
}

static void onecloud_sdio_pwr_off(unsigned port)
{
	return;
}

static int onecloud_sdio_detect(unsigned port)
{
	switch (port) {
		case SDIO_PORT_A:
			break;
		case SDIO_PORT_B:
			if(READ_CBUS_REG(PREG_PAD_GPIO0_I) & (1 << 28)) // no card
				return 1;

			// SDIO Debug Board
			// https://discourse.coreelec.org/t/coreelec-team-sdio-debug-board/9988
			// PERIPHS_PIN_MUX_8[10]: CARD_4: UART_TX_AO_A
			// PERIPHS_PIN_MUX_8[ 9]: CARD_5: UART_RX_AO_A
			if ((READ_CBUS_REG(PERIPHS_PIN_MUX_8) & (3 << 9))) {
				extern unsigned sdio_debug_1bit_flag;

				if (!(READ_CBUS_REG(PREG_PAD_GPIO0_I) & (1 << 22))) { // CARD_0
					printf("sdio debug board detected, sd card with 1bit mode\n");
					sdio_debug_1bit_flag = 1;
				} else {
					printf("sdio debug board detected, no sd card in\n");
					sdio_debug_1bit_flag = 0;
					return 1;
				}
			}

			return 0;
		case SDIO_PORT_C:
			return 0;
		case SDIO_PORT_XC_A:
		case SDIO_PORT_XC_B:
		case SDIO_PORT_XC_C:
		default:
			break;
	}

	return 1;
}

static void board_mmc_register(unsigned port)
{
	struct aml_card_sd_info *aml_priv = cpu_sdio_get(port);

	struct mmc *mmc = (struct mmc *) malloc(sizeof(struct mmc));
	if (aml_priv == NULL || mmc == NULL)
		return;

	memset(mmc, 0, sizeof(*mmc));
	aml_priv->sdio_init			 = onecloud_sdio_init;
	aml_priv->sdio_pwr_prepare	 = onecloud_sdio_pwr_prepare;
	aml_priv->sdio_pwr_on	 = onecloud_sdio_pwr_on;
	aml_priv->sdio_pwr_off	 = onecloud_sdio_pwr_off;
	aml_priv->sdio_detect		 = onecloud_sdio_detect;

	sdio_register(mmc, aml_priv);
}

int board_mmc_init(bd_t *bis)
{
	board_mmc_register(SDIO_PORT_B);
	board_mmc_register(SDIO_PORT_C);
	return 0;
}

#endif