#ifndef __BUILDER_H__
#define __BUILDER_H__

// Define Attribute and Element
typedef struct Attribute Attribute;
typedef struct Attributes Attributes;
typedef struct Element Element;
typedef struct Elements Elements;
struct Attribute
{
    char name[32];
    char value[128];
    Attribute *next;
};
struct Attributes
{
    Attribute *head, *tail;
};

struct Elements
{
    Element *head, *tail;
};
struct Element
{
    char name[32];
    Attributes attributes;
    Elements children;
    Element *next;
};

// Define Contiguous Memory for Attribute and Element for less malloc calls
#define ContiguousAttributeSize 50
#define ContiguousElementSize 50
typedef struct ContiguousAttribute ContiguousAttribute;
struct ContiguousAttribute
{
    Attribute attributes[ContiguousAttributeSize];
    ContiguousAttribute *next;
};
typedef struct ContiguousElement ContiguousElement;
struct ContiguousElement
{
    Element elements[ContiguousElementSize];
    ContiguousElement *next;
};
typedef struct ContiguousAttributeElement ContiguousAttributeElement;
struct ContiguousAttributeElement
{
    int attributeCount;
    int elementCount;
    ContiguousAttribute *attributesHead, *attributesTail;
    ContiguousElement *elementsHead, *elementsTail;
};
#define createContiguousAttributeElement(x) \
    ContiguousAttributeElement x = {0, 0, NULL, NULL, NULL, NULL}
void _destroyContiguousAttributeElement(ContiguousAttributeElement *contiguous);
#define destroyContiguousAttributeElement(x) _destroyContiguousAttributeElement(&x)
Attribute *createAttribute(ContiguousAttributeElement *contiguous, const char *name, const char *value);
Element *createElement(ContiguousAttributeElement *contiguous, const char *name);
void addAttribute(Element *element, Attribute *attribute);
void addChild(Element *element, Element *child);
Element *createTextElement(ContiguousAttributeElement *contiguous, const char *text);
void compileAttribute(Attribute *const attribute, char *const buffer, int *const pos);
void compileTextAttribute(Attribute *attribute, char *const buffer, int *const pos);
void compileElement(Element *const element, char *const buffer, int *const pos);

#endif // __BUILDER_H__