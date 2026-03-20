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
 * @brief Filters and sanitizes input text in-place.
 *
 * This function performs:
 * - Removal of leading spaces
 * - Removal of extra spaces (keeps only one space between words)
 * - Removal of special characters (keeps only alphanumeric characters)
 * - Trims trailing space
 *
 * @param[in,out] str Pointer to the string to be cleaned
 */
static void filter_text(char *str) {
    char *src = str;
    char *dst = str;

    // Skip leading spaces
    while (*src == ' ') {
        src++;
    }

    int space_found = 0;

    while (*src) {
        // Keep only alphanumeric characters
        if ((*src >= 'A' && *src <= 'Z') ||
            (*src >= 'a' && *src <= 'z') ||
            (*src >= '0' && *src <= '9')) {

            *dst++ = *src;
            space_found = 0;
        }
        // Handle whitespace (convert multiple spaces to single space)
        else if (*src == ' ' || *src == '\n' || *src == '\t' || *src == '\r') {
            if (!space_found) {
                *dst++ = ' ';
                space_found = 1;
            }
        }
        // Ignore all other special characters

        src++;
    }

    // Remove trailing space if present
    if (dst > str && *(dst - 1) == ' ') {
        dst--;
    }

    *dst = '\0';
}


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

        /* @brief Remove special characters from abstract */
        filter_text(articles[title_count].title);

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

        /* @brief Remove special characters from abstract */
        filter_text(articles[title_count].abstract);

        title_count++;
        ptr = end;
    }
}

