/////////////////////////////////////////////////////////////////////////////
// Name:        dateevt.h
// Purpose:     interface of wxDateEvent
// Author:      wxWidgets team
// RCS-ID:      $Id: dateevt.h 64940 2010-07-13 13:29:13Z VZ $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxDateEvent

    This event class holds information about a date change and is used together
    with wxDatePickerCtrl. It also serves as a base class
    for wxCalendarEvent.

    @library{wxadv}
    @category{events}
*/
class wxDateEvent : public wxCommandEvent
{
public:
    /**
        Returns the date.
    */
    const wxDateTime& GetDate() const;

    /**
        Sets the date carried by the event, normally only used by the library
        internally.
    */
    void SetDate(const wxDateTime& date);
};

