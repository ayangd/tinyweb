#include <stdlib.h>
#include <string.h>

#include "builder.h"

void _destroyContiguousAttributeElement(ContiguousAttributeElement *contiguous)
{
    ContiguousAttribute *currentContiguousAttribute = contiguous->attributesHead;
    while (currentContiguousAttribute)
    {
        ContiguousAttribute *todelete = currentContiguousAttribute;
        currentContiguousAttribute = currentContiguousAttribute->next;
        free(todelete);
    }
    ContiguousElement *currentContiguousElement = contiguous->elementsHead;
    while (currentContiguousElement)
    {
        ContiguousElement *todelete = currentContiguousElement;
        currentContiguousElement = currentContiguousElement->next;
        free(todelete);
    }
}

Attribute *createAttribute(ContiguousAttributeElement *contiguous, const char *name, const char *value)
{
    if (contiguous->attributeCount % ContiguousAttributeSize == 0)
    {
        if (contiguous->attributesTail == NULL)
        {
            contiguous->attributesHead = contiguous->attributesTail = (ContiguousAttribute *)malloc(sizeof(ContiguousAttribute));
            contiguous->attributesTail->next = NULL;
        }
        else
        {
            contiguous->attributesTail->next = (ContiguousAttribute *)malloc(sizeof(ContiguousAttribute));
            contiguous->attributesTail = contiguous->attributesTail->next;
            contiguous->attributesTail->next = NULL;
        }
    }
    int position = contiguous->attributeCount % ContiguousAttributeSize;
    Attribute *attribute = &(contiguous->attributesTail->attributes[position]);
    strcpy(attribute->name, name);
    strcpy(attribute->value, value);
    attribute->next = NULL;
    contiguous->attributeCount++;
    return attribute;
}
Element *createElement(ContiguousAttributeElement *contiguous, const char *name)
{
    if (contiguous->elementCount % ContiguousElementSize == 0)
    {
        if (contiguous->elementsTail == NULL)
        {
            contiguous->elementsHead = contiguous->elementsTail = (ContiguousElement *)malloc(sizeof(ContiguousElement));
            contiguous->elementsTail->next = NULL;
        }
        else
        {
            contiguous->elementsTail->next = (ContiguousElement *)malloc(sizeof(ContiguousElement));
            contiguous->elementsTail = contiguous->elementsTail->next;
            contiguous->elementsTail->next = NULL;
        }
    }
    int position = contiguous->elementCount % ContiguousElementSize;
    Element *element = &(contiguous->elementsTail->elements[position]);
    strcpy(element->name, name);
    element->attributes.head = element->attributes.tail = NULL;
    element->children.head = element->children.tail = NULL;
    element->next = NULL;
    contiguous->elementCount++;
    return element;
}
void addAttribute(Element *element, Attribute *attribute)
{
    if (element->attributes.head == NULL)
    {
        element->attributes.head = element->attributes.tail = attribute;
    }
    else
    {
        element->attributes.tail->next = attribute;
        element->attributes.tail = attribute;
    }
}
void addChild(Element *element, Element *child)
{
    if (element->children.head == NULL)
    {
        element->children.head = element->children.tail = child;
    }
    else
    {
        element->children.tail->next = child;
        element->children.tail = child;
    }
}
Element *createTextElement(ContiguousAttributeElement *contiguous, const char *text)
{
    Element *element = createElement(contiguous, "text");
    int textLength = strlen(text);
    char buffer[255];
    int i;
    for (i = 0; i < textLength; i += 127)
    {
        strncpy(buffer, &(text[i]), 127);
        buffer[127] = 0;
        Attribute *textAttribute = createAttribute(contiguous, "", buffer);
        addAttribute(element, textAttribute);
    }
    return element;
}

void compileAttribute(Attribute *const attribute, char *const buffer, int *const pos)
{
    buffer[(*pos)++] = ' ';
    // name
    int nameLength = strlen(attribute->name);
    strcpy(&(buffer[*pos]), attribute->name);
    *pos += nameLength;

    // separator
    buffer[(*pos)++] = '=';

    // value
    buffer[(*pos)++] = '"';
    int valueLength = strlen(attribute->value);
    int i;
    for (i = 0; i < valueLength; i++)
    {
        if (attribute->value[i] == '"')
        {
            buffer[(*pos)++] = '\\';
        }
        buffer[(*pos)++] = attribute->value[i];
    }
    buffer[(*pos)++] = '"';
}

void compileTextAttribute(Attribute *attribute, char *const buffer, int *const pos)
{
    int valueLength = strlen(attribute->value);
    strcpy(&(buffer[*pos]), attribute->value);
    *pos += valueLength;
}

void compileElement(Element *const element, char *const buffer, int *const pos)
{
    if (strcmp(element->name, "text") == 0)
    {
        Attribute *currentAttribute = element->attributes.head;
        while (currentAttribute)
        {
            compileTextAttribute(currentAttribute, buffer, pos);
            currentAttribute = currentAttribute->next;
        }
        return;
    }

    buffer[(*pos)++] = '<';
    int nameLength = strlen(element->name);
    strcpy(&(buffer[*pos]), element->name);
    *pos += nameLength;

    Attribute *currentAttribute = element->attributes.head;
    while (currentAttribute)
    {
        compileAttribute(currentAttribute, buffer, pos);
        currentAttribute = currentAttribute->next;
    }
    buffer[(*pos)++] = '>';

    Element *currentElement = element->children.head;
    while (currentElement)
    {
        compileElement(currentElement, buffer, pos);
        currentElement = currentElement->next;
    }

    buffer[(*pos)++] = '<';
    buffer[(*pos)++] = '/';
    nameLength = strlen(element->name);
    strcpy(&(buffer[*pos]), element->name);
    *pos += nameLength;
    buffer[(*pos)++] = '>';
}