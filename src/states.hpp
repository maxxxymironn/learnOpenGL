#pragma once

struct States {
    inline static bool updated = false;

    inline static bool polygonModeUpdated = false;
    inline static bool polygonMode = false;

    static void updatePolygonMode() {
        polygonMode = !polygonMode;
        polygonModeUpdated = true;
        updated = true;
    }
};
