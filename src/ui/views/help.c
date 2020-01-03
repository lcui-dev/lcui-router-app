#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
#include <LCUI/timer.h>
#include "home.h"

static LCUI_WidgetPrototype help_proto;

static void HelpView_OnTimer(void *arg)
{
	LCUI_WidgetEventRec e;

	LCUI_InitWidgetEvent(&e, "PageLoaded");
	e.cancel_bubble = FALSE;
	Widget_TriggerEvent(arg, &e, NULL);
}

static void HelpView_OnInit(LCUI_Widget w)
{
	LCUI_Widget wrapper;

	wrapper = LCUIBuilder_LoadFile("assets/views/help.xml");
	if (wrapper) {
		Widget_Append(w, wrapper);
		Widget_Unwrap(wrapper);
	}
	Widget_AddData(w, help_proto, 0);
	Widget_AddClass(w, "v-help");
	Widget_SetTitleW(w, L"About");
	LCUI_SetTimeout(0, HelpView_OnTimer, w);
}

void UI_InitHelpView(void)
{
	help_proto = LCUIWidget_NewPrototype("help", NULL);
	help_proto->init = HelpView_OnInit;
}
