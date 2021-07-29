#pragma once
#include "Scene.h"
class FinalLevel : public Scene {

public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};
