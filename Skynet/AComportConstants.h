#ifndef _ACOMPORTCONSTANTS_H_
#define _ACOMPORTCONSTANTS_H_

// Data bits

#define asDATABITS_DEFAULT						 0	// Use default setting
#define asDATABITS_7							 7  // 7 Databits
#define asDATABITS_8							 8  // 8 Databits

// Parity codes

#define asPARITY_DEFAULT						 0  // Use default setting
#define asPARITY_NONE							 1	// No parity bits
#define asPARITY_ODD							 2	// Odd parity
#define asPARITY_EVEN							 3	// Even parity bits
#define asPARITY_MARK							 4	// Mark parity
#define asPARITY_SPACE							 5	// Space parity

// Stopbit codes

#define asSTOPBITS_DEFAULT						 0  // Use default setting
#define asSTOPBITS_1							 1	// 1 bit stop bit (default)
#define asSTOPBITS_2							 2	// 2 bit stop bit
#define asSTOPBITS_15							15	// 1.5 bit parity

// Flow control

#define asFLOWCONTROL_DEFAULT					 0	// Use default setting
#define asFLOWCONTROL_DISABLE					 1  // Disable flow control
#define asFLOWCONTROL_ENABLE					 2	// Enable flow control

#endif	// _ACOMPORTCONSTANTS_H_