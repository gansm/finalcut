
First steps with the Final Cut widget toolkit 
=============================================


How to use the library
----------------------

At the beginning of this introduction to the Final Cut
we will start with a small example.

It creates an empty 30×10 character dialog.

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
*(Note: Use mouse or <kbd>Shift</kbd>+<kbd>F10</kbd> or
<kbd>Ctrl</kbd>+<kbd>^</kbd> to close the dialog)*


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
In this line creates the `finalcut::FApplication` object `app` with
the command line arguments `argc` and `argv`. This object manages
the application main event loop. It receives keyboard and mouse events
and sends them to the target widgets. Before widgets can be created,
an application object must be created! Only one `finalcut::FApplication`
object should be created.

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
The dialog window geometry is set to a width of 30 characters and
a height of 10 characters. The window in the terminal is positioned
at the positions x=25 and y=5 (note: x=1 and y=1 represents the upper
left corner).

```cpp
app.setMainWidget(&dialog);
```
The `dialog` object is selected as the main widget for the application.
When the user closes a main widget, the application will be closed.

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
the result to the operating system. When the application starts,
it enters the main event loop. This loop doesn't end until the
window/application is closed.

