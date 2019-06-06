/////////////////////////////////////////////////////////////////////////////
// Name:        servermergetoolapp.h
// Purpose:     
// Author:      songg
// Modified by: 
// Created:     15/07/2009 09:30:27
// RCS-ID:      
// Copyright:   songg
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _SERVERMERGETOOLAPP_H_
#define _SERVERMERGETOOLAPP_H_


/*!
 * Includes
 */

////@begin includes
#include "mainpage.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * ServerMergeToolApp class declaration
 */

class ServerMergeToolApp: public wxApp
{    
    DECLARE_CLASS( ServerMergeToolApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    ServerMergeToolApp();

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

	//-------------------------------------------------------------------------
	// ¸¨Öúº¯Êý
	//-------------------------------------------------------------------------
	TObjRef<Log>&	GetLog()	{ return m_pLog; }
	TObjRef<Util>&	GetUtil()	{ return m_pUtil; }

private:
	TObjRef<Log>			m_pLog;
	TObjRef<Util>			m_pUtil;
////@begin ServerMergeToolApp event handler declarations

////@end ServerMergeToolApp event handler declarations

////@begin ServerMergeToolApp member function declarations

////@end ServerMergeToolApp member function declarations

////@begin ServerMergeToolApp member variables
////@end ServerMergeToolApp member variables
};

/*!
 * Application instance declaration 
 */
#define ILOG	(wxGetApp().GetLog())
#define IUTIL	(wxGetApp().GetUtil())
////@begin declare app
DECLARE_APP(ServerMergeToolApp)
////@end declare app


#endif
    // _SERVERMERGETOOLAPP_H_
