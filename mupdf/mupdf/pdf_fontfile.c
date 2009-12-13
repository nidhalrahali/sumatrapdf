#define NOCJK /* disable built-in CJK support (use a system provided TTF instead - if available) */

#include "fitz.h"
#include "mupdf.h"

extern const unsigned char pdf_font_Dingbats_cff_buf[];
extern const unsigned int  pdf_font_Dingbats_cff_len;
extern const unsigned char pdf_font_NimbusMonL_Bold_cff_buf[];
extern const unsigned int  pdf_font_NimbusMonL_Bold_cff_len;
extern const unsigned char pdf_font_NimbusMonL_BoldObli_cff_buf[];
extern const unsigned int  pdf_font_NimbusMonL_BoldObli_cff_len;
extern const unsigned char pdf_font_NimbusMonL_Regu_cff_buf[];
extern const unsigned int  pdf_font_NimbusMonL_Regu_cff_len;
extern const unsigned char pdf_font_NimbusMonL_ReguObli_cff_buf[];
extern const unsigned int  pdf_font_NimbusMonL_ReguObli_cff_len;
extern const unsigned char pdf_font_NimbusRomNo9L_Medi_cff_buf[];
extern const unsigned int  pdf_font_NimbusRomNo9L_Medi_cff_len;
extern const unsigned char pdf_font_NimbusRomNo9L_MediItal_cff_buf[];
extern const unsigned int  pdf_font_NimbusRomNo9L_MediItal_cff_len;
extern const unsigned char pdf_font_NimbusRomNo9L_Regu_cff_buf[];
extern const unsigned int  pdf_font_NimbusRomNo9L_Regu_cff_len;
extern const unsigned char pdf_font_NimbusRomNo9L_ReguItal_cff_buf[];
extern const unsigned int  pdf_font_NimbusRomNo9L_ReguItal_cff_len;
extern const unsigned char pdf_font_NimbusSanL_Bold_cff_buf[];
extern const unsigned int  pdf_font_NimbusSanL_Bold_cff_len;
extern const unsigned char pdf_font_NimbusSanL_BoldItal_cff_buf[];
extern const unsigned int  pdf_font_NimbusSanL_BoldItal_cff_len;
extern const unsigned char pdf_font_NimbusSanL_Regu_cff_buf[];
extern const unsigned int  pdf_font_NimbusSanL_Regu_cff_len;
extern const unsigned char pdf_font_NimbusSanL_ReguItal_cff_buf[];
extern const unsigned int  pdf_font_NimbusSanL_ReguItal_cff_len;
extern const unsigned char pdf_font_StandardSymL_cff_buf[];
extern const unsigned int  pdf_font_StandardSymL_cff_len;
extern const unsigned char pdf_font_URWChanceryL_MediItal_cff_buf[];
extern const unsigned int  pdf_font_URWChanceryL_MediItal_cff_len;

#ifndef NOCJK
extern const unsigned char pdf_font_DroidSansFallback_ttf_buf[];
extern const unsigned int  pdf_font_DroidSansFallback_ttf_len;
#endif

enum
{
	FD_FIXED = 1 << 0,
	FD_SERIF = 1 << 1,
	FD_SYMBOLIC = 1 << 2,
	FD_SCRIPT = 1 << 3,
	FD_NONSYMBOLIC = 1 << 5,
	FD_ITALIC = 1 << 6,
	FD_ALLCAP = 1 << 16,
	FD_SMALLCAP = 1 << 17,
	FD_FORCEBOLD = 1 << 18
};

enum { CNS, GB, Japan, Korea };
enum { MINCHO, GOTHIC };

