//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright  1994-1996  Microsoft Corporation.  All Rights Reserved.
//
// PROGRAM: Generic.c
//
// PURPOSE: Illustrates the 'minimum' functionality of a well-behaved Win32 application..
//
// PLATFORMS:  Windows 95, Windows NT, Win32s
//
// FUNCTIONS:
//    WinMain() - calls initialization function, processes message loop
//    InitApplication() - Initializes window data nd registers window
//    InitInstance() -saves instance handle and creates main window
//    WindProc() Processes messages
//    MyRegisterClass() - Registers the application's window class
//
// SPECIAL INSTRUCTIONS: N/A
//
#define APPNAME "Scr_show"

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

// Makes it easier to determine appropriate code paths:
#if defined (WIN32)
   #define IS_WIN32 TRUE
#else
   #define IS_WIN32 FALSE
#endif
#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)
#define IS_WIN32S  IS_WIN32 && (BOOL)(!(IS_NT) && (LOBYTE(LOWORD(GetVersion()))<4))
#define IS_WIN95 (BOOL)(!(IS_NT) && !(IS_WIN32S)) && IS_WIN32

// Global Variables:

HINSTANCE hInst;      // current instance
char szAppName[100];  // Name of the app
char szTitle[] = "Screen show";    // The title bar text

int bHaveImage;
int image_width;
int image_height;
COLORREF *image;
int scaling_factor;
int background;

// Foward declarations of functions included in this code module:

ATOM MyRegisterClass(CONST WNDCLASS*);
BOOL InitApplication(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//
//  FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)
//
//  PURPOSE: Entry point for the application.
//
//  COMMENTS:
//
// This function initializes the application and processes the
// message loop.
//
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  int m;
  int n;
  int left;
  int top;
  int num_cells;
  MSG msg;
  HANDLE hAccelTable;
  FILE *fptr;
  COLORREF color;

  /* get scaling factor */
  for (n = 0; (lpCmdLine[n]); n++) {
    if (lpCmdLine[n] == ' ')
      break;
  }

  if (lpCmdLine[n] == ' ') {
    lpCmdLine[n++] = 0;
    sscanf(lpCmdLine,"%d",&scaling_factor);

    /* get background */
    m = n;

    for ( ; (lpCmdLine[n]); n++) {
      if (lpCmdLine[n] == ' ')
        break;
    }

    if (lpCmdLine[n] == ' ') {
      lpCmdLine[n++] = 0;
      sscanf(&lpCmdLine[m],"%d",&background);
    }
    else {
      background = COLOR_WINDOW;
      n = m;
    }
  }
  else {
    scaling_factor = 2;
    background = COLOR_WINDOW;
    n = 0;
  }

  /* read image file */
  if (lstrlen(lpCmdLine)) {
    if ((fptr = fopen(&lpCmdLine[n],"r")) != NULL) {
      fscanf(fptr,"%d",&left);
      fscanf(fptr,"%d",&top);
      fscanf(fptr,"%d",&image_width);
      fscanf(fptr,"%d",&image_height);
      num_cells = image_width * image_height;

      if ((image = (COLORREF *)malloc(num_cells * sizeof (COLORREF))) != NULL) {
        for (n = 0; n < num_cells; n++) {
          fscanf(fptr,"%x",&color);
          image[n] = color;
        }

        bHaveImage = TRUE;
      }

      fclose(fptr);
    }
  }

  // Initialize global strings
  lstrcpy (szAppName, APPNAME);

  if (!hPrevInstance) {
     // Perform instance initialization:
     if (!InitApplication(hInstance)) {
        return (FALSE);
     }
  }

  // Perform application initialization:
  if (!InitInstance(hInstance, nCmdShow)) {
     return (FALSE);
  }

  hAccelTable = LoadAccelerators (hInstance, szAppName);

  // Main message loop:
  while (GetMessage(&msg, NULL, 0, 0)) {
     if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
     }
  }

  return (msg.wParam);
}

