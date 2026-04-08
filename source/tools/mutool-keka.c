// Copyright (C) 2004-2025 Artifex Software, Inc.
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

/*
 * mutool-keka -- swiss army knife of pdf manipulation tools
 */

#include "mupdf/fitz.h"

#include <string.h>
#include <stdio.h>

int pdfextract_main(int argc, char *argv[]);

int main(int argc, char **argv)
{
	if (argc > 1 && !strcmp(argv[1], "-v"))
	{
		fprintf(stderr, "mutool version %s (modified for Keka)\n", FZ_VERSION);
		return 0;
	}
	else if (argc < 2)
	{
		fprintf(stderr, "mutool version %s (modified for Keka)\n", FZ_VERSION);
		fprintf(stderr, "usage: mutool extract [options] archivo.pdf [objects]\n");
		return 1;
	}

	if (!strcmp(argv[1], "extract"))
	{
		return pdfextract_main(argc - 1, argv + 1);
	}

	fprintf(stderr, "Command '%s' not supported in this modified version.\n", argv[1]);
	return 1;
}

// Disable conversion code for some images
void fz_register_image_handlers(fz_context *ctx) {}

// Disable inclusion of some fonts
const unsigned char _binary_Dingbats_cff[1] = {0};
const unsigned int _binary_Dingbats_cff_size = 0;
const unsigned char _binary_NimbusMonoPS_BoldItalic_cff[1] = {0};
const unsigned int _binary_NimbusMonoPS_BoldItalic_cff_size = 0;
const unsigned char _binary_NimbusMonoPS_Bold_cff[1] = {0};
const unsigned int _binary_NimbusMonoPS_Bold_cff_size = 0;
const unsigned char _binary_NimbusMonoPS_Italic_cff[1] = {0};
const unsigned int _binary_NimbusMonoPS_Italic_cff_size = 0;
const unsigned char _binary_NimbusMonoPS_Regular_cff[1] = {0};
const unsigned int _binary_NimbusMonoPS_Regular_cff_size = 0;
const unsigned char _binary_NimbusRoman_BoldItalic_cff[1] = {0};
const unsigned int _binary_NimbusRoman_BoldItalic_cff_size = 0;
const unsigned char _binary_NimbusRoman_Bold_cff[1] = {0};
const unsigned int _binary_NimbusRoman_Bold_cff_size = 0;
const unsigned char _binary_NimbusRoman_Italic_cff[1] = {0};
const unsigned int _binary_NimbusRoman_Italic_cff_size = 0;
const unsigned char _binary_NimbusRoman_Regular_cff[1] = {0};
const unsigned int _binary_NimbusRoman_Regular_cff_size = 0;
const unsigned char _binary_NimbusSans_BoldItalic_cff[1] = {0};
const unsigned int _binary_NimbusSans_BoldItalic_cff_size = 0;
const unsigned char _binary_NimbusSans_Bold_cff[1] = {0};
const unsigned int _binary_NimbusSans_Bold_cff_size = 0;
const unsigned char _binary_NimbusSans_Italic_cff[1] = {0};
const unsigned int _binary_NimbusSans_Italic_cff_size = 0;
const unsigned char _binary_NimbusSans_Regular_cff[1] = {0};
const unsigned int _binary_NimbusSans_Regular_cff_size = 0;
const unsigned char _binary_StandardSymbolsPS_cff[1] = {0};
const unsigned int _binary_StandardSymbolsPS_cff_size = 0;
