#include "ui.h"

int main(void)
{
	int ret = -1;

	if (UI_Init() == 0) {
		ret = UI_Run();
	}
	return ret;
}
