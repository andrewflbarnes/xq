#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libxml/parser.h>
#include "xml_to_yml.h"

char *tab_buffer = "                                  ";
bool asXpath = false;
bool asJson = false;
char *xpathDelim = "/";
char *lineEnd = "";

struct XpathEntry {
    char *xpath;
    char *value;
};

struct XpathEntry *map[500];
int xpathIdx = 0;

int main(int argc, char **argv)
{
    if (argc < 2) {
        help_text();
    }

    if (argc == 2) {
        // default
        asXpath = false;
        xml_to_yml(argv[1]);
    } else {
        // parse options
        char *option;

        for (int i = 1; i < (argc - 1); i++) {
            option = argv[i];

            if (strncmp(option, "-", 1)) {
                help_text();
            } else if (strlen(option) > 2) {
                fprintf(stderr, "Unrecognised option \"%s\"\n\n", option);
                help_text();
            }

            switch (argv[i][1])
            {
                case 'y':
                    asXpath = false;
                    break;
                case 'Y':
                    asXpath = true;
                    break;
                case 'j':
                    asXpath = true;
                    asJson = true;
                    lineEnd = ",";
                    break;
                default:
                    fprintf(stderr, "Unrecognised option \"%s\"\n\n", option);
                    help_text();
            }
        }
        xml_to_yml(argv[2]);
    }


    return 0;
}

void help_text()
{  
    fprintf(stderr, "Usage: xq [-y] [-Y] filename.xml\n");
    exit(1);
}

void xml_to_yml(char *filename)
{
    xmlDoc         *document;
    xmlNode        *root;
    char           *xpath;

    document = xmlReadFile(filename, NULL, 0);
    root = xmlDocGetRootElement(document);
    xpath = malloc(1001);

    strcpy(xpath, xpathDelim);
    strcat(xpath, (char *)root->name);
    strcat(xpath, xpathDelim);

    if (true == asJson) {
        fprintf(stdout, "{\n");
    }

    if (false == asXpath) {
        fprintf(stdout, "%s:\n", root->name);
    }

    print_attributes(root, xpath, 1);
    print_children(root, xpath, 1);

    if (true == asJson) {
        // hacky workaround as we we print direct to console...
        fprintf(stdout, "\"\":\"\"\n}\n");
    }

    xmlFreeDoc(document);
}

void print_children(xmlNode *root, char *xpath, int depth)
{
    xmlNode *first_child, *node;
    xmlChar *content;
    long xpathParentLen;

    xpathParentLen = strlen(xpath);

    first_child = root->children;
    
    for (node = first_child; node; node = node->next) {
        if (xmlIsBlankNode(node)) {
        } else if (xmlNodeIsText(node)) {
            content = node->content;
            if (content != NULL) {
                add_map(xpath, (char *)content);
                if (true == asXpath) {
                    fprintf(stdout, "\"%s\": \"%s\"%s\n", xpath, content, lineEnd);
                } else {
                    fprintf(stdout, "%.*s _text: \"%s\"\n", depth, tab_buffer, content);
                }
            }
        } else {
            append_xpath(xpath, node);
            if (false == asXpath) {
                fprintf(stdout, "%.*s %s:\n", depth, tab_buffer, node->name);
            }
        }

        if (false == asXpath) {
            print_attributes(node, xpath, depth + 1);
        }
        print_children(node, xpath, depth + 1);
        if (true == asXpath) {
            print_attributes(node, xpath, depth + 1);
        }
        reset_xpath(xpath, xpathParentLen);
    }

//    xmlFree(content);
//    xmlFree(first_child);
//    xmlFree(node);
}

void print_attributes(xmlNode *node, char *xpath, int depth)
{
    xmlAttr *attr = node->properties;
    xmlNs *ns = node->ns;
    long xpathParentLen;

    xpathParentLen = strlen(xpath);

    // if (ns) {
    //   char *href = (char *)ns->href;
    //   if (true == asXpath) {
    //       strcat(xpath, "@");
    //       strcat(xpath, "xmlns");
    //       fprintf(stdout, "\"%s\": \"%s\"\n", xpath, href);
    //       reset_xpath(xpath, xpathParentLen);
    //   } else {
    //       fprintf(stdout, "%.*s _xmlns: %s\n", depth, tab_buffer, href);
    //   }
    // }

    if (!attr) {
        return;
    }

    if (false == asXpath) {
        fprintf(stdout, "%.*s _attrs:\n", depth, tab_buffer);
    }

    while (attr) {
        xmlChar* value = xmlNodeListGetString(node->doc, attr->children, 1);
        const xmlChar* name = attr->name;

        strcat(xpath, "@");
        strcat(xpath, (char *)name);

        add_map(xpath, (char *)value);

        if (true == asXpath) {
            fprintf(stdout, "\"%s\": \"%s\"%s\n", xpath, value, lineEnd);
        } else {
            fprintf(stdout, "%.*s %s: \"%s\"%s\n", depth + 1, tab_buffer, name, value, lineEnd);
        }

        reset_xpath(xpath, xpathParentLen);

        xmlFree(value);

        attr = attr->next;
    }

    xmlFree(attr);
}

void append_xpath(char *xpath, xmlNode *node) {
    strcat(xpath, (char *)node->name);
    strcat(xpath, xpathDelim);
    // fprintf(stdout, "DEBUG XPATH: %s\n", xpath);
}

void reset_xpath(char *xpath, long len) {
    xpath[len] = 0;
    // fprintf(stdout, "DEBUG XPATH: %s\n", xpath);
}

void add_map(char *key, char *value) {
    struct XpathEntry entry;
    entry.xpath = key;
    entry.value = value;
    
    map[xpathIdx] = &entry;
    
    xpathIdx++;
}