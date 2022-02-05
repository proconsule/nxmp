/*	Copyright 2012, 2016 Christoph GÃ¤rtner
	Distributed under the Boost Software License, Version 1.0
*/

#include "entities.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define UNICODE_MAX 0x10FFFFul

static const char *const NAMED_ENTITIES[][2] = {
	{ "AElig;", "Ã†" },
	{ "Aacute;", "Ã" },
	{ "Acirc;", "Ã‚" },
	{ "Agrave;", "Ã€" },
	{ "Alpha;", "Î‘" },
	{ "Aring;", "Ã…" },
	{ "Atilde;", "Ãƒ" },
	{ "Auml;", "Ã„" },
	{ "Beta;", "Î’" },
	{ "Ccedil;", "Ã‡" },
	{ "Chi;", "Î§" },
	{ "Dagger;", "â€¡" },
	{ "Delta;", "Î”" },
	{ "ETH;", "Ã" },
	{ "Eacute;", "Ã‰" },
	{ "Ecirc;", "ÃŠ" },
	{ "Egrave;", "Ãˆ" },
	{ "Epsilon;", "Î•" },
	{ "Eta;", "Î—" },
	{ "Euml;", "Ã‹" },
	{ "Gamma;", "Î“" },
	{ "Iacute;", "Ã" },
	{ "Icirc;", "ÃŽ" },
	{ "Igrave;", "ÃŒ" },
	{ "Iota;", "Î™" },
	{ "Iuml;", "Ã" },
	{ "Kappa;", "Îš" },
	{ "Lambda;", "Î›" },
	{ "Mu;", "Îœ" },
	{ "Ntilde;", "Ã‘" },
	{ "Nu;", "Î" },
	{ "OElig;", "Å’" },
	{ "Oacute;", "Ã“" },
	{ "Ocirc;", "Ã”" },
	{ "Ograve;", "Ã’" },
	{ "Omega;", "Î©" },
	{ "Omicron;", "ÎŸ" },
	{ "Oslash;", "Ã˜" },
	{ "Otilde;", "Ã•" },
	{ "Ouml;", "Ã–" },
	{ "Phi;", "Î¦" },
	{ "Pi;", "Î " },
	{ "Prime;", "â€³" },
	{ "Psi;", "Î¨" },
	{ "Rho;", "Î¡" },
	{ "Scaron;", "Å " },
	{ "Sigma;", "Î£" },
	{ "THORN;", "Ãž" },
	{ "Tau;", "Î¤" },
	{ "Theta;", "Î˜" },
	{ "Uacute;", "Ãš" },
	{ "Ucirc;", "Ã›" },
	{ "Ugrave;", "Ã™" },
	{ "Upsilon;", "Î¥" },
	{ "Uuml;", "Ãœ" },
	{ "Xi;", "Îž" },
	{ "Yacute;", "Ã" },
	{ "Yuml;", "Å¸" },
	{ "Zeta;", "Î–" },
	{ "aacute;", "Ã¡" },
	{ "acirc;", "Ã¢" },
	{ "acute;", "Â´" },
	{ "aelig;", "Ã¦" },
	{ "agrave;", "Ã " },
	{ "alefsym;", "â„µ" },
	{ "alpha;", "Î±" },
	{ "amp;", "&" },
	{ "and;", "âˆ§" },
	{ "ang;", "âˆ " },
	{ "apos;", "'" },
	{ "aring;", "Ã¥" },
	{ "asymp;", "â‰ˆ" },
	{ "atilde;", "Ã£" },
	{ "auml;", "Ã¤" },
	{ "bdquo;", "â€ž" },
	{ "beta;", "Î²" },
	{ "brvbar;", "Â¦" },
	{ "bull;", "â€¢" },
	{ "cap;", "âˆ©" },
	{ "ccedil;", "Ã§" },
	{ "cedil;", "Â¸" },
	{ "cent;", "Â¢" },
	{ "chi;", "Ï‡" },
	{ "circ;", "Ë†" },
	{ "clubs;", "â™£" },
	{ "cong;", "â‰…" },
	{ "copy;", "Â©" },
	{ "crarr;", "â†µ" },
	{ "cup;", "âˆª" },
	{ "curren;", "Â¤" },
	{ "dArr;", "â‡“" },
	{ "dagger;", "â€ " },
	{ "darr;", "â†“" },
	{ "deg;", "Â°" },
	{ "delta;", "Î´" },
	{ "diams;", "â™¦" },
	{ "divide;", "Ã·" },
	{ "eacute;", "Ã©" },
	{ "ecirc;", "Ãª" },
	{ "egrave;", "Ã¨" },
	{ "empty;", "âˆ…" },
	{ "emsp;", "\xE2\x80\x83" },
	{ "ensp;", "\xE2\x80\x82" },
	{ "epsilon;", "Îµ" },
	{ "equiv;", "â‰¡" },
	{ "eta;", "Î·" },
	{ "eth;", "Ã°" },
	{ "euml;", "Ã«" },
	{ "euro;", "â‚¬" },
	{ "exist;", "âˆƒ" },
	{ "fnof;", "Æ’" },
	{ "forall;", "âˆ€" },
	{ "frac12;", "Â½" },
	{ "frac14;", "Â¼" },
	{ "frac34;", "Â¾" },
	{ "frasl;", "â„" },
	{ "gamma;", "Î³" },
	{ "ge;", "â‰¥" },
	{ "gt;", ">" },
	{ "hArr;", "â‡”" },
	{ "harr;", "â†”" },
	{ "hearts;", "â™¥" },
	{ "hellip;", "â€¦" },
	{ "iacute;", "Ã­" },
	{ "icirc;", "Ã®" },
	{ "iexcl;", "Â¡" },
	{ "igrave;", "Ã¬" },
	{ "image;", "â„‘" },
	{ "infin;", "âˆž" },
	{ "int;", "âˆ«" },
	{ "iota;", "Î¹" },
	{ "iquest;", "Â¿" },
	{ "isin;", "âˆˆ" },
	{ "iuml;", "Ã¯" },
	{ "kappa;", "Îº" },
	{ "lArr;", "â‡" },
	{ "lambda;", "Î»" },
	{ "lang;", "ã€ˆ" },
	{ "laquo;", "Â«" },
	{ "larr;", "â†" },
	{ "lceil;", "âŒˆ" },
	{ "ldquo;", "â€œ" },
	{ "le;", "â‰¤" },
	{ "lfloor;", "âŒŠ" },
	{ "lowast;", "âˆ—" },
	{ "loz;", "â—Š" },
	{ "lrm;", "\xE2\x80\x8E" },
	{ "lsaquo;", "â€¹" },
	{ "lsquo;", "â€˜" },
	{ "lt;", "<" },
	{ "macr;", "Â¯" },
	{ "mdash;", "â€”" },
	{ "micro;", "Âµ" },
	{ "middot;", "Â·" },
	{ "minus;", "âˆ’" },
	{ "mu;", "Î¼" },
	{ "nabla;", "âˆ‡" },
	{ "nbsp;", "\xC2\xA0" },
	{ "ndash;", "â€“" },
	{ "ne;", "â‰ " },
	{ "ni;", "âˆ‹" },
	{ "not;", "Â¬" },
	{ "notin;", "âˆ‰" },
	{ "nsub;", "âŠ„" },
	{ "ntilde;", "Ã±" },
	{ "nu;", "Î½" },
	{ "oacute;", "Ã³" },
	{ "ocirc;", "Ã´" },
	{ "oelig;", "Å“" },
	{ "ograve;", "Ã²" },
	{ "oline;", "â€¾" },
	{ "omega;", "Ï‰" },
	{ "omicron;", "Î¿" },
	{ "oplus;", "âŠ•" },
	{ "or;", "âˆ¨" },
	{ "ordf;", "Âª" },
	{ "ordm;", "Âº" },
	{ "oslash;", "Ã¸" },
	{ "otilde;", "Ãµ" },
	{ "otimes;", "âŠ—" },
	{ "ouml;", "Ã¶" },
	{ "para;", "Â¶" },
	{ "part;", "âˆ‚" },
	{ "permil;", "â€°" },
	{ "perp;", "âŠ¥" },
	{ "phi;", "Ï†" },
	{ "pi;", "Ï€" },
	{ "piv;", "Ï–" },
	{ "plusmn;", "Â±" },
	{ "pound;", "Â£" },
	{ "prime;", "â€²" },
	{ "prod;", "âˆ" },
	{ "prop;", "âˆ" },
	{ "psi;", "Ïˆ" },
	{ "quot;", "\"" },
	{ "rArr;", "â‡’" },
	{ "radic;", "âˆš" },
	{ "rang;", "ã€‰" },
	{ "raquo;", "Â»" },
	{ "rarr;", "â†’" },
	{ "rceil;", "âŒ‰" },
	{ "rdquo;", "â€" },
	{ "real;", "â„œ" },
	{ "reg;", "Â®" },
	{ "rfloor;", "âŒ‹" },
	{ "rho;", "Ï" },
	{ "rlm;", "\xE2\x80\x8F" },
	{ "rsaquo;", "â€º" },
	{ "rsquo;", "â€™" },
	{ "sbquo;", "â€š" },
	{ "scaron;", "Å¡" },
	{ "sdot;", "â‹…" },
	{ "sect;", "Â§" },
	{ "shy;", "\xC2\xAD" },
	{ "sigma;", "Ïƒ" },
	{ "sigmaf;", "Ï‚" },
	{ "sim;", "âˆ¼" },
	{ "spades;", "â™ " },
	{ "sub;", "âŠ‚" },
	{ "sube;", "âŠ†" },
	{ "sum;", "âˆ‘" },
	{ "sup1;", "Â¹" },
	{ "sup2;", "Â²" },
	{ "sup3;", "Â³" },
	{ "sup;", "âŠƒ" },
	{ "supe;", "âŠ‡" },
	{ "szlig;", "ÃŸ" },
	{ "tau;", "Ï„" },
	{ "there4;", "âˆ´" },
	{ "theta;", "Î¸" },
	{ "thetasym;", "Ï‘" },
	{ "thinsp;", "\xE2\x80\x89" },
	{ "thorn;", "Ã¾" },
	{ "tilde;", "Ëœ" },
	{ "times;", "Ã—" },
	{ "trade;", "â„¢" },
	{ "uArr;", "â‡‘" },
	{ "uacute;", "Ãº" },
	{ "uarr;", "â†‘" },
	{ "ucirc;", "Ã»" },
	{ "ugrave;", "Ã¹" },
	{ "uml;", "Â¨" },
	{ "upsih;", "Ï’" },
	{ "upsilon;", "Ï…" },
	{ "uuml;", "Ã¼" },
	{ "weierp;", "â„˜" },
	{ "xi;", "Î¾" },
	{ "yacute;", "Ã½" },
	{ "yen;", "Â¥" },
	{ "yuml;", "Ã¿" },
	{ "zeta;", "Î¶" },
	{ "zwj;", "\xE2\x80\x8D" },
	{ "zwnj;", "\xE2\x80\x8C" }
};

