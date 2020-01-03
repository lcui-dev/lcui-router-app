#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
#include <LCUI/timer.h>
#include "home.h"

static LCUI_WidgetPrototype home_proto;

static void HomeView_OnTimer(void *arg)
{
	LCUI_WidgetEventRec e;

	LCUI_InitWidgetEvent(&e, "PageLoaded");
	e.cancel_bubble = FALSE;
	Widget_TriggerEvent(arg, &e, NULL);
}

static void HomeView_OnInit(LCUI_Widget w)
{
	LCUI_Widget wrapper;

	wrapper = LCUIBuilder_LoadFile("assets/views/home.xml");
	if (wrapper) {
		Widget_Append(w, wrapper);
		Widget_Unwrap(wrapper);
	}
	Widget_AddData(w, home_proto, 0);
	Widget_AddClass(w, "v-home");
	Widget_SetTitleW(w, L"New tab");
	LCUI_SetTimeout(0, HomeView_OnTimer, w);
}

void UI_InitHomeView(void)
{
	home_proto = LCUIWidget_NewPrototype("home", NULL);
	home_proto->init = HomeView_OnInit;
}
