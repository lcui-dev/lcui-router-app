#include <LCUI.h>
#include <LCDesign.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
#include "ui/views/browser.h"
#include "version.h"
#include "ui.h"

int UI_Init(void)
{
	LCUI_Widget root;
	LCUI_Widget wrapper;
    LCUI_Widget browser;

	LCUI_Init();
	LCDesign_Init();
	UI_InitComponents();
	UI_InitViews();
	wrapper = LCUIBuilder_LoadFile("assets/views/app.xml");
	if (!wrapper) {
		return -1;
	}
	root = LCUIWidget_GetRoot();
	browser = LCUIWidget_GetById("browser");
	BrowserView_Active(browser, BrowserView_Load(browser, "/"));
	Widget_SetTitleW(root, L"Browser demo");
	Widget_Append(root, wrapper);
	Widget_Unwrap(wrapper);
	return 0;
}

int UI_Run(void)
{
    return LCUI_Main();
}
