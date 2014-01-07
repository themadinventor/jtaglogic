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

#ifndef JTAG_PLAIN_ANALYZER_H
#define JTAG_PLAIN_ANALYZER_H

#include "JtagAnalyzer.h"
#include "JtagAnalyzerResults.h"
#include "JtagAnalyzerSettings.h"

class JtagPlainAnalyzer : public JtagInnerAnalyzer
{
public:
	JtagPlainAnalyzer(JtagAnalyzerSettings *settings);
	void process(enum JtagState state, U64 in, U64 out, U32 bits,
			U64 begin, U64 end,
			JtagAnalyzerResults *results);
	void generateBubbleText(Frame &frame, Channel &channel,
			DisplayBase display_base, char *str);

private:
	JtagAnalyzerSettings *mSettings;
};

#endif //JTAG_PLAIN_ANALYZER_H

