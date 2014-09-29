#include "ClassDemoApp.h"
#include "SheetSprite.h"
#include "Entity.h"

int main(int argc, char *argv[]) {
	ClassDemoApp app;
	while (!app.UpdateAndRender()) {}
	return 0;
}