//
//  FUNCTION: MyRegisterClass(CONST WNDCLASS*)
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
// function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(CONST WNDCLASS *lpwc)
{
   HANDLE  hMod;
   FARPROC proc;
   WNDCLASSEX wcex;

   hMod = GetModuleHandle ("USER32");
   if (hMod != NULL) {

#if defined (UNICODE)
      proc = GetProcAddress (hMod, "RegisterClassExW");
#else
      proc = GetProcAddress (hMod, "RegisterClassExA");
#endif

      if (proc != NULL) {

         wcex.style         = lpwc->style;
         wcex.lpfnWndProc   = lpwc->lpfnWndProc;
         wcex.cbClsExtra    = lpwc->cbClsExtra;
         wcex.cbWndExtra    = lpwc->cbWndExtra;
         wcex.hInstance     = lpwc->hInstance;
         wcex.hIcon         = lpwc->hIcon;
         wcex.hCursor       = lpwc->hCursor;
         wcex.hbrBackground = lpwc->hbrBackground;
                     wcex.lpszMenuName  = lpwc->lpszMenuName;
         wcex.lpszClassName = lpwc->lpszClassName;

         // Added elements for Windows 95:
         wcex.cbSize = sizeof(WNDCLASSEX);
         wcex.hIconSm = LoadIcon(wcex.hInstance, "SMALL");

         return (*proc)(&wcex);//return RegisterClassEx(&wcex);
      }
   }
   return (RegisterClass(lpwc));
}


//
//  FUNCTION: InitApplication(HANDLE)
//
//  PURPOSE: Initializes window data and registers window class
//
//  COMMENTS:
//
//       In this function, we initialize a window class by filling out a data
//       structure of type WNDCLASS and calling either RegisterClass or
//       the internal MyRegisterClass.
//
BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASS  wc;

    // Fill in window class structure with parameters that describe
    // the main window.
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon (hInstance, szAppName);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(background+1);

    // Since Windows95 has a slightly different recommended
    // format for the 'Help' menu, lets put this in the alternate menu like this:
    if (IS_WIN95)
        wc.lpszMenuName  = "WIN95";
    else
        wc.lpszMenuName  = szAppName;

    wc.lpszClassName = szAppName;

    // Register the window class and return success/failure code.
    if (IS_WIN95)
        return MyRegisterClass(&wc);
    else
        return RegisterClass(&wc);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szAppName, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

   if (!hWnd) {
      return (FALSE);
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return (TRUE);
}

void do_scr_show(HWND hWnd)
{
  int m;
  int n;
  int x_off;
  int y_off;
  int X;
  int Y;
  PAINTSTRUCT ps;
  HDC hdc;
  COLORREF color;

  hdc = BeginPaint (hWnd, &ps);

  if (bHaveImage) {
    Y = 0;

    for (y_off = 0; y_off < image_height; y_off++) {
      for (m = 0; m < scaling_factor; m++,Y++) {
        X = 0;

        for (x_off = 0; x_off < image_width; x_off++) {
          color = image[y_off * image_width + x_off];

          for (n = 0; n < scaling_factor; n++)
            SetPixel(hdc,X++,Y,color);
        }
      }
    }
  }

  EndPaint (hWnd, &ps);
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  MESSAGES:
//
// WM_COMMAND - process the application menu
// WM_PAINT - Paint the main window
// WM_DESTROY - post a quit message and return
//    WM_DISPLAYCHANGE - message sent to Plug & Play systems when the display changes
//    WM_RBUTTONDOWN - Right mouse click -- put up context menu here if appropriate
//    WM_NCRBUTTONUP - User has clicked the right button on the application's system menu
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   POINT pnt;
   HMENU hMenu;

   switch (message) {
      case WM_NCRBUTTONUP: // RightClick on windows non-client area...
         if (IS_WIN95 && SendMessage(hWnd, WM_NCHITTEST, 0, lParam) == HTSYSMENU)
         {
            // The user has clicked the right button on the applications
            // 'System Menu'. Here is where you would alter the default
            // system menu to reflect your application. Notice how the
            // explorer deals with this. For this app, we aren't doing
            // anything
            return (DefWindowProc(hWnd, message, wParam, lParam));
         } else {
            // Nothing we are interested in, allow default handling...
            return (DefWindowProc(hWnd, message, wParam, lParam));
         }
            break;

        case WM_RBUTTONDOWN: // RightClick in windows client area...
            pnt.x = LOWORD(lParam);
            pnt.y = HIWORD(lParam);
            ClientToScreen(hWnd, (LPPOINT) &pnt);
      // This is where you would determine the appropriate 'context'
      // menu to bring up. Since this app has no real functionality,
      // we will just bring up the 'Help' menu:
            hMenu = GetSubMenu (GetMenu (hWnd), 2);
            if (hMenu) {
                TrackPopupMenu (hMenu, 0, pnt.x, pnt.y, 0, hWnd, NULL);
            } else {
            // Couldn't find the menu...
                MessageBeep(0);
            }
            break;

      case WM_PAINT:
         do_scr_show(hWnd);
         break;

      case WM_DESTROY:
         // Tell WinHelp we don't need it any more...
               WinHelp (hWnd, APPNAME".HLP", HELP_QUIT,(DWORD)0);
         PostQuitMessage(0);
         break;

      default:
         return (DefWindowProc(hWnd, message, wParam, lParam));
   }
   return (0);
}
