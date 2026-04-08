// Copyright (C) 2004-2024 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

// Only include essential CMaps to save ~1MB of binary size
#include "cmaps/Identity-H.h"
#include "cmaps/Identity-V.h"
#include "cmaps/TrueType-UCS2.h"

#include <string.h>

static pdf_cmap *
pdf_load_embedded_cmap_imp(fz_context *ctx, pdf_document *doc, pdf_obj *stmobj, pdf_cycle_list *cycle_up)
{
	pdf_cycle_list cycle;
	fz_stream *file = NULL;
	pdf_cmap *cmap = NULL;
	pdf_cmap *usecmap = NULL;
	pdf_obj *obj;

	fz_var(file);
	fz_var(cmap);
	fz_var(usecmap);

	if ((cmap = pdf_find_item(ctx, pdf_drop_cmap_imp, stmobj)) != NULL)
		return cmap;

	fz_try(ctx)
	{
		file = pdf_open_stream(ctx, stmobj);
		cmap = pdf_load_cmap(ctx, file);

		obj = pdf_dict_get(ctx, stmobj, PDF_NAME(WMode));
		if (pdf_is_int(ctx, obj))
			pdf_set_cmap_wmode(ctx, cmap, pdf_to_int(ctx, obj));

		obj = pdf_dict_get(ctx, stmobj, PDF_NAME(UseCMap));
		if (pdf_is_name(ctx, obj))
		{
			fz_try(ctx)
			{
				usecmap = pdf_load_system_cmap(ctx, pdf_to_name(ctx, obj));
				pdf_set_usecmap(ctx, cmap, usecmap);
			}
			fz_catch(ctx)
			{
				fz_rethrow_if(ctx, FZ_ERROR_SYSTEM);
				fz_report_error(ctx);
				fz_warn(ctx, "cannot load system CMap: %s", pdf_to_name(ctx, obj));
			}
		}
		else if (pdf_is_indirect(ctx, obj))
		{
			if (pdf_cycle(ctx, &cycle, cycle_up, obj))
				fz_throw(ctx, FZ_ERROR_FORMAT, "recursive CMap");
			usecmap = pdf_load_embedded_cmap_imp(ctx, doc, obj, &cycle);
			pdf_set_usecmap(ctx, cmap, usecmap);
		}
		else if (strlen(cmap->usecmap_name) > 0)
		{
			fz_try(ctx)
			{
				usecmap = pdf_load_system_cmap(ctx, cmap->usecmap_name);
				pdf_set_usecmap(ctx, cmap, usecmap);
			}
			fz_catch(ctx)
			{
				fz_rethrow_if(ctx, FZ_ERROR_SYSTEM);
				fz_report_error(ctx);
				fz_warn(ctx, "cannot load system CMap: %s", pdf_to_name(ctx, obj));
			}
		}

		pdf_store_item(ctx, stmobj, cmap, pdf_cmap_size(ctx, cmap));
	}
	fz_always(ctx)
	{
		fz_drop_stream(ctx, file);
		pdf_drop_cmap(ctx, usecmap);
	}
	fz_catch(ctx)
	{
		pdf_drop_cmap(ctx, cmap);
		fz_rethrow(ctx);
	}

	return cmap;
}

pdf_cmap *
pdf_load_embedded_cmap(fz_context *ctx, pdf_document *doc, pdf_obj *stmobj)
{
	return pdf_load_embedded_cmap_imp(ctx, doc, stmobj, NULL);
}

pdf_cmap *
pdf_new_identity_cmap(fz_context *ctx, int wmode, int bytes)
{
	pdf_cmap *cmap = pdf_new_cmap(ctx);
	fz_try(ctx)
	{
		unsigned int high = (1 << (bytes * 8)) - 1;
		if (wmode)
			fz_strlcpy(cmap->cmap_name, "Identity-V", sizeof cmap->cmap_name);
		else
			fz_strlcpy(cmap->cmap_name, "Identity-H", sizeof cmap->cmap_name);
		pdf_add_codespace(ctx, cmap, 0, high, bytes);
		pdf_map_range_to_range(ctx, cmap, 0, high, 0);
		pdf_sort_cmap(ctx, cmap);
		pdf_set_cmap_wmode(ctx, cmap, wmode);
	}
	fz_catch(ctx)
	{
		pdf_drop_cmap(ctx, cmap);
		fz_rethrow(ctx);
	}
	return cmap;
}

// Replaced the massive CMap lookup table with a minimal switch-case
pdf_cmap *
pdf_load_builtin_cmap(fz_context *ctx, const char *name)
{
	if (!strcmp(name, "Identity-H")) return &cmap_Identity_H;
	if (!strcmp(name, "Identity-V")) return &cmap_Identity_V;
	if (!strcmp(name, "TrueType-UCS2")) return &cmap_TrueType_UCS2;
	return NULL;
}

pdf_cmap *
pdf_load_system_cmap(fz_context *ctx, const char *cmap_name)
{
	pdf_cmap *usecmap;
	pdf_cmap *cmap;

	cmap = pdf_load_builtin_cmap(ctx, cmap_name);
	if (!cmap) {
		/* Keka Hack: Return NULL instead of throwing an exception to avoid crashes on missing Asian CMaps */
		return NULL;
	}

	if (cmap->usecmap_name[0] && !cmap->usecmap)
	{
		usecmap = pdf_load_system_cmap(ctx, cmap->usecmap_name);
		if (!usecmap)
			fz_warn(ctx, "no builtin cmap file: %s", cmap->usecmap_name);
		pdf_set_usecmap(ctx, cmap, usecmap);
	}

	return cmap;
}
