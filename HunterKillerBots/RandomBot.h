#pragma once
#include "BaseBot.h"

class RandomBot :
    public BaseBot
{
public:
    RandomBot() = default;
    ~RandomBot() override;
    HunterKillerAction* Handle(const HunterKillerState& rState) override { return CreateRandomAction(rState); }
    [[nodiscard]] const std::string* GetBotName() const override { return BotName; }
    static HunterKillerAction* CreateRandomAction(const HunterKillerState& rState);
    static UnitOrder* CreateRandomOrder(const HunterKillerState& rState, Unit* pUnit);
private:
    const std::string* BotName = new std::string("RandomBot");
};

