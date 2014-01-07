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

#include "JtagArmAnalyzer.h"
#include <cstring>
#include <cstdio>

static const char *DPRegs[] = {"DP0 (invalid)","CTRL/STAT","SELECT","RDBUFF"};
static const char *APRegs[] = {"CSW","TAR","AP08","DRW","BD0","BD1","BD2","BD3"};

JtagArmAnalyzer::JtagArmAnalyzer(JtagAnalyzerSettings *settings)
	: mSettings(settings), mLastInstruction(0), mAPBank(0)
{
}

void JtagArmAnalyzer::process(enum JtagState state, U64 in, U64 out, U32 bits,
		U64 begin, U64 end, JtagAnalyzerResults *results)
{
	in = JtagAnalyzer::FlipWord(in, bits);
	out = JtagAnalyzer::FlipWord(out, bits);

	if (state == JtagShiftIR) {
		mLastInstruction = in & 0x0f;

		if (mLastInstruction == ArmBYPASS) {
			Frame result_frame;
			result_frame.mStartingSampleInclusive = begin;
			result_frame.mEndingSampleInclusive = end;
			result_frame.mData1 = 0;
			result_frame.mData2 = 0;
			result_frame.mFlags = mAPBank | mLastInstruction;

			results->AddFrame(result_frame);
			results->CommitResults();
		}
	} else if (state == JtagShiftDR) {
		Frame result_frame;
		result_frame.mStartingSampleInclusive = begin;
		result_frame.mEndingSampleInclusive = end;
		result_frame.mData1 = in;
		result_frame.mData2 = out;
		result_frame.mFlags = mAPBank | mLastInstruction;

		if (mLastInstruction == ArmDPACC && ((in & 7) == 4)) {
			// Write to DP SELECT
			mAPBank = (in >> 3) & 0xf0;
		}

		results->AddFrame(result_frame);
		results->CommitResults();
	}
} 

void JtagArmAnalyzer::generateBubbleText(Frame &frame, Channel &channel,
		DisplayBase display_base, char *str)
{
	ArmInstruction instr = static_cast<ArmInstruction>(frame.mFlags & 0xf);
	U32 APbank = frame.mFlags & 0xf0;

	if (channel == mSettings->mTDIChannel) {
		switch (instr) {
		case ArmABORT:
			if (frame.mData1 == 0x08) {
				strcpy(str, "ABORT");
			} else {
				sprintf(str, "Invalid ABORT (%08x)", frame.mData1);
			}
			break;

		case ArmDPACC:
			if (frame.mData1 & 1) {
				sprintf(str, "Read %s", DPRegs[(frame.mData1 >> 1) & 3]);
			} else {
				sprintf(str, "Write %s = %08x",
						DPRegs[(frame.mData1 >> 1) & 3], frame.mData1 >> 3);
			}
			break;

		case ArmAPACC:
			{
				U32 APaddr = APbank | ((frame.mData1 << 1) & 0xc);
				if (frame.mData1 & 1) {
					if (APaddr == 0xfc) {
						strcpy(str, "Read IDR");
					} else if (APaddr == 0xf8) {
						strcpy(str, "Read DBGDRAR");
					} else if (APaddr <= 0x1C) {
						sprintf(str, "Read %s", APRegs[APaddr >> 2]);
					} else {
						sprintf(str, "Read AP %x", APaddr);
					}
				} else {
					if (APaddr > 0x1C) {
						sprintf(str, "Write AP %x (%x %x) = %08x",
								APaddr,
								frame.mData1 >> 3);
					} else {
						sprintf(str, "Write %s = %08x",
								APRegs[APaddr >> 2],
								frame.mData1 >> 3);
					}
				}
			}
			break;

		case ArmBYPASS:
			strcpy(str, "BYPASS");
			break;

		default:
			sprintf(str, "Unknown %x", instr);
		}
	} else if (channel == mSettings->mTDOChannel) {
		switch (instr) {
		case ArmIDCODE:
			sprintf(str, "IDCODE %08x", frame.mData2);
			break;

		case ArmDPACC:
		case ArmAPACC:
			if ((frame.mData2 & 3) == StatusOkFault) {
				sprintf(str, "%cP OK %08x",
						instr == ArmDPACC ? 'D' : 'A',
						frame.mData2 >> 3);
			} else if ((frame.mData2 & 3) == StatusWait) {
				sprintf(str, "%cP WAIT",
						instr == ArmDPACC ? 'D' : 'A');
			} else {
				sprintf(str, "%cP Invalid ACK! (%2x)",
						instr == ArmDPACC ? 'D' : 'A',
						frame.mData2 & 3);
			}
			break;
		}
	}
}

