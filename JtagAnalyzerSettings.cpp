/*
 * jtaglogic
 *
 * Copyright (C) 2013 Fredrik Ahlberg
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

#include "JtagAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>

JtagAnalyzerSettings::JtagAnalyzerSettings()
:	mTCKChannel(UNDEFINED_CHANNEL),
	mTMSChannel(UNDEFINED_CHANNEL),
	mTDIChannel(UNDEFINED_CHANNEL),
	mTDOChannel(UNDEFINED_CHANNEL),
	mTRSTChannel(UNDEFINED_CHANNEL),
	mShiftOrder(AnalyzerEnums::MsbFirst),
	mInnerProto(InnerPlain)
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

	mInnerProtoInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mInnerProtoInterface->SetTitleAndTooltip("Inner protocol", "");
	mInnerProtoInterface->AddNumber(InnerPlain, "None", "");
	mInnerProtoInterface->AddNumber(InnerAVR, "AVR ICE", "");
	mInnerProtoInterface->AddNumber(InnerARM, "ARM CoreSight", "");
	mInnerProtoInterface->SetNumber(mInnerProto);

	mShiftOrderInterface.reset(new AnalyzerSettingInterfaceNumberList() );
	mShiftOrderInterface->SetTitleAndTooltip( "", "" );
	mShiftOrderInterface->AddNumber( AnalyzerEnums::MsbFirst, "Most Significant Bit First (Standard)", "" );
	mShiftOrderInterface->AddNumber( AnalyzerEnums::LsbFirst, "Least Significant Bit First", "" );
	mShiftOrderInterface->SetNumber( mShiftOrder );

	AddInterface(mTCKChannelInterface.get());
	AddInterface(mTMSChannelInterface.get());
	AddInterface(mTDIChannelInterface.get());
	AddInterface(mTDOChannelInterface.get());
	AddInterface(mTRSTChannelInterface.get());
	AddInterface(mInnerProtoInterface.get());
	AddInterface(mShiftOrderInterface.get());

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

	if (AnalyzerHelpers::DoChannelsOverlap(&channels[0], channels.size()) == true) {
		SetErrorText("Please select different channels for each input.");
		return false;
	}

	mTCKChannel = mTCKChannelInterface->GetChannel();
	mTMSChannel = mTMSChannelInterface->GetChannel();
	mTDIChannel = mTDIChannelInterface->GetChannel();
	mTDOChannel = mTDOChannelInterface->GetChannel();
	mTRSTChannel = mTRSTChannelInterface->GetChannel();

	mInnerProto = (JtagInner) U32( mInnerProtoInterface->GetNumber() );

	mShiftOrder = (AnalyzerEnums::ShiftOrder) U32( mShiftOrderInterface->GetNumber() );

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

	const char* name_string;    //the first thing in the archive is the name of the protocol analyzer that the data belongs to.
	text_archive >> &name_string;
	if (strcmp(name_string, "KongoJtagAnalyzer") != 0) {
		AnalyzerHelpers::Assert( "KongoJtagAnalyzer: Provided with a settings string that doesn't belong to us;" );
	}

	text_archive >> mTCKChannel;
	text_archive >> mTMSChannel;
	text_archive >> mTDIChannel;
	text_archive >> mTDOChannel;
	text_archive >> mTRSTChannel;
	text_archive >> *(U32*)&mInnerProto;
	text_archive >> *(U32*)&mShiftOrder;

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
	text_archive << mInnerProto;
	text_archive << mShiftOrder;

	return SetReturnString(text_archive.GetString());
}

void JtagAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mTCKChannelInterface->SetChannel(mTCKChannel);
	mTMSChannelInterface->SetChannel(mTMSChannel);
	mTDIChannelInterface->SetChannel(mTDIChannel);
	mTDOChannelInterface->SetChannel(mTDOChannel);
	mTRSTChannelInterface->SetChannel(mTRSTChannel);
	mInnerProtoInterface->SetNumber(mInnerProto);
	mShiftOrderInterface->SetNumber(mShiftOrder);
}
