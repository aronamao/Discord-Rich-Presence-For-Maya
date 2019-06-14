#pragma once
#include <discord_register.h>
#include <discord_rpc.h>
#include <Windows.h>
#include <time.h>
#include <chrono>
#include <maya/MString.h>

//adding the timer
static int64_t eptime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

//prototyping Discord related things
class Discord {
public:
	void Initialize(const char* ID);
	void Update(const char* result,const char* workspace,int name,int project, int time);
	void ShutDown();
};