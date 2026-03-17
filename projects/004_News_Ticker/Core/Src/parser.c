/*
 * parser.c
 *
 *  Created on: Jun 16, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#include "core_json.h"
#include <stdio.h>
#include <string.h>
#include "parser.h"

news_article_t articles[MAX_TITLES];
uint8_t title_count = 0;

/**
 * @brief Finds the value for a given JSON key in a raw JSON string (naive method).
 *
 * @param json  The input JSON string.
 * @param field The field name (key) to search for.
 * @return Pointer to the start of the value string (after key and ':'), or NULL if not found.
 */
static char* find_json_field(const char* json, const char* field) {
    char* ptr = strstr(json, field);
    if (!ptr) return NULL;

    ptr = strchr(ptr, ':');
    if (!ptr) return NULL;

    ptr++;
    while (*ptr == ' ' || *ptr == '\"') ptr++;
    return ptr;
}

/**
 * @brief Parses titles and abstracts from a raw JSON string using string searching.
 *
 * @param json Input raw JSON buffer containing article information.
 */
void parse_titles_and_abstracts(const char *json) {
    const char *ptr = json;
    title_count = 0;

    while ((ptr = strstr(ptr, "\"title\"")) != NULL && title_count < MAX_TITLES) {
        ptr = find_json_field(ptr, "\"title\"");
        if (!ptr) break;

        char *end = strchr(ptr, '\"');
        if (!end) break;

        size_t len = end - ptr;
        if (len >= MAX_TITLE_LEN) len = MAX_TITLE_LEN - 1;
        strncpy(articles[title_count].title, ptr, len);
        articles[title_count].title[len] = '\0';

        ptr = end;
        ptr = strstr(ptr, "\"abstract\"");
        if (!ptr) break;

        ptr = find_json_field(ptr, "\"abstract\"");
        if (!ptr) break;

        end = strchr(ptr, '\"');
        if (!end) break;

        len = end - ptr;
        if (len >= MAX_ABSTRACT_LEN) len = MAX_ABSTRACT_LEN - 1;
        strncpy(articles[title_count].abstract, ptr, len);
        articles[title_count].abstract[len] = '\0';

        title_count++;
        ptr = end;
    }
}

