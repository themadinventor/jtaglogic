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

#ifndef JTAG_ARM_ANALYZER_H
#define JTAG_ARM_ANALYZER_H

#include "JtagAnalyzer.h"
#include "JtagAnalyzerResults.h"
#include "JtagAnalyzerSettings.h"

enum ArmInstruction {
	ArmABORT	= 0x08,
	ArmDPACC	= 0x0A,
	ArmAPACC	= 0x0B,
	ArmIDCODE	= 0x0E,
	ArmBYPASS	= 0x0F
};

enum ArmDAPStatus {
	StatusOkFault	= 0x02,
	StatusWait	= 0x01
};

class JtagArmAnalyzer : public JtagInnerAnalyzer
{
public:
	JtagArmAnalyzer(JtagAnalyzerSettings *settings);
	void process(enum JtagState state, U64 in, U64 out, U32 bits,
			U64 begin, U64 end,
			JtagAnalyzerResults *results);
	void generateBubbleText(Frame &frame, Channel &channel,
			DisplayBase display_base, char *str);

private:
	JtagAnalyzerSettings *mSettings;

	U32 mLastInstruction, mAPBank;
};

#endif //JTAG_ARM_ANALYZER_H

