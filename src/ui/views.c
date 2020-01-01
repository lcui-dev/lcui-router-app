#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include "views.h"
#include "views/frame.h"
#include "views/browser.h"
#include "views/welcome.h"

void UI_InitViews(void)
{
	UI_InitWelcomeView();
	UI_InitBrowserView();
	UI_InitFrameView();
}
