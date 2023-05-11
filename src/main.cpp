#include "app.h"

int main(void)
{
    App *app = new App();

    while (app->update())
    {
        app->render();
    }

    delete app;

    return 0;
}