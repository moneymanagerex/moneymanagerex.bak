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
#include <boost/unordered_map.hpp>
#include "util.h"
#include "utils.h"
//----------------------------------------------------------------------------

struct TestData
{
	TestData(int i, wxString n): id(i), name(n) {}
	int id;
	wxString name;
};

using namespace collections;

template <template<class> class CompareTraits = CompareTraits_Equal>
class MatchTestData_Id: public Matcher<TestData>
{
	int id;
public:
	MatchTestData_Id(int i): id(i) {}

	virtual bool Match(const TestData& t)
	{
		return CompareTraits<int>::Equal(id, t.id);
	}
};

template <template<class> class CompareTraits = CompareTraits_Equal>
class MatchTestDataPtr_Id: public Matcher<boost::shared_ptr<TestData> >
{
	int id;
public:
	MatchTestDataPtr_Id(int i): id(i) {}

	virtual bool Match(const boost::shared_ptr<TestData>& t)
	{
		return CompareTraits<int>::Equal(id, t->id);
	}
};

SUITE(collections)
{

TEST(Matcher)
{
	TestData d1(55, wxT("data-55"));
	TestData d2(56, wxT("data-56"));
	TestData d3(99, wxT("data-99"));

	MatchTestData_Id<> match_equal(55);
	CHECK(match_equal.Match(d1));
	CHECK(!match_equal.Match(d2));
	CHECK(!match_equal.Match(d3));

	MatchTestData_Id<CompareTraits_NotEqual> match_not_equal(56);
	CHECK(match_not_equal.Match(d1));
	CHECK(!match_not_equal.Match(d2));
	CHECK(match_not_equal.Match(d3));
}

TEST(MatcherPtr)
{
	boost::shared_ptr<TestData> d1(new TestData(55, wxT("data-55")));
	boost::shared_ptr<TestData> d2(new TestData(56, wxT("data-56")));
	boost::shared_ptr<TestData> d3(new TestData(99, wxT("data-99")));

	MatchTestDataPtr_Id<> match_equal(55);
	CHECK(match_equal.Match(d1));
	CHECK(!match_equal.Match(d2));
	CHECK(!match_equal.Match(d3));

	MatchTestDataPtr_Id<CompareTraits_NotEqual> match_not_equal(56);
	CHECK(match_not_equal.Match(d1));
	CHECK(!match_not_equal.Match(d2));
	CHECK(match_not_equal.Match(d3));
}

typedef boost::unordered_map<wxString, wxString/*, utils::swStringHash*/> TransactionMatchMap;

static const wxString s_view_reconciled(wxT("View Reconciled"));
static const wxString s_view_void(wxT("View Void"));
static const wxString s_view_flagged(wxT("View Flagged"));
static const wxString s_view_unreconciled(wxT("View UnReconciled"));
static const wxString s_view_not_reconciled(wxT("View Not-Reconciled"));
static const wxString s_view_duplicates(wxT("View Duplicates"));

const TransactionMatchMap& initTransactionMatchMap()
{
	static TransactionMatchMap map;

	map[s_view_reconciled] = wxT("R");
	map[s_view_void] = wxT("V");
	map[s_view_flagged] = wxT("F");
	map[s_view_unreconciled] = wxT("");
	map[s_view_not_reconciled] = wxT("R");
	map[s_view_duplicates] = wxT("D");

	return map;
}
static const TransactionMatchMap& s_transactionMatchers_Map = initTransactionMatchMap();

TEST(BoostHashMap)
{
	TransactionMatchMap::const_iterator end;

	size_t size = s_transactionMatchers_Map.size();
	CHECK(size == 6);

	TransactionMatchMap::hasher hasher = s_transactionMatchers_Map.hash_function();
	std::size_t h1 = hasher(s_view_reconciled);
	std::size_t h2 = hasher(wxT("View Reconciled"));
	CHECK(h1 == h2);

	TransactionMatchMap::const_iterator result = s_transactionMatchers_Map.find(wxT("View Void"));
	CHECK(result != end);
}

} // SUITE

