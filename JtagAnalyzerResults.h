#ifndef JTAG_ANALYZER_RESULTS
#define JTAG_ANALYZER_RESULTS

#include <AnalyzerResults.h>

//#define JTAG_ERROR_FLAG ( 1 << 0 )

class JtagAnalyzer;
class JtagAnalyzerSettings;

class JtagAnalyzerResults : public AnalyzerResults
{
public:
	JtagAnalyzerResults(JtagAnalyzer* analyzer, JtagAnalyzerSettings* settings);
	virtual ~JtagAnalyzerResults();

	virtual void GenerateBubbleText(U64 frame_index, Channel& channel, DisplayBase display_base);
	virtual void GenerateExportFile(const char* file, DisplayBase display_base, U32 export_type_user_id);

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base);
	virtual void GeneratePacketTabularText(U64 packet_id, DisplayBase display_base);
	virtual void GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base);

protected: //functions

protected:  //vars
	JtagAnalyzerSettings* mSettings;
	JtagAnalyzer* mAnalyzer;
};

#endif //JTAG_ANALYZER_RESULTS
