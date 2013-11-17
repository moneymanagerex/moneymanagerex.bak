/*******************************************************
Copyright (C) 2006-2012

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

#pragma once
#include <wx/textctrl.h>
#include <wx/string.h>
#include "model/Model_Currency.h"
#include "model/Model_Account.h"

class mmTextCtrl : public wxTextCtrl
{
public:
    using wxTextCtrl::SetValue;

    mmTextCtrl() : currency_(0) {}
    mmTextCtrl(wxWindow *parent, wxWindowID id, const wxString &value=wxEmptyString
            , const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize
            , long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr)
            : wxTextCtrl(parent, id, value, pos, size, style, validator, name), currency_(0)
            {}
    void SetValue(double value)
    {
        currency_ = Model_Currency::GetBaseCurrency();
        this->SetValue(Model_Currency::toString(value, currency_));
    }
    void SetValue(double value, const Model_Account::Data* account)
    {
        currency_ = Model_Currency::instance().get(account->CURRENCYID);
        this->SetValue(Model_Account::toString(value, account));
    }
    wxString GetValue() const
    {
        // Remove prefix and suffix characters from value
        // Base class handles the thousands seperator
        return /*Model_Currency::fromString(*/wxTextCtrl::GetValue()/*, currency_)*/;
    }
private:
    const Model_Currency::Data* currency_;
};


