#pragma once

#if TAZAEXTRAS // Extras will always start with this - Extras are to ease the developer process
/// <summary>
/// Errors that come from the result in human readable form! (Output error means that the file does exist in archive but for some reason, we can't get it. Details may be in log)
/// </summary>
namespace FileResultError {
	constexpr int OK = 0;
	constexpr int BAD_FILE = 1;
	constexpr int BAD_ARCHIVE = 2;
	constexpr int OUTPUT_ERROR = 3;
	
}
#endif

