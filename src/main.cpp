#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(MyPlayerObject, PlayerObject) {
    void playerDestroyed(bool p0) {
        if (Mod::get()->getSettingValue<bool>("noclip")) {
            return; // Ignore death when Noclip is enabled
        }
        PlayerObject::playerDestroyed(p0);
    }
};
