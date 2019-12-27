#ifndef __xml_to_yml__
#define __xml_to_yml__

void xml_to_yml(char *filename);

void print_children(xmlNode *root, int depth);

void print_attributes(xmlNode *node, int depth);

#endif
