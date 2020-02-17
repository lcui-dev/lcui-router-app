#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <LCUI.h>
#include <LCDesign.h>
#include <LCUI/util/dirent.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
#include <LCUI/gui/widget/textview.h>
#include "file.h"
#include "router.h"

typedef struct FileViewTaskRec_ {
	wchar_t path[1024];
	LCUI_BOOL active;
	LCUI_Widget folders;
	LCUI_Widget files;
} FileViewTaskRec, *FileViewTask;

typedef struct FileViewRec_ {
	LCUI_Widget header;
	LCUI_Widget body;
	FileViewTask task;
} FileViewRec, *FileView;

static LCUI_WidgetPrototype file_proto;

static LCUI_Widget CreateFileItem(const wchar_t *dirname,
				  const wchar_t *filename, LCUI_BOOL is_dir)
{
	size_t len;
	size_t dirname_len;

	char units[][6] = {
		"B", "KB", "MB", "GB", "TB", "PB", "EB",
	};
	int unit_i;
	double file_size;

	char path[1024];
	char full_path[2048];
	char date_str[64];
	char size_str[32];
	struct stat file_stat;

	LCUI_Widget left;
	LCUI_Widget name;
	LCUI_Widget icon;
	LCUI_Widget link;
	LCUI_Widget item;
	LCUI_Widget size;
	LCUI_Widget date;

	icon = LCUIWidget_New("icon");
	link = LCUIWidget_New("router-link");
	name = LCUIWidget_New(NULL);
	left = LCUIWidget_New(NULL);
	size = LCUIWidget_New("text");
	date = LCUIWidget_New("text");
	item = LCUIWidget_New(NULL);
	dirname_len = LCUI_EncodeString(path, dirname, 1023, ENCODING_ANSI);
	path[dirname_len++] = '/';
	path[dirname_len] = 0;
	len = LCUI_EncodeString(path + dirname_len, filename,
				1023 - dirname_len, ENCODING_ANSI);
	path[dirname_len + len] = 0;
	if (stat(path, &file_stat) == 0) {
		strftime(date_str, 64, "%F %T", localtime(&file_stat.st_ctime));
		TextView_SetText(date, date_str);

		unit_i = 0;
		file_size = file_stat.st_size;
		while (file_size > 1024.0) {
			file_size /= 1024.0;
			unit_i++;
		}
		if (unit_i > 0) {
			snprintf(size_str, 32, "%.2f %s", file_size,
				 units[unit_i]);
		} else {
			snprintf(size_str, 32, "%d %s", (int)file_size,
				 units[unit_i]);
		}
		TextView_SetText(size, size_str);
	}
	if (is_dir) {
		Icon_SetName(icon, "folder");
		link = LCUIWidget_New("router-link");
		if (wcscmp(dirname, L".") == 0) {
			snprintf(full_path, 2048, "/file%s", path + 1);
		} else {
			snprintf(full_path, 2048, "/file/%s", path);
		}
		full_path[2047] = 0;
		Widget_SetAttribute(link, "to", full_path);
	} else {
		Icon_SetName(icon, "file-outline");
		link = LCUIWidget_New("text");
	}
	TextView_SetTextW(link, filename);
	Widget_AddClass(item, "c-file-list-item");
	Widget_AddClass(left, "c-file-list-item__left");
	Widget_AddClass(icon, "c-file-list-item__icon");
	Widget_AddClass(name, "c-file-list-item__name");
	Widget_AddClass(link, "c-file-list-item__link");
	Widget_AddClass(size, "c-file-list-item__size");
	Widget_AddClass(date, "c-file-list-item__date");
	Widget_Append(name, link);
	Widget_Append(left, icon);
	Widget_Append(left, name);
	Widget_Append(item, left);
	Widget_Append(item, size);
	Widget_Append(item, date);
	return item;
}

static void FileView_OnLinkFiles(void *arg1, void *arg2)
{
	FileViewTask task = arg2;
	FileView self = Widget_GetData(arg1, file_proto);

	if (task->active) {
		Widget_Append(self->body, task->folders);
		Widget_Append(self->body, task->files);
		Widget_Unwrap(task->folders);
		Widget_Unwrap(task->files);
	}
	free(task);
}

