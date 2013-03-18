#include "JtagAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>

JtagAnalyzerSettings::JtagAnalyzerSettings()
:	mTCKChannel(UNDEFINED_CHANNEL),
	mTMSChannel(UNDEFINED_CHANNEL),
	mTDIChannel(UNDEFINED_CHANNEL),
	mTDOChannel(UNDEFINED_CHANNEL),
    mTRSTChannel(UNDEFINED_CHANNEL)
#if 0
	mShiftOrder( AnalyzerEnums::MsbFirst ),
	mBitsPerTransfer( 8 ),
	mClockInactiveState( BIT_LOW ),
	mDataValidEdge( AnalyzerEnums::LeadingEdge ), 
	mEnableActiveState( BIT_LOW )
#endif
{
	mTCKChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mTCKChannelInterface->SetTitleAndTooltip("TCK", "Test Clock");
	mTCKChannelInterface->SetChannel(mTCKChannel);

	mTMSChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mTMSChannelInterface->SetTitleAndTooltip("TMS", "Test Mode Select");
	mTMSChannelInterface->SetChannel(mTMSChannel);

	mTDIChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mTDIChannelInterface->SetTitleAndTooltip("TDI", "Test Data In");
	mTDIChannelInterface->SetChannel(mTDIChannel);

	mTDOChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mTDOChannelInterface->SetTitleAndTooltip("TDO", "Test Data Out");
	mTDOChannelInterface->SetChannel(mTDOChannel);

	mTRSTChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mTRSTChannelInterface->SetTitleAndTooltip("TRST", "Test Reset");
	mTRSTChannelInterface->SetChannel(mTRSTChannel);
	mTRSTChannelInterface->SetSelectionOfNoneIsAllowed(true );

#if 0
	mShiftOrderInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mShiftOrderInterface->SetTitleAndTooltip( "", "" );
	mShiftOrderInterface->AddNumber( AnalyzerEnums::MsbFirst, "Most Significant Bit First (Standard)", "" );
	mShiftOrderInterface->AddNumber( AnalyzerEnums::LsbFirst, "Least Significant Bit First", "" );
	mShiftOrderInterface->SetNumber( mShiftOrder );

	mBitsPerTransferInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mBitsPerTransferInterface->SetTitleAndTooltip( "", "" );
	for( U32 i=1; i<=64; i++ )
	{
		std::stringstream ss;

		if( i == 8 )
			ss << "8 Bits per Transfer (Standard)";
		else
			ss << i << " Bits per Transfer";
		
		mBitsPerTransferInterface->AddNumber( i, ss.str().c_str(), "" );
	}
	mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );

	mClockInactiveStateInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mClockInactiveStateInterface->SetTitleAndTooltip( "", "" );
	mClockInactiveStateInterface->AddNumber( BIT_LOW, "Clock is Low when inactive (CPOL = 0)", "CPOL = 0 (Clock Polarity)" );
	mClockInactiveStateInterface->AddNumber( BIT_HIGH, "Clock is High when inactive (CPOL = 1)", "CPOL = 1 (Clock Polarity)" );
	mClockInactiveStateInterface->SetNumber( mClockInactiveState );

	mDataValidEdgeInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mDataValidEdgeInterface->SetTitleAndTooltip( "", "" );
	mDataValidEdgeInterface->AddNumber( AnalyzerEnums::LeadingEdge, "Data is Valid on Clock Leading Edge (CPHA = 0)", "CPHA = 0 (Clock Phase)" );
	mDataValidEdgeInterface->AddNumber( AnalyzerEnums::TrailingEdge, "Data is Valid on Clock Trailing Edge (CPHA = 1)", "CPHA = 1 (Clock Phase)" );
	mDataValidEdgeInterface->SetNumber( mDataValidEdge );

	mEnableActiveStateInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mEnableActiveStateInterface->SetTitleAndTooltip( "", "" );
	mEnableActiveStateInterface->AddNumber( BIT_LOW, "Enable line is Active Low (Standard)", "" );
	mEnableActiveStateInterface->AddNumber( BIT_HIGH, "Enable line is Active High", "" );
	mEnableActiveStateInterface->SetNumber( mEnableActiveState );
#endif

	AddInterface(mTCKChannelInterface.get());
	AddInterface(mTMSChannelInterface.get());
	AddInterface(mTDIChannelInterface.get());
	AddInterface(mTDOChannelInterface.get());
	AddInterface(mTRSTChannelInterface.get());

#if 0
	AddInterface( mShiftOrderInterface.get() );
	AddInterface( mBitsPerTransferInterface.get() );
	AddInterface( mClockInactiveStateInterface.get() );
	AddInterface( mDataValidEdgeInterface.get() );
	AddInterface( mEnableActiveStateInterface.get() );
#endif

	//AddExportOption( 0, "Export as text/csv file", "text (*.txt);;csv (*.csv)" );
	AddExportOption(0, "Export as text/csv file");
	AddExportExtension(0, "text", "txt");
	AddExportExtension(0, "csv", "csv");

	ClearChannels();
	AddChannel(mTCKChannel, "TCK", false);
	AddChannel(mTMSChannel, "TMS", false);
	AddChannel(mTDIChannel, "TDI", false);
	AddChannel(mTDOChannel, "TDO", false);
	AddChannel(mTRSTChannel, "TRST", false);
}

JtagAnalyzerSettings::~JtagAnalyzerSettings()
{
}

