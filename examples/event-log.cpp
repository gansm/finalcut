/***********************************************************************
* event-log.cpp - Logs events in a dialog box                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <fstream>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include <final/final.h>

using finalcut::FPoint;
using finalcut::FRect;
using finalcut::FSize;

class EventLog;  // class forward declaration

//----------------------------------------------------------------------
// class EventLog::EventLog
//----------------------------------------------------------------------

class EventDialog final : public finalcut::FDialog
{
  public:
    // Using-declaration
    using FDialog::setGeometry;

    // Constructor
    explicit EventDialog (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    EventDialog (const EventDialog&) = delete;

    // Destructor
    ~EventDialog();

    // Disable copy assignment operator (=)
    EventDialog& operator = (const EventDialog&) = delete;

  private:
    // Methods
    finalcut::FString getMouseButtonName (int) const;
    void logMouseEvent ( const finalcut::FString&
                       , const finalcut::FMouseEvent& );

    // Event handlers
    void onClose (finalcut::FCloseEvent*) override;
    void onShow (finalcut::FShowEvent*) override;
    void onTimer (finalcut::FTimerEvent*) override;
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onMouseDown (finalcut::FMouseEvent*) override;
    void onMouseMove (finalcut::FMouseEvent*) override;
    void onMouseUp (finalcut::FMouseEvent*) override;
    void onMouseDoubleClick (finalcut::FMouseEvent* ev) override;
    void onWindowActive (finalcut::FEvent*) override;
    void onWindowInactive (finalcut::FEvent*) override;
    void onWindowRaised (finalcut::FEvent*) override;
    void onWindowLowered (finalcut::FEvent*) override;

    // Data members
    finalcut::FLog& log{*finalcut::FApplication::getLog()};
    finalcut::FLabel label{this};
};

//----------------------------------------------------------------------
EventDialog::EventDialog (finalcut::FWidget* parent)
  : FDialog{parent}
{
  // Dialog settings
  //   Avoids calling a virtual function from the constructor
  //   (CERT, OOP50-CPP)
  FDialog::setText ("Event dialog");
  FDialog::setGeometry (FPoint{15, 2}, FSize{53, 12});
  setShadow();
  label.setText("\n\nUse the keyboard or mouse\n"
                "in this dialog to create events");
  label.setAlignment(finalcut::fc::alignCenter);
  label.setGeometry (FPoint(1, 1), getClientSize(), false);
  addTimer(60000);  // Starts the timer every minute
}

//----------------------------------------------------------------------
EventDialog::~EventDialog()  // destructor
{ }

//----------------------------------------------------------------------
finalcut::FString EventDialog::getMouseButtonName (int btn_state) const
{
  switch ( btn_state )
  {
    case finalcut::fc::LeftButton:
      return "left";

    case finalcut::fc::RightButton:
      return "right";

    case finalcut::fc::MiddleButton:
      return "middle";
  }

  return "unknown";
}

//----------------------------------------------------------------------
void EventDialog::logMouseEvent ( const finalcut::FString& state
                                , const finalcut::FMouseEvent& ev )
{
  const int mouse_x = ev.getX();
  const int mouse_y = ev.getY();

  log << finalcut::FLog::Info
      << getMouseButtonName(ev.getButton())
      << " mouse button " << state << " at ("
      << mouse_x << ", " << mouse_y << ")" << std::flush;
}

//----------------------------------------------------------------------
void EventDialog::onClose (finalcut::FCloseEvent* ev)
{
  log.info("The event dialog was closed");
  ev->accept();
}

//----------------------------------------------------------------------
void EventDialog::onShow (finalcut::FShowEvent*)
{
  log.info("The event dialog is now displayed");
}

//----------------------------------------------------------------------
void EventDialog::onTimer (finalcut::FTimerEvent*)
{
  log.info("-- minute marker --");
}


//----------------------------------------------------------------------
void EventDialog::onKeyPress (finalcut::FKeyEvent* ev)
{
  const FKey key_id = ev->key();
  finalcut::FString key_name = finalcut::FTerm::getKeyName(key_id);

  if ( key_name.isEmpty() )
    key_name = wchar_t(key_id);

  // std::clog redirects all stream data to FLogger
  std::clog << finalcut::FLog::Info
            << "Key " << key_name
            << " (id " << key_id << ")" << std::flush;

  finalcut::FDialog::onKeyPress(ev);
}

//----------------------------------------------------------------------
void EventDialog::onMouseDown (finalcut::FMouseEvent* ev)
{
  logMouseEvent("down", *ev);
  finalcut::FDialog::onMouseDown(ev);
}

//----------------------------------------------------------------------
void EventDialog::onMouseMove (finalcut::FMouseEvent* ev)
{
  logMouseEvent("move", *ev);
  finalcut::FDialog::onMouseMove(ev);
}

//----------------------------------------------------------------------
void EventDialog::onMouseUp (finalcut::FMouseEvent* ev)
{
  logMouseEvent("up", *ev);
  finalcut::FDialog::onMouseUp(ev);
}

//----------------------------------------------------------------------
void EventDialog::onMouseDoubleClick (finalcut::FMouseEvent* ev)
{
  logMouseEvent("double click", *ev);
  finalcut::FDialog::onMouseDoubleClick(ev);
}

//----------------------------------------------------------------------
void EventDialog::onWindowActive (finalcut::FEvent* ev)
{
  log.info("The Event dialog is now active");
  finalcut::FDialog::onWindowActive(ev);
}

//----------------------------------------------------------------------
void EventDialog::onWindowInactive (finalcut::FEvent* ev)
{
  log.info("The Event dialog is now inactive");
  finalcut::FDialog::onWindowInactive(ev);
}


//----------------------------------------------------------------------
void EventDialog::onWindowRaised (finalcut::FEvent* ev)
{
  log.info("The dialog was raised");
  finalcut::FDialog::onWindowRaised(ev);
}

//----------------------------------------------------------------------
void EventDialog::onWindowLowered (finalcut::FEvent* ev)
{
  log.info("The dialog was lowered");
  finalcut::FDialog::onWindowLowered(ev);
}


//----------------------------------------------------------------------
// class EventLog
//----------------------------------------------------------------------

class EventLog final : public finalcut::FDialog, public std::ostringstream
{
  public:
    // Using-declaration
    using FDialog::setGeometry;

    // Constructor
    explicit EventLog (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    EventLog (const EventLog&) = delete;

    // Destructor
    ~EventLog();

    // Disable copy assignment operator (=)
    EventLog& operator = (const EventLog&) = delete;

    // Event handlers
    void onTimer (finalcut::FTimerEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

  private:
    // Method
    void adjustSize() override;

    // Data members
    finalcut::FTextView scrolltext{this};
    EventDialog* event_dialog{new EventDialog(this)};
};

//----------------------------------------------------------------------
EventLog::EventLog (finalcut::FWidget* parent)
  : FDialog{parent}
{
  // Dialog settings
  //   Avoids calling a virtual function from the constructor
  //   (CERT, OOP50-CPP)
  FDialog::setText ("Event log");
  FDialog::setGeometry (FPoint{4, 16}, FSize{75, 8});
  FDialog::setResizeable();
  setMinimumSize (FSize{75, 5});
  setShadow();
  scrolltext.ignorePadding();
  scrolltext.setGeometry (FPoint{1, 2}, FSize{getWidth(), getHeight() - 1});
  event_dialog->setFocus();
  addTimer(250);  // Starts the timer every 250 milliseconds
}

//----------------------------------------------------------------------
EventLog::~EventLog()  // destructor
{
  if ( event_dialog )
    delete event_dialog;
}

//----------------------------------------------------------------------
void EventLog::onTimer (finalcut::FTimerEvent*)
{
  if ( ! str().empty() )
  {
    scrolltext.append(str());
    str("");
    scrolltext.scrollToEnd();
    redraw();
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void EventLog::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void EventLog::adjustSize()
{
  finalcut::FDialog::adjustSize();
  scrolltext.setGeometry (FPoint{1, 2}, FSize(getWidth(), getHeight() - 1));
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  finalcut::FApplication app(argc, argv);
  EventLog dialog(&app);

  // Get the global logger object
  finalcut::FLog& log = *finalcut::FApplication::getLog();

  // Set the line endings (default = CRLF)
  log.setLineEnding (finalcut::FLog::LF);

  // Write a timestamp before each output line
  log.enableTimestamp();

  // Set the dialog object as output stream
  log.setOutputStream(dialog);

  // ----------------------------------------------
  // Remove the comment characters in the following
  // two lines to log the output to a file.
  // ----------------------------------------------
  //std::ofstream file_stream("test.log", std::ofstream::out | std::ofstream::app);
  //log.setOutputStream(file_stream);

  // Sets the dialog as main widget
  finalcut::FWidget::setMainWidget(&dialog);

  // Show the dialog
  dialog.show();

  // Run the application
  return app.exec();
}

