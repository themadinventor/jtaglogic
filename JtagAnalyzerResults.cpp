#include "JtagAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "JtagAnalyzer.h"
#include "JtagAnalyzerSettings.h"
#include <iostream>
#include <sstream>
#include <cstdio>

#pragma warning(disable: 4996) //warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.

const char *JtagStateStr[] = {
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

const char *AvrOCDRegs[] = {
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

JtagAnalyzerResults::JtagAnalyzerResults(JtagAnalyzer* analyzer, JtagAnalyzerSettings* settings)
:	AnalyzerResults(),
	mSettings(settings),
	mAnalyzer(analyzer)
{
}

JtagAnalyzerResults::~JtagAnalyzerResults()
{
}

void JtagAnalyzerResults::GenerateBubbleText(U64 frame_index, Channel& channel, DisplayBase display_base)
{
    char buf[128];

	ClearResultStrings();
	Frame frame = GetFrame(frame_index);

    AvrInstruction instr = static_cast<AvrInstruction>(frame.mFlags & 0xf);

    if (channel == mSettings->mTDIChannel) {
        switch (instr) {
        case AvrProgEnable:
            sprintf(buf, "ProgEnable %08x", frame.mData1);
			AddResultString(buf);
            break;

        case AvrProgCmd:
            sprintf(buf, "ProgCmd %08x", frame.mData1);
			AddResultString(buf);
            break;

        case AvrPageLoad:
            sprintf(buf, "PageLoad %08x", frame.mData1);
			AddResultString(buf);
            break;

        case AvrForceBreak:
            AddResultString("ForceBreak");
            break;

        case AvrRun:
            AddResultString("Run");
            break;

        case AvrExecute:
            sprintf(buf, "Execute %04x", frame.mData1);
            AddResultString(buf);
            break;

        case AvrReset:
            if (frame.mData1) {
                AddResultString("EnterReset");
            } else {
                AddResultString("LeaveReset");
            }
            break;

        case AvrOCD:
            if (frame.mData1 & 0x100000) {
                sprintf(buf, "OCD Write %s = %04x", AvrOCDRegs[(frame.mData1 >> 16) & 0xf], frame.mData1 & 0xffff);
                AddResultString(buf);
            }
            break;
        }
    } else if (channel == mSettings->mTDOChannel) {
        switch (instr) {
        case AvrIdCode:
            sprintf(buf, "IdCode %08x", frame.mData2);
			AddResultString(buf);
            break;

        case AvrPageRead:
            sprintf(buf, "PageRead %08x", frame.mData2);
			AddResultString(buf);
            break;

        case AvrExecute:
            sprintf(buf, "PC = %04x", frame.mData2);
            AddResultString(buf);
            break;

        case AvrOCD:
            if (!(frame.mData1 & 0x100000)) {
                sprintf(buf, "OCD Read %s = %04x", AvrOCDRegs[(frame.mData1 >> 16) & 0xf], frame.mData2 & 0xffff);
                AddResultString(buf);
            }
            break;
        }
    }

#if 0
    if ((frame.mFlags & 0xf) == JtagShiftDR || (frame.mFlags & 0xf) == JtagShiftIR) {
		if (channel == mSettings->mTDIChannel) {
            AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 0, number_str, 128);
            AddResultString(number_str);
		} else if (channel == mSettings->mTDOChannel) {
			AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 0, number_str, 128);
			AddResultString(number_str);
        }
    }

    if (channel == mSettings->mTMSChannel) {
		AddResultString(JtagStateStr[frame.mFlags & 0x0f]);
	}
#endif
}

void JtagAnalyzerResults::GenerateExportFile(const char* file, DisplayBase display_base, U32 /*export_type_user_id*/)
{
	std::stringstream ss;
	void* f = AnalyzerHelpers::StartFile(file);

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	ss << "Time [s],Packet ID,State,TDI,TDO" << std::endl;

	U64 num_frames = GetNumFrames();
	for (U32 i=0; i < num_frames; i++) {
		Frame frame = GetFrame(i);

        if ((frame.mFlags & 0xf) != JtagShiftDR && (frame.mFlags & 0xf) != JtagShiftIR) {
            continue;
        }

		char time_str[128], tdi_str[128], tdo_str[128];

		AnalyzerHelpers::GetTimeString(frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128);

		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 0, tdi_str, 128);
		AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 0, tdo_str, 128);

		U64 packet_id = GetPacketContainingFrameSequential(i); 
		if (packet_id != INVALID_RESULT_INDEX) {
			ss << time_str << "," << packet_id << "," << JtagStateStr[frame.mFlags & 0x0f] << "," << tdi_str << "," << tdo_str << std::endl;
        } else {
			ss << time_str << ",," << JtagStateStr[frame.mFlags & 0x0f] << "," << tdi_str << "," << tdo_str << std::endl;
        }
	
		AnalyzerHelpers::AppendToFile((U8 *) ss.str().c_str(), ss.str().length(), f);
		ss.str(std::string());
							
		if (UpdateExportProgressAndCheckForCancel(i, num_frames) == true) {
			AnalyzerHelpers::EndFile(f);
			return;
		}
	}

	UpdateExportProgressAndCheckForCancel(num_frames, num_frames);
	AnalyzerHelpers::EndFile(f);
}

void JtagAnalyzerResults::GenerateFrameTabularText(U64 /*frame_index*/, DisplayBase /*display_base*/)
{
	ClearResultStrings();
	AddResultString("not supported");
}

void JtagAnalyzerResults::GeneratePacketTabularText(U64 /*packet_id*/, DisplayBase /*display_base*/)
{
	ClearResultStrings();
	AddResultString("not supported");
}

void JtagAnalyzerResults::GenerateTransactionTabularText(U64 /*transaction_id*/, DisplayBase /*display_base*/)
{
	ClearResultStrings();
	AddResultString("not supported");
}
