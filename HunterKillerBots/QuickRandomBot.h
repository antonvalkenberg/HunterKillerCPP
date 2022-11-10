#pragma once
#include "BaseBot.h"

class QuickRandomBot :
    public BaseBot
{
public:
    QuickRandomBot() = default;
    ~QuickRandomBot() override;
    HunterKillerAction* Handle(const HunterKillerState& rState) override;
    [[nodiscard]] const std::string* GetBotName() const override { return BotName; }
private:
    const std::string* BotName = new std::string("QuickRandomBot");
};

