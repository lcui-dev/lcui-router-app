#include <stdio.h>
#include <LCUI.h>
#include <LCDesign.h>
#include <LCUI/input.h>
#include <LCUI/timer.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/widget/textedit.h>
#include <LCUI/gui/widget/scrollbar.h>
#include "frame.h"
#include "router.h"

typedef struct FrameViewRec_ {
	size_t id;
	router_t *router;
	router_watcher_t *watcher;
	LCUI_Widget navbar;
	LCUI_Widget btn_back;
	LCUI_Widget btn_forward;
	LCUI_Widget btn_refresh;
	LCUI_Widget btn_home;
	LCUI_Widget input;
	LCUI_Widget content;
	LCUI_Widget client;
	LCUI_Widget scrollbar;
	LCUI_Widget hscrollbar;
} FrameViewRec, *FrameView;

static size_t frame_id_count = 0;
static LCUI_WidgetPrototype frame_proto;

static void BrowserView_UpdateNavbar(void *arg)
{
	size_t index;
	size_t length;
	router_history_t *history;

	FrameView self;
	LCUI_Widget w;

	w = arg;
	self = Widget_GetData(w, frame_proto);
	history = router_get_history(self->router);
	index = router_history_get_index(history);
	length = router_history_get_length(history);
	if (index > 0) {
		Widget_SetDisabled(self->btn_back, FALSE);
	} else {
		Widget_SetDisabled(self->btn_back, TRUE);
	}
	if (index + 1 < length) {
		Widget_SetDisabled(self->btn_forward, FALSE);
	} else {
		Widget_SetDisabled(self->btn_forward, TRUE);
	}
}

static void FrameView_OnBtnBackClick(LCUI_Widget w, LCUI_WidgetEvent e,
				     void *arg)
{
	if (w->disabled) {
		return;
	}
	router_back(((FrameView)e->data)->router);
}

static void FrameView_OnBtnForwardClick(LCUI_Widget w, LCUI_WidgetEvent e,
					void *arg)
{
	if (w->disabled) {
		return;
	}
	router_forward(((FrameView)e->data)->router);
}

static void FrameView_OnBtnRefreshClick(LCUI_Widget w, LCUI_WidgetEvent e,
					void *arg)
{
	router_go(((FrameView)e->data)->router, 0);
}

static void FrameView_OnBtnHomeClick(LCUI_Widget w, LCUI_WidgetEvent e,
				     void *arg)
{
	router_location_t *location;
	FrameView self;

	self = e->data;
	location = router_location_create(NULL, "/welcome");
	router_push(self->router, location);
	router_location_destroy(location);
}

static void FrameView_OnInputKeydown(LCUI_Widget w, LCUI_WidgetEvent e,
				     void *arg)
{
	char *path;
	size_t len;
	wchar_t raw_path[1024];
	router_location_t *location;
	FrameView self;

	if (e->key.code != LCUI_KEY_ENTER) {
		return;
	}
	self = e->data;
	len = TextEdit_GetTextW(self->input, 0, 1023, raw_path);
	raw_path[len] = 0;
	len = LCUI_EncodeUTF8String(NULL, raw_path, 1023);
	path = malloc(len * (sizeof(char) + 1));
	len = LCUI_EncodeUTF8String(path, raw_path, 1023);
	path[len] = 0;
	location = router_location_create(NULL, path);
	router_push(self->router, location);
	router_location_destroy(location);
}

static void FrameView_OnRouteUpdate(void *arg, const router_route_t *to,
				    const router_route_t *from)
{
	const char *path;
	FrameView self;
	LCUI_WidgetEventRec e;

	self = Widget_GetData(arg, frame_proto);
	if (to) {
		path = router_route_get_full_path(to);
		TextEdit_SetText(self->input, path);
	}
	LCUI_SetTimeout(0, BrowserView_UpdateNavbar, arg);
	LCUI_InitWidgetEvent(&e, "PageLoad");
	Widget_TriggerEvent(arg, &e, NULL);
}

