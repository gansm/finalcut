
First steps with the Final Cut widget toolkit 
=============================================


How to use the library
----------------------

At the beginning of this introduction to the Final Cut
we will start with a small example.

The following example creates an empty 30×10 character dialog.

**File:** *dialog.cpp*
```cpp
#include <final/final.h>

int main (int argc, char* argv[])
{
  finalcut::FApplication app(argc, argv);
  finalcut::FDialog dialog(&app);
  dialog.setText ("A dialog");
  dialog.setGeometry (25, 5, 30, 10);
  app.setMainWidget(&dialog);
  dialog.show();
  return app.exec();
}
```
*(Note: You can close the dialog with the mouse, 
<kbd>Shift</kbd>+<kbd>F10</kbd> or <kbd>Ctrl</kbd>+<kbd>^</kbd>)*


After entering the source code in *dialog.cpp* you can compile
the above program with gcc:
```cpp
g++ -O2 -lfinal dialog.cpp -o dialog
```


How it works
------------


```cpp
#include <final/final.h>
```
All final cut programs must include the *final.h* header.

```cpp
finalcut::FApplication app(argc, argv);
```
This line creates the `finalcut::FApplication` object `app` with
the command line arguments `argc` and `argv`. This object manages
the application main event loop. It receives keyboard and mouse events
and sends them to the target widgets. You must create an application
object before you can create a widgets object.

The next line
```cpp
finalcut::FDialog dialog(&app);
```
creates the `finalcut::FDialog` object `dialog` with the object `app`
as parent object. The `finalcut::FDialog` class is the base class for
creating dialog windows.

```cpp
dialog.setText ("A dialog");
```
The title bar of the dialog box gets the text "A dialog".

```cpp
dialog.setGeometry (25, 5, 30, 10);
```
The dialog window gets a width of 30 and a height of 10 characters.
The position of the window in the terminal is at x=25 and
y=5 (note: x=1 and y=1 represents the upper left corner).

```cpp
app.setMainWidget(&dialog);
```
The `dialog` object was now selected as the main widget for the application.
When you close the main widget, the entire application quits.


```cpp
dialog.show();
```
A window or widget is not visible directly after its creation.
Only the call of `show()` makes it (and its child objects,
if available) visible.

```cpp
return app.exec();
```
The last line calls `exec()` to start the application and return
the result to the operating system. The started application enters
the main event loop. This loop does not end until the window is
not closed.


Memory Management
-----------------

To create a hierarchy of FObjects (or derived classes/widgets), 
a new FObject must initialize with its parent object.

```cpp
FObject* parent = new FObject();
FObject* child  = new FObject(parent);
```

To deallocate the used memory of a parent FObject, the allocated memory 
of its child objects will also automatically deallocate.

An object can also be assigned to another object later via `addChild()`.

```cpp
FObject* parent = new FObject();
FObject* child = new FObject();
parent->addChild(child);
```

The child object assignment can also remove at any time with 
`delChild()`.

```cpp
FObject* parent = new FObject();
FObject* child  = new FObject(parent);
parent->delChild(child);
```

If an FObject with a parent will remove from the hierarchy, 
the destructor automatically deletes the object assignment from 
its parent object. If a class object doesn't derive from FObject, 
you must implement storage deallocation yourself.

**File:** *memory.cpp*
```cpp
#include <final/final.h>

using namespace finalcut;

int main (int argc, char* argv[])
{
  FApplication app(argc, argv);

  // The object dialog is managed by app
  FDialog* dialog = new FDialog(&app);
  dialog->setText ("Window Title");
  dialog->setGeometry (25, 5, 40, 8);

  // The object input is managed by dialog
  FLineEdit* input = new FLineEdit("predefined text", dialog);
  input->setGeometry(8, 2, 29, 1);
  input->setLabelText (L"&Input");

  // The object label is managed by dialog
  FLabel* label = new FLabel ( "Lorem ipsum dolor sit amet, consectetur "
                               "adipiscing elit, sed do eiusmod tempor "
                               "incididunt ut labore et dolore magna aliqua."
                             , dialog );
  label->setGeometry (2, 4, 36, 1);
  app.setMainWidget(dialog);
  dialog->show();
  return app.exec();
}
```
*(Note: You can close the window with the mouse, 
<kbd>Shift</kbd>+<kbd>F10</kbd> or <kbd>Ctrl</kbd>+<kbd>^</kbd>)*


