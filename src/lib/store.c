#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "store.h"
#include "version.h"

#define STORE_FILE "store.data"
#define STORE_HEAD_STR "app store file"

void store_init(struct store *s)
{
	s->message[0] = 0;
	strcpy(s->head, STORE_HEAD_STR);
	strcpy(s->version, APP_VERSION);
}

int store_load(struct store *s)
{
	FILE *fp;

	fp = fopen(STORE_FILE, "rb");
	if (!fp) {
		return errno;
	}
	if (fread(s, sizeof(struct store), 1, fp) != 1) {
		fclose(fp);
		return -1;
	}
	fclose(fp);
	if (strcmp(s->head, STORE_HEAD_STR) != 0 ||
	    strcmp(s->version, APP_VERSION) != 0) {
		return -1;
	}
	return 0;
}

int store_save(struct store *s)
{
	FILE *fp;

	fp = fopen(STORE_FILE, "wb+");
	if (!fp) {
		return errno;
	}
	if (fwrite(s, sizeof(struct store), 1, fp) != 1) {
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}
