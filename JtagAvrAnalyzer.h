/*
 * jtaglogic
 *
 * Copyright (C) 2013-2014 Fredrik Ahlberg
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef JTAG_AVR_ANALYZER_H
#define JTAG_AVR_ANALYZER_H

#include "JtagAnalyzer.h"
#include "JtagAnalyzerResults.h"
#include "JtagAnalyzerSettings.h"

enum AvrInstruction {
	AvrExtest	= 0x01,
	AvrIdCode	= 0x02,

	AvrProgEnable	= 0x04,
	AvrProgCmd	= 0x05,
	AvrPageLoad	= 0x06,
	AvrPageRead	= 0x07,

	AvrForceBreak	= 0x08,
	AvrRun		= 0x09,
	AvrExecute	= 0x0A,
	AvrOCD		= 0x0B,
	AvrReset	= 0x0C,

	AvrBypass	= 0x0f
};

class JtagAvrAnalyzer : public JtagInnerAnalyzer
{
public:
	JtagAvrAnalyzer(JtagAnalyzerSettings *settings);
	void process(enum JtagState state, U64 in, U64 out, U32 bits,
			U64 begin, U64 end,
			JtagAnalyzerResults *results);
	void generateBubbleText(Frame &frame, Channel &channel,
			DisplayBase display_base, char *str);

private:
	static void Disassemble(U64 instr, char *buf);

	JtagAnalyzerSettings *mSettings;

	U32 mLastInstruction, mLastOCDOp;
	U64 mInstructionStart;
};

#endif //JTAG_AVR_ANALYZER_H

