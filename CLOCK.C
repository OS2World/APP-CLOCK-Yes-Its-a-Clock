/*******************************************************************************
    Clock
    (c) 1995 P.Koller
*******************************************************************************/

#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clock.h"

/**** Function prototypes ****/
int main(void);
MRESULT EXPENTRY    SysMenuProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    TitleBarProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    MainDlgProc (HWND,ULONG,MPARAM,MPARAM);
void                ClockSetDateString(PDATETIME dt);

/**** Profile Strings ****/
char                pAppname[] = "Yes - It's a clock";
char                pKeyname[] = "screenposn";
DATETIME            datetime[2] = {0};
ULONG               timerID = 0;

/**** OS2 API ****/
HAB                 hab;
HWND                hWndFrame;
PFNWP               TitleDefProc;
PFNWP               SysMenuDefProc;
POINTERINFO         PointerInfo;
HPOINTER            SysMenuButton;

/**** String buffers ****/
CHAR                szTitle[30];
CHAR                szHours[10];
CHAR                szSecs[5];

/**** Update Flag ****/
BOOL                UPDATE_HOURS;
BOOL                UPDATE_DATE;
BOOL                DTSWITCH;

/****************************************************************************
   Main Program start
*****************************************************************************/
int main()
{
    HMQ   hmq;
    QMSG  qmsg;

    hab = WinInitialize (0);
    hmq = WinCreateMsgQueue (hab, 0);

    hWndFrame = WinLoadDlg (HWND_DESKTOP, HWND_DESKTOP,
        MainDlgProc, 0, IDD_CLOCK,NULL);
    TitleDefProc = WinSubclassWindow(WinWindowFromID(hWndFrame, FID_TITLEBAR),
                   (PFNWP)TitleBarProc );
    SysMenuDefProc = WinSubclassWindow(WinWindowFromID(hWndFrame, FID_SYSMENU),
                   (PFNWP)SysMenuProc );

    SysMenuButton = WinLoadPointer (HWND_DESKTOP, 0, IDD_CLOCK);
    WinQueryPointerInfo(SysMenuButton,&PointerInfo);
    WinSendMsg (hWndFrame, WM_SETICON, (MPARAM)SysMenuButton, NULL);


    while (WinGetMsg (hab, &qmsg, 0, 0, 0))
        WinDispatchMsg (hab, &qmsg);

    WinDestroyWindow (hWndFrame);
    WinDestroyMsgQueue (hmq);
    WinTerminate (hab);
    return (0);
}
/****************************************************************************
   SysMenu Dialog Procedure
*****************************************************************************/
MRESULT EXPENTRY SysMenuProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
    {
        BOOL    Handled = TRUE;
        MRESULT mReturn  = 0;
        RECTL   Rectl;
        POINTL  Pointl;
        ULONG   temp;
        HPS     hps;
        HBITMAP hbm;

        switch (msg)
            {
                case    WM_PAINT:
                    hps = WinBeginPaint(hWnd, 0, 0);
                    WinQueryWindowRect(hWnd,&Rectl);
                    WinFillRect(hps, &Rectl, CLR_BLACK);
                    hbm = PointerInfo.hbmMiniColor;
                    WinInflateRect(hab, &Rectl, -1L, -1L);
                    WinDrawBitmap(hps, hbm, NULL, (PPOINTL)&Rectl, 0L, 0L, DBM_NORMAL);
                    WinEndPaint(hps);
                    break;
                case WM_ERASEBACKGROUND:
                    WinFillRect((HPS)LONGFROMMP(mp1),PVOIDFROMMP(mp2),CLR_BLACK);
                    mReturn = MRFROMLONG(1L);
                    break;
                default:
                    Handled = FALSE;
                    break;
            }
        if (!Handled) mReturn =  (*SysMenuDefProc)(hWnd, msg, mp1, mp2);
        return (mReturn);
    }

