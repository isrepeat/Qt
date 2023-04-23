#include "CMDDiskFormater.h"
#include <stdexcept>
#include <locale>
#include <codecvt>
#include "throw_if_fail.h"
#include "Scope.h"
#include <cassert>
#include <algorithm>
#include <chrono>

#define UDF							L"UDF"
#define UDF_REVISION				L"2.50"
#define CMD_PATH					L"C:\\Windows\\System32\\cmd.exe"
#ifdef _DEBUG
#define CMD_SHOW					SW_SHOW
#else
#define CMD_SHOW					SW_HIDE
#endif

#define FSMessageStart				31
#define BUFSIZE						8192

#define OneMinute					60000
#define TenMinute					600000

std::wstring StrToWStr(const std::string& str, int codePage) {
	if (str.size() == 0)
		return std::wstring{};

	int sz = MultiByteToWideChar(codePage, 0, str.c_str(), -1, 0, 0);
	std::wstring res(sz, 0);
	MultiByteToWideChar(codePage, 0, str.c_str(), -1, &res[0], sz);
	return res.c_str(); // To delete '\0' use .c_str()
}

std::wstring GetLastErrorAsString() {
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return L""; //No error message has been recorded
	}

	LPWSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPWSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::wstring message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}


//namespace Test {

	#define BUFSIZE_ 4096 

	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	HANDLE g_hInputFile = NULL;

	void ErrorExit(PTSTR lpszFunction)

		// Format a readable error message, display a message box, 
		// and exit from the application.
	{
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();

		//FormatMessage(
		//	FORMAT_MESSAGE_ALLOCATE_BUFFER |
		//	FORMAT_MESSAGE_FROM_SYSTEM |
		//	FORMAT_MESSAGE_IGNORE_INSERTS,
		//	NULL,
		//	dw,
		//	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		//	(LPTSTR)&lpMsgBuf,
		//	0, NULL);

		//lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		//	(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
		//StringCchPrintf((LPTSTR)lpDisplayBuf,
		//	LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		//	TEXT("%s failed with error %d: %s"),
		//	lpszFunction, dw, lpMsgBuf);
		//MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

		//LocalFree(lpMsgBuf);
		//LocalFree(lpDisplayBuf);
		ExitProcess(1);
	}


	void WriteToPipeTest(void)

		// Read from a file and write its contents to the pipe for the child's STDIN.
		// Stop when there is no more data. 
	{
		DWORD dwRead, dwWritten;
		CHAR chBuf[BUFSIZE_];
		BOOL bSuccess = FALSE;

		for (;;)
		{
			bSuccess = ReadFile(g_hInputFile, chBuf, BUFSIZE, &dwRead, NULL);
			if (!bSuccess || dwRead == 0) break;

			bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
			if (!bSuccess) break;
		}

		// Close the pipe handle so the child process stops reading. 

		if (!CloseHandle(g_hChildStd_IN_Wr))
			ErrorExit((PTSTR)(L"StdInWr CloseHandle"));
	}

	void ReadFromPipeTest(void)

		// Read output from the child process's pipe for STDOUT
		// and write to the parent process's pipe for STDOUT. 
		// Stop when there is no more data. 
	{
		DWORD dwRead, dwWritten;
		CHAR chBuf[BUFSIZE_];
		BOOL bSuccess = FALSE;
		HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

		for (;;)
		{
			bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE_, &dwRead, NULL);
			if (!bSuccess || dwRead == 0) break;


			auto res = StrToWStr(chBuf, CP_UTF8);

			bSuccess = WriteFile(hParentStdOut, chBuf,
				dwRead, &dwWritten, NULL);
			if (!bSuccess) break;
		}
	}

	void CreateChildProcess()
		// Create a child process that uses the previously created pipes for STDIN and STDOUT.
	{
		//TCHAR szCmdline[] = L"C:\\Windows\\system32\\cmd.exe /k format h: \fs";
		TCHAR szCmdline[] = "/k format h: \fs";
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFO siStartInfo;
		BOOL bSuccess = FALSE;

		// Set up members of the PROCESS_INFORMATION structure. 

		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

		// Set up members of the STARTUPINFO structure. 
		// This structure specifies the STDIN and STDOUT handles for redirection.

		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
		siStartInfo.cb = sizeof(STARTUPINFO);
		siStartInfo.hStdError = g_hChildStd_OUT_Wr;
		siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
		siStartInfo.hStdInput = g_hChildStd_IN_Rd;
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
		//siStartInfo.wShowWindow = SW_SHOW;

		// Create the child process. 

		bSuccess = CreateProcess("C:\\Windows\\system32\\cmd.exe",
			szCmdline,     // command line 
			NULL,          // process security attributes 
			NULL,          // primary thread security attributes 
			TRUE,          // handles are inherited 
			0,             // creation flags 
			NULL,          // use parent's environment 
			NULL,          // use parent's current directory 
			&siStartInfo,  // STARTUPINFO pointer 
			&piProcInfo);  // receives PROCESS_INFORMATION 

		 // If an error occurs, exit the application. 
		if (!bSuccess)
			ErrorExit((PTSTR)("CreateProcess"));
		else
		{
			bool s = false;
			//Sleep(1000);
			// Attach to command prompt console and change code page before running batch file
			//bool s = AttachConsole(piProcInfo.dwProcessId);

			//auto err = GetLastErrorAsString();

			//s = SetConsoleCP(CP_UTF8);
			//s = SetConsoleOutputCP(CP_UTF8);
			//auto err = GetLastErrorAsString();
			//b = FreeConsole();


			// Close handles to the child process and its primary thread.
			// Some applications might keep these handles to monitor the status
			// of the child process, for example. 

			CloseHandle(piProcInfo.hProcess);
			CloseHandle(piProcInfo.hThread);

			// Close handles to the stdin and stdout pipes no longer needed by the child process.
			// If they are not explicitly closed, there is no way to recognize that the child process has ended.

			CloseHandle(g_hChildStd_OUT_Wr);
			CloseHandle(g_hChildStd_IN_Rd);
		}
	}


	void Test() {
		AllocConsole();
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);

		SECURITY_ATTRIBUTES saAttr;

		TCHAR inputFile[] = "D:\\hello.txt";

		printf("\n->Start of parent execution.\n");

		// Set the bInheritHandle flag so pipe handles are inherited. 

		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;

		// Create a pipe for the child process's STDOUT. 

		if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
			ErrorExit((PTSTR)(L"StdoutRd CreatePipe"));

		// Ensure the read handle to the pipe for STDOUT is not inherited.

		if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
			ErrorExit((PTSTR)(L"Stdout SetHandleInformation"));

		// Create a pipe for the child process's STDIN. 

		if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
			ErrorExit((PTSTR)(L"Stdin CreatePipe"));

		// Ensure the write handle to the pipe for STDIN is not inherited. 

		if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
			ErrorExit((PTSTR)(L"Stdin SetHandleInformation"));

		// Create the child process. 

		CreateChildProcess();

		// Get a handle to an input file for the parent. 
		// This example assumes a plain text file and uses string output to verify data flow. 

		//if (argc == 1)
		//    ErrorExit((PTSTR)(L"Please specify an input file.\n"));

		g_hInputFile = CreateFile(
			//argv[1],
			inputFile,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_READONLY,
			NULL);

		if (g_hInputFile == INVALID_HANDLE_VALUE)
			ErrorExit((PTSTR)(L"CreateFile"));

		// Write to the pipe that is the standard input for a child process. 
		// Data is written to the pipe's buffers, so it is not necessary to wait
		// until the child process is running before writing data.

		WriteToPipeTest();
		//printf("\n->Contents of %S written to child STDIN pipe.\n", argv[1]);
		printf("\n->Contents of %S written to child STDIN pipe.\n", inputFile);

		// Read from pipe that is the standard output for child process. 

		printf("\n->Contents of child process STDOUT:\n\n");
		ReadFromPipeTest();

		printf("\n->End of parent execution.\n");

		// The remaining open handles are cleaned up when this process terminates. 
		// To avoid resource leaks in a larger application, close handles explicitly. 


		Sleep(5'000);
		FreeConsole();
	}
