#include <string.h>
#include "router.h"

static void router_add_default_config(router_t *router)
{
	router_config_t *config;

	config = router_config_create();
	router_config_set_name(config, "home");
	router_config_set_path(config, "/");
	router_config_set_component(config, NULL, "home");
	router_add_route_record(router, config, NULL);
	router_config_destroy(config);

	config = router_config_create();
	router_config_set_name(config, "welcome");
	router_config_set_path(config, "/welcome");
	router_config_set_component(config, NULL, "welcome");
	router_add_route_record(router, config, NULL);
	router_config_destroy(config);

	config = router_config_create();
	router_config_set_name(config, "help");
	router_config_set_path(config, "/help");
	router_config_set_component(config, NULL, "help");
	router_add_route_record(router, config, NULL);
	router_config_destroy(config);

	config = router_config_create();
	router_config_set_path(config, "*");
	router_config_set_component(config, NULL, "notfound");
	router_add_route_record(router, config, NULL);
	router_config_destroy(config);

}

router_t *router_create_with_config(const char *name, const char *config_name)
{
	router_t *router;

	router = router_create(name);
	if (!config_name) {
		config_name = "default";
	}
	if (strcmp(config_name, "default") == 0) {
		router_add_default_config(router);
	}
	return router;
}
