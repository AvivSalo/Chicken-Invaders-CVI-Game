/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: exitFunc */
#define  PANEL_CANVAS                     2       /* control type: canvas, callback function: CanvasClickFunc */
#define  PANEL_TIMER                      3       /* control type: timer, callback function: timerFunc */
#define  PANEL_TABLE                      4       /* control type: table, callback function: (none) */
#define  PANEL_VOLUME                     5       /* control type: scale, callback function: VolumeFunc */
#define  PANEL_POLKE_NUMERIC              6       /* control type: numeric, callback function: (none) */
#define  PANEL_LIFE                       7       /* control type: numeric, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CanvasClickFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exitFunc(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK timerFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK VolumeFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
