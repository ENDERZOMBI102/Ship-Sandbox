//
// Created by ENDERZOMBI102 on 06/06/2023.
//

#include "ShipSandbox.hpp"

#include "GameElement.hpp"


void GameElement::OnRender() {
	shipSandbox->beginFrame();
	shipSandbox->gm.render();
	shipSandbox->endFrame();
}