//}





















namespace Core
{
	const std::string CMDDiskFormater::ProceedFormat = "Proceed with Format (Y/N)?";
	const std::string CMDDiskFormater::ProceedQuickFormat = "Proceed with quick format instead (Y/N)? ";
	const std::string CMDDiskFormater::DismountVolume = "Would you like to force a dismount on this volume? (Y/N)";
	const std::string CMDDiskFormater::VolumeLabel = "Volume label (32 characters, ENTER for none)? ";
	const std::string CMDDiskFormater::LowLevelFormat = "Proceed with low level format (Y/N)?";
	const std::string CMDDiskFormater::FormatFailed = "C:\\Windows\\SysWOW64";
	const std::string CMDDiskFormater::CompletedProgress = "percent completed";
	const std::string CMDDiskFormater::FindReadyToParse = "ReadyToParse";
	const std::string CMDDiskFormater::FindFormatComplete = "Format complete.";
	const std::string CMDDiskFormater::FindMediaAlreadyFormated = "Enter current volume label for drive";
	const std::string CMDDiskFormater::FormatComplete = "are available.";

	const std::string CMDDiskFormater::ValueToIgnore1 = "Creating file system structures.";
	const std::string CMDDiskFormater::ValueToIgnore2 = "Formatting";
	const std::string CMDDiskFormater::ValueToIgnore3 = "Format complete";
	const std::string CMDDiskFormater::ValueToIgnore4 = "The new file system is UDF.";
	const std::string CMDDiskFormater::ValueToIgnore5 = "Performing low level format";
	const std::string CMDDiskFormater::ValueToIgnore6 = "Blanking media";
	const std::string CMDDiskFormater::ValueToIgnore7 = "Volume label (32 characters, ENTER for none)? Y";
	const std::string CMDDiskFormater::ValueToIgnore8 = "Volume label (32 characters, ENTER for none)? N";
	const std::string CMDDiskFormater::ValueToIgnore9 = "Proceed with quick format instead (Y/N)? N";
	const std::string CMDDiskFormater::ValueToIgnore10 = "Proceed with low level format (Y/N)? Y";
	const std::string CMDDiskFormater::ValueToIgnore11 = "Volume dismounted.  All opened handles to this volume are now invalid.";
	const std::string CMDDiskFormater::ValueToIgnore12 = "Proceed with quick format instead (Y/N)? Y";
	const std::string CMDDiskFormater::ValueToIgnore13 = "Proceed with low level format (Y/N)? N";

