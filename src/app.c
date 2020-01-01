#include "ui.h"
#include "router.h"

int main(void)
{
	int ret = -1;
	router_t *router;

	router = router_create_with_config("main", "main");
	if (UI_Init() == 0) {
		ret = UI_Run();
	}
	router_destroy(router);
	return ret;
}
