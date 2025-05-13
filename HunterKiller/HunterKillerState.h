#pragma once
#include <vector>

#include "HunterKillerPlayer.h"

class HunterKillerState
{
public:
    HunterKillerState(const HunterKillerState& rHKState);
    HunterKillerState(HunterKillerMap* pMap, std::vector<HunterKillerPlayer*>* pPlayers, const int currentRound, const int currentPlayerID) : CurrentRound(currentRound), ActivePlayerID(currentPlayerID), Map(pMap), Players(pPlayers) {}
    ~HunterKillerState();
    [[nodiscard]] HunterKillerState* Copy() const;
    [[nodiscard]] int GetHashCode() const;
    [[nodiscard]] int GetCurrentRound() const { return CurrentRound; }
    [[nodiscard]] int GetActivePlayerID() const { return ActivePlayerID; }
    [[nodiscard]] HunterKillerMap& GetMap() const { return *Map; }
    [[nodiscard]] std::vector<HunterKillerPlayer*>* GetPlayers() const { return Players; }
    [[nodiscard]] int GetNumberOfPlayers() const { return static_cast<int>(Players->size()); }
    [[nodiscard]] HunterKillerPlayer* GetPlayer(int playerID) const;
    [[nodiscard]] HunterKillerPlayer& GetActivePlayer() const { return *GetPlayer(ActivePlayerID); }
    [[nodiscard]] std::unordered_map<int, int>* GetScores() const;
    [[nodiscard]] bool IsDone() const;
    void EndPlayerTurn();
    [[nodiscard]] bool IsStructureSpawnAreaFree(const Structure& rStructure) const;
    [[nodiscard]] bool CanStructureSpawnAUnit(const Structure& rStructure) const;
    [[nodiscard]] bool CanStructureExecute(const Structure& rStructure, StructureOrderType structureOrderType) const;
    [[nodiscard]] bool CanStructureSpawn(const Structure& rStructure, UnitType unitType) const;
    void Prepare(int activePlayerID) const;
private:
    int CurrentRound;
    int ActivePlayerID;
    HunterKillerMap* Map;
    std::vector<HunterKillerPlayer*>* Players = nullptr;
    void AwardResourcesToController(const Structure& rStructure) const;
    void AwardScoreToController(const Structure& rStructure) const;
    void MapCleanup() const;
    bool ShouldGameObjectBeRemoved(GameObject* pGameObject) const;
protected:
    int TurnCounter = 0;
};

