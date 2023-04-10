#include "System.h"


namespace H {
    namespace System {
        ComException::ComException(HRESULT hr, const std::wstring& message)
            : std::exception(H::WStrToStr(message).c_str())
            , backtrace{ std::make_unique<Backtrace>() }
            , errorMessage{ message }
            , errorCode{ hr }
        {
        }

        ComException::~ComException() = default; // declare in cpp explicitly to allow compile unique_ptr with pimpl


        const std::unique_ptr<Backtrace>& ComException::GetBacktrace() const {
            return backtrace;
        }

        std::wstring ComException::ErrorMessage() const {
            return errorMessage;
        }

        HRESULT ComException::ErrorCode() const {
            return errorCode;
        }


        std::wstring ComException::GetStacktrace() const {
            if (stacktrace.size()) {
                return stacktrace;
            }

            return L"";
        }

        void ComException::LogStacktrace() const {
        }
    }
}