/****************************************************************************
   Titlebar Dialog Procedure
*****************************************************************************/
MRESULT EXPENTRY TitleBarProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
    {
        BOOL    Handled = TRUE;
        MRESULT mReturn  = 0;
        RECTL   Rectl;
        POINTL  Pointl;
        
        CHARBUNDLE cb;
        FONTMETRICS Fontsz;
        PSZ     TitleText = szTitle;
        ULONG   temp;
        HPS     hps;

        switch (msg)
            {
                case    WM_SETWINDOWPARAMS:
                    if( ((WNDPARAMS*)mp1)->fsStatus & WPM_TEXT )
                        {
                            TitleText = ((WNDPARAMS*)mp1)->pszText;
                        }
                    Handled = FALSE;
                    break;
                case    TBM_SETHILITE:
                    mReturn = (MRESULT)TRUE;
                    hps = WinGetPS(hWnd);
                    WinQueryWindowRect(hWnd,&Rectl);
                    WinFillRect(hps, &Rectl, CLR_BLACK);
                    GpiQueryFontMetrics(hps,sizeof(FONTMETRICS),(PFONTMETRICS)&Fontsz);
                    Pointl.x = 10;
                    Pointl.y = (Rectl.yTop - Fontsz.lMaxAscender) / 2;
                    cb.lColor = CLR_GREEN;
                    cb.lBackColor = CLR_BLACK;
                    cb.usMixMode = BM_OVERPAINT;
                    cb.usBackMixMode = BM_OVERPAINT;
                    GpiSetAttrs(hps,PRIM_CHAR, CBB_COLOR | CBB_BACK_COLOR 
                       | CBB_MIX_MODE | CBB_BACK_MIX_MODE, 0,(PBUNDLE)&cb);
                    GpiCharStringAt(hps, &Pointl, strlen(TitleText), TitleText);
                    WinEndPaint(hps);
                    WinReleasePS(hps);
                    break;
                case    WM_PAINT:
                    hps = WinBeginPaint(hWnd, 0, 0);
                    WinQueryWindowRect(hWnd,&Rectl);
                    WinFillRect(hps, &Rectl, CLR_BLACK);
                    GpiQueryFontMetrics(hps,sizeof(FONTMETRICS),(PFONTMETRICS)&Fontsz);
                    Pointl.x = 10;
                    Pointl.y = (Rectl.yTop - Fontsz.lMaxAscender) / 2;
                    cb.lColor = CLR_GREEN;
                    cb.lBackColor = CLR_BLACK;
                    cb.usMixMode = BM_OVERPAINT;
                    cb.usBackMixMode = BM_OVERPAINT;
                    GpiSetAttrs(hps,PRIM_CHAR, CBB_COLOR | CBB_BACK_COLOR 
                       | CBB_MIX_MODE | CBB_BACK_MIX_MODE, 0,(PBUNDLE)&cb);
                    GpiCharStringAt(hps, &Pointl, strlen(TitleText), TitleText);
                    WinEndPaint(hps);
                    break;
                default:
                    Handled = FALSE;
                    break;
            }
        if (!Handled) mReturn =  (*TitleDefProc)(hWnd, msg, mp1, mp2);
        return (mReturn);
    }

/****************************************************************************
   Main Message Cue Dialog Procedure
*****************************************************************************/
MRESULT EXPENTRY MainDlgProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
    {
        BOOL    Handled = TRUE;
        MRESULT mReturn  = 0;
        ULONG   ulScrWidth, ulScrHeight;
        RECTL   Rectl;
        POINTL  Pointl;
        HINI    hini;
        ULONG   temp;

        switch (msg)
            {

                /* Called on startup */
                case WM_INITDLG:
                    ulScrWidth  = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);
                    ulScrHeight = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);
                    UPDATE_HOURS = TRUE;
                    UPDATE_DATE = TRUE;
                    DTSWITCH = FALSE;
                    hini = HINI_USERPROFILE;
                    Pointl.x = Pointl.y = 10;
                    if(PrfQueryProfileSize(hini,pAppname,pKeyname,(PULONG)&temp) &&temp)
                        { 
                            if(temp == sizeof(POINTL)) PrfQueryProfileData(hini, pAppname,
                                                       pKeyname, &Pointl, (PULONG)&temp);
                        }
                    WinQueryWindowRect (hWnd, &Rectl);
