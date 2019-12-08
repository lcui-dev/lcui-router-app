#include <LCUI.h>
#include <LCDesign.h>

#include "preview.h"

static LCUI_WidgetPrototype preview_proto;

static PreviewView_OnReady(LCUI_Widget w, LCUI_WidgetEvent e, void *arg)
{
	LCDesign_SetMessageContainer(LCUIWidget_GetById("preview-messages"));
	LCDesign_SetNotificationContainer(
	    LCUIWidget_GetById("preview-notifications"));
	Widget_UnbindEvent(w, "ready", PreviewView_OnReady);
	LCDesign_OpenLoadingMessage(L"Checking for updates ...", 0);
	LCDesign_OpenInfoMessage(L"The new version has been released!", 0);
	LCDesign_OpenErrorMessage(L"Failed to get updates", 0);
	LCDesign_OpenSuccessNotification(
	    L"Update is ready!",
	    L"You can upgrade to the new version by clicking the upgrade button", NULL, 0);
	LCDesign_OpenErrorNotification(
	    L"Request failed",
	    L"The requested URL /releases was not found on server.",
	    NULL, 0);
}

static void PreviewView_OnInit(LCUI_Widget w)
{
	Widget_AddClass(w, "v-preview");
	Widget_BindEvent(w, "ready", PreviewView_OnReady, NULL, NULL);
}

static void PreviewView_OnDestroy(LCUI_Widget w)
{
}

void UI_InitPreviewView(void)
{
	preview_proto = LCUIWidget_NewPrototype("preview", NULL);
	preview_proto->init = PreviewView_OnInit;
	preview_proto->destroy = PreviewView_OnDestroy;
}
