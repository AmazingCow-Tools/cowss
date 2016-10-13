#ifndef SS_H
#define SS_H

// Includes //
//c
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
//std
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
//X11
#include<X11/Xlib.h>
#include<X11/cursorfont.h>
#include <X11/Xutil.h>
//CImg
#include "CImg.h"


class SS
{
    // Enums / Constants / Typedefs //
public:
    enum class ScreenShotType { Selection, Fullscreen };
    enum class TrackState     { NotStarted, Tracking, Done };


    // CTOR / DTOR //
public:
     SS(ScreenShotType type);
    ~SS();


    // Public Methods //
public:
    void loop();
    void saveScreenshot();


    // Private Methods //
private:
    //Setup
    void setupDisplayScreenWindow();
    void setupCursors();
    void setupGC();

    //Input
    void grapInput  ();
    void ungrabInput();

    void onMouseMove   ();
    void onMousePress  ();
    void onMouseRelease();

    //Helpers
    void drawRegion(int x, int y, int w, int h);
    std::string createPictureFullpath();

    // iVars //
private:
    Display *m_pDisplay;
    Screen  *m_pScreen;
    Window   m_rootWindow;

    GC m_gc;

    Cursor m_cursor;

    XEvent     m_event;
    TrackState m_trackState;

    ScreenShotType m_screenShotType;

    int m_startPosX;
    int m_startPosY;
    int m_rectX;
    int m_rectY;
    int m_rectW;
    int m_rectH;
};

#endif // SS_H
