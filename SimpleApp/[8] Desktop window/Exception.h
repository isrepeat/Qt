#pragma once
#include <stdexcept>

namespace H {
    class NugetNotFoundException : public std::logic_error {
    public:
        NugetNotFoundException() 
            : std::logic_error("No nuget for this operation")
        {
        }
    };
}