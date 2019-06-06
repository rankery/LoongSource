/////////////////////////////////////////////////////////////////////////////
// Name:        screenshot_app.h
// Purpose:     Defines the Application Class
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id: screenshot_app.h 64940 2010-07-13 13:29:13Z VZ $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _SCREENSHOT_APP_H_
#define _SCREENSHOT_APP_H_

#include "wx/app.h"

class ScreenshotApp : public wxApp
{
public:
    virtual bool OnInit();
};

#endif // _SCREENSHOT_APP_H_
