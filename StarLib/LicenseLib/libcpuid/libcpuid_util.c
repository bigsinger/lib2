/*
 * Copyright 2008  Veselin Georgiev,
 * anrieffNOSPAM @ mgail_DOT.com (convert to gmail)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "libcpuid.h"
#include "libcpuid_util.h"

int _current_verboselevel;

void match_features(const struct feature_map_t* matchtable, int count, uint32_t reg, struct cpu_id_t* data)
{
	int i;
	for (i = 0; i < count; i++)
		if (reg & (1 << matchtable[i].bit))
			data->flags[matchtable[i].feature] = 1;
}

static void default_warn(const char *msg)
{
	fprintf(stderr, "%s", msg);
}

libcpuid_warn_fn_t _warn_fun = default_warn;

#if defined(_MSC_VER) && defined(_M_AMD64)
#	define vsnprintf _vsnprintf
#endif
void warnf(const char* format, ...)
{
	char buff[1024];
	va_list va;
	if (!_warn_fun) return;
	va_start(va, format);
	vsnprintf(buff, sizeof(buff), format, va);
	va_end(va);
	_warn_fun(buff);
}

void debugf(int verboselevel, const char* format, ...)
{
	char buff[1024];
	va_list va;
	if (verboselevel > _current_verboselevel) return;
	va_start(va, format);
	vsnprintf(buff, sizeof(buff), format, va);
	va_end(va);
	_warn_fun(buff);
}

static int score(const struct match_entry_t* entry, const struct cpu_id_t* data,
                 int brand_code, int model_code)
{
	int res = 0;
	if (entry->family	== data->family    ) res++;
	if (entry->model	== data->model     ) res++;
	if (entry->stepping	== data->stepping  ) res++;
	if (entry->ext_family	== data->ext_family) res++;
	if (entry->ext_model	== data->ext_model ) res++;
	if (entry->ncores	== data->num_cores ) res++;
	if (entry->l2cache	== data->l2_cache  ) res++;
	if (entry->brand_code   == brand_code      ) res++;
	if (entry->model_code   == model_code      ) res++;
	return res;
}

void match_cpu_codename(const struct match_entry_t* matchtable, int count,
                        struct cpu_id_t* data, int brand_code, int model_code)
{
	int bestscore = -1;
	int bestindex = 0;
	int i, t;
	
	debugf(3, "Matching cpu f:%d, m:%d, s:%d, xf:%d, xm:%d, ncore:%d, l2:%d, bcode:%d, code:%d\n",
		data->family, data->model, data->stepping, data->ext_family,
		data->ext_model, data->num_cores, data->l2_cache, brand_code, model_code);
	
	for (i = 0; i < count; i++) {
		t = score(&matchtable[i], data, brand_code, model_code);
		debugf(3, "Entry %d, `%s', score %d\n", i, matchtable[i].name, t);
		if (t > bestscore) {
			debugf(2, "Entry `%s' selected - best score so far (%d)\n", matchtable[i].name, t);
			bestscore = t;
			bestindex = i;
		}
	}
	strcpy(data->cpu_codename, matchtable[bestindex].name);
}

void generic_get_cpu_list(const struct match_entry_t* matchtable, int count,
                          struct cpu_list_t* list)
{
	int i, j, n, good;
	n = 0;
	list->names = (char**) malloc(sizeof(char*) * count);
	for (i = 0; i < count; i++) {
		if (strstr(matchtable[i].name, "Unknown")) continue;
		good = 1;
		for (j = n - 1; j >= 0; j--)
			if (!strcmp(list->names[j], matchtable[i].name)) {
				good = 0;
				break;
			}
		if (!good) continue;
		list->names[n++] = _strdup(matchtable[i].name);
	}
	list->num_entries = n;
}
