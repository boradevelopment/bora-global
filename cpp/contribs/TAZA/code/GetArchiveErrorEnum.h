#pragma once

#if TAZAEXTRAS // Extras will always start with this - Extras are to ease the developer process
/// <summary>
/// Errors that come from the result in human readable form!
/// Parameter Error means that you don't meet the requirements of using the function
/// </summary>
namespace GetArchiveError {
	constexpr int OK = 0;
	constexpr int UNSUPPORTED = -1;
	constexpr int BAD_FILE = 1;
	constexpr int NOT_TAZA_FORMAT = 2;
	constexpr int CORRUPTED = 3;
}
#endif

