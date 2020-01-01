
#include <stdio.h>
#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/widget/textedit.h>
#include <LCDesign.h>
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
} FrameViewRec, *FrameView;

static size_t frame_id_count = 0;
static LCUI_WidgetPrototype frame_proto;

static void BrowserView_UpdateNavbar(LCUI_Widget w)
{
	size_t index;
	size_t length;
	router_history_t *history;

	FrameView self;

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

static void FrameView_OnRouteUpdate(void *arg, const router_route_t *to,
				    const router_route_t *from)
{
	const char *path;
	FrameView self;

	self = Widget_GetData(arg, frame_proto);
	if (to) {
		path = router_route_get_full_path(to);
		TextEdit_SetText(self->input, path);
	}
	BrowserView_UpdateNavbar(arg);
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
	Widget_SetAttribute(w, "router", router_name);
	Widget_AddClass(w, "v-frame");
	Widget_AddClass(self->navbar, "c-navbar");
	Widget_AddClass(self->btn_back, "c-navbar__btn");
	Widget_AddClass(self->btn_forward, "c-navbar__btn");
	Widget_AddClass(self->btn_home, "c-navbar__btn");
	Widget_AddClass(self->btn_refresh, "c-navbar__btn");
	Widget_AddClass(self->input, "c-navbar__input");
	Widget_Append(self->navbar, self->btn_back);
	Widget_Append(self->navbar, self->btn_forward);
	Widget_Append(self->navbar, self->btn_refresh);
	Widget_Append(self->navbar, self->btn_home);
	Widget_Append(self->navbar, self->input);
	Icon_SetName(self->btn_back, "arrow-left");
	Icon_SetName(self->btn_forward, "arrow-right");
	Icon_SetName(self->btn_refresh, "refresh");
	Icon_SetName(self->btn_home, "home-outline");
	Widget_Append(w, self->navbar);
	Widget_Append(w, self->content);
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
	LCUI_WidgetEventRec e;

	Widget_Update(w);
	LCUI_InitWidgetEvent(&e, "PageLoad");
	Widget_TriggerEvent(w, &e, NULL);
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
