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

#ifndef JTAG_ANALYZER_H
#define JTAG_ANALYZER_H

#include <Analyzer.h>
#include "JtagAnalyzerResults.h"
#include "JtagSimulationDataGenerator.h"

enum JtagState {
	JtagReset,
	JtagIdle,

	JtagSelectDR,
	JtagCaptureDR,
	JtagShiftDR,
	JtagExit1DR,
	JtagPauseDR,
	JtagExit2DR,
	JtagUpdateDR,

	JtagSelectIR,
	JtagCaptureIR,
	JtagShiftIR,
	JtagExit1IR,
	JtagPauseIR,
	JtagExit2IR,
	JtagUpdateIR
};

enum JtagInner {
	InnerPlain,
	InnerAVR,
	InnerARM
};

class JtagInnerAnalyzer
{
public:
	virtual ~JtagInnerAnalyzer();
	virtual void process(enum JtagState state, U64 in, U64 out, U32 bits,
			U64 begin, U64 end, JtagAnalyzerResults *results) = 0;
	virtual void generateBubbleText(Frame &frame, Channel &channel,
			DisplayBase display_base, char *str) = 0;
};

class JtagAnalyzerSettings;
class JtagAnalyzer : public Analyzer2
{
public:
	JtagAnalyzer();
	virtual ~JtagAnalyzer();
	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

	static U64 FlipWord(U64 word, U32 bits);

protected: //functions
	void Setup();
	void ProcessStep();
	
protected:  //vars
	std::auto_ptr< JtagAnalyzerSettings > mSettings;
	std::auto_ptr< JtagAnalyzerResults > mResults;
	bool mSimulationInitilized;
	JtagSimulationDataGenerator mSimulationDataGenerator;

	AnalyzerChannelData* mTCK; 
	AnalyzerChannelData* mTMS;
	AnalyzerChannelData* mTDI;
	AnalyzerChannelData* mTDO;
	AnalyzerChannelData* mTRST;

	U64 mCurrentSample, mFirstSample;

	JtagInnerAnalyzer *mInnerAnalyzer;

	enum JtagState mState;
	U64 mDataIn, mDataOut;
	U32 mBits;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //JTAG_ANALYZER_H
