#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Text/Text.h"

class KeyTextDemo : public Game {
    Text text;
    std::string keyText = "";

public:
    KeyTextDemo() {
        text.init("assets/fonts/rex_engine.ttf", 32);
    }

    void update(float dt, Input& input) override {
        if (input.isKeyHeld(RexKey::N1))  keyText = "N1";
        else if (input.isKeyHeld(RexKey::N2))  keyText = "N2";
        else if (input.isKeyHeld(RexKey::N3))  keyText = "N3";
        else if (input.isKeyHeld(RexKey::N4))  keyText = "N4";
        else if (input.isKeyHeld(RexKey::N5))  keyText = "N5";
        else if (input.isKeyHeld(RexKey::N6))  keyText = "N6";
        else if (input.isKeyHeld(RexKey::N7))  keyText = "N7";
        else if (input.isKeyHeld(RexKey::N8))  keyText = "N8";
        else if (input.isKeyHeld(RexKey::N9))  keyText = "N9";
        else if (input.isKeyHeld(RexKey::A))  keyText = "A";
        else if (input.isKeyHeld(RexKey::B))  keyText = "B";
        else if (input.isKeyHeld(RexKey::C))  keyText = "C";
        else if (input.isKeyHeld(RexKey::D))  keyText = "D";
        else if (input.isKeyHeld(RexKey::E))  keyText = "E";
        else if (input.isKeyHeld(RexKey::F))  keyText = "F";
        else if (input.isKeyHeld(RexKey::G))  keyText = "G";
        else if (input.isKeyHeld(RexKey::H))  keyText = "H";
        else if (input.isKeyHeld(RexKey::I))  keyText = "I";
        else if (input.isKeyHeld(RexKey::J))  keyText = "J";
        else if (input.isKeyHeld(RexKey::K))  keyText = "K";
        else if (input.isKeyHeld(RexKey::L))  keyText = "L";
        else if (input.isKeyHeld(RexKey::M))  keyText = "M";
        else if (input.isKeyHeld(RexKey::N))  keyText = "N";
        else if (input.isKeyHeld(RexKey::O))  keyText = "O";
        else if (input.isKeyHeld(RexKey::P))  keyText = "P";
        else if (input.isKeyHeld(RexKey::Q))  keyText = "Q";
        else if (input.isKeyHeld(RexKey::R))  keyText = "R";
        else if (input.isKeyHeld(RexKey::S))  keyText = "S";
        else if (input.isKeyHeld(RexKey::T))  keyText = "T";
        else if (input.isKeyHeld(RexKey::U))  keyText = "U";
        else if (input.isKeyHeld(RexKey::V))  keyText = "V";
        else if (input.isKeyHeld(RexKey::W))  keyText = "W";
        else if (input.isKeyHeld(RexKey::X))  keyText = "X";
        else if (input.isKeyHeld(RexKey::Y))  keyText = "Y";
        else if (input.isKeyHeld(RexKey::Z))  keyText = "Z";
        else if (input.isKeyHeld(RexKey::UP))  keyText = "UP";
        else if (input.isKeyHeld(RexKey::DOWN))  keyText = "DOWN";
        else if (input.isKeyHeld(RexKey::LEFT))  keyText = "LEFT";
        else if (input.isKeyHeld(RexKey::RIGHT))  keyText = "RIGHT";

        else keyText = "";
    }

    void render(Graphics& g) override {
        g.clear(0, 0, 0, 255);

        if (!keyText.empty())
            text.render(g, keyText.c_str(), 250, 250, {255, 255, 255, 255});

        g.present();
    }

    ~KeyTextDemo() {
        text.quit();
    }
};

int main() {
    Engine::run<KeyTextDemo>("Rex Engine Key Text Demo", 800, 600);
}
