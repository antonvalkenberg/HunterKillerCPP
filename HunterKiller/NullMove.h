#pragma once
#include "HunterKillerAction.h"

class NullMove
    : public HunterKillerAction
{
public:
    NullMove() = default;
    explicit NullMove(const HunterKillerState& rState) : HunterKillerAction(rState) {}
    ~NullMove() = default;
};

