
#include "../AudioPacket.h"
#include "AbstractFilter.h"
#include "FilterRepository.h"

namespace DX {
namespace Audio {
    
    AbstractFilter::AbstractFilter(std::shared_ptr<AbstractFilter> filter, FilterType type)
    {
        FilterRepository::registerFilter(filter, type);
    }

    AbstractFilter::~AbstractFilter()
    {
    }

}
}
