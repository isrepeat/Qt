#pragma once
#include "HWindows.h"
#include "Exception.h"
#include "Helpers.h"
#include "Macros.h"
#include <stdexcept>
#include <comdef.h>
#include <memory>

namespace CrashHandling {
    class Backtrace;
}

namespace H {
    namespace System {
        // NOTE: For unqie_ptr over pipmpl you need define Dtor (even default) in .cpp file

        class ComException : public std::exception {
        public:
            ComException(HRESULT hr, const std::wstring& message);
            ~ComException() = default;

            ComException(const ComException&) = default;
            ComException& operator=(const ComException&) = default;

            NO_MOVE(ComException); // std::exception not support move Ctor

            // [may throw NugetNotFoundException]
            const std::shared_ptr<CrashHandling::Backtrace>& GetBacktrace() const; 
            void LogBacktrace() const;

            std::wstring ErrorMessage() const;
            HRESULT ErrorCode() const;

        private:
            std::shared_ptr<CrashHandling::Backtrace> backtrace;
            std::wstring errorMessage;
            HRESULT errorCode = S_OK;
        };

        inline void ThrowIfFailed(HRESULT hr) {
            if (FAILED(hr)) {
                throw ComException(hr, _com_error(hr).ErrorMessage());
            }
        }
    }
}