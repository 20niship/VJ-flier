#include "runApp.h"
extern runApp* g_ptr;

int main() {
	runApp App;

	App.setup();
	g_ptr = &App;

	App.MainLoop();

	App.ExitSystem();
}