static const struct
{
	const char *name;
	const unsigned char *cff;
	const unsigned int *len;
	} basefonts[] = {
	{ "Courier",
		pdf_font_NimbusMonL_Regu_cff_buf,
		&pdf_font_NimbusMonL_Regu_cff_len },
	{ "Courier-Bold",
		pdf_font_NimbusMonL_Bold_cff_buf,
		&pdf_font_NimbusMonL_Bold_cff_len },
	{ "Courier-Oblique",
		pdf_font_NimbusMonL_ReguObli_cff_buf,
		&pdf_font_NimbusMonL_ReguObli_cff_len },
	{ "Courier-BoldOblique",
		pdf_font_NimbusMonL_BoldObli_cff_buf,
		&pdf_font_NimbusMonL_BoldObli_cff_len },
	{ "Helvetica",
		pdf_font_NimbusSanL_Regu_cff_buf,
		&pdf_font_NimbusSanL_Regu_cff_len },
	{ "Helvetica-Bold",
		pdf_font_NimbusSanL_Bold_cff_buf,
		&pdf_font_NimbusSanL_Bold_cff_len },
	{ "Helvetica-Oblique",
		pdf_font_NimbusSanL_ReguItal_cff_buf,
		&pdf_font_NimbusSanL_ReguItal_cff_len },
	{ "Helvetica-BoldOblique",
		pdf_font_NimbusSanL_BoldItal_cff_buf,
		&pdf_font_NimbusSanL_BoldItal_cff_len },
	{ "Times-Roman",
		pdf_font_NimbusRomNo9L_Regu_cff_buf,
		&pdf_font_NimbusRomNo9L_Regu_cff_len },
	{ "Times-Bold",
		pdf_font_NimbusRomNo9L_Medi_cff_buf,
		&pdf_font_NimbusRomNo9L_Medi_cff_len },
	{ "Times-Italic",
		pdf_font_NimbusRomNo9L_ReguItal_cff_buf,
		&pdf_font_NimbusRomNo9L_ReguItal_cff_len },
	{ "Times-BoldItalic",
		pdf_font_NimbusRomNo9L_MediItal_cff_buf,
		&pdf_font_NimbusRomNo9L_MediItal_cff_len },
	{ "Symbol",
		pdf_font_StandardSymL_cff_buf,
		&pdf_font_StandardSymL_cff_len },
	{ "ZapfDingbats",
		pdf_font_Dingbats_cff_buf,
		&pdf_font_Dingbats_cff_len },
	{ "Chancery",
		pdf_font_URWChanceryL_MediItal_cff_buf,
		&pdf_font_URWChanceryL_MediItal_cff_len },
	{ nil, nil, nil }
};

#ifdef WIN32
/***** start of Windows font loading code *****/

#include <windows.h>

// TODO: Use more of FreeType for TTF parsing (for performance reasons,
//       the fonts can't be parsed completely, though)
#include <ft2build.h>
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TAGS_H

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

#define TTC_VERSION1	0x00010000
#define TTC_VERSION2	0x00020000

#define MAX_FACENAME	128

typedef struct pdf_fontmapMS_s
{
	char fontface[MAX_FACENAME];
	char fontpath[MAX_PATH];
	int index;
} pdf_fontmapMS;

typedef struct pdf_fontlistMS_s
{
	pdf_fontmapMS *fontmap;
	int len;
	int cap;
} pdf_fontlistMS;

typedef struct _tagTT_OFFSET_TABLE
{
	ULONG	uVersion;
	USHORT	uNumOfTables;
	USHORT	uSearchRange;
	USHORT	uEntrySelector;
	USHORT	uRangeShift;
} TT_OFFSET_TABLE;

typedef struct _tagTT_TABLE_DIRECTORY
{
	ULONG	uTag;				//table name
	ULONG	uCheckSum;			//Check sum
	ULONG	uOffset;			//Offset from beginning of file
	ULONG	uLength;			//length of the table in bytes
} TT_TABLE_DIRECTORY;

typedef struct _tagTT_NAME_TABLE_HEADER
{
	USHORT	uFSelector;			//format selector. Always 0
	USHORT	uNRCount;			//Name Records count
	USHORT	uStorageOffset;		//Offset for strings storage, from start of the table
} TT_NAME_TABLE_HEADER;

