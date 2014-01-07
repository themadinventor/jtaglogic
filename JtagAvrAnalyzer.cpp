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

#include "JtagAvrAnalyzer.h"
#include <cstring>
#include <cstdio>

static const char *AvrOCDRegs[] = {
	"PSB0",
	"PSB1",
	"PDMSB",
	"PDSB",
	"$4", "$5", "$6", "$7",
	"BCR",
	"BSR",
	"$A", "$B",
	"OCDR",
	"CSR",
	"$E", "$F"
};

JtagAvrAnalyzer::JtagAvrAnalyzer(JtagAnalyzerSettings *settings)
	: mSettings(settings), mLastInstruction(0), mInstructionStart(0),
	mLastOCDOp(0)
{
}

void JtagAvrAnalyzer::process(enum JtagState state, U64 in, U64 out, U32 bits,
		U64 begin, U64 end, JtagAnalyzerResults *results)
{
	in = JtagAnalyzer::FlipWord(in, bits);
	out = JtagAnalyzer::FlipWord(out, bits);

	if (state == JtagShiftIR) {
		mLastInstruction = in;
		mInstructionStart = end;

		if (mLastInstruction == AvrForceBreak || mLastInstruction == AvrRun) {
			Frame result_frame;
			result_frame.mStartingSampleInclusive = begin;
			result_frame.mEndingSampleInclusive = end;
			result_frame.mData1 = 0;
			result_frame.mData2 = 0;
			result_frame.mFlags = mLastInstruction;

			results->AddFrame(result_frame);
			results->CommitResults();
		}
	} else if (state == JtagShiftDR) {
		Frame result_frame;
		result_frame.mStartingSampleInclusive = mInstructionStart;
		result_frame.mEndingSampleInclusive = end;
		result_frame.mData1 = in;
		result_frame.mData2 = out;
		result_frame.mFlags = mLastInstruction;

		if (mLastInstruction == AvrOCD && !(in & 0x100000)) {
			result_frame.mData1 = mLastOCDOp << 16;
		}

		mInstructionStart = end;

		if (mLastInstruction == AvrOCD && bits == 5) {
			mLastOCDOp = in;
			return;
		}

		results->AddFrame(result_frame);
		results->CommitResults();
	}
} 

void JtagAvrAnalyzer::Disassemble(U64 instr, char *buf)
{
	if ((instr & 0xffff0000) == 0xffff0000) {
		strcpy(buf, "PCReadout?");
	} else {
		if ((instr & 0xf000) == 0xe000) { // LDI Rd, K
			sprintf(buf, "LDI r%d, $%02x", 16+((instr & 0x00f0) >> 4), (instr & 0xf) | ((instr & 0xf00) >> 4));
		} else if ((instr & 0xfc00) == 0x2c00) { // MOV Rd, Rr
			sprintf(buf, "MOV r%d, r%d", ((instr & 0x01f0) >> 4), (instr & 0xf) | ((instr & 0x200) >> 5));
		} else if ((instr & 0xf800) == 0xb800) { // OUT A, Rr
			sprintf(buf, "OUT $%02x, r%d", (instr & 0xf) | ((instr & 0x600) >> 5), (instr & 0x01f0) >> 4);
		} else if ((instr & 0xf800) == 0xb000) { // IN Rd, A
			sprintf(buf, "IN r%d, $%02x", (instr & 0x01f0) >> 4, (instr & 0xf) | ((instr & 0x600) >> 5));
		} else if (instr == 0x9409) { // IJMP
			strcpy(buf, "IJMP");
		} else if (instr == 0x9598) { // BREAK
			strcpy(buf, "BREAK");
		} else if (instr == 0x95a8) { // WDR
			strcpy(buf, "WDR");
		} else {
			sprintf(buf, "%04x", instr);
		}
	}
}

void JtagAvrAnalyzer::generateBubbleText(Frame &frame, Channel &channel,
		DisplayBase display_base, char *str)
{
	char disasm[32];

	AvrInstruction instr = static_cast<AvrInstruction>(frame.mFlags & 0xf);

	if (channel == mSettings->mTDIChannel) {
		switch (instr) {
		case AvrProgEnable:
			sprintf(str, "ProgEnable %08x", frame.mData1);
			break;

		case AvrProgCmd:
			sprintf(str, "ProgCmd %08x", frame.mData1);
			break;

		case AvrPageLoad:
			sprintf(str, "PageLoad %08x", frame.mData1);
			break;

		case AvrForceBreak:
			strcpy(str, "ForceBreak");
			break;

		case AvrRun:
			strcpy(str, "Run");
			break;

		case AvrExecute:
			Disassemble(frame.mData1, disasm);
			sprintf(str, "Ex %s", disasm);
			break;

		case AvrReset:
			if (frame.mData1) {
				strcpy(str, "EnterReset");
			} else {
				strcpy(str, "LeaveReset");
			}
			break;

		case AvrOCD:
			if (frame.mData1 & 0x100000) {
				sprintf(str, "OCD Write %s = %04x", AvrOCDRegs[(frame.mData1 >> 16) & 0xf], frame.mData1 & 0xffff);
			}
			break;
		}
	} else if (channel == mSettings->mTDOChannel) {
		switch (instr) {
		case AvrIdCode:
			sprintf(str, "IdCode %08x", frame.mData2);
			break;

		case AvrPageRead:
			sprintf(str, "PageRead %08x", frame.mData2);
			break;

		case AvrExecute:
			sprintf(str, "PC = %04x", frame.mData2);
			break;

		case AvrOCD:
			if (!(frame.mData1 & 0x100000)) {
				sprintf(str, "OCD Read %s = %04x", AvrOCDRegs[(frame.mData1 >> 16) & 0xf], frame.mData2 & 0xffff);
			}
			break;
		}
	}
}

