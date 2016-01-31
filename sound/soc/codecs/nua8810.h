/*
 * nua8810.h  --  NUA8810 Soc Audio driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _NUA8810_H
#define _NUA8810_H

/* NUA8810 register space */

#define NUA8810_RESET			0x0
#define NUA8810_POWER1			0x1
#define NUA8810_POWER2			0x2
#define NUA8810_POWER3			0x3
#define NUA8810_IFACE			0x4
#define NUA8810_COMP			0x5
#define NUA8810_CLOCK			0x6
#define NUA8810_ADD			0x7
#define NUA8810_GPIO			0x8
#define NUA8810_DAC			0xa
#define NUA8810_DACVOL			0xb
#define NUA8810_ADC			0xe
#define NUA8810_ADCVOL			0xf
#define NUA8810_EQ1			0x12
#define NUA8810_EQ2			0x13
#define NUA8810_EQ3			0x14
#define NUA8810_EQ4			0x15
#define NUA8810_EQ5			0x16
#define NUA8810_DACLIM1			0x18
#define NUA8810_DACLIM2			0x19
#define NUA8810_NOTCH1			0x1b
#define NUA8810_NOTCH2			0x1c
#define NUA8810_NOTCH3			0x1d
#define NUA8810_NOTCH4			0x1e
#define NUA8810_ALC1			0x20
#define NUA8810_ALC2			0x21
#define NUA8810_ALC3			0x22
#define NUA8810_NGATE			0x23
#define NUA8810_PLLN			0x24
#define NUA8810_PLLK1			0x25
#define NUA8810_PLLK2			0x26
#define NUA8810_PLLK3			0x27
#define NUA8810_ATTEN			0x28
#define NUA8810_INPUT			0x2c
#define NUA8810_INPPGA			0x2d
#define NUA8810_ADCBOOST		0x2f
#define NUA8810_OUTPUT			0x31
#define NUA8810_SPKMIX			0x32
#define NUA8810_SPKVOL			0x36
#define NUA8810_MONOMIX			0x38

#define NUA8810_CACHEREGNUM 		57

/* Clock divider Id's */
#define NUA8810_OPCLKDIV		0
#define NUA8810_MCLKDIV			1
#define NUA8810_ADCCLK			2
#define NUA8810_DACCLK			3
#define NUA8810_BCLKDIV			4

/* DAC clock dividers */
#define NUA8810_DACCLK_F2		(1 << 3)
#define NUA8810_DACCLK_F4		(0 << 3)

/* ADC clock dividers */
#define NUA8810_ADCCLK_F2		(1 << 3)
#define NUA8810_ADCCLK_F4		(0 << 3)

/* PLL Out dividers */
#define NUA8810_OPCLKDIV_1		(0 << 4)
#define NUA8810_OPCLKDIV_2		(1 << 4)
#define NUA8810_OPCLKDIV_3		(2 << 4)
#define NUA8810_OPCLKDIV_4		(3 << 4)

/* BCLK clock dividers */
#define NUA8810_BCLKDIV_1		(0 << 2)
#define NUA8810_BCLKDIV_2		(1 << 2)
#define NUA8810_BCLKDIV_4		(2 << 2)
#define NUA8810_BCLKDIV_8		(3 << 2)
#define NUA8810_BCLKDIV_16		(4 << 2)
#define NUA8810_BCLKDIV_32		(5 << 2)

/* MCLK clock dividers */
#define NUA8810_MCLKDIV_1		(0 << 5)
#define NUA8810_MCLKDIV_1_5		(1 << 5)
#define NUA8810_MCLKDIV_2		(2 << 5)
#define NUA8810_MCLKDIV_3		(3 << 5)
#define NUA8810_MCLKDIV_4		(4 << 5)
#define NUA8810_MCLKDIV_6		(5 << 5)
#define NUA8810_MCLKDIV_8		(6 << 5)
#define NUA8810_MCLKDIV_12		(7 << 5)

struct nua8810_setup_data {
	int spi;
	int i2c_bus;
	unsigned short i2c_address;
};

#endif
