#pragma once
#include "Helpers.h"
#include <Windows.h>
#include <stdexcept>
#include <comdef.h>
#include <memory>


namespace H {
    namespace System {
        // NOTE: If you create a default destructor here, it will not be able to delete the incomplete type (for unique_ptr).
        //       To solve it, you need to declare a (even default) destructor in the source file
        class Backtrace;

        class ComException : public std::exception {
        public:
            ComException(HRESULT hr, const std::wstring& message);
            ~ComException();

            ComException(ComException&&) = default;

            const std::unique_ptr<Backtrace>& GetBacktrace() const;
            std::wstring ErrorMessage() const;
            HRESULT ErrorCode() const;

            std::wstring GetStacktrace() const;
            void LogStacktrace() const;

        private:
            std::unique_ptr<Backtrace> backtrace;
            mutable std::wstring stacktrace;
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