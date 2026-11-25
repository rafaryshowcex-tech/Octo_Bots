#pragma once
#include <Geode/Geode.hpp>
#include <vector>
#include <filesystem>

using namespace geode::prelude;

struct MacroInput {
    int frame;
    int button;
    bool push;
    bool player1;
};

enum class BotMode {
    Disabled,
    Recording,
    Playing
};

class BotManager {
public:
    BotMode mode = BotMode::Disabled;
    std::vector<MacroInput> inputs;
    int currentFrame = 0;
    size_t replayIndex = 0;

    static BotManager& get() {
        static BotManager instance;
        return instance;
    }

    void resetFrame() {
        currentFrame = 0;
        replayIndex = 0;
    }

    void clearMacro() {
        inputs.clear();
        resetFrame();
    }

    std::filesystem::path getMacroPath() {
        return Mod::get()->getSaveDir() / "macro.json";
    }

    bool saveMacro() {
        matjson::Value arr = matjson::Value::array();
        for (const auto& in : inputs) {
            matjson::Value obj = matjson::Value::object();
            obj["f"] = in.frame;
            obj["b"] = in.button;
            obj["p"] = in.push;
            obj["p1"] = in.player1;
            arr.push(obj);
        }
        auto res = file::writeString(getMacroPath(), arr.dump());
        return res.isOk();
    }

    bool loadMacro() {
        auto content = file::readString(getMacroPath());
        if (!content) return false;

        auto parsed = matjson::parse(content.unwrap());
        if (!parsed) return false;

        auto arr = parsed.unwrap();
        if (!arr.isArray()) return false;

        clearMacro();
        for (const auto& item : arr) {
            inputs.push_back({
                static_cast<int>(item["f"].asInt().unwrapOr(0)),
                static_cast<int>(item["b"].asInt().unwrapOr(1)),
                item["p"].asBool().unwrapOr(false),
                item["p1"].asBool().unwrapOr(true)
            });
        }
        return true;
    }
};
