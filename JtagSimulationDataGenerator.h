#ifndef JTAG_SIMULATION_DATA_GENERATOR
#define JTAG_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>

class JtagAnalyzerSettings;

class JtagSimulationDataGenerator
{
public:
	JtagSimulationDataGenerator();
	~JtagSimulationDataGenerator();

	void Initialize(U32 simulation_sample_rate, JtagAnalyzerSettings* settings);
	U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels);

protected:
	JtagAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected: //Jtag specific
	ClockGenerator mClockGenerator;

	void CreateJtagTransaction();
	void MoveState(const char *tms);
	void Scan(U32 in, U32 out, U32 bits);

	SimulationChannelDescriptorGroup mJtagSimulationChannels;
	SimulationChannelDescriptor* mTCK;
	SimulationChannelDescriptor* mTMS;
	SimulationChannelDescriptor* mTDI;
	SimulationChannelDescriptor* mTDO;
	SimulationChannelDescriptor* mTRST;
};
#endif //JTAG_SIMULATION_DATA_GENERATOR
