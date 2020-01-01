#include <LCUI.h>
#include <lcui-router.h>
#include <lcui-router-link.h>
#include <lcui-router-view.h>

#include "components.h"
#include "components/about.h"
#include "components/frame-tab.h"

void UI_InitComponents(void)
{
	UI_InitAbout();
	UI_InitRouterLink();
	UI_InitRouterView();
	UI_InitFrameTab();
}
