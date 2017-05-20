/*
 * execstat.h
 *
 *  Created on: April 13, 2017
 *      Author: lazzrek
 */

#ifndef EXECSTAT_H_
#define EXECSTAT_H_

#include "call.h"

void write_stat_data(struct callmaster *cm);
void execstat_init(char* prefix, int interval);
void execstat_loop(void *d);
#endif /* EXECSTAT_H_ */
