
#include "../AudioPacket.h"
#include "AbstractFilter.h"
#include "FilterRepository.h"

namespace DX {
namespace Audio {
    
    AbstractFilter::AbstractFilter(FilterType type)
    {
        FilterRepository::registerFilter(*this, type);
    }

    AbstractFilter::~AbstractFilter()
    {
    }

}
}
