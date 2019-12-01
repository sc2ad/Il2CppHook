#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

#ifdef _Windows

#else
#include <android/log.h>
enum LOG_VERBOSE_TYPE {
	CRITICAL = ANDROID_LOG_FATAL,
	ERROR = ANDROID_LOG_ERROR,
	WARNING = ANDROID_LOG_WARN,
	INFO = ANDROID_LOG_INFO,
	DEBUG = ANDROID_LOG_DEBUG
};
#define log_base(lvl, ...) __android_log_print(lvl, TAG, __VA_ARGS__)
#define log_vbase(lvl, ...) __android_log_vprint(lvl, TAG, __VA_ARGS__)
#endif

#ifndef MOD_ID
// This is too annoying, let's change it to default to some stupid stuff
// #error "'MOD_ID' must be defined in the mod!"
#define MOD_ID "PLACEHOLDER_MOD_ID"
#endif
#ifndef VERSION
// This is too annoying, let's change it to default to some stupid stuff
// #error "'VERSION' must be defined in the mod!"
#define VERSION "0.0.0"
#endif

#define TAG "QuestHook[" MOD_ID "|" VERSION "]"

namespace logging {
	static int log(LOG_VERBOSE_TYPE level, const char* fmt, va_list args) {
		return log_vbase(level, fmt, args);
	}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
	template<typename... TArgs>
	static int log(LOG_VERBOSE_TYPE level, TArgs ...args) {
		return log_base(level, args...);
	}
#pragma clang diagnostic pop
}
using logging::log;

#ifndef STD_BUFFER_SIZE
#define STD_BUFFER_SIZE 256
#endif

// From: https://codelab.wordpress.com/2014/11/03/how-to-use-standard-output-streams-for-logging-in-android-apps/
static int pfd[2];
static pthread_t thr;
static void* thread_func(void*)
{
	ssize_t rdsz;
	char buf[STD_BUFFER_SIZE];
	while ((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0) {
		if (buf[rdsz - 1] == '\n') --rdsz;
		buf[rdsz] = 0;  /* add null-terminator */
		log_base(INFO, TAG, buf);
	}
	return 0;
}

extern "C" {
	// Redirects stdout and stderr to the android log stream. Call this once before using stdout/stderr
	// Returns 0 on success, -1 otherwise
	static int start_logger()
	{
		/* make stdout line-buffered and stderr unbuffered */
		setvbuf(stdout, NULL, _IOLBF, 0);
		setvbuf(stderr, NULL, _IONBF, 0);

		/* create the pipe and redirect stdout and stderr */
		pipe(pfd);
		dup2(pfd[1], 1);
		dup2(pfd[1], 2);

		/* spawn the logging thread */
		if (pthread_create(&thr, 0, thread_func, 0) == -1)
			return -1;
		pthread_detach(thr);
		return 0;
	}
}
