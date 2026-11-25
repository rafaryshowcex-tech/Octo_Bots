#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include "BotManager.hpp"

using namespace geode::prelude;

class BotPopup : public geode::Popup<> {
protected:
    CCLabelBMFont* m_statusLabel = nullptr;

    bool setup() override {
        this->setTitle("Octo Bots");

        auto winSize = m_mainLayer->getContentSize();
        auto& bot = BotManager::get();

        m_statusLabel = CCLabelBMFont::create("", "goldFont.fnt");
        m_statusLabel->setScale(0.55f);
        m_statusLabel->setPosition({winSize.width / 2.f, winSize.height - 45.f});
        m_mainLayer->addChild(m_statusLabel);
        updateStatus();

        auto menu = CCMenu::create();
        menu->setPosition({winSize.width / 2.f, winSize.height / 2.f - 10.f});
        menu->setContentSize({winSize.width - 40.f, 130.f});
        menu->setLayout(
            RowLayout::create()
                ->setGap(10.f)
                ->setGrowCrossAxis(true)
                ->setCrossAxisOverflow(false)
        );
        m_mainLayer->addChild(menu);

        auto addBtn = [&](const char* text, SEL_MenuHandler callback) {
            auto spr = ButtonSprite::create(text, "goldFont.fnt", "GJ_button_01.png", 0.7f);
            spr->setScale(0.75f);
            auto btn = CCMenuItemSpriteExtra::create(spr, this, callback);
            menu->addChild(btn);
        };

        addBtn("Record", menu_selector(BotPopup::onRecord));
        addBtn("Play", menu_selector(BotPopup::onPlay));
        addBtn("Stop", menu_selector(BotPopup::onStop));
        addBtn("Clear", menu_selector(BotPopup::onClear));
        addBtn("Save", menu_selector(BotPopup::onSave));
        addBtn("Load", menu_selector(BotPopup::onLoad));
        addBtn("Toggle Noclip", menu_selector(BotPopup::onNoclip));

        menu->updateLayout();
        return true;
    }

    void updateStatus() {
        auto& bot = BotManager::get();
        std::string modeStr = "Disabled";
        if (bot.mode == BotMode::Recording) modeStr = "Recording";
        if (bot.mode == BotMode::Playing) modeStr = "Playing";

        bool noclip = Mod::get()->getSettingValue<bool>("noclip");
        auto text = fmt::format(
            "Mode: {} | Inputs: {} | Noclip: {}",
            modeStr, bot.inputs.size(), noclip ? "ON" : "OFF"
        );
        m_statusLabel->setString(text.c_str());
    }

    void onRecord(CCObject*) {
        auto& bot = BotManager::get();
        bot.clearMacro();
        bot.mode = BotMode::Recording;
        updateStatus();
    }

    void onPlay(CCObject*) {
        auto& bot = BotManager::get();
        bot.resetFrame();
        bot.mode = BotMode::Playing;
        updateStatus();
    }

    void onStop(CCObject*) {
        BotManager::get().mode = BotMode::Disabled;
        updateStatus();
    }

    void onClear(CCObject*) {
        BotManager::get().clearMacro();
        updateStatus();
        FLAlertLayer::create("Octo Bots", "Macro cleared.", "OK")->show();
    }

    void onSave(CCObject*) {
        bool ok = BotManager::get().saveMacro();
        FLAlertLayer::create("Octo Bots", ok ? "Saved macro.json!" : "Failed to save macro.", "OK")->show();
    }

    void onLoad(CCObject*) {
        bool ok = BotManager::get().loadMacro();
        updateStatus();
        FLAlertLayer::create("Octo Bots", ok ? "Loaded macro.json!" : "No saved macro found.", "OK")->show();
    }

    void onNoclip(CCObject*) {
        bool current = Mod::get()->getSettingValue<bool>("noclip");
        Mod::get()->setSettingValue<bool>("noclip", !current);
        updateStatus();
    }

public:
    static BotPopup* create() {
        auto ret = new BotPopup();
        if (ret->initAnchored(340.f, 210.f)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};