static void FileView_OnLoadFiles(void *arg1, void *arg2)
{
	const wchar_t *name;

	FileViewTask t = arg2;

	LCUI_Dir dir;
	LCUI_DirEntry *entry;

	if (!t->active) {
		free(t);
		return;
	}
		if (LCUI_OpenDirW(t->path, &dir) != 0) {
			return;
		}
	t->folders = LCUIWidget_New(NULL);
	t->files = LCUIWidget_New(NULL);
	while ((entry = LCUI_ReadDirW(&dir))) {
		name = LCUI_GetFileNameW(entry);
		if (name[0] == '.') {
			if (name[1] == '.' && name[2] == 0) {
				continue;
			}
			if (name[1] == 0) {
				continue;
			}
		}
		if (LCUI_FileIsDirectory(entry)) {
			Widget_Append(t->folders,
				      CreateFileItem(t->path, name, TRUE));
			continue;
		}
		Widget_Append(t->files, CreateFileItem(t->path, name, FALSE));
	}
	if (!t->active) {
		Widget_Destroy(t->files);
		Widget_Destroy(t->folders);
		free(t);
		return;
	}
	LCUI_PostSimpleTask(FileView_OnLinkFiles, arg1, t);
}

static void FileView_OnReady(LCUI_Widget w, LCUI_WidgetEvent e, void *arg)
{
	const char *name;
	const char *path;
	wchar_t header_str[1024];
	const router_route_t *route;
	router_t *router;

	Dict *refs;
	FileView self;

	LCUI_TaskRec task;
	LCUI_Widget parent;

	for (parent = w; parent; parent = parent->parent) {
		name = Widget_GetAttribute(parent, "router");
		if (name) {
			break;
		}
	}
	if (!name) {
		name = "default";
	}
	self = Widget_GetData(w, file_proto);
	refs = Widget_CollectReferences(w);
	router = router_get_by_name(name);
	route = router_get_current_route(router);
	self->task = malloc(sizeof(FileViewTaskRec));
	self->task->active = TRUE;
	self->header = Dict_FetchValue(refs, "header");
	self->body = Dict_FetchValue(refs, "body");
	path = router_route_get_param(route, "pathMatch");
	if (path) {
		LCUI_DecodeUTF8String(self->task->path, path, 1023);
		self->task->path[1023] = 0;
		swprintf(header_str, 1023, L"Directory listing for %s",
			 self->task->path);
		header_str[1023] = 0;
	} else {
		self->task->path[0] = '.';
		self->task->path[1] = 0;
		wcscpy(header_str, L"Directory listing for /");
	}
	TextView_SetTextW(self->header, header_str);
	task.arg[0] = w;
	task.arg[1] = self->task;
	task.destroy_arg[0] = NULL;
	task.destroy_arg[1] = NULL;
	task.func = FileView_OnLoadFiles;
	LCUI_PostAsyncTask(&task);
	Widget_UnbindEvent(w, "ready", FileView_OnReady);
	Dict_Release(refs);
}

static void FileView_OnInit(LCUI_Widget w)
{
	FileView self;

	LCUI_Widget wrapper;

	wrapper = LCUIBuilder_LoadFile("assets/views/file.xml");
	if (wrapper) {
		Widget_Append(w, wrapper);
		Widget_Unwrap(wrapper);
	}
	self = Widget_AddData(w, file_proto, sizeof(FileViewRec));
	self->task = NULL;
	Widget_AddClass(w, "v-file");
	Widget_BindEvent(w, "ready", FileView_OnReady, NULL, NULL);
}

static void FileView_OnDestroy(LCUI_Widget w)
{
	FileView self;

	self = Widget_GetData(w, file_proto);
	if (self->task) {
		self->task->active = FALSE;
	}
}

void UI_InitFileView(void)
{
	file_proto = LCUIWidget_NewPrototype("file", NULL);
	file_proto->init = FileView_OnInit;
	file_proto->destroy = FileView_OnDestroy;
}
