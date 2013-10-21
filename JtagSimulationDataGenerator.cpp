#include <cstdlib> // for rand

#include "JtagSimulationDataGenerator.h"
#include "JtagAnalyzerSettings.h"

JtagSimulationDataGenerator::JtagSimulationDataGenerator()
{
}

JtagSimulationDataGenerator::~JtagSimulationDataGenerator()
{
}

void JtagSimulationDataGenerator::Initialize(U32 simulation_sample_rate, JtagAnalyzerSettings* settings)
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mClockGenerator.Init(simulation_sample_rate / 10, simulation_sample_rate);

	mTCK = mJtagSimulationChannels.Add(settings->mTCKChannel, mSimulationSampleRateHz, BIT_LOW);
	mTMS = mJtagSimulationChannels.Add(settings->mTMSChannel, mSimulationSampleRateHz, BIT_LOW);
	mTDI = mJtagSimulationChannels.Add(settings->mTDIChannel, mSimulationSampleRateHz, BIT_LOW);
	mTDO = mJtagSimulationChannels.Add(settings->mTDOChannel, mSimulationSampleRateHz, BIT_LOW);

	if (settings->mTRSTChannel != UNDEFINED_CHANNEL) {
		mTRST = mJtagSimulationChannels.Add(settings->mTRSTChannel, mSimulationSampleRateHz, BIT_LOW);
	} else {
		mTRST = NULL;
	}

	mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10.0));
}

U32 JtagSimulationDataGenerator::GenerateSimulationData(U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels)
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample(largest_sample_requested, sample_rate, mSimulationSampleRateHz);

	while (mTCK->GetCurrentSampleNumber() < adjusted_largest_sample_requested) {
		CreateJtagTransaction();

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod((rand() % 10) * 10.0));
	}

	*simulation_channels = mJtagSimulationChannels.GetArray();
	return mJtagSimulationChannels.GetCount();
}

void JtagSimulationDataGenerator::CreateJtagTransaction()
{
	switch (rand() % 2) {
	case 0:
		MoveState("0100"); // SelectDR
		Scan(rand(), rand(), rand() % 24 + 1);
		MoveState("1111"); // Reset
		break;

	case 1:
		MoveState("0110"); // SelectIR
		Scan(rand(), rand(), rand() % 24 + 1);
		MoveState("1111"); // Reset
		break;
	}
}

void JtagSimulationDataGenerator::MoveState(const char *tms)
{
	mTDI->TransitionIfNeeded(BIT_LOW);
	mTDO->TransitionIfNeeded(BIT_LOW);

	while (*tms) {
		mTMS->TransitionIfNeeded(static_cast<BitState>(*tms++ == '1'));

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
		mTCK->Transition();

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
		mTCK->Transition();
	}
}

void JtagSimulationDataGenerator::Scan(U32 in, U32 out, U32 bits)
{
	mTMS->TransitionIfNeeded(BIT_LOW);

	while (bits--) {
		mTDI->TransitionIfNeeded(static_cast<BitState>(in & 1));
		mTDO->TransitionIfNeeded(static_cast<BitState>(out & 1));

		in >>= 1;
		out >>= 1;

		if (bits == 0) {
			mTMS->Transition();
		}

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
		mTCK->Transition();

		mJtagSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
		mTCK->Transition();
	}
}

