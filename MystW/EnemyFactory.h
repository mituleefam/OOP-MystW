#pragma once
#include <memory>
#include <string>
#include "Enemy.h"
#include "Elf.h"
#include "Striker.h"
#include "Wizard.h"
#include "LeafBoss.h"
#include "WaterBoss.h"

class EnemyFactory
{
public:
    static std::unique_ptr<Enemy> createEnemy(const std::string& type, float x, float y) {
        if (type == "Elf")
            return std::make_unique<Elf>("Assets/Enemy/Elf/Textures", x, y);
        else if (type == "Striker")
            return std::make_unique<Striker>("Assets/Enemy/Striker/Textures", x, y);
        else if (type == "Wizard")
            return std::make_unique<Wizard>("Assets/Enemy/Wizard/Textures", x, y);
		else if (type == "LeafBoss")
			return std::make_unique<LeafBoss>("Assets/Enemy/LeafBoss", x, y);
		else if (type == "WaterBoss")
			return std::make_unique<WaterBoss>("Assets/Enemy/WaterBoss", x, y);
        // Add more types as needed
        return nullptr;
    }
};