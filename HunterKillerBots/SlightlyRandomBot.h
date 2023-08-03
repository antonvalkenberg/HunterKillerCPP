#pragma once
#include "BaseBot.h"

class SlightlyRandomBot :
    public BaseBot
{
public:
    SlightlyRandomBot() = default;
    ~SlightlyRandomBot() override;
    HunterKillerAction* Handle(const HunterKillerState& rState) override { return CreateSlightlyRandomAction(rState); }
    [[nodiscard]] const std::string* GetBotName() const override { return BotName; }
    static HunterKillerAction* CreateSlightlyRandomAction(const HunterKillerState& rState);
    static UnitOrder* CreateSlightlyRandomOrder(const HunterKillerState& rState, Unit* pUnit);
private:
    const std::string* BotName = new std::string("SlightlyRandomBot");
};

