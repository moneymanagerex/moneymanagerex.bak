/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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
#include "guiid.h"
//----------------------------------------------------------------------------
const int mmex::MIN_DATAVERSION = 2;
const wxChar * const mmex::DATAVERSION = wxT("2");
const wxChar *const mmex::DEFDATEFORMAT = wxT("%m/%d/%y");
const wxChar *const mmex::DEFDELIMTER = wxT(",");

// Using google: To specify the exchange, use exch:code 
// Using yahoo: To specify the exchange, use code.exch 

// const wxChar *const mmex::DEFSTOCKURL = wxT("http://www.google.com/finance?q=%s");

// Will display the stock page when using Looks up the current value
const wxChar *const mmex::DEFSTOCKURL = wxT("http://finance.yahoo.com/echarts?s=%s");

// Looks up the current value
// const wxChar *const mmex::DEFSTOCKURL = wxT("http://finance.yahoo.com/lookup?s=%s");

//US Dollar (USD) in Euro (EUR) Chart
//http://www.google.com/finance?q=CURRENCY%3AUSD

//----------------------------------------------------------------------------
