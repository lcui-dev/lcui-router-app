#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include "notfound.h"

typedef struct NotfoundViewRec_ {
	int this_is_example_data;
	// Your view data
	// ...
} NotfoundViewRec, *NotfoundView;

static LCUI_WidgetPrototype notfound_proto;

static NotfoundView_OnReady(LCUI_Widget w, LCUI_WidgetEvent e, void *arg)
{
	NotfoundView self;

	self = Widget_GetData(w, notfound_proto);
	// Do something after this view is ready
	// ...
	Widget_UnbindEvent(w, "ready", NotfoundView_OnReady);
}

static void NotfoundView_OnInit(LCUI_Widget w)
{
	NotfoundView self;

	self = Widget_AddData(w, notfound_proto, sizeof(NotfoundViewRec));
	self->this_is_example_data = 32;
	Widget_AddClass(w, "v-notfound");
	Widget_BindEvent(w, "ready", NotfoundView_OnReady, NULL, NULL);
}

static void NotfoundView_OnDestroy(LCUI_Widget w)
{
	NotfoundView self;

	self = Widget_GetData(w, notfound_proto);
}

void UI_InitNotfoundView(void)
{
	notfound_proto = LCUIWidget_NewPrototype("notfound", NULL);
	notfound_proto->init = NotfoundView_OnInit;
	notfound_proto->destroy = NotfoundView_OnDestroy;
}