typedef struct _tagTT_NAME_RECORD
{
	USHORT	uPlatformID;
	USHORT	uEncodingID;
	USHORT	uLanguageID;
	USHORT	uNameID;
	USHORT	uStringLength;
	USHORT	uStringOffset;	//from start of storage area
} TT_NAME_RECORD;

typedef struct _tagFONT_COLLECTION
{
	ULONG	Tag;
	ULONG	Version;
	ULONG	NumFonts;
} FONT_COLLECTION;

static struct {
	char *name;
	char *pattern;
} baseSubstitutes[] = {
	{ "Courier", "CourierNewPSMT" },
	{ "Courier-Bold", "CourierNewPS-BoldMT" },
	{ "Courier-Oblique", "CourierNewPS-ItalicMT" },
	{ "Courier-BoldOblique", "CourierNewPS-BoldItalicMT" },
	{ "Helvetica", "ArialMT" },
	{ "Helvetica-Bold", "Arial-BoldMT" },
	{ "Helvetica-Oblique", "Arial-ItalicMT" },
	{ "Helvetica-BoldOblique", "Arial-BoldItalicMT" },
	{ "Times-Roman", "TimesNewRomanPSMT" },
	{ "Times-Bold", "TimesNewRomanPS-BoldMT" },
	{ "Times-Italic", "TimesNewRomanPS-ItalicMT" },
	{ "Times-BoldItalic", "TimesNewRomanPS-BoldItalicMT" },
	{ "Symbol", "SymbolMT" },
};

static pdf_fontlistMS fontlistMS =
{
	NULL,
	0,
	0,
};

static int
lookupcompare(const void *elem1, const void *elem2)
{
	pdf_fontmapMS *val1 = (pdf_fontmapMS *)elem1;
	pdf_fontmapMS *val2 = (pdf_fontmapMS *)elem2;
	int len1, len2;

	if (val1->fontface[0] == 0)
		return 1;
	if (val2->fontface[0] == 0)
		return -1;

	/* A little bit more sophisticated name matching so that e.g. "EurostileExtended"
	   matches "EurostileExtended-Roman" or "Tahoma-Bold,Bold" matches "Tahoma-Bold" */
	len1 = strlen(val1->fontface);
	len2 = strlen(val2->fontface);
	if (len1 != len2)
	{
		char *rest = len1 > len2 ? val1->fontface + len2 : val2->fontface + len1;
		if (',' == *rest || !stricmp(rest, "-roman"))
			return strnicmp(val1->fontface, val2->fontface, MIN(len1, len2));
	}

	return stricmp(val1->fontface, val2->fontface);
}

static int
sortcompare(const void *elem1, const void *elem2)
{
	pdf_fontmapMS *val1 = (pdf_fontmapMS *)elem1;
	pdf_fontmapMS *val2 = (pdf_fontmapMS *)elem2;

	if (val1->fontface[0] == 0)
		return 1;
	if (val2->fontface[0] == 0)
		return -1;

	return stricmp(val1->fontface, val2->fontface);
}

/* source and dest can be same */
static fz_error
decodeunicodeBMP(char* source, int sourcelen, char* dest, int destlen)
{
	wchar_t tmp[1024 * 2];
	int converted, i;

	if (sourcelen % 2 != 0)
		return fz_throw("fonterror");

	memset(tmp, 0, sizeof(tmp));
	for (i = 0; i < sourcelen / 2; i++)
		tmp[i] = SWAPWORD(((wchar_t *)source)[i]);

	converted = WideCharToMultiByte(CP_ACP, 0, tmp, -1, dest, destlen, NULL, NULL);
	if (converted == 0)
		return fz_throw("fonterror");

	return fz_okay;
}

