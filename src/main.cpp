#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "BotManager.hpp"
#include "BotPopup.hpp"

using namespace geode::prelude;

// 1. Noclip Hook
class $modify(OctoPlayerObject, PlayerObject) {
    void playerDestroyed(bool p0) {
        if (Mod::get()->getSettingValue<bool>("noclip")) {
            return;
        }
        PlayerObject::playerDestroyed(p0);
    }
};

// 2. Input Capture & Replay Hooks
class $modify(OctoGameLayer, GJBaseGameLayer) {
    void processCommands(float dt) {
        auto& bot = BotManager::get();

        if (bot.mode == BotMode::Playing) {
            while (bot.replayIndex < bot.inputs.size() &&
                   bot.inputs[bot.replayIndex].frame <= bot.currentFrame) {
                const auto& action = bot.inputs[bot.replayIndex];
                if (action.push) {
                    GJBaseGameLayer::pushButton(action.button, action.player1);
                } else {
                    GJBaseGameLayer::releaseButton(action.button, action.player1);
                }
                bot.replayIndex++;
            }
        }

        GJBaseGameLayer::processCommands(dt);

        if (bot.mode != BotMode::Disabled) {
            bot.currentFrame++;
        }
    }

    void pushButton(int button, bool player1) {
        auto& bot = BotManager::get();
        if (bot.mode == BotMode::Recording) {
            bot.inputs.push_back({bot.currentFrame, button, true, player1});
        }
        GJBaseGameLayer::pushButton(button, player1);
    }

    void releaseButton(int button, bool player1) {
        auto& bot = BotManager::get();
        if (bot.mode == BotMode::Recording) {
            bot.inputs.push_back({bot.currentFrame, button, false, player1});
        }
        GJBaseGameLayer::releaseButton(button, player1);
    }
};

// 3. Reset frame counter on level restart
class $modify(OctoPlayLayer, PlayLayer) {
    void resetLevel() {
        auto& bot = BotManager::get();
        if (bot.mode == BotMode::Recording) {
            bot.clearMacro();
        } else {
            bot.resetFrame();
        }
        PlayLayer::resetLevel();
    }
};

// 4. Add Octo Bots button to Pause Menu
class $modify(OctoPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = this->getChildByID("right-button-menu");
        if (!menu) return;

        auto spr = CircleButtonSprite::createWithSpriteFrameName(
            "GJ_playBtn2_001.png", 1.f, CircleBaseColor::Green, CircleBaseSize::Small
        );
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(OctoPauseLayer::onOpenOctoBots)
        );
        btn->setID("octo-bots-btn"_spr);
        menu->addChild(btn);
        menu->updateLayout();
    }

    void onOpenOctoBots(CCObject*) {
        BotPopup::create()->show();
    }
};