static int cmp(const void *key, const void *value)
{
	return strncmp((const char *)key, *(const char *const *)value,
		strlen(*(const char *const *)value));
}

static const char *get_named_entity(const char *name)
{
	const char *const *entity = (const char *const *)bsearch(name,
		NAMED_ENTITIES, sizeof NAMED_ENTITIES / sizeof *NAMED_ENTITIES,
		sizeof *NAMED_ENTITIES, cmp);

	return entity ? entity[1] : NULL;
}

static size_t putc_utf8(unsigned long cp, char *buffer)
{
	unsigned char *bytes = (unsigned char *)buffer;

	if(cp <= 0x007Ful)
	{
		bytes[0] = (unsigned char)cp;
		return 1;
	}

	if(cp <= 0x07FFul)
	{
		bytes[1] = (unsigned char)((2 << 6) | (cp & 0x3F));
		bytes[0] = (unsigned char)((6 << 5) | (cp >> 6));
		return 2;
	}

	if(cp <= 0xFFFFul)
	{
		bytes[2] = (unsigned char)(( 2 << 6) | ( cp       & 0x3F));
		bytes[1] = (unsigned char)(( 2 << 6) | ((cp >> 6) & 0x3F));
		bytes[0] = (unsigned char)((14 << 4) |  (cp >> 12));
		return 3;
	}

	if(cp <= 0x10FFFFul)
	{
		bytes[3] = (unsigned char)(( 2 << 6) | ( cp        & 0x3F));
		bytes[2] = (unsigned char)(( 2 << 6) | ((cp >>  6) & 0x3F));
		bytes[1] = (unsigned char)(( 2 << 6) | ((cp >> 12) & 0x3F));
		bytes[0] = (unsigned char)((30 << 3) |  (cp >> 18));
		return 4;
	}

	return 0;
}

