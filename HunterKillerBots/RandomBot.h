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
    static UnitOrder* CreateSlightlyRandomOrder(const HunterKillerState& rState, Unit* pUnit);
private:
    const std::string* BotName = new std::string("RandomBot");
    static constexpr double NO_UNIT_ORDER_THRESHOLD = 0.2;
    static constexpr double NO_BASE_ORDER_THRESHOLD = 0.1;
};