static fz_error
decodeplatformstring(int platform, int enctype, char* source, int sourcelen, char* dest, int destlen)
{
	switch (platform)
	{
	case TT_PLATFORM_APPLE_UNICODE:
		switch (enctype)
		{
		case TT_APPLE_ID_DEFAULT:
		case TT_APPLE_ID_UNICODE_2_0:
			return decodeunicodeBMP(source, sourcelen, dest, destlen);
		}
		return fz_throw("fonterror : unsupported encoding");
	case TT_PLATFORM_MACINTOSH:
		switch( enctype)
		{
		case TT_MAC_ID_ROMAN:
			if (sourcelen + 1 > destlen)
				return fz_throw("fonterror : short buf lenth");
			memcpy(source, dest, sourcelen);
			dest[sourcelen] = 0;
			return fz_okay;
		}
		return fz_throw("fonterror : unsupported encoding");
	case TT_PLATFORM_MICROSOFT:
		switch (enctype)
		{
		case TT_MS_ID_SYMBOL_CS:
		case TT_MS_ID_UNICODE_CS:
		case TT_MS_ID_UCS_4:
			return decodeunicodeBMP(source, sourcelen, dest, destlen);
		}
		return fz_throw("fonterror : unsupported encoding");
	default:
		return fz_throw("fonterror : unsupported platform");
	}
}

static fz_error
growfontlist(pdf_fontlistMS *fl)
{
	int newcap;
	pdf_fontmapMS *newitems;

	if (fl->cap == 0)
		newcap = 1024;
	else
		newcap = fl->cap * 2;

	newitems = fz_realloc(fl->fontmap, sizeof(pdf_fontmapMS) * newcap);
	if (!newitems)
		return fz_rethrow(-1, "out of memory");;

	memset(newitems + fl->cap, 0, sizeof(pdf_fontmapMS) * (newcap - fl->cap));

	fl->fontmap = newitems;
	fl->cap = newcap;

	return fz_okay;
}

static fz_error
insertmapping(pdf_fontlistMS *fl, char *facename, char *path, int index)
{
	if (fl->len == fl->cap)
	{
		fz_error err = growfontlist(fl);
		if (err) return err;
	}

	if (fl->len >= fl->cap)
		return fz_throw("fonterror : fontlist overflow");

	strlcpy(fl->fontmap[fl->len].fontface, facename, sizeof(fl->fontmap[0].fontface));
	strlcpy(fl->fontmap[fl->len].fontpath, path, sizeof(fl->fontmap[0].fontpath));
	fl->fontmap[fl->len].index = index;

	++fl->len;

	return fz_okay;
}

static fz_error
safe_read(fz_stream *file, char *buf, int size)
{
	int bytesRead;
	fz_error err = fz_read(&bytesRead, file, buf, size);
	if (err)
		return err;

	if (bytesRead != size)
		return fz_throw("ioerror");
	return fz_okay;
}

