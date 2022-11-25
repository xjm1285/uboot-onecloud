#include <common.h>

#include <asm/arch/aml_eth_reg.h>
#include <asm/arch/aml_eth_pinmux.h>
#include <asm/arch/io.h>

#ifdef CONFIG_CMD_NET

static void onecloud_eth_init(void)
{
	// clock(125m/25m/2.5m)
	SET_CBUS_REG_MASK(HHI_GEN_CLK_CNTL,
		(3<<0) // CLK_DIV: 3+1=4
		| (1<<11) // CLK_EN
		| (11<<12) // CLK_SEL: Probably fclk_div5?
		);
	SET_CBUS_REG_MASK(HHI_MPLL_CNTL6, 1<<27); // CKEN[4]: MPLL output DIV2 enable?
	SET_CBUS_REG_MASK(HHI_MPLL_CNTL9, // 2550/(5+1638/16384)=500Mhz
		(1638<<0) // sdm_in
		| (1<<14) // en_dds2
		| (1<<15) // sdn_en2
		| (5<<16) // n_in2
		| (0<<25) // lp_en2
		| (0<<26) // ir_byin2
		| (0<<30) // modlsel2
		| (0<<31) // ir_bypass2
		);

	// pin mux
	SET_CBUS_REG_MASK(PERIPHS_PIN_MUX_6,
		(1<<13) // ETH_TXD3
		| (1<<12) // ETH_TXD2
		| (1<<11) // ETH_TX_CLK

		| (1<<0) // DIF_0_P: ETH_RXD1
		| (1<<1) // DIF_0_N: ETH_RXD0
		| (1<<2) // DIF_1_P: ETH_RX_DV
		| (1<<3) // DIF_1_N: ETH_RX_CLK
		| (0<<4) // not DIF_2_P: ETH_TXD0
		| (0<<5) // not DIF_2_N: ETH_TXD1
		| (1<<6) // DIF_3_P: ETH_TX_EN
		| (1<<8) // DIF_3_N: ETH_PHY_REF_CLK
		| (1<<9) // DIF_4_P: ETH_MDC
		| (1<<10) // DIF_4_N: ETH_MDIO
		);
	SET_CBUS_REG_MASK(PERIPHS_PIN_MUX_7, 0
		| (1<<23) // Probably GPIOH_3: HDMI_CEC
		| (1<<22) // Probably GPIOH_4:
		| (1<<21) // GPIOH_5: ETH_TXD1
		| (1<<20) // GPIOH_6: ETH_TXD0
		);

	// mode
	eth_aml_reg0_t eth_reg0;
	eth_reg0.d32 = 0;
	eth_reg0.b.phy_intf_sel = 1; // RGMII
	eth_reg0.b.data_endian = 0; // transfer data in little-endian
	eth_reg0.b.desc_endian = 0; // transfer descriptor in the same endian as the data
	eth_reg0.b.rx_clk_rmii_invert = 0; // For RGMII, use RX_CLK
	eth_reg0.b.rgmii_tx_clk_src = 1; // choose fclk_div2(unset) or mpll2(set)
	eth_reg0.b.rgmii_tx_clk_phase = 1; // TX_CLK delay 1/4 cycle
	eth_reg0.b.rgmii_tx_clk_ratio = 2; // div: 500MHz / 250MHz = 2
	eth_reg0.b.phy_ref_clk_enable = 1; // Generate 25MHz clock for PHY
	eth_reg0.b.clk_rmii_i_invert = 0; //RMII only. Use inverted clk_rmii_i to generate 25/2.5 tx_rx_clk
	eth_reg0.b.clk_en = 1; // Enable TX_CLK and PHY_REF_CLK generator
	eth_reg0.b.adj_enable = 1; // Enable data delay adjustment and calibration logic
	eth_reg0.b.adj_setup = 1; // auto delay and skew the RXDV and RXD setup time
	eth_reg0.b.adj_delay = 0; // Capture input data at this clock index
	eth_reg0.b.adj_skew = 0; // 5 bits correspondent to {RXDV, RXD[3:0]}
	eth_reg0.b.cali_start = 0; // Not to start calibration logic
	eth_reg0.b.cali_rise = 0; // test fallng edge
	eth_reg0.b.cali_sel = 0; // Select one signal from {RXDV, RXD[3:0]} to calibrate
	eth_reg0.b.rgmii_rx_reuse = 0; // Not to use RX_CLK as TX_CLK
	eth_reg0.b.eth_urgent = 0; // Not to set AHB to DDR interface as urgent
	WRITE_CBUS_REG(PREG_ETHERNET_ADDR0, eth_reg0.d32);// rgmii mode

	// [3:2]: Ethernet Memory PD
	//   0x3 = power off
	//   0x0 = normal operation
	// Clearing mask 0x3 means set to 0x0.
	CLEAR_CBUS_REG_MASK(HHI_MEM_PD_REG0, (0x3 << 2));

	// hardware reset ethernet phy
	// gpioh_4 connect phyreset pin
	CLEAR_CBUS_REG_MASK(PREG_PAD_GPIO3_EN_N, 1 << 23);
	CLEAR_CBUS_REG_MASK(PREG_PAD_GPIO3_O, 1 << 23);
	udelay(2000);
	SET_CBUS_REG_MASK(PREG_PAD_GPIO3_O, 1 << 23);
}

int board_eth_init(bd_t *bis)
{
	onecloud_eth_init();
	udelay(1000);

	extern int aml_eth_init(bd_t *bis);
	aml_eth_init(bis);

	return 0;
}

#endif /* (CONFIG_CMD_NET) */
