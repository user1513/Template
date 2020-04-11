#pragma once
unsigned short ff_convert(	/* Converted code, 0 means conversion error */
	unsigned short	chr,	/* Character code to be converted */
	unsigned int	dir		/* 0: Unicode to OEM code, 1: OEM code to Unicode */
);