static fz_error
parseTTF(fz_stream *file, int offset, int index, char *path)
{
	fz_error err = fz_okay;

	TT_OFFSET_TABLE ttOffsetTable;
	TT_TABLE_DIRECTORY tblDir;
	TT_NAME_TABLE_HEADER ttNTHeader;
	TT_NAME_RECORD ttRecord;

	char szTemp[MAX_FACENAME * 2], szPSName[MAX_FACENAME] = { 0 };
	char szTTName[MAX_FACENAME] = { 0 }, szStyle[MAX_FACENAME] = { 0 };
	int i, count, tblOffset;

	fz_seek(file,offset,0);
	err = safe_read(file, (char *)&ttOffsetTable, sizeof(TT_OFFSET_TABLE));
	if (err) return err;

	// check if this is a TrueType font and the version is 1.0
	if (SWAPLONG(ttOffsetTable.uVersion) != TTC_VERSION1)
		return fz_throw("fonterror : invalid font version");

	// determine the name table's offset by iterating through the offset table
	count = SWAPWORD(ttOffsetTable.uNumOfTables);
	for (i = 0; i < count; i++)
	{
		err = safe_read(file, (char *)&tblDir, sizeof(TT_TABLE_DIRECTORY));
		if (err) return err;
		if (!tblDir.uTag || SWAPLONG(tblDir.uTag) == TTAG_name)
			break;
	}
	if (count == i || !tblDir.uTag)
		return fz_throw("fonterror : nameless font");
	tblOffset = SWAPLONG(tblDir.uOffset);

	// read the 'name' table for record count and offsets
	fz_seek(file, tblOffset, 0);
	err = safe_read(file, (char *)&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER));
	if (err) return err;
	offset = tblOffset + sizeof(TT_NAME_TABLE_HEADER);
	tblOffset += SWAPWORD(ttNTHeader.uStorageOffset);

	// read through the strings for PostScript name and font family
	count = SWAPWORD(ttNTHeader.uNRCount);
	for (i = 0; i < count; i++)
	{
		short nameId;
		char *target;
		int stringLength;

		fz_seek(file, offset + i * sizeof(TT_NAME_RECORD), 0);
		err = safe_read(file, (char *)&ttRecord, sizeof(TT_NAME_RECORD));
		if (err) return err;

		// ignore non-English strings
		if (ttRecord.uLanguageID && SWAPWORD(ttRecord.uLanguageID) != 0x409)
			continue;
		// ignore empty and overlong strings
		stringLength = SWAPWORD(ttRecord.uStringLength);
		if (stringLength == 0 || stringLength >= sizeof(szTemp))
			continue;
		// ignore names other than font (sub)family and PostScript name
		nameId = SWAPWORD(ttRecord.uNameID);
		if (TT_NAME_ID_FONT_FAMILY == nameId)
			target = szTTName;
		else if (TT_NAME_ID_FONT_SUBFAMILY == nameId)
			target = szStyle;
		else if (TT_NAME_ID_PS_NAME == nameId)
			target = szPSName;
		else
			continue;

		fz_seek(file, tblOffset + SWAPWORD(ttRecord.uStringOffset), 0);
		err = safe_read(file, szTemp, stringLength);
		if (!err)
			err = decodeplatformstring(SWAPWORD(ttRecord.uPlatformID), SWAPWORD(ttRecord.uEncodingID),
				szTemp, stringLength, target, MAX_FACENAME);
		if (err) return err;
	}

	if (szPSName[0])
	{
		err = insertmapping(&fontlistMS, szPSName, path, index);
		if (err) return err;
	}
	if (szTTName[0])
	{
		// derive a PostScript-like name and add it, if it's different from the font's
		// included PostScript name; cf. http://code.google.com/p/sumatrapdf/issues/detail?id=376
		char *p1, *p2;
		// append the font's subfamily, unless it's a Regular font
		if (szStyle[0] && stricmp(szStyle, "Regular") != 0)
		{
			strlcat(szTTName, "-", MAX_FACENAME);
			strlcat(szTTName, szStyle, MAX_FACENAME);
		}
		// remove all spaces from the font name
		for (p2 = p1 = szTTName; *p2; p2++)
			if (!isspace(*p2))
				*p1++ = *p2;
		*p1 = 0;
		// compare the two names before adding this one
		if (lookupcompare(szTTName, szPSName))
		{
			err = insertmapping(&fontlistMS, szTTName, path, index);
			if (err) return err;
		}
	}
	return fz_okay;
}

static fz_error
parseTTFs(char *path)
{
	fz_stream *file = nil;
	fz_error err = fz_openrfile(&file, path);
	if (!err)
		err = parseTTF(file, 0, 0, path);

	if (file)
		fz_dropstream(file);
	return err;
}