SUITE(util)
{

TEST(formatDoubleToCurrency1)
{
	mmex::CurrencyFormatter::instance().loadDefaultSettings();

	wxString s;
	
	mmex::formatDoubleToCurrency(0, s);
	CHECK(s == wxT("$ 0.00"));

	mmex::formatDoubleToCurrency(-0, s);
	CHECK(s == wxT("$ 0.00"));

	mmex::formatDoubleToCurrency(1.0, s);
	CHECK(s == wxT("$ 1.00"));

	mmex::formatDoubleToCurrency(-12.3, s);
	CHECK(s == wxT("$ -12.30"));

	mmex::formatDoubleToCurrency(123.4, s);
	CHECK(s == wxT("$ 123.40"));
	
	mmex::formatDoubleToCurrency(-1000.0, s);
	CHECK(s == wxT("$ -1,000.00"));

	mmex::formatDoubleToCurrency(10000.0, s);
	CHECK(s == wxT("$ 10,000.00"));

	mmex::formatDoubleToCurrency(-100000.0, s);
	CHECK(s == wxT("$ -100,000.00"));

	mmex::formatDoubleToCurrency(1000000.0, s);
	CHECK(s == wxT("$ 1,000,000.00"));

	mmex::formatDoubleToCurrency(-10000000.0, s);
	CHECK(s == wxT("$ -10,000,000.00"));

	mmex::formatDoubleToCurrency(100000000.0, s);
	CHECK(s == wxT("$ 100,000,000.00"));

	mmex::formatDoubleToCurrency(1000000000.0, s);
	CHECK(s == wxT("$ 1,000,000,000.00"));

	mmex::formatDoubleToCurrency(-10000000000.0, s);
	CHECK(s == wxT("$ -10,000,000,000.00"));

	mmex::formatDoubleToCurrency(100000000000.0, s);
	CHECK(s == wxT("$ 100,000,000,000.00"));

	mmex::formatDoubleToCurrency(-1234567890123.45, s);
	CHECK(s == wxT("$ -1,234,567,890,123.45"));
}
//----------------------------------------------------------------------------

TEST(formatDoubleToCurrency2)
{
	double val = 1234.1415;
	wxString s;

	mmex::formatDoubleToCurrency(val, s);
	CHECK(s == wxT("$ 1,234.14"));

	mmex::formatDoubleToCurrency(-val, s);
	CHECK(s == wxT("$ -1,234.14"));
}
//----------------------------------------------------------------------------

TEST(formatDoubleToCurrency3)
{
        double val = 0.0099;
	wxString s;

	mmex::formatDoubleToCurrency(val, s);
	CHECK(s == wxT("$ 0.01"));

	mmex::formatDoubleToCurrency(-val, s);
	CHECK(s == wxT("$ -0.01"));

	mmex::formatDoubleToCurrency(0.004, s);
	CHECK(s == wxT("$ 0.00"));

	mmex::formatDoubleToCurrency(0.005, s);
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

	mmex::CurrencyFormatter::instance().loadSettings(pfx, sfx, dec, grp, unit, cent, scale);

	wxString s;
	mmex::formatDoubleToCurrency(-9123456789012.34, s);
	CHECK(s == wxT("AAA -9:123:456:789:012%34ZZZ"));
}
//----------------------------------------------------------------------------

TEST(formatDoubleToCurrencyEdit)
{
	wxString s;

	mmex::formatDoubleToCurrencyEdit(0, s);
	CHECK(s == wxT("0%00"));

	mmex::formatDoubleToCurrencyEdit(-9123456789012.34, s);
	CHECK(s == wxT("-9:123:456:789:012%34"));
}
//----------------------------------------------------------------------------

TEST(formatDoubleToCurrency5)
{
	wxString es;
	wxChar dec = wxT('\a'); // isprint() -> false
	wxChar grp = wxT('\0');

	mmex::CurrencyFormatter::instance().loadSettings(es, es, dec, grp, es, es, 1000);

	wxString s;
	mmex::formatDoubleToCurrency(12345670.895, s);
	CHECK(s == wxT("12345670.895"));

	// --

	mmex::CurrencyFormatter::instance().loadSettings(es, es, dec, grp, es, es, 10);

	s.clear();
	mmex::formatDoubleToCurrency(12345670.895, s);
	CHECK(s == wxT("12345670.9"));

	// --

	mmex::CurrencyFormatter::instance().loadSettings(es, es, dec, grp, es, es, 10000);

	s.clear();
	mmex::formatDoubleToCurrency(12345670.8954, s);
	CHECK(s == wxT("12345670.8954"));

	// --

	mmex::CurrencyFormatter::instance().loadSettings(es, es, dec, grp, es, es, 0);

	s.clear();
	mmex::formatDoubleToCurrency(12345670.89, s);
	CHECK(s == wxT("12345670.89"));

	// --

	mmex::CurrencyFormatter::instance().loadSettings(es, es, dec, grp, es, es, -100);

	s.clear();
	mmex::formatDoubleToCurrency(12345670.89, s);
	CHECK(s == wxT("12345670.89"));
}
//----------------------------------------------------------------------------

} // SUITE

//----------------------------------------------------------------------------
