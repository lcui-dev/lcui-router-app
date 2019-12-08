#include "ui.h"

int main(void)
{
	if (UI_Init() == 0) {
		return UI_Run();
	}
	return -1;
}
