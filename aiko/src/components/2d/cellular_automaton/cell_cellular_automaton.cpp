#include "cell_cellular_automaton.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

#include "chunk_cellular_automaton.h"

namespace aiko
{

    CellCellularAutomaton::CellCellularAutomaton(ChunkCellularAutomaton* chunk, const int i, const int j, const Color color)
        : chunk(chunk)
        , i(i)
        , j(j)
        , color(color)
    {
        init();
    }

    void CellCellularAutomaton::init()
    {

    }

    void CellCellularAutomaton::update()
    {

    }

    Color CellCellularAutomaton::getColor()
    {
        return color;
    }

    void CellCellularAutomaton::setColor(Color c)
    {
        color = c;
    }

}
