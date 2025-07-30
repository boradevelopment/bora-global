#pragma once

#if TAZAEXTRAS // Extras will always start with this - Extras are to ease the developer process
/// <summary>
/// Errors that come from the result in human readable form!
/// Parameter Error means that you don't meet the requirements of using the function
/// </summary>
namespace MakeArchiveError {
	constexpr int OK = 0;
	constexpr int UNSUPPORTED = -1;
	constexpr int PARAMETER_ERROR = 1;
	constexpr int  INPUT_ERROR = 2;
	constexpr int INPUT_EMPTY = 3;
	constexpr int OUTPUT_ERROR = 4;
}
#endif

