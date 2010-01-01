/*******************************************************
Copyright (C) 2010 VaDiM

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

//----------------------------------------------------------------------------
#include <UnitTest++.h>
#include "util.h"
//----------------------------------------------------------------------------

SUITE(util)
{

TEST(formatDoubleToCurrency1)
{
	mmCurrencyFormatter::loadDefaultSettings();

	wxString s;
	
	mmCurrencyFormatter::formatDoubleToCurrency(0, s);
	CHECK(s == wxT("$ 0.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(-0, s);
	CHECK(s == wxT("$ 0.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(1.0, s);
	CHECK(s == wxT("$ 1.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(-12.3, s);
	CHECK(s == wxT("$ -12.30"));

	mmCurrencyFormatter::formatDoubleToCurrency(123.4, s);
	CHECK(s == wxT("$ 123.40"));
	
	mmCurrencyFormatter::formatDoubleToCurrency(-1000.0, s);
	CHECK(s == wxT("$ -1,000.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(10000.0, s);
	CHECK(s == wxT("$ 10,000.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(-100000.0, s);
	CHECK(s == wxT("$ -100,000.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(1000000.0, s);
	CHECK(s == wxT("$ 1,000,000.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(-10000000.0, s);
	CHECK(s == wxT("$ -10,000,000.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(100000000.0, s);
	CHECK(s == wxT("$ 100,000,000.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(1000000000.0, s);
	CHECK(s == wxT("$ 1,000,000,000.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(-10000000000.0, s);
	CHECK(s == wxT("$ -10,000,000,000.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(100000000000.0, s);
	CHECK(s == wxT("$ 100,000,000,000.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(-1234567890123.45, s);
	CHECK(s == wxT("$ -1,234,567,890,123.45"));
}
//----------------------------------------------------------------------------

TEST(formatDoubleToCurrency2)
{
	double val = 1234.1415;
	wxString s;

	mmCurrencyFormatter::formatDoubleToCurrency(val, s);
	CHECK(s == wxT("$ 1,234.14"));

	mmCurrencyFormatter::formatDoubleToCurrency(-val, s);
	CHECK(s == wxT("$ -1,234.14"));
}
//----------------------------------------------------------------------------

TEST(formatDoubleToCurrency3)
{
        double val = 0.0099;
	wxString s;

	mmCurrencyFormatter::formatDoubleToCurrency(val, s);
	CHECK(s == wxT("$ 0.01"));

	mmCurrencyFormatter::formatDoubleToCurrency(-val, s);
	CHECK(s == wxT("$ -0.01"));

	mmCurrencyFormatter::formatDoubleToCurrency(0.004, s);
	CHECK(s == wxT("$ 0.00"));

	mmCurrencyFormatter::formatDoubleToCurrency(0.005, s);
	CHECK(s == wxT("$ 0.01"));
}
//----------------------------------------------------------------------------

TEST(formatDoubleToCurrency4)
{
	wxString pfx = wxT("AAA");
	wxString sfx = wxT("ZZZ");
	wxChar dec = wxT('%');
	wxChar grp = wxT(':');
	wxString unit = wxT("baboses");
	wxString cent = wxT("kop.");
	double scale = 100;

	mmCurrencyFormatter::loadSettings(pfx, sfx, dec, grp, unit, cent, scale);

	wxString s;
	mmCurrencyFormatter::formatDoubleToCurrency(-9123456789012.34, s);
	CHECK(s == wxT("AAA -9:123:456:789:012%34ZZZ"));
}
//----------------------------------------------------------------------------

TEST(formatDoubleToCurrency5)
{
	wxString s;

	mmCurrencyFormatter::formatDoubleToCurrencyEdit(0, s);
	CHECK(s == wxT("0%00"));

	mmCurrencyFormatter::formatDoubleToCurrencyEdit(-9123456789012.34, s);
	CHECK(s == wxT("-9:123:456:789:012%34"));
}
//----------------------------------------------------------------------------

} // SUITE

//----------------------------------------------------------------------------
