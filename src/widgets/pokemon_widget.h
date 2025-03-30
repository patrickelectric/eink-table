#pragma once

#include <lvgl.h>
#include <random>
#include "utils/date.h"
#include "utils/debug.h"
#include "sprites/all.h"

class PokemonWidget {
private:
    tm last_update;
    static constexpr int TOTAL_POKEMON = 151;

public:
    PokemonWidget()
        : last_update()
    {
    }

    static void update_pokemon(int pokemon_index) {
        static lv_obj_t* img = lv_img_create(lv_scr_act());
        lv_img_set_src(img, all_sprites()[pokemon_index]);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

        debug("Pokemon updated to #%d", pokemon_index);
    }

    void update() {
        debug("Checking Pokemon update...");
        auto now = Date::today();

        if (now.tm_hour != last_update.tm_hour ||
            now.tm_mday != last_update.tm_mday) {

            debug("Updating Pokemon: hour changed from %d to %d",
                last_update.tm_hour, now.tm_hour);

            PokemonWidget::update_pokemon(random(0, 151));
            last_update = now;
        }
    }
};
