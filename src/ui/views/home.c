#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/widget/textview.h>
#include <LCUI/gui/widget/textedit.h>
#include <LCUI/timer.h>
#include "home.h"
#include "store.h"

typedef struct HomeViewRec_ {
	LCUI_Widget input;
	LCUI_Widget feedback;
	LCUI_Widget tabs;
	LCUI_Widget tab_content;

	struct store store;
	int feedback_timer;
} HomeViewRec, *HomeView;

LCUI_WidgetPrototype home_view_proto;

static void OnFeedbackTimeout(void *arg)
{
	HomeView self;

	self = Widget_GetData(arg, home_view_proto);
	Widget_Hide(self->feedback);
	self->feedback_timer = 0;
}

static void OnBtnSaveClick(LCUI_Widget w, LCUI_WidgetEvent e, void *unused)
{
	size_t len;
	wchar_t wcs[256];

	HomeView self;

	self = Widget_GetData(e->data, home_view_proto);
	len = TextEdit_GetTextW(self->input, 0, 255, wcs);
	wcs[len] = 0;

	len = LCUI_EncodeUTF8String(self->store.message, wcs, 255);
	self->store.message[len] = 0;
	store_save(&self->store);
	if (self->feedback_timer) {
		LCUITimer_Free(self->feedback_timer);
	}
	Widget_Show(self->feedback);
	self->feedback_timer =
	    LCUI_SetTimeout(2000, OnFeedbackTimeout, e->data);
}

static void HomeView_OnEachTab(LCUI_Widget w, void *arg)
{
	const char *target;

	if (!Widget_HasClass(w, "nav-link")) {
		return;
	}
	target = Widget_GetAttribute(w, "data-target");
	if (target && strcmp(target, arg) == 0) {
		Widget_AddClass(w, "active");
	} else {
		Widget_RemoveClass(w, "active");
	}
}

static void HomeView_SetTab(LCUI_Widget w, const char *tabname)
{
	HomeView self;
	LCUI_Widget child;
	LinkedListNode *node;

	self = Widget_GetData(w, home_view_proto);
	Widget_Each(self->tabs, HomeView_OnEachTab, (void *)tabname);
	for (LinkedList_Each(node, &self->tab_content->children)) {
		child = node->data;
		if (!Widget_HasClass(child, "tab-pane")) {
			return;
		}
		if (child->id && strcmp(child->id, tabname) == 0) {
			Widget_AddClass(child, "active");
		} else {
			Widget_RemoveClass(child, "active");
		}
	}
}

static void HomeView_OnTabClick(LCUI_Widget w, LCUI_WidgetEvent e, void *arg)
{
	if (Widget_HasClass(e->target, "nav-link")) {
		HomeView_SetTab(e->data,
				Widget_GetAttribute(e->target, "data-target"));
	}
}

static void HomeView_OnReady(LCUI_Widget w, LCUI_WidgetEvent e, void *arg)
{
	Dict *dict;
	HomeView self;
	LCUI_Widget btn;

	self = Widget_GetData(w, home_view_proto);
	if (store_load(&self->store) != 0) {
		store_init(&self->store);
		store_save(&self->store);
	}
	dict = Widget_CollectReferences(w);
	btn = Dict_FetchValue(dict, "btn-save-message");
	self->tabs = Dict_FetchValue(dict, "tabs");
	self->tab_content = Dict_FetchValue(dict, "tab-content");
	self->input = Dict_FetchValue(dict, "input-message");
	self->feedback = Dict_FetchValue(dict, "feedback");
	Widget_BindEvent(btn, "click", OnBtnSaveClick, w, NULL);
	TextEdit_SetText(self->input, self->store.message);
	Widget_Hide(self->feedback);
	Widget_UnbindEvent(w, "ready", HomeView_OnReady);
	Widget_BindEvent(self->tabs, "click", HomeView_OnTabClick, w, NULL);
	Dict_Release(dict);
}

static void HomeView_OnInit(LCUI_Widget w)
{
	Widget_AddClass(w, "view v-home");
	Widget_AddData(w, home_view_proto, sizeof(HomeViewRec));
	Widget_BindEvent(w, "ready", HomeView_OnReady, NULL, NULL);
}

void UI_InitHomeView(void)
{
	home_view_proto = LCUIWidget_NewPrototype("home", NULL);
	home_view_proto->init = HomeView_OnInit;
}
