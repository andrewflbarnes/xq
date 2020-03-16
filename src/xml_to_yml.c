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

struct XpathEntry map[500];
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

    if (false == asXpath) {
        fprintf(stdout, "%s:\n", root->name);
    }

    print_attributes(root, xpath, 1);
    print_children(root, xpath, 1);

    xmlFreeDoc(document);

    // output
    if (true == asXpath || true == asJson) {
        if (true == asJson) {
            fprintf(stdout, "{\n");
        }

        for (int i = 0; i < xpathIdx - 1; i++) {
            fprintf(stdout, "\"%s\": \"%s\"%s\n", map[i].xpath, map[i].value, lineEnd);
        }
        
        fprintf(stdout, "\"%s\": \"%s\"\n", map[xpathIdx - 1].xpath, map[xpathIdx - 1].value);

        if (true == asJson) {
            fprintf(stdout, "}\n");
        }
    }
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
                if (false == asXpath) {
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

        if (false == asXpath) {
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
}

void reset_xpath(char *xpath, long len) {
    xpath[len] = 0;
}

void add_map(char *xpath, char *value) {

    char *xpathCopy;
    xpathCopy = malloc(sizeof(char) * (strlen(xpath) + 1));
    strcpy(xpathCopy, xpath);
    char *valueCopy;
    valueCopy = malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(valueCopy, value);

    struct XpathEntry entry;
    entry.xpath = xpathCopy;
    entry.value = valueCopy;
    
    map[xpathIdx] = entry;
    
    xpathIdx++;
}