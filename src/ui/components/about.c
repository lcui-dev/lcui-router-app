#include <stdio.h>
#include <LCUI.h>
#include <LCDesign.h>
#include <lcui-router.h>
#include <LCUI/gui/widget/textview.h>
#include "version.h"
#include "about.h"

static LCUI_WidgetPrototype about_proto;

void About_OnInit(LCUI_Widget w)
{
	LCUI_Widget icon;
	LCUI_Widget name;
	LCUI_Widget version;
	LCUI_Widget os_info;
	LCUI_Widget lcui_version;
	LCUI_Widget lcui_router_version;
	LCUI_Widget lc_design_version;
	LCUI_Widget description;
	LCUI_Widget time;
	LCUI_Widget meta;
	LCUI_Widget homepage;
	LCUI_Widget bugs_url;
	LCUI_Widget item;

	char os_info_str[32];
	char lcui_version_str[64];
	char lcui_router_version_str[64];
	char lc_design_version_str[64];
	char version_str[32];
	char time_str[64];

	icon = LCUIWidget_New(NULL);
	name = LCUIWidget_New("textview");
	item = LCUIWidget_New(NULL);
	TextView_SetTextW(name, APP_NAME);
	Widget_AddClass(icon, "c-about__icon");
	Widget_AddClass(name, "c-about__name");
	Widget_AddClass(item, "c-about__item");
	Widget_Append(item, icon);
	Widget_Append(item, name);
	Widget_Append(w, item);

	time = LCUIWidget_New("textview");
	version = LCUIWidget_New("textview");
	os_info = LCUIWidget_New("textview");
	lcui_version = LCUIWidget_New("textview");
	lcui_router_version = LCUIWidget_New("textview");
	lc_design_version = LCUIWidget_New("textview");
	description = LCUIWidget_New("textview");
	meta = LCUIWidget_New(NULL);
	item = LCUIWidget_New(NULL);
	sprintf(version_str, "Version: %s", APP_VERSION);
	sprintf(time_str, "Build at: %s", APP_BUILD_TIME);
	sprintf(lcui_version_str, "LCUI: %s", LCUI_GetVersion());
	sprintf(lcui_router_version_str, "LCUI Router: %s", router_get_version());
	sprintf(lc_design_version_str, "LC Design: %s", LCDesign_GetVersion());
	sprintf(os_info_str, "OS: %s", OS_INFO);
	TextView_SetTextW(description, APP_DESCRIPTION);
	TextView_SetText(version, version_str);
	TextView_SetText(time, time_str);
	TextView_SetText(lcui_version, lcui_version_str);
	TextView_SetText(lcui_router_version, lcui_router_version_str);
	TextView_SetText(lc_design_version, lc_design_version_str);
	TextView_SetText(os_info, os_info_str);
	Widget_AddClass(item, "c-about__item");
	Widget_AddClass(meta, "c-about__meta");
	Widget_Append(meta, version);
	Widget_Append(meta, time);
	Widget_Append(meta, lcui_version);
	Widget_Append(meta, lcui_router_version);
	Widget_Append(meta, lc_design_version);
	Widget_Append(meta, os_info);
	Widget_Append(item, description);
	Widget_Append(item, meta);
	Widget_Append(w, item);

	icon = LCUIWidget_New("icon");
	homepage = LCUIWidget_New("a");
	Icon_SetName(icon, "open-in-new");
	Widget_AddClass(homepage, "c-about__item");
	TextView_SetText(homepage, "Visit homepage");
	Widget_SetAttribute(homepage, "href", APP_HOMEPAGE);
	Widget_Append(homepage, icon);
	Widget_Append(w, homepage);

	icon = LCUIWidget_New("icon");
	bugs_url = LCUIWidget_New("a");
	Icon_SetName(icon, "open-in-new");
	Widget_AddClass(bugs_url, "c-about__item");
	TextView_SetText(bugs_url, "Report an issue");
	Widget_SetAttribute(bugs_url, "href", APP_BUGS_URL);
	Widget_Append(bugs_url, icon);
	Widget_Append(w, bugs_url);

	Widget_AddClass(w, "c-about");
}

void UI_InitAbout(void)
{
	about_proto = LCUIWidget_NewPrototype("about", NULL);
	about_proto->init = About_OnInit;
}
