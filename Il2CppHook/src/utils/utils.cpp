// utils.cpp : Defines functions declared in utils.h

// thx https://github.com/jbro129/Unity-Substrate-Hook-Android

#include <jni.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../../include/utils.h"
#include <android/log.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "../../include/logging.h"

using namespace std;

long long baseAddr(const char* soname)  // credits to https://github.com/ikoz/AndroidSubstrate_hookingC_examples/blob/master/nativeHook3/jni/nativeHook3.cy.cpp
{
	void* imagehandle = dlopen(soname, RTLD_LOCAL | RTLD_LAZY);
	if (soname == NULL)
		return (long long)NULL;
	if (imagehandle == NULL) {
		return (long long)NULL;
	}
	uintptr_t* irc = NULL;
	FILE* f = NULL;
	char line[200] = { 0 };
	char* state = NULL;
	char* tok = NULL;
	char* baseAddr = NULL;
	if ((f = fopen("/proc/self/maps", "r")) == NULL)
		return (long long)NULL;
	while (fgets(line, 199, f) != NULL)
	{
		tok = strtok_r(line, "-", &state);
		baseAddr = tok;
		tok = strtok_r(NULL, "\t ", &state);
		tok = strtok_r(NULL, "\t ", &state); // "r-xp" field
		tok = strtok_r(NULL, "\t ", &state); // "0000000" field
		tok = strtok_r(NULL, "\t ", &state); // "01:02" field
		tok = strtok_r(NULL, "\t ", &state); // "133224" field
		tok = strtok_r(NULL, "\t ", &state); // path field

		if (tok != NULL) {
			int i;
			for (i = (int)strlen(tok) - 1; i >= 0; --i) {
				if (!(tok[i] == ' ' || tok[i] == '\r' || tok[i] == '\n' || tok[i] == '\t'))
					break;
				tok[i] = 0;
			}
			{
				size_t toklen = strlen(tok);
				size_t solen = strlen(soname);
				if (toklen > 0) {
					if (toklen >= solen && strcmp(tok + (toklen - solen), soname) == 0) {
						fclose(f);
						return (long long)strtoll(baseAddr, NULL, 16);
					}
				}
			}
		}
	}
	fclose(f);
	return (long long)NULL;
}

long long location; // save lib.so base address so we do not have to recalculate every time causing lag.

long long getRealOffset(void* offset) // calculate dump.cs address + lib.so base address.
{
	if (location == 0)
	{
		//arm
		location = baseAddr(IL2CPP_SO_PATH); // replace the com.package.name with the package name of the app you are modding.
	}
	return location + (long long)offset;
}

string to_utf8(u16string_view view) {
	char dat[view.length() + 1];
	transform(view.data(), view.data() + view.size(), dat, [](auto utf16_char) {
		return static_cast<char>(utf16_char);
		});
	dat[view.length()] = '\0';
	return { dat };
}

u16string to_utf16(string_view view) {
	char16_t dat[view.length() + 1];
	transform(view.data(), view.data() + view.size(), dat, [](auto standardChar) {
		return static_cast<char16_t>(standardChar);
		});
	dat[view.length()] = '\0';
	return { dat };
}

void dump(int before, int after, void* ptr) {
	log(DEBUG, "Dumping Immediate Pointer: %p: %08x", ptr, *reinterpret_cast<int*>(ptr));
	auto begin = static_cast<int*>(ptr) - before;
	auto end = static_cast<int*>(ptr) + after;
	for (auto cur = begin; cur != end; ++cur) {
		log(DEBUG, "%p: %08x", cur, *cur);
	}
}

bool fileexists(const char* filename) {
	return access(filename, W_OK | R_OK) != -1;
}

bool direxists(const char* dirname) {
	struct stat info;

	if (stat(dirname, &info) != 0) {
		return false;
	} if (info.st_mode & S_IFDIR) {
		return true;
	}
	return false;
}

char* readfile(const char* filename) {
	FILE* fp = fopen(filename, "r");
	char* content = NULL;
	long size = 0;
	if (fp) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		rewind(fp);
		content = (char*)malloc(size * sizeof(char));
		fread(content, sizeof(char), size, fp);
		fclose(fp);
	}
	return content;
}

int writefile(const char* filename, const char* text) {
	FILE* fp = fopen(filename, "w");
	if (fp) {
		fwrite(text, sizeof(char), strlen(text), fp);
		fclose(fp);
		return 0;
	}
	return -1;
}
