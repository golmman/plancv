#ifndef _TEXT_H_
#define _TEXT_H_

#include "common.h"

void tess_init(TessBaseAPI& api, const char* language);
void tess_end(TessBaseAPI& api);

void test_text();

#endif