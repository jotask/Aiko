#include "mapper.h"

namespace nes
{
    Mapper::Mapper(Byte prg_bank, Byte chr_bank)
        : n_prg_banks(prg_bank)
        , n_chr_banks(chr_bank)
    {

    }
}
