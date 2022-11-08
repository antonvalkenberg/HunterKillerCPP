#pragma once
#include "HunterKillerAction.h"

class Result
{
public:
    Result(const bool wasAccepted, const std::string* pInformation) : Accepted(wasAccepted), Information(pInformation) {}
    ~Result()
    {
        delete PlayerRanking; PlayerRanking = nullptr;
        delete Information; Information = nullptr;
    }
    bool Accepted = false;
    const std::string* Information = nullptr;
    std::vector<std::pair<int, int>>* PlayerRanking = nullptr;
};

class HunterKillerRules
{
public:
    static Result* Handle(HunterKillerState& rState, const HunterKillerAction& rAction);
    static Result* PerformAction(const HunterKillerState& rState, const HunterKillerAction& rAction);
    static void ExecuteOrder(const HunterKillerState& rState, HunterKillerOrder& rOrder, std::string* pFailureReasons);
    static bool IsOrderPossible(const HunterKillerState& rState, HunterKillerOrder& rOrder, std::string* pFailureReasons);
    static bool AddAndExecuteOrderIfPossible(const HunterKillerAction& rAction, const HunterKillerState& rState, HunterKillerOrder* pOrder, std::string* pPossibleCheckFails, std::string* pOrderFails);
    static constexpr bool IGNORE_FAILURES = false;
private:
    static void AwardPointsForUnitDeath(HunterKillerPlayer& rPlayer, Unit& rKilledUnit);
    static constexpr bool LOG_TO_CONSOLE = true;
};