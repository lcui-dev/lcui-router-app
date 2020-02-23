#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
#include <LCUI/timer.h>
#include "home.h"

static LCUI_WidgetPrototype notfound_proto;

static void NotFoundView_OnTimer(void *arg)
{
	LCUI_WidgetEventRec e;

	LCUI_InitWidgetEvent(&e, "PageLoaded");
	e.cancel_bubble = FALSE;
	Widget_TriggerEvent(arg, &e, NULL);
}

static void NotFoundView_OnInit(LCUI_Widget w)
{
	LCUI_Widget wrapper;

	wrapper = LCUIBuilder_LoadFile("assets/views/notfound.xml");
	if (wrapper) {
		Widget_Append(w, wrapper);
		Widget_Unwrap(wrapper);
	}
	Widget_AddData(w, notfound_proto, 0);
	Widget_AddClass(w, "v-notfound");
	Widget_SetTitleW(w, L"Not Found!");
	LCUI_SetTimeout(0, NotFoundView_OnTimer, w);
}

void UI_InitNotFoundView(void)
{
	notfound_proto = LCUIWidget_NewPrototype("notfound", NULL);
	notfound_proto->init = NotFoundView_OnInit;
}
