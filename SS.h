//----------------------------------------------------------------------------//
//               █      █                                                     //
//               ████████                                                     //
//             ██        ██                                                   //
//            ███  █  █  ███        SS.h                                      //
//            █ █        █ █        ss                                        //
//             ████████████                                                   //
//           █              █       Copyright (c) 2016                        //
//          █     █    █     █      AmazingCow - www.AmazingCow.com           //
//          █     █    █     █                                                //
//           █              █       N2OMatt - n2omatt@amazingcow.com          //
//             ████████████         www.amazingcow.com/n2omatt                //
//                                                                            //
//                  This software is licensed as GPLv3                        //
//                 CHECK THE COPYING FILE TO MORE DETAILS                     //
//                                                                            //
//    Permission is granted to anyone to use this software for any purpose,   //
//   including commercial applications, and to alter it and redistribute it   //
//               freely, subject to the following restrictions:               //
//                                                                            //
//     0. You **CANNOT** change the type of the license.                      //
//     1. The origin of this software must not be misrepresented;             //
//        you must not claim that you wrote the original software.            //
//     2. If you use this software in a product, an acknowledgment in the     //
//        product IS HIGHLY APPRECIATED, both in source and binary forms.     //
//        (See opensource.AmazingCow.com/acknowledgment.html for details).    //
//        If you will not acknowledge, just send us a email. We'll be         //
//        *VERY* happy to see our work being used by other people. :)         //
//        The email is: acknowledgment_opensource@AmazingCow.com              //
//     3. Altered source versions must be plainly marked as such,             //
//        and must not be misrepresented as being the original software.      //
//     4. This notice may not be removed or altered from any source           //
//        distribution.                                                       //
//     5. Most important, you must have fun. ;)                               //
//                                                                            //
//      Visit opensource.amazingcow.com for more open-source projects.        //
//                                                                            //
//                                  Enjoy :)                                  //
//----------------------------------------------------------------------------//

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