/*
                    WinSetWindowPos (hWnd, HWND_TOP, Pointl.x,
                            Pointl.y, 0, 0, SWP_SHOW | SWP_MOVE | SWP_ACTIVATE);
*/
                    WinSetWindowPos (hWnd, HWND_TOP, (ulScrWidth-Rectl.xRight)/2,
                            (ulScrHeight-Rectl.yTop) - 10, 0, 0, SWP_SHOW | SWP_MOVE | SWP_ACTIVATE);
                    /* A neat little trick to float the window on top... */
                    WinFocusChange(HWND_DESKTOP,HWND_TOP,0);
                    timerID = WinStartTimer(hab, hWnd, 1, 1000L);
                    break;
                case WM_TIMER:
                    if(SHORT1FROMMP(mp1) == timerID)
                        {
                            while(DosGetDateTime( &(datetime[DTSWITCH?1:0]) ));
                            UPDATE_HOURS = (    (datetime[DTSWITCH?1:0].minutes != datetime[DTSWITCH?0:1].minutes)
                                             || (datetime[DTSWITCH?1:0].hours   != datetime[DTSWITCH?0:1].hours  ) );
                            UPDATE_DATE |= (    (datetime[DTSWITCH?1:0].day     != datetime[DTSWITCH?0:1].day    )
                                             || (datetime[DTSWITCH?1:0].weekday != datetime[DTSWITCH?0:1].weekday)
                                             || (datetime[DTSWITCH?1:0].month   != datetime[DTSWITCH?0:1].month  )
                                             || (datetime[DTSWITCH?1:0].year    != datetime[DTSWITCH?0:1].year   ) );
                            ClockSetDateString( &(datetime[DTSWITCH?1:0]));
                            DTSWITCH = !DTSWITCH;
                            if(UPDATE_HOURS)
                                {
                                    WinSetDlgItemText (hWnd, IDD_TIME, szHours);
                                    if(UPDATE_DATE)
                                        { 
                                            WinSetWindowText(hWndFrame,(PSZ)szTitle);
                                            UPDATE_DATE = FALSE;
                                        }
                                            UPDATE_HOURS = FALSE;
                                }
                            WinSetDlgItemText (hWnd, IDD_SEC, szSecs);
                        }
                    Handled = FALSE;
                    break;
                case WM_SYSCOMMAND:
                case WM_COMMAND:
                    switch (LOUSHORT(mp1))
                      {
                            case SC_CLOSE:
                                hini = HINI_USERPROFILE;
                                WinStopTimer(hab, hWnd, timerID);
                                Pointl.x = Pointl.y = 0;
                                WinMapWindowPoints(hWnd, HWND_DESKTOP, &Pointl, 1);
                                temp = sizeof(POINTL);
                                PrfWriteProfileData(hini, pAppname, pKeyname, &Pointl, temp);
                                WinPostMsg(hWnd, WM_QUIT, 0L, 0L);
                                break;
                            default:
                                Handled = (msg == WM_COMMAND);
                                break;
                        }
                    break; // forget this break and you are in dead trouble !!!
                default:
                    Handled = FALSE;
                    break;
            }
        if (!Handled) mReturn = WinDefDlgProc (hWnd, msg, mp1, mp2);
        return (mReturn);
    }

void ClockSetDateString(PDATETIME dt)
    {
        ULONG   x;
        char    temp[10];

        sprintf(szSecs, "%.2d", dt->seconds);
        if(UPDATE_HOURS)
        {
        sprintf(szHours, "%.2d : %.2d", dt->hours, dt->minutes);
            if(UPDATE_DATE)
            {
            switch(dt->weekday)
                {
                    case 0:
                        strcat(szTitle,"Sun ");
                        break;
                    case 1:
                        strcat(szTitle,"Mon ");
                        break;
                    case 2:
                        strcat(szTitle,"Tue ");
                        break;
                    case 3:
                        strcat(szTitle,"Wed ");
                        break;
                    case 4:
                        strcat(szTitle,"Thu ");
                        break;
                    case 5:
                        strcat(szTitle,"Fri ");
                        break;
                    case 6:
                        strcat(szTitle,"Sat ");
                        break;
                }
            _itoa(dt->day, szTitle + 4, 10);
            switch(dt->day)
                {
                    case 1:
                    case 21:
                    case 31:
                        strcat(szTitle,"st ");
                        break;
                    case 2:
                    case 22:
                        strcat(szTitle,"nd ");
                        break;
                    case 3:
                    case 23:
                        strcat(szTitle,"rd ");
                        break;
                    default:
                        strcat(szTitle,"th ");
                        break;
    
                }
            switch(dt->month)
                {
                    case 1:
                        strcat(szTitle,"Jan ");
                        break;
                    case 2:
                        strcat(szTitle,"Feb ");
                        break;
                    case 3:
                        strcat(szTitle,"Mar ");
                        break;
                    case 4:
                        strcat(szTitle,"Apr ");
                        break;
                    case 5:
                        strcat(szTitle,"May ");
                        break;
                    case 6:
                        strcat(szTitle,"Jun ");
                        break;
                    case 7:
                        strcat(szTitle,"Jul ");
                        break;
                    case 8:
                        strcat(szTitle,"Aug ");
                        break;
                    case 9:
                        strcat(szTitle,"Sep ");
                        break;
                    case 10:
                        strcat(szTitle,"Oct ");
                        break;
                    case 11:
                        strcat(szTitle,"Nov ");
                        break;
                    case 12:
                    default:
                        strcat(szTitle,"Dec ");
                        break;
                }
            _itoa(dt->year, temp, 10);
            strcat(szTitle, temp);
            }
        }
    }
