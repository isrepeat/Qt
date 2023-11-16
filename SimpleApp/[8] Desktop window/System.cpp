#include "System.h"
//#include "Logger.h"
#ifdef CRASH_HANDLING_NUGET
#include <CrashHandling/CrashHandling.h>
#endif


namespace H {
    namespace System {
        ComException::ComException(HRESULT hr, const std::wstring& message)
            : std::exception(H::WStrToStr(message).c_str())
#ifdef CRASH_HANDLING_NUGET
            , backtrace{ std::make_shared<CrashHandling::Backtrace>() }
#endif
            , errorMessage{message}
            , errorCode{hr}
        {
            //LOG_ERROR_D(L"Com exception = [{:#08x}] {}", static_cast<unsigned int>(hr), message);
        }

        const std::shared_ptr<CrashHandling::Backtrace>& ComException::GetBacktrace() const throw(NugetNotFoundException) {
#ifdef CRASH_HANDLING_NUGET
            return backtrace;
#else
            throw NugetNotFoundException();
#endif
        }

        void ComException::LogBacktrace() const throw(NugetNotFoundException) {
#ifdef CRASH_HANDLING_NUGET
            LOG_ERROR_D(L"Com exception = [{:#08x}] {}", static_cast<unsigned int>(errorCode), errorMessage);
            LOG_ERROR_D(L"\nBacktrace: \n{}", backtrace->GetBacktraceStr());
#else
            throw NugetNotFoundException();
#endif
        }

        std::wstring ComException::ErrorMessage() const {
            return errorMessage;
        }

        HRESULT ComException::ErrorCode() const {
            return errorCode;
        }
    }
}