bool JtagAnalyzerSettings::SetSettingsFromInterfaces()
{
	Channel tck = mTCKChannelInterface->GetChannel();
	Channel tms = mTMSChannelInterface->GetChannel();
	Channel tdi = mTDIChannelInterface->GetChannel();
	Channel tdo = mTDOChannelInterface->GetChannel();
	Channel trst = mTRSTChannelInterface->GetChannel();

	std::vector<Channel> channels;
	channels.push_back(tck);
	channels.push_back(tms);
	channels.push_back(tdi);
	channels.push_back(tdo);
	channels.push_back(trst);

	if( AnalyzerHelpers::DoChannelsOverlap(&channels[0], channels.size()) == true )
	{
		SetErrorText("Please select different channels for each input.");
		return false;
	}

#if 0
	if((mosi == UNDEFINED_CHANNEL) && (miso == UNDEFINED_CHANNEL))
	{
		SetErrorText( "Please select at least one input for either MISO or MOSI." );
		return false;
	}
#endif

	mTCKChannel = mTCKChannelInterface->GetChannel();
	mTMSChannel = mTMSChannelInterface->GetChannel();
	mTDIChannel = mTDIChannelInterface->GetChannel();
	mTDOChannel = mTDOChannelInterface->GetChannel();
	mTRSTChannel = mTRSTChannelInterface->GetChannel();

#if 0
	mShiftOrder =			(AnalyzerEnums::ShiftOrder) U32( mShiftOrderInterface->GetNumber() );
	mBitsPerTransfer =		U32( mBitsPerTransferInterface->GetNumber() );
	mClockInactiveState =	(BitState) U32( mClockInactiveStateInterface->GetNumber() );
	mDataValidEdge =		(AnalyzerEnums::Edge)  U32( mDataValidEdgeInterface->GetNumber() );
	mEnableActiveState =	(BitState) U32( mEnableActiveStateInterface->GetNumber() );
#endif

	ClearChannels();
	AddChannel(mTCKChannel, "TCK", mTCKChannel != UNDEFINED_CHANNEL);
	AddChannel(mTMSChannel, "TMS", mTMSChannel != UNDEFINED_CHANNEL);
	AddChannel(mTDIChannel, "TDI", mTDIChannel != UNDEFINED_CHANNEL);
	AddChannel(mTDOChannel, "TDO", mTDOChannel != UNDEFINED_CHANNEL);
	AddChannel(mTRSTChannel, "TRST", mTRSTChannel != UNDEFINED_CHANNEL);

	return true;
}

void JtagAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString(settings);

	const char* name_string;	//the first thing in the archive is the name of the protocol analyzer that the data belongs to.
	text_archive >> &name_string;
	if( strcmp( name_string, "KongoJtagAnalyzer" ) != 0 )
		AnalyzerHelpers::Assert( "KongoJtagAnalyzer: Provided with a settings string that doesn't belong to us;" );

	text_archive >>  mTCKChannel;
	text_archive >>  mTMSChannel;
	text_archive >>  mTDIChannel;
	text_archive >>  mTDOChannel;
	text_archive >>  mTRSTChannel;

#if 0
	text_archive >>  *(U32*)&mShiftOrder;
	text_archive >>  mBitsPerTransfer;
	text_archive >>  *(U32*)&mClockInactiveState;
	text_archive >>  *(U32*)&mDataValidEdge;
	text_archive >>  *(U32*)&mEnableActiveState;
#endif

	//bool success = text_archive >> mUsePackets;  //new paramater added -- do this for backwards compatibility
	//if( success == false )
	//	mUsePackets = false; //if the archive fails, set the default value

	ClearChannels();
	AddChannel(mTCKChannel, "TCK", mTCKChannel != UNDEFINED_CHANNEL);
	AddChannel(mTMSChannel, "TMS", mTMSChannel != UNDEFINED_CHANNEL);
	AddChannel(mTDIChannel, "TDI", mTDIChannel != UNDEFINED_CHANNEL);
	AddChannel(mTDOChannel, "TDO", mTDOChannel != UNDEFINED_CHANNEL);
	AddChannel(mTRSTChannel, "TRST", mTRSTChannel != UNDEFINED_CHANNEL);

	UpdateInterfacesFromSettings();
}

const char* JtagAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << "KongoJtagAnalyzer";
	text_archive <<  mTCKChannel;
	text_archive <<  mTMSChannel;
	text_archive <<  mTDIChannel;
	text_archive <<  mTDOChannel;
	text_archive <<  mTRSTChannel;

#if 0
	text_archive <<  mShiftOrder;
	text_archive <<  mBitsPerTransfer;
	text_archive <<  mClockInactiveState;
	text_archive <<  mDataValidEdge;
	text_archive <<  mEnableActiveState;
#endif

	return SetReturnString(text_archive.GetString());
}

void JtagAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mTCKChannelInterface->SetChannel(mTCKChannel);
	mTMSChannelInterface->SetChannel(mTMSChannel);
	mTDIChannelInterface->SetChannel(mTDIChannel);
	mTDOChannelInterface->SetChannel(mTDOChannel);
	mTRSTChannelInterface->SetChannel(mTRSTChannel);

#if 0
	mShiftOrderInterface->SetNumber( mShiftOrder );
	mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );
	mClockInactiveStateInterface->SetNumber( mClockInactiveState );
	mDataValidEdgeInterface->SetNumber( mDataValidEdge );
	mEnableActiveStateInterface->SetNumber( mEnableActiveState );
#endif
}
