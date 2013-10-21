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

	AnalyzerEnums::ShiftOrder mShiftOrder;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTCKChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTMSChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTDIChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTDOChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTRSTChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mShiftOrderInterface;
};

#endif //JTAG_ANALYZER_SETTINGS
