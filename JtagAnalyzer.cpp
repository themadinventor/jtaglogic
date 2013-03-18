#include "JtagAnalyzer.h"
#include "JtagAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
 
JtagAnalyzer::JtagAnalyzer()
:	Analyzer2(),
	mSettings(new JtagAnalyzerSettings()),
	mSimulationInitilized(false),
	mTCK(NULL),
	mTMS(NULL),
	mTDI(NULL),
	mTDO(NULL),
    mTRST(NULL)
{	
	SetAnalyzerSettings(mSettings.get());
}

JtagAnalyzer::~JtagAnalyzer()
{
	KillThread();
}

void JtagAnalyzer::SetupResults()
{
	mResults.reset(new JtagAnalyzerResults(this, mSettings.get()));
	SetAnalyzerResults(mResults.get());

	mResults->AddChannelBubblesWillAppearOn(mSettings->mTMSChannel);
	mResults->AddChannelBubblesWillAppearOn(mSettings->mTDIChannel);
	mResults->AddChannelBubblesWillAppearOn(mSettings->mTDOChannel);
}

void JtagAnalyzer::WorkerThread()
{
	Setup();

    mState = JtagReset;
    mFirstSample = mTCK->GetSampleNumber();
    mDataIn = mDataOut = 0;
    mBits = 0;

    if (mTCK->GetBitState() != 0) {
        mTCK->AdvanceToNextEdge();
    }

	for ( ; ; ) {
		mTCK->AdvanceToNextEdge();
		mCurrentSample = mTCK->GetSampleNumber();

        ProcessStep();

		mTCK->AdvanceToNextEdge();

		CheckIfThreadShouldExit();
	}
}

void JtagAnalyzer::Setup()
{
	bool allow_last_trailing_clock_edge_to_fall_outside_enable = false;

	mTCK = GetAnalyzerChannelData(mSettings->mTCKChannel);
	mTMS = GetAnalyzerChannelData(mSettings->mTMSChannel);
	mTDI = GetAnalyzerChannelData(mSettings->mTDIChannel);
	mTDO = GetAnalyzerChannelData(mSettings->mTDOChannel);

	if (mSettings->mTRSTChannel != UNDEFINED_CHANNEL) {
		mTRST = GetAnalyzerChannelData(mSettings->mTRSTChannel);
    } else {
		mTRST = NULL;
    }

}

void JtagAnalyzer::ProcessStep()
{
    mTMS->AdvanceToAbsPosition(mCurrentSample);
    mTDI->AdvanceToAbsPosition(mCurrentSample);
    mTDO->AdvanceToAbsPosition(mCurrentSample);

	mResults->AddMarker(mCurrentSample, AnalyzerResults::UpArrow, mSettings->mTCKChannel);

    // Fetch data
    if (mState == JtagShiftDR || mState == JtagShiftIR) {
        mDataIn = (mDataIn << 1) | (mTDI->GetBitState() ? 1 : 0);
        mDataOut = (mDataOut << 1) | (mTDO->GetBitState() ? 1 : 0);
        mBits++;

	    mResults->AddMarker(mCurrentSample, AnalyzerResults::Dot, mSettings->mTDIChannel);
	    mResults->AddMarker(mCurrentSample, AnalyzerResults::Dot, mSettings->mTDOChannel);
    }
    
    // Update JTAG state machine
    enum JtagState next_state = mState;
    bool tms = mTMS->GetBitState();

    switch (mState) {
        case JtagReset:
            next_state = tms ? JtagReset : JtagIdle;
            break;

        case JtagIdle:
            next_state = tms ? JtagSelectDR : JtagIdle;
            break;

        case JtagSelectDR:
            next_state = tms ? JtagSelectIR : JtagCaptureDR;
            break;

        case JtagCaptureDR:
            next_state = tms ? JtagExit1DR : JtagShiftDR;
            break;

        case JtagShiftDR:
            next_state = tms ? JtagExit1DR : JtagShiftDR;
            break;

        case JtagExit1DR:
            next_state = tms ? JtagUpdateDR : JtagPauseDR;
            break;

        case JtagPauseDR:
            next_state = tms ? JtagExit2DR : JtagPauseDR;
            break;

        case JtagExit2DR:
            next_state = tms ? JtagUpdateDR : JtagShiftDR;
            break;

        case JtagUpdateDR:
            next_state = tms ? JtagSelectDR : JtagIdle;
            break;

        case JtagSelectIR:
            next_state = tms ? JtagReset : JtagCaptureIR;
            break;

        case JtagCaptureIR:
            next_state = tms ? JtagExit1IR : JtagShiftIR;
            break;

        case JtagShiftIR:
            next_state = tms ? JtagExit1IR : JtagShiftIR;
            break;

        case JtagExit1IR:
            next_state = tms ? JtagUpdateIR : JtagPauseIR;
            break;

        case JtagPauseIR:
            next_state = tms ? JtagExit2IR : JtagPauseIR;
            break;

        case JtagExit2IR:
            next_state = tms ? JtagUpdateIR : JtagShiftIR;
            break;

        case JtagUpdateIR:
            next_state = tms ? JtagSelectDR : JtagIdle;
            break;
    }

    if (next_state != mState) {
        // transition. yay. flush the current transaction.

        Frame result_frame;
        result_frame.mStartingSampleInclusive = mFirstSample;
        result_frame.mEndingSampleInclusive = mTCK->GetSampleNumber();
        result_frame.mData1 = FlipWord(mDataIn, mBits);
        result_frame.mData2 = FlipWord(mDataOut, mBits);
        result_frame.mFlags = mState;

        U64 frame_id = mResults->AddFrame(result_frame);
        mResults->CommitResults();

        mFirstSample = mTCK->GetSampleNumber();
        mState = next_state;
        mDataIn = mDataOut = 0;
        mBits = 0;
    }
}

U64 JtagAnalyzer::FlipWord(U64 word, U32 bits)
{
    U64 result = 0;

    for (int idx=0; idx<bits; idx++) {
        if (word & (1 << idx)) {
            result |= (1 << (bits-idx-1));
        }
    }

    return result;
}

bool JtagAnalyzer::NeedsRerun()
{
	return false;
}

U32 JtagAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if (mSimulationInitilized == false) {
		mSimulationDataGenerator.Initialize(GetSimulationSampleRate(), mSettings.get());
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData(minimum_sample_index, device_sample_rate, simulation_channels);
}


U32 JtagAnalyzer::GetMinimumSampleRateHz()
{
	return 10000; //we don't have any idea, depends on the JTAG TCK rate, etc.; return the lowest rate.
}

const char* JtagAnalyzer::GetAnalyzerName() const
{
	return "JTAG";
}

const char* GetAnalyzerName()
{
	return "JTAG";
}

Analyzer* CreateAnalyzer()
{
	return new JtagAnalyzer();
}

void DestroyAnalyzer(Analyzer* analyzer)
{
	delete analyzer;
}