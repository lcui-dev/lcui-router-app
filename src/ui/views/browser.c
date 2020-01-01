#include <LCUI.h>
#include <LCUI/gui/widget.h>
#include "../components/frame-tab.h"
#include "browser.h"
#include "frame.h"

typedef struct PageRec_ {
	int id;
	wchar_t title[256];
	LCUI_BOOL loading;
	LCUI_Widget tab;
	LCUI_Widget frame;
	LCUI_Widget browser;
	LinkedListNode node;
} PageRec, *Page;

typedef struct BrowserViewRec_ {
	int page_id_count;
	Page current_page;
	LCUI_Widget tabbar;
	LinkedList pages;
} BrowserViewRec, *BrowserView;

static LCUI_WidgetPrototype browser_proto;

static void BrowserView_OnInit(LCUI_Widget w)
{
	BrowserView self;

	self = Widget_AddData(w, browser_proto, sizeof(BrowserViewRec));
	self->tabbar = LCUIWidget_New(NULL);
	self->page_id_count = 0;
	self->current_page = NULL;
	LinkedList_Init(&self->pages);
	Widget_AddClass(w, "v-browser");
	Widget_AddClass(self->tabbar, "v-browser__tabbar");
	Widget_Append(w, self->tabbar);
	BrowserView_Load(w, NULL);
}

static void BrowserView_OnDestroy(LCUI_Widget w)
{
	BrowserView self;

	self = Widget_GetData(w, browser_proto);
}

static void BrowserView_OnPageTabClick(LCUI_Widget w, LCUI_WidgetEvent e,
				       void *arg)
{
	Page page;

	page = e->data;
	BrowserView_Active(page->browser, page->id);
}

static void BrowserView_OnPageLoad(LCUI_Widget w, LCUI_WidgetEvent e, void *arg)
{
	Page page;

	page = e->data;
	page->loading = TRUE;
	FrameTab_SetLoading(page->tab, page->loading);
}

static void BrowserView_OnPageLoaded(LCUI_Widget w, LCUI_WidgetEvent e,
				     void *arg)
{
	Page page;

	page = e->data;
	page->title[255] = 0;
	page->loading = FALSE;
	wcsncpy(page->title, e->target->title, 255);
	FrameTab_SetLoading(page->tab, page->loading);
	FrameTab_SetTextW(page->tab, page->title);
}

int BrowserView_Load(LCUI_Widget w, const char *path)
{
	Page page;
	BrowserView self;

	page = malloc(sizeof(PageRec));
	self = Widget_GetData(w, browser_proto);
	if (!path) {
		path = "/welcome";
	}
	page->id = self->page_id_count++;
	page->node.next = page->node.prev = 0;
	page->node.data = page;
	page->loading = TRUE;
	page->tab = LCUIWidget_New("frame-tab");
	page->frame = LCUIWidget_New("frame");
	page->browser = w;
	wcscpy(page->title, L"Loading...");
	FrameTab_SetTextW(page->tab, page->title);
	FrameTab_SetLoading(page->tab, TRUE);
	Widget_Append(self->tabbar, page->tab);
	Widget_BindEvent(page->tab, "mousedown", BrowserView_OnPageTabClick,
			 page, NULL);
	Widget_BindEvent(page->frame, "PageLoad", BrowserView_OnPageLoad, page,
			 NULL);
	Widget_BindEvent(page->frame, "PageLoaded", BrowserView_OnPageLoaded,
			 page, NULL);
	LinkedList_AppendNode(&self->pages, &page->node);
	FrameView_Load(page->frame, path);
	return page->id;
}

LCUI_BOOL BrowserView_Active(LCUI_Widget w, int id)
{
	Page page;
	BrowserView self;
	LinkedListNode *node;

	self = Widget_GetData(w, browser_proto);
	for (LinkedList_Each(node, &self->pages)) {
		page = node->data;
		if (page->id == id) {
			break;
		}
		page = NULL;
	}
	if (!page) {
		return FALSE;
	}
	if (self->current_page) {
		Widget_RemoveClass(self->current_page->tab, "active");
		Widget_Unlink(self->current_page->frame);
	}
	Widget_AddClass(page->tab, "active");
	Widget_Append(w, page->frame);
	self->current_page = page;
	return TRUE;
}

void UI_InitBrowserView(void)
{
	browser_proto = LCUIWidget_NewPrototype("browser", NULL);
	browser_proto->init = BrowserView_OnInit;
	browser_proto->destroy = BrowserView_OnDestroy;
}
