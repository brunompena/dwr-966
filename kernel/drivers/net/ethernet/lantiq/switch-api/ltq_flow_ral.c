/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2013 Reddy Mallikarjuna <Reddy.Mallikarjun@lantiq.com>
 */
#if 0
#include "ltq_ethsw_init.h"

/** read the GSWIP register */
u32 ethsw_r32(short Offset, short Shift, short Size, u32 *value)
{
	u32 regValue, regAddr, mask;
	/* Prepare the register address */
	regAddr = GSWITCH_BASE_ADDRESS + (Offset * 4);
	/* Read the Whole 32bit register */
	regValue = GSWITCH_R32_ACCESS(regAddr);
	/* Prepare the mask	*/
	mask = (1 << Size) - 1 ;
	/* Bit shifting to the exract bit field */
	regValue = (regValue >> Shift);
	*value = (regValue & mask);
	return 0;
}

/** read and update the GSWIP register */
int ethsw_w32(short Offset, short Shift, short Size, u32 value)
{
	u32 regValue, regAddr, mask;
	/* Prepare the register address */
	regAddr = GSWITCH_BASE_ADDRESS + (Offset * 4);
	/* Read the Whole 32bit register */
	regValue = GSWITCH_R32_ACCESS(regAddr);
	/* Prepare the mask	*/
	mask = (1 << Size) - 1;
	mask = (mask << Shift);
	/* Shift the value to the right place and mask the rest of the bit*/
	value = (value << Shift) & mask;
	/*  Mask out the bit field and place in the new value */
	value = (regValue & ~mask) | value ;
	/* Write into register */
	GSWITCH_R32_ACCESS(regAddr) = value ;
	return 0;
}
#endif