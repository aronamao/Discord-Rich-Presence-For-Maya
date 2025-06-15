#pragma once
#include <string>
#include <maya/MTimerMessage.h>
#include <maya/MString.h>

#include "discordpp.h"


template <typename T> class Singleton
{
protected:
	Singleton() = default;
	~Singleton() = default;

    inline static T* instance;


public:
    Singleton(Singleton& other) = delete;
    void operator=(const Singleton&) = delete;


	inline static T* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new T();
		}
		return instance;
	}

	inline static void DeleteInstance()
	{
		if (instance != nullptr)
		{
			delete instance;
			instance = nullptr;
		}
	}
};


class RichPresence : public Singleton<RichPresence>
{
private:
	inline static const uint64_t APPLICATION_ID = 1119332858394316880;
	MString dDetails;
	MString dState;
	discordpp::ActivityTimestamps dTimestamp;
	bool resetTimeOnChange = false;

	MCallbackId projectChangeCallbackId = -1;
	std::vector<MCallbackId> sceneCallbackIds;

	inline static std::unique_ptr<discordpp::Client> client;
	inline static discordpp::Activity activity;

	bool updatesBlocked = false;

public:
    RichPresence();
    ~RichPresence();

	void BlockUpdates() { updatesBlocked = true; };
	void UnblockUpdates() { updatesBlocked = false; };

	void Enable();
	void Disable();

	void Initialize();
	void Update();

	void SetDetails(MString details);
	void SetState(MString state);
	void RefreshTimestamp();

	void RegisterSceneCallbacks();
	void RemoveSceneCallbacks();

	void RegisterProjectCallback();
	void RemoveProjectCallback();

	static void OnProjectChange(void* clientData);
	static void OnFileChange(void* clientData);

	void SetResetTimeOnChange(bool reset);
};