static fz_error
parseTTCs(char *path)
{
	fz_stream *file = nil;
	FONT_COLLECTION fontcollection;
	ULONG i, numFonts, *offsettable = nil;

	fz_error err = fz_openrfile(&file, path);
	if (err) goto cleanup;

	err = safe_read(file, (char *)&fontcollection, sizeof(FONT_COLLECTION));
	if (err) goto cleanup;
	if (SWAPLONG(fontcollection.Tag) != TTAG_ttcf)
	{
		err = fz_throw("fonterror : wrong format");
		goto cleanup;
	}

	if (SWAPLONG(fontcollection.Version) != TTC_VERSION1 && SWAPLONG(fontcollection.Version) != TTC_VERSION2)
	{
		err = fz_throw("fonterror : invalid version");
		goto cleanup;
	}

	numFonts = SWAPLONG(fontcollection.NumFonts);
	offsettable = fz_malloc(numFonts * sizeof(ULONG));
	if (offsettable == nil)
	{
		err = fz_throw("out of memory");
		goto cleanup;
	}

	err = safe_read(file, (char *)offsettable, numFonts * sizeof(ULONG));
	for (i = 0; i < numFonts && !err; i++)
	{
		err = parseTTF(file, SWAPLONG(offsettable[i]), i, path);
	}

cleanup:
	if (offsettable)
		fz_free(offsettable);
	if (file)
		fz_dropstream(file);

	return err;
}

static fz_error
pdf_createfontlistMS()
{
	char szFontDir[MAX_PATH];
	char szFile[MAX_PATH*2];
	HANDLE hList;
	WIN32_FIND_DATAA FileData;

	// Get the proper directory path
	GetWindowsDirectoryA(szFontDir, sizeof(szFontDir));
	strlcat(szFontDir,"\\Fonts\\*.?t?", MAX_PATH);

	hList = FindFirstFileA(szFontDir, &FileData);
	if (hList == INVALID_HANDLE_VALUE)
	{
		/* Don't complain about missing directories */
		if (errno == ENOENT)
			return fz_throw("fonterror : can't find system fonts dir");
		return fz_throw("ioerror");
	}
	// drop the wildcards
	szFontDir[strlen(szFontDir) - 5] = 0;
	// Traverse through the directory structure
	do
	{
		if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			char *fileExt;
			// Get the full path for sub directory
			sprintf(szFile, "%s%s", szFontDir, FileData.cFileName);
			fileExt = szFile + strlen(szFile) - 4;
			if (!stricmp(fileExt, ".ttc"))
				parseTTCs(szFile);
			else if (!stricmp(fileExt, ".ttf") || !stricmp(fileExt, ".otf"))
				parseTTFs(szFile);
			// ignore errors occurring while parsing a given font file
		}
	} while (FindNextFileA(hList, &FileData));
	FindClose(hList);

	// sort the font list, so that it can be searched binarily
	qsort(fontlistMS.fontmap, fontlistMS.len, sizeof(pdf_fontmapMS), sortcompare);
	// TODO: make "TimesNewRomanPSMT" default substitute font?

	return fz_okay;
}

void
pdf_destroyfontlistMS()
{
	if (fontlistMS.fontmap != nil)
		fz_free(fontlistMS.fontmap);

	fontlistMS.len = 0;
	fontlistMS.cap = 0;
}

static fz_error
loadwindowsfont(pdf_fontdesc *font, char *fontname)
{
	fz_error error;
	pdf_fontmapMS fontmap;
	pdf_fontmapMS *found;
	char *pattern;
	int i;

	if (fontlistMS.len == 0)
	{
		pdf_createfontlistMS();
		if (fontlistMS.len == 0)
			return fz_throw("fonterror : no fonts in the system");
	}

	pattern = fontname;
	for (i = 0; i < _countof(baseSubstitutes); i++)
	{
		if (!strcmp(fontname, baseSubstitutes[i].name))
		{
			pattern = baseSubstitutes[i].pattern;
			break;
		}
	}

	strlcpy(fontmap.fontface, pattern, sizeof(fontmap.fontface));
	found = bsearch(&fontmap, fontlistMS.fontmap, fontlistMS.len, sizeof(pdf_fontmapMS), lookupcompare);
	if (!found)
	{
		return !fz_okay;
	}

	error = fz_newfontfromfile(&font->font, found->fontpath, found->index);
	if (error)
		return fz_rethrow(error, "cannot load freetype font from a file %s", found->fontpath);
	return fz_okay;
}

