///////////////////////////////////////////////////////////////////////////////
// Name:        wxAutoComboBox.h
// Purpose:     wxAutoComboBox class declaration.
// Author:      Guru Kathiresan
// Modified by:
// Created:     03/24/2006
// Copyright:   (c) Guru Kathiresan
// Licence:     BSD licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
        #pragma hdrstop
#endif

#ifndef WX_PRECOMP
        #include "wx/wx.h"
#endif

#if wxUSE_COMBOBOX

class wxAutoComboBox : public wxComboBox
{
        DECLARE_DYNAMIC_CLASS(wxAutoComboBox)

public:
        wxAutoComboBox();

        /**
         * Constructor
         * \param parent        Parent window pointer
         * \param id        Control ID
         * \param value  Text control value
         * \param pos      Position of the control
         * \param size    Size of the control
         * \param choices       List of String Items
         * \param style  Style values for the text control
         * \param validator     Validator for validating the user input
         * \param name    Name of the control.
         */
        wxAutoComboBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, const wxArrayString& choices = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("Auto Text Control"));

        /**
         * Default Destructor.
         */
        virtual ~wxAutoComboBox(void);

protected:
        /**
         * Event handler for key Up.
         * \detail For the alpha numeric characters the partial match of the combo items will be done.
         * \param &event
         */
        void OnKeyUp(wxKeyEvent &event);
private:
        // Any class wishing to process wxWindows events must use this macro
        DECLARE_EVENT_TABLE()
};

#endif // wxUSE_TEXTCTRL 