	bool WriteStringToFile(HANDLE hFile, const std::string& buffer) {
		return ::WriteFile(hFile, buffer.c_str(), buffer.size(), NULL, NULL);
	}

	CMDDiskFormater::CMDDiskFormater() {
	}

	void CMDDiskFormater::FormatDisk(WCHAR drive_letter, std::wstring label, std::shared_ptr<IWriteProgress> current_progress, std::shared_ptr<ICancelWrite> token,  long double *estimated_write_time, int *completed){
		//Test();
		//throw 1;

		//bool s = AttachConsole(pi.dwProcessId);
		//auto err = GetLastErrorAsString();

		bool s = AllocConsole();
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		//err = GetLastErrorAsString();

		auto AllocateConsoleScope = MakeScope([&] {
			FreeConsole();
			});

		std::wstring commandline = L"/k \"format ";
		commandline.push_back(drive_letter);
		std::replace(label.begin(), label.end(), '\\', '-');
		std::replace(label.begin(), label.end(), '/', '-');
		std::replace(label.begin(), label.end(), ' ', '_');
		commandline = commandline + L": /fs:" + UDF + L" /v:" + label /*+ L" /r:" + UDF_REVISION*/;
		commandline += L" & exit\"";
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
		std::string label_name = convert.to_bytes(label);
		
		label_name = label_name + "\r\n";

		SECURITY_ATTRIBUTES sa;

		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = 0;
		sa.bInheritHandle = TRUE;

		HANDLE hReadPipe1, hWritePipe1, hReadPipe2, hWritePipe2;

		if (!CreatePipe(&hReadPipe1, &hWritePipe1, &sa, BUFSIZE))
			throw_if_fail(HRESULT_FROM_WIN32(GetLastError()));
		
		auto ReadPipeScope = MakeScope([&] {
			CloseHandle(hReadPipe1);
			CloseHandle(hWritePipe1);
		});

		if (!CreatePipe(&hReadPipe2, &hWritePipe2, &sa, BUFSIZE))
			throw_if_fail(HRESULT_FROM_WIN32(GetLastError()));
		
		auto WritePipeScope = MakeScope([&] {
			CloseHandle(hReadPipe2);
			CloseHandle(hWritePipe2);
		});

		STARTUPINFOW si = {};
		GetStartupInfoW(&si);

		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = CMD_SHOW;
		si.hStdInput = hReadPipe2;
		si.hStdOutput = si.hStdError = hWritePipe1;

		PROCESS_INFORMATION pi = {};
		if (!CreateProcessW(CMD_PATH, (LPWSTR)commandline.c_str(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
			throw_if_fail(HRESULT_FROM_WIN32(GetLastError()));
		}


		auto CreateProcessScope = MakeScope([&] {
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		});

		auto ConsoleScope = MakeScope([&] {
			WriteStringToFile(hWritePipe2, "^C\r\n");
			WriteStringToFile(hWritePipe2, "exit\r\n");
		});

		int start_time = GetTickCount64();
		int time_elapsed = 0;
		int time_estimate = 0;
		int previous_progress = 0;

		bool format_failed = false;
		bool confirm_low_level_format = true;
		bool first_step_complete = false;
		*completed = 0;
		uint64_t report = 0;

		while (true) {
			//Sleep(10);
			auto buff = this->GetCMDInfo(hReadPipe1);
			auto res = StrToWStr(buff, CP_UTF8);

			time_elapsed = GetTickCount64() - start_time;

			if (token->IsCancellationRequested()) {
				WriteStringToFile(hWritePipe2, "^C\r\n");
				WriteStringToFile(hWritePipe2, "exit\r\n");
				current_progress->Report(WriteAction::Canceled, 0, MAX_PROGRESS, 0, 0, 0);
				token->ThrowIfCancellationRequested();
			}
			else if (CMDDiskFormater::Contains(buff, CMDDiskFormater::ProceedFormat)) {
				WriteStringToFile(hWritePipe2, "Y\r\n");
			}
			else if (buff == CMDDiskFormater::ProceedQuickFormat) {
				confirm_low_level_format = false;
				WriteStringToFile(hWritePipe2, "N\r\n");
			}
			else if (CMDDiskFormater::Contains(buff, CMDDiskFormater::DismountVolume)) {
				WriteStringToFile(hWritePipe2, "Y\r\n");
			}
			else if (buff == CMDDiskFormater::VolumeLabel) {
				WriteStringToFile(hWritePipe2, label_name);
			}
			else if (CMDDiskFormater::Contains(buff, CMDDiskFormater::LowLevelFormat)) {
				WriteStringToFile(hWritePipe2, "Y\r\n");
			}
			else if (CMDDiskFormater::Contains(buff, CMDDiskFormater::FormatFailed))
			{
				format_failed = true;
				break;
				//throw std::runtime_error("Format failed!");
			}
			else if (CMDDiskFormater::Contains(buff, CMDDiskFormater::FindReadyToParse)) {
				buff = buff.substr(buff.find(CMDDiskFormater::FindReadyToParse) + CMDDiskFormater::FindReadyToParse.size());
				int percent = std::stoi(buff);
				if (percent == 100) {
					first_step_complete = true;
				}

				if (confirm_low_level_format) {
					if (first_step_complete)
						percent = percent / 4 + 25;
					else
						percent = percent / 4;

					if (percent == 0)
						percent = 1;

					time_estimate = (time_elapsed * MAX_PROGRESS) / percent;
					if (percent < 25)
						time_estimate += TenMinute;

					if (report % 300 == 0) {
						current_progress->Report(WriteAction::FormattingMedia, percent, MAX_PROGRESS, time_elapsed / 1000, (time_estimate + static_cast<int>(*estimated_write_time)) / 1000, 0);
					}
					*completed = percent;
				}
				else {
					if (percent == 0) {
						current_progress->Report(WriteAction::FormattingMedia, 0, MAX_PROGRESS, time_elapsed / 1000, (OneMinute + static_cast<int>(*estimated_write_time)) / 1000, 0);
						*completed = 0;
					}
					else {
						current_progress->Report(WriteAction::FormattingMedia, MAX_PROGRESS / 10, MAX_PROGRESS, time_elapsed /1000, (OneMinute + static_cast<int>(*estimated_write_time)) / 1000, 0);
						*completed = MAX_PROGRESS / 10;
					}
				}
				
			}
			else if (CMDDiskFormater::Contains(buff, CMDDiskFormater::FormatComplete)) {
				//current_progress->Report(WriteAction::FormatCompleted, MAX_PROGRESS, MAX_PROGRESS, 0, 0, 0);
				break;
			}
			else if (CMDDiskFormater::Contains(buff, CMDDiskFormater::FindMediaAlreadyFormated)){
				break;
			}
			else if (CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore1)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore2)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore3)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore4)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore5)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore6)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore7)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore8)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore9)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore10)	||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore11)  ||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore12)  ||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::ValueToIgnore13)  ||
					 CMDDiskFormater::Contains(buff, CMDDiskFormater::VolumeLabel))
			{
				if (report % 300 == 0) {
					current_progress->Report(WriteAction::FormattingMedia, *completed, MAX_PROGRESS, time_elapsed / 1000, (OneMinute + static_cast<int>(*estimated_write_time)) / 1000, 0);
				}
			}
			else {
				throw std::runtime_error(buff);
			}
			++report;
		}

		WaitForSingleObject(pi.hProcess, INFINITE);

		if (format_failed) {
			if (this->GetExistedFileSystem(drive_letter) != L"UDF") {
				throw std::runtime_error("Format failed!");
			}
		}

		//*estimated_write_time = time_elapsed;
	}

	std::wstring CMDDiskFormater::GetExistedFileSystem(WCHAR drive_letter) {
		std::wstring commandline = L"/k \"format ";
		commandline.push_back(drive_letter);
		commandline = commandline + L": /fs:" + UDF;
		commandline += L" & exit\"";
		std::string buff;
		SECURITY_ATTRIBUTES sa;

		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = 0;
		sa.bInheritHandle = TRUE;

		HANDLE hReadPipe1, hWritePipe1, hReadPipe2, hWritePipe2;

		if (!CreatePipe(&hReadPipe1, &hWritePipe1, &sa, BUFSIZE))
			throw_if_fail(HRESULT_FROM_WIN32(GetLastError()));

		auto ReadPipeScope = MakeScope([&]
			{
				CloseHandle(hReadPipe1);
				CloseHandle(hWritePipe1);
			});

		if (!CreatePipe(&hReadPipe2, &hWritePipe2, &sa, BUFSIZE))
			throw_if_fail(HRESULT_FROM_WIN32(GetLastError()));

		auto WritePipeScope = MakeScope([&]
			{
				CloseHandle(hReadPipe2);
				CloseHandle(hWritePipe2);
			});

		STARTUPINFOW si = {};

		GetStartupInfoW(&si);

		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = CMD_SHOW;
		si.hStdInput = hReadPipe2;
		si.hStdOutput = si.hStdError = hWritePipe1;

		PROCESS_INFORMATION pi = {};
		if (!CreateProcessW(CMD_PATH, (LPWSTR)commandline.c_str(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
			throw_if_fail(HRESULT_FROM_WIN32(GetLastError()));
		}
		auto CreateProcessScope = MakeScope([&]
			{
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			});

		unsigned long lBytesWrite = 0;

		auto ConsoleScope = MakeScope([&] {
			WriteStringToFile(hWritePipe2, "^C\r\n");
			WriteStringToFile(hWritePipe2, "exit\r\n");
		});

		buff = this->GetFileSystemInfo(hReadPipe1);
		WriteStringToFile(hWritePipe2, "N\r\n");

		buff = buff.substr(FSMessageStart, 3);
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
		
		WaitForSingleObject(pi.hProcess, INFINITE);
		return convert.from_bytes(buff);
	}

	BOOL CMDDiskFormater::ReadFromPipe(HANDLE pipe, std::string& buf) {
		if (!CMDDiskFormater::ReadPipe(pipe, buf)) {
			return FALSE;
		}

		if (buf.rfind(CMDDiskFormater::FindFormatComplete) != std::string::npos) {
			buf = CMDDiskFormater::FormatComplete;
			return TRUE;
		}

		buf = CMDDiskFormater::GetLastLine(buf);

		if (buf == CMDDiskFormater::VolumeLabel) {
			return TRUE;
		}

		int progress_size = std::string(CMDDiskFormater::CompletedProgress).size();
		if (CMDDiskFormater::Contains(buf, CMDDiskFormater::CompletedProgress)) {
			size_t end_index = buf.rfind(CompletedProgress);
			auto start_index = end_index;
			
			while (true) {
				if (start_index == 0)
					break;
				else if (buf.at(start_index) == '\r')
					break;
				else
					start_index--;
			}

			if (start_index == 0)
				buf = CMDDiskFormater::FindReadyToParse + buf.substr(start_index, end_index - start_index - 1);
			else
				buf = CMDDiskFormater::FindReadyToParse + buf.substr(start_index + 1, end_index - start_index - 2);
		}

		return TRUE;
	}

	BOOL CMDDiskFormater::ReadFileSystemInfo(HANDLE pipe, std::string& buf) {
		return CMDDiskFormater::ReadPipe(pipe, buf);
	}

	BOOL CMDDiskFormater::ReadPipe(HANDLE pipe, std::string& buf) {
		unsigned long lpBytesRead, lpBytesLeft, readed;

		std::vector<char> tmp(BUFSIZE);
		lpBytesRead = 0;
		lpBytesLeft = 0;

		while (lpBytesRead == 0 && PeekNamedPipe(pipe, tmp.data(), static_cast<DWORD>(tmp.size()), &readed, &lpBytesRead, &lpBytesLeft)) {
			if (readed == 0)
				return FALSE;
		}

		buf = std::string(tmp.begin(), tmp.begin() + readed);
		//OutputDebugStringA(buf.c_str());

		while (!buf.empty() && CMDDiskFormater::IsEndOfLine(buf.back())) {
			buf.pop_back();
		}

		return TRUE;
	}

	std::string CMDDiskFormater::GetLastLine(const std::string& buf) {
		if (buf.empty()) {
			return std::string();
		}

		size_t idx = buf.size() - 1;

		while (idx > 0 && CMDDiskFormater::IsEndOfLine(buf[idx])) {
			idx--;
		}

		auto strEnd = idx + 1;

		while (idx > 0 && !CMDDiskFormater::IsEndOfLine(buf[idx])) {
			idx--;
		}

		auto strStart = idx;

		if (CMDDiskFormater::IsEndOfLine(buf[strStart])) {
			strStart++;
			assert(!CMDDiskFormater::IsEndOfLine(buf[strStart]));
		}

		return buf.substr(strStart, strEnd - strStart);
	}

	bool CMDDiskFormater::IsEndOfLine(char c) {
		return c == '\r' || c == '\n';
	}

	bool CMDDiskFormater::Contains(const std::string& a, const std::string& b) {
		return a.find(b) != std::string::npos;
	}

	std::string CMDDiskFormater::GetFileSystemInfo(HANDLE pipe) {
		std::string buf;
		std::string prev_buff;
		while (true) {
			//Sleep(100);
			BOOL result1 = ReadFileSystemInfo(pipe, buf);
			if (result1 == TRUE && prev_buff == buf)
				break;
			else
				prev_buff = buf;
		}
		return buf;
	}

	std::string CMDDiskFormater::GetCMDInfo(HANDLE pipe) {
		std::string buf;
		std::string prev_buff;
		while (true) {
			//Sleep(100);
			BOOL result1 = ReadFromPipe(pipe, buf);
			if (result1 == TRUE && prev_buff == buf)
				break;
			else
				prev_buff = buf;
		}
		return buf;
	}
}
