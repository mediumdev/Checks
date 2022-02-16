#include "View.h"
#include "Controller.h"
#include "Field.h"
#include "Piece.h"
#include "Human.h"
#include "AI.h"

using namespace std;

constexpr int fieldWidth = 8;
constexpr int fieldHeight = 8;
constexpr int tileSize = 64;

vector<shared_ptr<Player>> players;

auto field = make_unique<Field>(fieldWidth, fieldHeight, tileSize);
auto controller = make_unique<Controller>(players);
auto view = make_unique<View>();

int main()
{
    view->LoadResources();
     
    controller->StartNewGame(*field);

    while (view->WindowsIsOpen())
    {
        view->EventHandler();
        controller->Update(*view, *field);
        view->Draw(*field, players);
    }

    return 0;
}