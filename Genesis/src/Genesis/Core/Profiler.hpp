#pragma once

#define USING_EASY_PROFILER
#include <easy/profiler.h>

#define GENESIS_PROFILE_START() EASY_PROFILER_ENABLE
#define GENESIS_PROFILE_WRITE_TO_FILE(...)  profiler::dumpBlocksToFile(__VA_ARGS__)

#define GENESIS_PROFILE_FUNCTION(...) EASY_FUNCTION(__VA_ARGS__)
#define GENESIS_PROFILE_BLOCK_START(...) EASY_BLOCK(__VA_ARGS__)
#define GENESIS_PROFILE_BLOCK_END() EASY_END_BLOCK