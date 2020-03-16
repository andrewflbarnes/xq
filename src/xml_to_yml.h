#ifndef __xml_to_yml__
#define __xml_to_yml__

void help_text();

void xml_to_yml(char *filename);

void print_children(xmlNode *root, char *xpath, int depth);

void print_attributes(xmlNode *node, char *xpath, int depth);

void append_xpath(char *xpath, xmlNode *node);

void reset_xpath(char *xpath, long len);

void add_map(char *key, char *value);

#endif
