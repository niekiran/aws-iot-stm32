/*
 * parser.c
 *
 *  Created on: Jun 16, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef INC_SCROLL_H_
#define INC_SCROLL_H_

#include <stdint.h>
#include <stdbool.h>

/* Call once after parsing new articles */
void scroll_reinit(void);

/* Call periodically inside main loop */
void scroll_update(void);

void scroll_set_section(const char *section_name);
#endif /* INC_SCROLL_H_ */
