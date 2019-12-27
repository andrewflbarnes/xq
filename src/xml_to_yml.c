#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include "xml_to_yml.h"

char *tab_buffer = "                                  ";

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.xml\n", argv[0]);
        return 1;
    }

    xml_to_yml(argv[1]);

    return 0;
}

void xml_to_yml(char *filename)
{
    xmlDoc         *document;
    xmlNode        *root;

    document = xmlReadFile(filename, NULL, 0);
    root = xmlDocGetRootElement(document);
    fprintf(stdout, "%s:\n", root->name);

    print_attributes(root, 1);
    print_children(root, 1);

    xmlFreeDoc(document);
}

void print_children(xmlNode *root, int depth)
{
    xmlNode *first_child, *node;
    xmlChar *content;

    first_child = root->children;
    for (node = first_child; node; node = node->next) {
        if (xmlIsBlankNode(node)) {
        } else if (xmlNodeIsText(node)) {
            content = node->content;
            if (content != NULL) {
                fprintf(stdout, "%.*s _text: \"%s\"\n", depth, tab_buffer, content);
            }
        } else {
            fprintf(stdout, "%.*s %s:\n", depth, tab_buffer, node->name);
        }

        print_attributes(node, depth + 1);
        print_children(node, depth + 1);
    }

//    xmlFree(content);
//    xmlFree(first_child);
//    xmlFree(node);
}

void print_attributes(xmlNode *node, int depth)
{
    xmlAttr *attr = node->properties;

    if (!attr) {
        return;
    }

    fprintf(stdout, "%.*s _attrs:\n", depth, tab_buffer);

    while (attr) {
        xmlChar* value = xmlNodeListGetString(node->doc, attr->children, 1);
        fprintf(stdout, "%.*s %s: \"%s\"\n", depth + 1, tab_buffer, attr->name, value);
        xmlFree(value);
        attr = attr->next;
    }

    xmlFree(attr);
}
