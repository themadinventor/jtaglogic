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

#include "JtagPlainAnalyzer.h"
#include <cstring>

static const char *JtagStateStr[] = {
	"Reset",
	"Idle",

	"SelectDR",
	"CaptureDR",
	"ShiftDR",
	"Exit1DR",
	"PauseDR",
	"Exit2DR",
	"UpdateDR",

	"SelectIR",
	"CaptureIR",
	"ShiftIR",
	"Exit1IR",
	"PauseIR",
	"Exit2IR",
	"UpdateIR"
};

JtagPlainAnalyzer::JtagPlainAnalyzer(JtagAnalyzerSettings *settings)
	: mSettings(settings)
{
}

void JtagPlainAnalyzer::process(enum JtagState state, U64 in, U64 out, U32 bits,
		U64 begin, U64 end, JtagAnalyzerResults *results)
{
	Frame result_frame;
	result_frame.mStartingSampleInclusive = begin;
	result_frame.mEndingSampleInclusive = end;
	result_frame.mData1 = mSettings->mShiftOrder == AnalyzerEnums::MsbFirst ? JtagAnalyzer::FlipWord(in, bits) : in;
	result_frame.mData2 = mSettings->mShiftOrder == AnalyzerEnums::MsbFirst ? JtagAnalyzer::FlipWord(out, bits) : out;
	result_frame.mFlags = state;

	results->AddFrame(result_frame);
	results->CommitResults();
} 

void JtagPlainAnalyzer::generateBubbleText(Frame &frame, Channel &channel,
		DisplayBase display_base, char *str)
{
	if ((frame.mFlags & 0xf) == JtagShiftDR || (frame.mFlags & 0xf) == JtagShiftIR) {
		if (channel == mSettings->mTDIChannel) {
			AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 0, str, 128);
		} else if (channel == mSettings->mTDOChannel) {
			AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 0, str, 128);
		}
	}

	if (channel == mSettings->mTMSChannel) {
		strcpy(str, JtagStateStr[frame.mFlags & 0x0f]);
	}
}
