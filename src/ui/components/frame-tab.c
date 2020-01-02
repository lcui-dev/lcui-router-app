#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/widget/textview.h>
#include <LCDesign.h>
#include "frame-tab.h"

typedef struct FrameTabRec_ {
	LCUI_Widget icon;
	LCUI_Widget spinner;
	LCUI_Widget text;
	LCUI_Widget close;
} FrameTabRec, *FrameTab;

static LCUI_WidgetPrototype frame_tab_proto;

static void FrameTab_OnClose(LCUI_Widget w, LCUI_WidgetEvent e, void *arg)
{
	LCUI_WidgetEventRec ev;

	LCUI_InitWidgetEvent(&ev, "TabClose");
	Widget_TriggerEvent(e->data, &ev, NULL);
}

static void FrameTab_OnInit(LCUI_Widget w)
{
	FrameTab self;

	self = Widget_AddData(w, frame_tab_proto, sizeof(FrameTabRec));
	self->icon = LCUIWidget_New("icon");
	self->spinner = LCUIWidget_New("spinner");
	self->text = LCUIWidget_New("textview");
	self->close = LCUIWidget_New("icon");
	Icon_SetName(self->icon, "earth");
	Icon_SetName(self->close, "close");
	Widget_AddClass(self->icon, "c-frame-tab__icon");
	Widget_AddClass(self->spinner, "c-frame-tab__spinner");
	Widget_AddClass(self->text, "c-frame-tab__text");
	Widget_AddClass(self->close, "c-frame-tab__close");
	Widget_Append(w, self->icon);
	Widget_Append(w, self->spinner);
	Widget_Append(w, self->text);
	Widget_Append(w, self->close);
	Widget_AddClass(w, "c-frame-tab");
	Widget_BindEvent(self->close, "click", FrameTab_OnClose, w, NULL);
}

static void FrameTab_OnDestroy(LCUI_Widget w)
{
	FrameTab self;

	self = Widget_GetData(w, frame_tab_proto);
}

void FrameTab_SetActive(LCUI_Widget w, LCUI_BOOL active)
{
	if (active) {
		Widget_AddClass(w, "active");
	} else {
		Widget_RemoveClass(w, "active");
	}
}

void FrameTab_SetLoading(LCUI_Widget w, LCUI_BOOL loading)
{
	if (loading) {
		Widget_AddClass(w, "loading");
	} else {
		Widget_RemoveClass(w, "loading");
	}
}

void FrameTab_SetTextW(LCUI_Widget w, const wchar_t *text)
{
	FrameTab self;

	self = Widget_GetData(w, frame_tab_proto);
	TextView_SetTextW(self->text, text);
}

void UI_InitFrameTab(void)
{
	frame_tab_proto = LCUIWidget_NewPrototype("frame-tab", NULL);
	frame_tab_proto->init = FrameTab_OnInit;
	frame_tab_proto->destroy = FrameTab_OnDestroy;
}
