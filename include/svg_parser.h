#ifndef SVG_PARSER_H
#define SVG_PARSER_H

#include "svg_types.h"

int svg_load_from_file(const char *filename, SvgDocument **doc_out);
void svg_free_document(SvgDocument *doc);

#endif /* SVG_PARSER_H */