static bool parse_entity(
	const char *current, char **to, const char **from)
{
	const char *end = strchr(current, ';');
	if(!end) return 0;

	if(current[1] == '#')
	{
		char *tail = NULL;
		int errno_save = errno;
		bool hex = current[2] == 'x' || current[2] == 'X';

		errno = 0;
		unsigned long cp = strtoul(
			current + (hex ? 3 : 2), &tail, hex ? 16 : 10);

		bool fail = errno || tail != end || cp > UNICODE_MAX;
		errno = errno_save;
		if(fail) return 0;

		*to += putc_utf8(cp, *to);
		*from = end + 1;

		return 1;
	}
	else
	{
		const char *entity = get_named_entity(&current[1]);
		if(!entity) return 0;

		size_t len = strlen(entity);
		memcpy(*to, entity, len);

		*to += len;
		*from = end + 1;

		return 1;
	}
}

size_t decode_html_entities_utf8(char *dest, const char *src)
{
	if(!src) src = dest;

	char *to = dest;
	const char *from = src;

	for(const char *current; (current = strchr(from, '&'));)
	{
		memmove(to, from, (size_t)(current - from));
		to += current - from;

		if(parse_entity(current, &to, &from))
			continue;

		from = current;
		*to++ = *from++;
	}

	size_t remaining = strlen(from);

	memmove(to, from, remaining);
	to += remaining;
	*to = 0;

	return (size_t)(to - dest);
}