#if 0
/* TODO: those rules conflict with pdf_loadsystemfont() logic. */
static fz_error
loadjapansubstitute(pdf_fontdesc *font, char *fontname)
{
	if (!strcmp(fontname, "GothicBBB-Medium"))
		return loadwindowsfont(font, "MS-Gothic");

	if (!strcmp(fontname, "Ryumin-Light"))
		return loadwindowsfont(font, "MS-Mincho");

	if (font->flags & FD_FIXED)
	{
		if (font->flags & FD_SERIF)
			return loadwindowsfont(font, "MS-Mincho");

		return loadwindowsfont(font, "MS-Gothic");
	}

	if (font->flags & FD_SERIF)
			return loadwindowsfont(font, "MS-PMincho");

	return loadwindowsfont(font, "MS-PGothic");
}
#endif

/***** end of Windows font loading code *****/

#endif

fz_error
pdf_loadbuiltinfont(pdf_fontdesc *font, char *fontname)
{
	fz_error error;
	unsigned char *data;
	unsigned int len;
	int i;

	for (i = 0; basefonts[i].name; i++)
		if (!strcmp(fontname, basefonts[i].name))
			goto found;

#ifdef WIN32
	/* we use built-in fonts in addition to those installed on windows
	   because the metric for Times-Roman in windows fonts seems wrong
	   and we end up with over-lapping text if this font is used.
	   poppler doesn't have this problem even when using windows fonts
	   so maybe there's a better fix. */
	error = loadwindowsfont(font, fontname);
	if (fz_okay == error)
		return fz_okay;
#endif

	return fz_throw("cannot find font: '%s'", fontname);

found:
	pdf_logfont("load builtin font %s\n", fontname);

	data = (unsigned char *) basefonts[i].cff;
	len = *basefonts[i].len;

	error = fz_newfontfrombuffer(&font->font, data, len, 0);
	if (error)
		return fz_rethrow(error, "cannot load freetype font from buffer");

	return fz_okay;
}

static fz_error
loadsystemcidfont(pdf_fontdesc *font, int ros, int kind)
{
#ifndef NOCJK
	fz_error error;
	/* We only have one builtin fallback font, we'd really like
	 * to have one for each combination of ROS and Kind.
	 */
	pdf_logfont("loading builtin CJK font\n");
	error = fz_newfontfrombuffer(&font->font,
		(unsigned char *)pdf_font_DroidSansFallback_ttf_buf,
		pdf_font_DroidSansFallback_ttf_len, 0);
	if (error)
		return fz_rethrow(error, "cannot load builtin CJK font");
	font->font->ftsubstitute = 1; /* substitute font */
	return fz_okay;
#else
	/* Try to fall back to a reasonable TrueType font that might be installed locally */
	switch (kind)
	{
	case MINCHO:
		switch (ros)
		{
		case CNS: return pdf_loadsystemfont(font, "MingLiU", nil);
		case GB: return pdf_loadsystemfont(font, "SimSun", nil);
		case Japan: return pdf_loadsystemfont(font, "MS-Mincho", nil);
		case Korea: return pdf_loadsystemfont(font, "Batang", nil);
		}
		break;
	case GOTHIC:
		switch (ros)
		{
		case CNS: return pdf_loadsystemfont(font, "DFKaiShu-SB-Estd-BF", nil);
		case GB:
			if (fz_okay == pdf_loadsystemfont(font, "KaiTi", nil))
				return fz_okay;
			return pdf_loadsystemfont(font, "KaiTi_GB2312", nil);
		case Japan: return pdf_loadsystemfont(font, "MS-Gothic", nil);
		case Korea: return pdf_loadsystemfont(font, "Gulim", nil);
		}
		break;
	default:
		return fz_throw("Unknown cid kind %d", kind);
	}
	return fz_throw("no builtin CJK font file");
#endif
}

