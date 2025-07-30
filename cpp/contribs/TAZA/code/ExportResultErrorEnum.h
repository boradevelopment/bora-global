#pragma once

#if TAZAEXTRAS // Extras will always start with this - Extras are to ease the developer process
/// <summary>
	/// Errors that come from the result in human readable form!
	/// </summary>
namespace ExportResultError {
	constexpr int OK = 0;
	constexpr int BAD_ARCHIVE = 0;
	constexpr int BAD_FILE = 3;
	constexpr int GET_FAILURE = 1;
}
#endif

