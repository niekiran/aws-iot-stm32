/*
 * parser.h
 *
 *  Created on: Jun 16, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef INC_PARSER_H_
#define INC_PARSER_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_TITLES       10
#define MAX_TITLE_LEN    128
#define MAX_ABSTRACT_LEN 256
#define MAX_ARTICLES     10

typedef struct {
    char title[MAX_TITLE_LEN];
    char abstract[MAX_ABSTRACT_LEN];
} news_article_t;

extern news_article_t articles[MAX_TITLES];
extern uint8_t title_count;

/**
 * @brief Parses titles and abstracts from a raw JSON string using basic string matching.
 */
void parse_titles_and_abstracts(const char *json);

/**
 * @brief Parses titles and abstracts using coreJSON-compliant parsing.
 */
void parse_news_coreJSON(const char *json, size_t length);

#endif /* INC_PARSER_H_ */
