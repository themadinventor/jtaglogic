#ifndef JTAG_ANALYZER_SETTINGS
#define JTAG_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class JtagAnalyzerSettings : public AnalyzerSettings
{
public:
	JtagAnalyzerSettings();
	virtual ~JtagAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	void UpdateInterfacesFromSettings();
	
	Channel mTCKChannel;
	Channel mTMSChannel;
	Channel mTDIChannel;
	Channel mTDOChannel;
	Channel mTRSTChannel;

#if 0
	AnalyzerEnums::ShiftOrder mShiftOrder;
	U32 mBitsPerTransfer;
	BitState mClockInactiveState;
	AnalyzerEnums::Edge mDataValidEdge;
	BitState mEnableActiveState;
#endif

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTCKChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTMSChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTDIChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTDOChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTRSTChannelInterface;
#if 0
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mShiftOrderInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mBitsPerTransferInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mClockInactiveStateInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mDataValidEdgeInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mEnableActiveStateInterface;
#endif
};

#endif //JTAG_ANALYZER_SETTINGS