/////////////////////////////////////////////////////////////////////////////
// Name:        screenshot_main.h
// Purpose:     Defines the Application Frame
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id: screenshot_main.h 64940 2010-07-13 13:29:13Z VZ $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _SCREENSHOT_MAIN_H_
#define _SCREENSHOT_MAIN_H_

#include "guiframe.h"

class ScreenshotFrame : public GUIFrame
{
public:
    ScreenshotFrame(wxFrame *frame);
    ~ScreenshotFrame() {}

protected:      // event handlers

    virtual void OnClose(wxCloseEvent& event);
    virtual void OnQuit(wxCommandEvent& event);
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnSeeScreenshots( wxCommandEvent& event);

    virtual void OnCaptureFullScreen( wxCommandEvent& event );
    virtual void OnCaptureAllControls( wxCommandEvent& event );
};

#endif // _SCREENSHOT_MAIN_H_
