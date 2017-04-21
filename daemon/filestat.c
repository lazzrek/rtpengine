/*
 * filestat.c
 *
 *  Created on: April 13, 2017
 *      Author: lazzrek
 */
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "log.h"
#include "call.h"
#include "filestat.h"

static char* stat_command = NULL;

void stat_set_command(char* command) {
	stat_command = command;
}

void write_stat_data(struct callmaster *cm) {
  struct timeval avg, calls_dur_iv;
	u_int64_t num_sessions, min_sess_iv, max_sess_iv;
	struct request_time offer_iv, answer_iv, delete_iv;

  // sanity checks
  if (!cm) {
    ilog(LOG_ERROR, "NULL callmaster when trying to send data");
    return;
  }

	mutex_lock(&cm->totalstats.total_average_lock);
	avg = cm->totalstats.total_average_call_dur;
	num_sessions = cm->totalstats.total_managed_sess;
	mutex_unlock(&cm->totalstats.total_average_lock);

  /*char* stats_filepath = "/tmp/rtpengine.stats";
  chmod(stats_filepath, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

  FILE *mfp = fopen(stats_filepath, "w");
  if (mfp == NULL) {
		ilog(LOG_ERROR, "Could not open stat file: %s", stats_filepath);
		//free(stats_filepath);
		return;
	}*/

  FILE* ffd = popen(stat_command, "w");
  if(!ffd) {
    fprintf(stderr, "fail\n");
    return;
  }

  fprintf(ffd, "uptime %llu\n", (unsigned long long)time(NULL)-cm->totalstats.started);
  fprintf(ffd, "total_managed_sess "UINT64F"\n", num_sessions);
  fprintf(ffd, "total_rejected_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_rejected_sess));
  fprintf(ffd, "total_timeout_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_timeout_sess));
  fprintf(ffd, "total_silent_timeout_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_silent_timeout_sess));
  fprintf(ffd, "total_final_timeout_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_final_timeout_sess));
  fprintf(ffd, "total_regular_term_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_regular_term_sess));
  fprintf(ffd, "total_forced_term_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_forced_term_sess));
  fprintf(ffd, "total_relayed_packets "UINT64F"\n", atomic64_get(&cm->totalstats.total_relayed_packets));
  fprintf(ffd, "total_relayed_errors "UINT64F"\n", atomic64_get(&cm->totalstats.total_relayed_errors));
  fprintf(ffd, "total_nopacket_relayed_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_nopacket_relayed_sess));
  fprintf(ffd, "total_rejected_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_oneway_stream_sess));
  fprintf(ffd, "average_call_dur %ld.%06ld\n", avg.tv_sec, avg.tv_usec);

  pclose(ffd);
}

void filestat_loop(void *d) {
	struct callmaster *cm = d;
  int passed = 0;
  int interval = 10;

    // sanity checks
  if (!cm) {
    ilog(LOG_ERROR, "NULL callmaster");
    return ;
  }

	while (!g_shutdown) {
    sleep(1);
    passed += 1;
    if(passed >= interval) {
      write_stat_data(cm);
      passed = 0;
    }
  }
}
