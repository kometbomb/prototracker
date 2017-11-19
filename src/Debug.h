#pragma once

#include <cstdio>

void debug(const char *message, ...) __attribute__((format(printf, 1, 2)));
