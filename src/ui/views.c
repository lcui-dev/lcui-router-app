#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include "views.h"
#include "views/frame.h"
#include "views/browser.h"
#include "views/welcome.h"
#include "views/home.h"
#include "views/help.h"
#include "views/file.h"
#include "views/notfound.h"

void UI_InitViews(void)
{
	UI_InitNotFoundView();
	UI_InitFileView();
	UI_InitHelpView();
	UI_InitHomeView();
	UI_InitWelcomeView();
	UI_InitBrowserView();
	UI_InitFrameView();
}
