//Header
#include "SS.h"


////////////////////////////////////////////////////////////////////////////////
// CTOR / DTOR                                                                //
////////////////////////////////////////////////////////////////////////////////
SS::SS(ScreenShotType type) :
    m_screenShotType(type),
    m_startPosX(0),
    m_startPosY(0),
    m_rectX    (0),
    m_rectY    (0),
    m_rectW    (0),
    m_rectH    (0)
{
    setupDisplayScreenWindow();

    if(m_screenShotType == ScreenShotType::Selection)
    {
        setupCursors();
        setupGC();
    }
}

SS::~SS()
{
    XCloseDisplay(m_pDisplay);
}


////////////////////////////////////////////////////////////////////////////////
// Public Methods                                                             //
////////////////////////////////////////////////////////////////////////////////
void SS::loop()
{
    if(m_screenShotType == ScreenShotType::Selection)
    {
        grapInput();
        m_trackState = TrackState::NotStarted;

        while(m_trackState != TrackState::Done)
        {
            while(m_trackState != TrackState::Done && XPending(m_pDisplay))
            {
                XNextEvent(m_pDisplay, &m_event);
                switch(m_event.type)
                {
                    case MotionNotify  : onMouseMove   (); break;
                    case ButtonPress   : onMousePress  (); break;
                    case ButtonRelease : onMouseRelease(); break;
                }
            }
        }

        ungrabInput();

        //Clear the screen.
        drawRegion(m_rectX, m_rectY, m_rectW, m_rectH);
        XFlush(m_pDisplay);
    }
    else //if(m_screenShotType == ScreenShotType::Fullscreen)
    {
        auto windowAttr = XWindowAttributes{};
        XGetWindowAttributes(m_pDisplay, m_rootWindow, &windowAttr);
        m_rectX = 0;
        m_rectY = 0;
        m_rectW = windowAttr.width;
        m_rectH = windowAttr.height;
    }
}

void SS::saveScreenshot()
{
    if(m_rectW <= 0 || m_rectH <= 0)
        return;

    auto pImage = XGetImage(
        m_pDisplay,
        m_rootWindow,
        m_rectX, m_rectY,
        m_rectW, m_rectH,
        AllPlanes,
        ZPixmap
    );

    auto rMask = pImage->red_mask;
    auto gMask = pImage->green_mask;
    auto bMask = pImage->blue_mask;

    auto array   = new std::uint8_t[m_rectW * m_rectH * 3];
    auto picture = cimg_library::CImg<std::uint8_t>(array, m_rectW, m_rectH, 1, 3);

    for(int x = 0; x < m_rectW; ++x)
    {
        for(int y = 0; y < m_rectH; ++y)
        {
            auto pixel = XGetPixel(pImage, x, y);

            auto blue  = pixel & bMask;
            auto green = (pixel & gMask) >> 8;
            auto red   = (pixel & rMask) >> 16;

            picture(x, y, 0) = red;
            picture(x, y, 1) = green;
            picture(x, y, 2) = blue;
        }
    }

    picture.save_png(createPictureFullpath().c_str());
    XDestroyImage(pImage);
}


////////////////////////////////////////////////////////////////////////////////
// Setup                                                                      //
////////////////////////////////////////////////////////////////////////////////
void SS::setupDisplayScreenWindow()
{
    m_pDisplay   = XOpenDisplay   (nullptr);
    m_pScreen    = ScreenOfDisplay(m_pDisplay, DefaultScreen(m_pDisplay));
    m_rootWindow = RootWindow     (m_pDisplay, XScreenNumberOfScreen(m_pScreen));
}

void SS::setupCursors()
{
    m_cursor = XCreateFontCursor(m_pDisplay, XC_crosshair);
}

void SS::setupGC()
{
    //Setup the Graphics Context Values.
    XGCValues gcValues;
    gcValues.foreground     = XWhitePixel(m_pDisplay, 0);
    gcValues.function       = GXxor;
    gcValues.background     = XBlackPixel(m_pDisplay, 0);
    gcValues.plane_mask     = gcValues.background ^ gcValues.foreground;
    gcValues.subwindow_mode = IncludeInferiors;


    //Setup the Graphic Context.
    m_gc = XCreateGC(
        m_pDisplay,
        m_rootWindow,
        GCFunction  | GCForeground | GCBackground | GCSubwindowMode,
        &gcValues
    );
}


////////////////////////////////////////////////////////////////////////////////
// Inputs                                                                     //
////////////////////////////////////////////////////////////////////////////////
void SS::grapInput()
{
    if((XGrabPointer(
            m_pDisplay,
            m_rootWindow,
            false,
            ButtonMotionMask | ButtonPressMask | ButtonReleaseMask,
            GrabModeAsync,
            GrabModeAsync,
            m_rootWindow,
            m_cursor,
            CurrentTime) != GrabSuccess))
    {
        exit(EXIT_FAILURE);
    }

    if((XGrabKeyboard(
            m_pDisplay,
            m_rootWindow,
            false,
            GrabModeAsync,
            GrabModeAsync,
            CurrentTime) != GrabSuccess))
    {
        exit(EXIT_FAILURE);
    }
}

void SS::ungrabInput()
{
    XUngrabPointer (m_pDisplay, CurrentTime);
    XUngrabKeyboard(m_pDisplay, CurrentTime);
}

void SS::onMouseMove()
{
    if(m_trackState != TrackState::Tracking)
        return;

    //Clear.
    drawRegion(m_rectX, m_rectY, m_rectW, m_rectH);

    //Calculate the Draw Rect.
    m_rectX = std::min(m_event.xmotion.x, m_startPosX);
    m_rectY = std::min(m_event.xmotion.y, m_startPosY);

    m_rectW = std::max(m_event.xmotion.x, m_startPosX) - m_rectX;
    m_rectH = std::max(m_event.xmotion.y, m_startPosY) - m_rectY;

    //Draw the selection.
    drawRegion(m_rectX, m_rectY, m_rectW, m_rectH);
    XFlush(m_pDisplay);
}

void SS::onMousePress()
{
    m_trackState = TrackState::Tracking;

    m_startPosX = m_event.xbutton.x;
    m_startPosY = m_event.xbutton.y;
}

void SS::onMouseRelease()
{
    m_trackState = TrackState::Done;
}


////////////////////////////////////////////////////////////////////////////////
// Helpers                                                                    //
////////////////////////////////////////////////////////////////////////////////
void SS::drawRegion(int x, int y, int w, int h)
{
    XDrawRectangle(
        m_pDisplay,
        m_rootWindow,
        m_gc,
        x, y,
        w, h
    );
}

std::string SS::createPictureFullpath()
{
    //Get the user's desktop directory.
    // Try get the evironment var if fails
    // try get from passwd database if still
    // fails, so there's nothing that we can do.
    const char *pHomePath = nullptr;
    if((pHomePath = getenv("HOME")) == nullptr)
        pHomePath = getpwuid(getuid())->pw_dir;

    if(!pHomePath)
        exit(EXIT_FAILURE);


    //Get the current time.
    timeval currTime;
    gettimeofday(&currTime, nullptr);

    char dateTimeNow[100];
    strftime(
        dateTimeNow,
        sizeof(dateTimeNow)-1,
        "%c",
        localtime(&currTime.tv_sec)
    );


    //Build the fullpath.
    std::stringstream ss;
    ss << pHomePath
       << "/Desktop/"
       << dateTimeNow
       << "(" << currTime.tv_usec / 1000 << ")"//milliseconds
       << ".png";

    return ss.str();
}
