// WeaponSystem.h
#pragma once
#include "Bullet.h"
#include <memory>

struct AmmoReserve {
    int reservedMags = 5;
    int partialAmmo = 0;
    int currentAmmo = 0;
    std::unique_ptr<Bullet> type;
};

class WeaponSystem {
public:
    AmmoReserve weapons[3];  // 0: Single, 1: Triple, 2: Five
    int currentWeapon = 2;   // Start with FiveHit (index 2)

    WeaponSystem() {
        weapons[0].type = std::make_unique<SingleShotBullet>();
        weapons[1].type = std::make_unique<ThreeHitBullet>();
        weapons[2].type = std::make_unique<FiveHitBullet>();

        weapons[0].currentAmmo = 5;
        weapons[1].currentAmmo = 10;
        weapons[2].currentAmmo = 15;
    }

    Bullet* getCurrentBullet() const { return weapons[currentWeapon].type.get(); }
    AmmoReserve& getCurrentReserve() { return weapons[currentWeapon]; }

    void switchWeapon(int index) {
        if (index >= 0 && index <= 2) currentWeapon = index;
    }

    void reloadCurrent() {
        auto& r = getCurrentReserve();
        int magSize = r.type->getMagSize();
        int needed = magSize - r.currentAmmo;
        if (needed <= 0) return;

        int use_partial = (r.partialAmmo < needed) ? r.partialAmmo : needed;
        r.currentAmmo += use_partial;
        r.partialAmmo -= use_partial;
        needed -= use_partial;

        if (needed > 0 && r.reservedMags > 0) {
            r.reservedMags--;
            int give = (magSize < needed) ? magSize : needed;
            r.currentAmmo += give;
            needed -= give;
            if (give < magSize) {
                r.partialAmmo += (magSize - give);
            }
        }
    }
};
