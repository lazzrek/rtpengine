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

static char* file_prefix = NULL;

void filestat_set_prefix(char* prefix) {
	file_prefix = prefix;
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

  char* stats_filepath = "/tmp/rtpengine.stats";
  chmod(stats_filepath, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

  FILE *mfp = fopen(stats_filepath, "w");
  if (mfp == NULL) {
		ilog(LOG_ERROR, "Could not open stat file: %s", stats_filepath);
		//free(stats_filepath);
		return;
	}

  fprintf(mfp, "uptime %llu\n", (unsigned long long)time(NULL)-cm->totalstats.started);
  fprintf(mfp, "total_managed_sess "UINT64F"\n", num_sessions);
  fprintf(mfp, "total_rejected_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_rejected_sess));
  fprintf(mfp, "total_timeout_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_timeout_sess));
  fprintf(mfp, "total_silent_timeout_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_silent_timeout_sess));
  fprintf(mfp, "total_final_timeout_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_final_timeout_sess));
  fprintf(mfp, "total_regular_term_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_regular_term_sess));
  fprintf(mfp, "total_forced_term_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_forced_term_sess));
  fprintf(mfp, "total_relayed_packets "UINT64F"\n", atomic64_get(&cm->totalstats.total_relayed_packets));
  fprintf(mfp, "total_relayed_errors "UINT64F"\n", atomic64_get(&cm->totalstats.total_relayed_errors));
  fprintf(mfp, "total_nopacket_relayed_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_nopacket_relayed_sess));
  fprintf(mfp, "total_rejected_sess "UINT64F"\n", atomic64_get(&cm->totalstats.total_oneway_stream_sess));
  fprintf(mfp, "average_call_dur %ld.%06ld\n", avg.tv_sec, avg.tv_usec);
  fclose(mfp);
}

void filestat_loop(void *d) {
	struct callmaster *cm = d;

  // sanity checks
  if (!cm) {
    ilog(LOG_ERROR, "NULL callmaster");
    return ;
  }

	while (!g_shutdown) {
    write_stat_data(cm);
    sleep(10);
  }

}
