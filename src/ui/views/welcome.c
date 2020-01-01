#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
#include <LCUI/timer.h>
#include "welcome.h"

static LCUI_WidgetPrototype welcome_proto;

static void WelcomeView_OnTimer(void *arg)
{
	LCUI_WidgetEventRec e;

	LCUI_InitWidgetEvent(&e, "PageLoaded");
	e.cancel_bubble = FALSE;
	Widget_TriggerEvent(arg, &e, NULL);
}

static void WelcomeView_OnInit(LCUI_Widget w)
{
	LCUI_Widget wrapper;

	wrapper = LCUIBuilder_LoadFile("assets/views/welcome.xml");
	if (wrapper) {
		Widget_Append(w, wrapper);
		Widget_Unwrap(wrapper);
	}
	Widget_AddData(w, welcome_proto, 0);
	Widget_AddClass(w, "v-welcome");
	Widget_SetTitleW(w, L"Welcome!");
	LCUI_SetTimeout(1000, WelcomeView_OnTimer, w);
}

void UI_InitWelcomeView(void)
{
	welcome_proto = LCUIWidget_NewPrototype("welcome", NULL);
	welcome_proto->init = WelcomeView_OnInit;
}
