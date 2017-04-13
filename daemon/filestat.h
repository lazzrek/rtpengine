/*
 * filestat.h
 *
 *  Created on: April 13, 2017
 *      Author: lazzrek
 */

#ifndef FILESTAT_H_
#define FILESTAT_H_

#include "call.h"

void write_stat_data(struct callmaster *cm);
void filestat_set_prefix(char* prefix);
void filestat_loop(void *d);
#endif /* FILESTAT_H_ */