After entering the source code in *memory.cpp* you can compile
the above program with gcc:
```cpp
g++ -O2 -lfinal memory.cpp -o memory
```


Event Processing
----------------

Calling `FApplication::exec()` starts the FINAL CUT main event loop. 
While the event loop is running, the system constantly checks whether 
an event has occurred and sends it to the application's currently focused 
object. The events of the terminal such as keystrokes, mouse actions or 
resizing the terminal are translated into `FEvent` objects and sent it to 
the active `FObject`. It is also possible to use `FApplication::sendEvent()` 
or `FApplication::queueEvent()` to send your own events to an object.

`FObject`-derived objects process incoming events by reimplementing the 
virtual method `event()`. The `FObject` itself calls only 
`onTimer()` or `onUserEvent()` and ignores all other events. The 
`FObject`-derived class `FWidget` also reimplements the `event()` method 
to handle further events. `FWidget` calls the `FWidget::onKeyPress` method
when you press a key, or the `FWidget::onMouseDown` method when you click 
a mouse button.


### Event handler reimplementation ###

An event in FINAL CUT is an object that inherits from the base class 
`FEvent`. There are several event types, represented by an enum value. 
For example, the method `FEvent::type()` returns the type 
`fc::MouseDown_Event` when you press down a mouse button. 

Some event types have data that cannot store in an `FEvent` object. 
For example, a click event of the mouse must store which button it 
triggered where the mouse pointer was at that time. In classes derived 
from `FEvent`, such as `FMouseEvent()`, we store this data.

Widgets get their events from the `event()` method inherited from FObject. 
The implementation of `event()` in `FWidget` forwards the most common event 
types to specific event handlers such as `FMouseEvent()`, `FKeyEvent()` or 
`FResizeEvent()`. There are many other event types. It is also possible to 
create own event types and send them to other objects.


**The FINAL CUT event types:**
```cpp
enum events
{
  None_Event,               // invalid event
  KeyPress_Event,           // key pressed
  KeyUp_Event,              // key released
  KeyDown_Event,            // key pressed
  MouseDown_Event,          // mouse button pressed
  MouseUp_Event,            // mouse button released
  MouseDoubleClick_Event,   // mouse button double click
  MouseWheel_Event,         // mouse wheel rolled
  MouseMove_Event,          // mouse move
  FocusIn_Event,            // focus in
  FocusOut_Event,           // focus out
  ChildFocusIn_Event,       // child focus in
  ChildFocusOut_Event,      // child focus out
  WindowActive_Event,       // activate window
  WindowInactive_Event,     // deactivate window
  WindowRaised_Event,       // raise window
  WindowLowered_Event,      // lower window
  Accelerator_Event,        // keyboard accelerator
  Resize_Event,             // terminal resize
  Show_Event,               // widget is shown
  Hide_Event,               // widget is hidden
  Close_Event,              // widget close
  Timer_Event,              // timer event occur
  User_Event                // user defined event
};
```


**File:** *timer.cpp*
```cpp
#include <final/final.h>

using namespace finalcut;

class dialogWidget : public FDialog
{
  public:
    explicit dialogWidget (FWidget* parent = nullptr)
      : FDialog(parent)
    {
      setText ("Dialog");
      setGeometry (25, 5, 23, 4);
      label.setGeometry (1, 1, 10, 1);
      label.setAlignment (fc::alignRight);
      value.setGeometry (11, 1, 10, 1);
      id = addTimer(100);
    }

  private:
    virtual void onTimer (FTimerEvent* ev)
    {
      if ( id == ev->getTimerId() && n < 9999999999 )
      {
        value.setNumber(n);
        value.redraw();
        n++;
      }
    }

    FLabel label{"Counter: ", this};
    FLabel value{"0", this};
    long n{0};
    int id{0};
};

int main (int argc, char* argv[])
{
  FApplication app(argc, argv);
  dialogWidget dialog(&app);
  app.setMainWidget(&dialog);
  dialog.show();
  return app.exec();
}
```
*(Note: You can close the window with the mouse, 
<kbd>Shift</kbd>+<kbd>F10</kbd> or <kbd>Ctrl</kbd>+<kbd>^</kbd>)*


After entering the source code in *timer.cpp* you can compile
the above program with gcc:
```cpp
g++ -O2 -std=c++11 -lfinal timer.cpp -o timer
```
