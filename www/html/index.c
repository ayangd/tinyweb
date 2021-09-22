#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builder.h"

void load(const char *request, char **contentPtr, int *lengthPtr)
{
    createContiguousAttributeElement(contiguous);
    Element *html = createElement(&contiguous, "html");
    Element *head = createElement(&contiguous, "head");
    Element *title = createElement(&contiguous, "title");
    Element *titleText = createTextElement(&contiguous, "Hi");
    addChild(title, titleText);
    Element *style = createElement(&contiguous, "style");
    Element *styleText = createTextElement(&contiguous,
"body {"
" margin: 0;"
" padding: 0;"
" font-family: -apple-system, BlinkMacSystemFont, Segoe UI, Roboto, Oxygen, Ubuntu, Cantarell, Fira Sans, Droid Sans, Helvetica Neue, sans-serif;"
"}"
"* {"
" box-sizing: border-box;"
"}"
".btn {"
" border: none;"
" padding: 8px 16px;"
" background-color: black;"
" color: white;"
" text-transform: uppercase;"
" font-size: 1.2rem;"
" border-radius: 8px;"
" transition: .1s;"
" cursor: pointer;"
"}"
".btn:hover {"
" background-color: #777;"
"}"
".btn:active {"
" background-color: #999;"
"}"
);
    addChild(style, styleText);
    addChild(head, title);
    addChild(head, style);
    Element *body = createElement(&contiguous, "body");
    Element *div1 = createElement(&contiguous, "div");
    Element *lorem = createTextElement(&contiguous, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.");
    addChild(div1, lorem);
    Element *div2 = createElement(&contiguous, "div");
    Element *button = createElement(&contiguous, "button");
    Attribute *buttonStyleAttribute = createAttribute(&contiguous, "class", "btn");
    addAttribute(button, buttonStyleAttribute);
    Element *buttonText = createTextElement(&contiguous, "Click me!");
    addChild(button, buttonText);
    addChild(div2, button);
    addChild(body, div1);
    addChild(body, div2);
    addChild(html, head);
    addChild(html, body);

    char *buffer = (char *)malloc(0xffff);
    strcpy(buffer, "<!DOCTYPE html>");
    int bufferLength = strlen(buffer);
    compileElement(html, buffer, &bufferLength);

    *contentPtr = (char *)malloc(bufferLength);
    strcpy(*contentPtr, buffer);
    *lengthPtr = bufferLength;
    free(buffer);
    destroyContiguousAttributeElement(contiguous);
}
