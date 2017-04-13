/*
 * graphite.h
 *
 *  Created on: Jan 19, 2015
 *      Author: fmetz
 */

#ifndef FILESTAT_H_
#define FILESTAT_H_

#include "call.h"

void write_stat_data(struct callmaster *cm);
void filestat_set_prefix(char* prefix);
void filestat_loop(void *d);
#endif /* FILESTAT_H_ */