fz_error
pdf_loadsystemfont(pdf_fontdesc *font, char *fontname, char *collection)
{
	fz_error error;
	char *name;

	int isbold = 0;
	int isitalic = 0;
	int isserif = 0;
	int isscript = 0;
	int isfixed = 0;

#ifdef WIN32
	/* try to find a precise match in Windows' fonts before falling back to a built-in one */
	error = loadwindowsfont(font, fontname);
	if (fz_okay == error)
		return fz_okay;
#endif

	if (strstr(fontname, "Bold"))
		isbold = 1;
	if (strstr(fontname, "Italic"))
		isitalic = 1;
	if (strstr(fontname, "Oblique"))
		isitalic = 1;

	if (font->flags & FD_FIXED)
		isfixed = 1;
	if (font->flags & FD_SERIF)
		isserif = 1;
	if (font->flags & FD_ITALIC)
		isitalic = 1;
	if (font->flags & FD_SCRIPT)
		isscript = 1;
	if (font->flags & FD_FORCEBOLD)
		isbold = 1;

	pdf_logfont("fixed-%d serif-%d italic-%d script-%d bold-%d\n",
		isfixed, isserif, isitalic, isscript, isbold);

	if (collection)
	{
		int kind;

		if (isserif)
			kind = MINCHO;
		else
			kind = GOTHIC;

		if (!strcmp(collection, "Adobe-CNS1"))
			return loadsystemcidfont(font, CNS, kind);
		else if (!strcmp(collection, "Adobe-GB1"))
			return loadsystemcidfont(font, GB, kind);
		else if (!strcmp(collection, "Adobe-Japan1"))
			return loadsystemcidfont(font, Japan, kind);
		else if (!strcmp(collection, "Adobe-Japan2"))
			return loadsystemcidfont(font, Japan, kind);
		else if (!strcmp(collection, "Adobe-Korea1"))
			return loadsystemcidfont(font, Korea, kind);

		fz_warn("unknown cid collection: %s", collection);
	}

	if (isscript)
		name = "Chancery";

	else if (isfixed)
	{
		if (isitalic) {
			if (isbold) name = "Courier-BoldOblique";
			else name = "Courier-Oblique";
		}
		else {
			if (isbold) name = "Courier-Bold";
			else name = "Courier";
		}
	}

	else if (isserif)
	{
		if (isitalic) {
			if (isbold) name = "Times-BoldItalic";
			else name = "Times-Italic";
		}
		else {
			if (isbold) name = "Times-Bold";
			else name = "Times-Roman";
		}
	}

	else
	{
		if (isitalic) {
			if (isbold) name = "Helvetica-BoldOblique";
			else name = "Helvetica-Oblique";
		}
		else {
			if (isbold) name = "Helvetica-Bold";
			else name = "Helvetica";
		}
	}

	error = pdf_loadbuiltinfont(font, name);
	if (error)
		return fz_throw("cannot load builtin substitute font: %s", name);

	/* it's a substitute font: override the metrics */
	font->font->ftsubstitute = 1;

	return fz_okay;
}

fz_error
pdf_loadembeddedfont(pdf_fontdesc *font, pdf_xref *xref, fz_obj *stmref)
{
	fz_error error;
	fz_buffer *buf;

	pdf_logfont("load embedded font\n");

	error = pdf_loadstream(&buf, xref, fz_tonum(stmref), fz_togen(stmref));
	if (error)
		return fz_rethrow(error, "cannot load font stream");

	error = fz_newfontfrombuffer(&font->font, buf->rp, buf->wp - buf->rp, 0);
	if (error)
	{
		fz_dropbuffer(buf);
		return fz_rethrow(error, "cannot load embedded font (%d %d R)", fz_tonum(stmref), fz_togen(stmref));
	}

	font->buffer = buf->rp; /* save the buffer so we can free it later */
	fz_free(buf); /* only free the fz_buffer struct, not the contained data */

	font->isembedded = 1;

	return fz_okay;
}