static void FrameView_OnInit(LCUI_Widget w)
{
	FrameView self;
	char router_name[256];

	self = Widget_AddData(w, frame_proto, sizeof(FrameViewRec));
	self->id = ++frame_id_count;
	sprintf(router_name, "frame/%zu", self->id);
	self->router = router_create_with_config(router_name, NULL);
	self->watcher = router_watch(self->router, FrameView_OnRouteUpdate, w);
	self->navbar = LCUIWidget_New(NULL);
	self->btn_back = LCUIWidget_New("icon");
	self->btn_forward = LCUIWidget_New("icon");
	self->btn_home = LCUIWidget_New("icon");
	self->btn_refresh = LCUIWidget_New("icon");
	self->input = LCUIWidget_New("textedit");
	self->content = LCUIWidget_New("router-view");
	self->client = LCUIWidget_New(NULL);
	self->scrollbar = LCUIWidget_New("scrollbar");
	ScrollBar_BindTarget(self->scrollbar, self->content);
	Widget_SetAttribute(w, "router", router_name);
	Widget_AddClass(w, "v-frame");
	Widget_AddClass(self->navbar, "c-navbar v-frame__navbar");
	Widget_AddClass(self->btn_back, "c-navbar__btn");
	Widget_AddClass(self->btn_forward, "c-navbar__btn");
	Widget_AddClass(self->btn_home, "c-navbar__btn");
	Widget_AddClass(self->btn_refresh, "c-navbar__btn");
	Widget_AddClass(self->input, "c-navbar__input");
	Widget_AddClass(self->content, "v-frame__content");
	Widget_AddClass(self->client, "v-frame__client");
	Widget_Append(self->navbar, self->btn_back);
	Widget_Append(self->navbar, self->btn_forward);
	Widget_Append(self->navbar, self->btn_refresh);
	Widget_Append(self->navbar, self->btn_home);
	Widget_Append(self->navbar, self->input);
	Widget_Append(self->client, self->content);
	Widget_Append(self->client, self->scrollbar);
	Icon_SetName(self->btn_back, "arrow-left");
	Icon_SetName(self->btn_forward, "arrow-right");
	Icon_SetName(self->btn_refresh, "refresh");
	Icon_SetName(self->btn_home, "home-outline");
	Widget_BindEvent(self->btn_back, "click", FrameView_OnBtnBackClick,
			 self, NULL);
	Widget_BindEvent(self->btn_forward, "click",
			 FrameView_OnBtnForwardClick, self, NULL);
	Widget_BindEvent(self->btn_home, "click", FrameView_OnBtnHomeClick,
			 self, NULL);
	Widget_BindEvent(self->btn_refresh, "click",
			 FrameView_OnBtnRefreshClick, self, NULL);
	Widget_BindEvent(self->input, "keydown", FrameView_OnInputKeydown, self, NULL);
	Widget_Append(w, self->navbar);
	Widget_Append(w, self->client);
	Widget_SetId(w, router_name);
	FrameView_OnRouteUpdate(w, router_get_current_route(self->router),
				NULL);
}

static void FrameView_OnDestroy(LCUI_Widget w)
{
	FrameView self;

	self = Widget_GetData(w, frame_proto);
	router_unwatch(self->router, self->watcher);
	self->watcher = NULL;
}

void FrameView_Load(LCUI_Widget w, const char *path)
{
	router_location_t *location;
	FrameView self;

	Widget_Update(w);
	location = router_location_create(NULL, path);
	self = Widget_GetData(w, frame_proto);
	router_push(self->router, location);
	router_location_destroy(location);
}

void UI_InitFrameView(void)
{
	frame_proto = LCUIWidget_NewPrototype("frame", NULL);
	frame_proto->init = FrameView_OnInit;
	frame_proto->destroy = FrameView_OnDestroy;
}
