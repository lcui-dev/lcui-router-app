#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
#include <LCUI/timer.h>
#include "welcome.h"

typedef struct WelcomeViewRec_ {
	int timer;
} WelcomeViewRec, *WelcomeView;

static LCUI_WidgetPrototype welcome_proto;

static void WelcomeView_OnTimer(void *arg)
{
	WelcomeView self;
	LCUI_WidgetEventRec e;

	self = Widget_GetData(arg, welcome_proto);
	self->timer = 0;
	LCUI_InitWidgetEvent(&e, "PageLoaded");
	e.cancel_bubble = FALSE;
	Widget_TriggerEvent(arg, &e, NULL);
}

static void WelcomeView_OnInit(LCUI_Widget w)
{
	WelcomeView self;
	LCUI_Widget wrapper;

	self = Widget_AddData(w, welcome_proto, sizeof(WelcomeViewRec));
	wrapper = LCUIBuilder_LoadFile("assets/views/welcome.xml");
	if (wrapper) {
		Widget_Append(w, wrapper);
		Widget_Unwrap(wrapper);
	}
	Widget_AddData(w, welcome_proto, 0);
	Widget_AddClass(w, "v-welcome");
	Widget_SetTitleW(w, L"Welcome!");
	self->timer = LCUI_SetTimeout(500, WelcomeView_OnTimer, w);
}

static void WelcomeView_OnDestroy(LCUI_Widget w)
{
	WelcomeView self;

	self = Widget_GetData(w, welcome_proto);
	if (self->timer) {
		LCUITimer_Free(self->timer);
	}
}

void UI_InitWelcomeView(void)
{
	welcome_proto = LCUIWidget_NewPrototype("welcome", NULL);
	welcome_proto->init = WelcomeView_OnInit;
	welcome_proto->destroy = WelcomeView_OnDestroy;
}
