#include "RichPresence.h"

#include <maya/MGlobal.h>
#include <maya/MFileIO.h>
#include <maya/MEventMessage.h>
#include <maya/MSceneMessage.h>
#include <sstream>
#include <chrono>

#include <filesystem>
#include "ini.h"


RichPresence::RichPresence()
{	
	Initialize();
}

RichPresence::~RichPresence()
{
	Disable();
	client.reset();
}

void RichPresence::Enable()
{
	BlockUpdates();

	// TODO remove hard-coding of names through python
	MString result;
	MGlobal::executeCommand("getModulePath -moduleName DRPForMaya", result);
	std::filesystem::path path((result + "/config/config.ini").asChar());

	mINI::INIFile file(path);
	mINI::INIStructure ini;
	file.read(ini);

	mINI::INIMap<std::string> section = ini.get("General");
	bool details = (section.get("details") == "true");
	if (details)
		RegisterSceneCallbacks();
	OnFileChange(nullptr); // we run it once manually to force a refresh


	bool state = (section.get("state") == "true");
	if (state)
		RegisterProjectCallback();
	OnProjectChange(nullptr); // we run it once manually to force a refresh


	bool reset_time_with_scene = (section.get("reset_time_with_scene") == "true");
	SetResetTimeOnChange(reset_time_with_scene);

	RefreshTimestamp(); // one forced refresh to add the timestamp to activity
	UnblockUpdates();

	Update();
}

void RichPresence::Disable()
{
	RemoveSceneCallbacks();
	RemoveProjectCallback();
}

void RichPresence::Initialize()
{
	client = std::make_unique<discordpp::Client>();
	client->SetApplicationId(APPLICATION_ID);
}

void RichPresence::Update()
{
	if (updatesBlocked) 
		return;
	client->UpdateRichPresence(activity, [](const discordpp::ClientResult& result) {});
}

void RichPresence::SetDetails(MString details)
{
	dDetails = details;
	if (!dDetails.length())
		activity.SetDetails(std::nullopt);
	else
		activity.SetDetails(dDetails.asChar());
}

void RichPresence::SetState(MString state)
{
	dState = state;
	if (!dState.length())
		activity.SetState(std::nullopt);
	else
		activity.SetState(dState.asChar());
}

void RichPresence::RefreshTimestamp()
{
	dTimestamp.SetStart(time(nullptr));
	activity.SetTimestamps(dTimestamp);
}

void RichPresence::RegisterSceneCallbacks()
{
	if (sceneCallbackIds.size() != 0)
	{
#ifdef DEBUG
		MGlobal::displayInfo("Scene callbacks are already registered.");
#endif // DEBUG
		return;
	}
	sceneCallbackIds.push_back(MSceneMessage::addCallback(MSceneMessage::kAfterNew, OnFileChange));
	sceneCallbackIds.push_back(MSceneMessage::addCallback(MSceneMessage::kAfterOpen, OnFileChange));
}

void RichPresence::RemoveSceneCallbacks()
{
	if (sceneCallbackIds.size() == 0)
	{
#ifdef DEBUG
		MGlobal::displayInfo("Scene callbacks are not registered.");
#endif
		return;
	}
	for (auto& id : sceneCallbackIds)
	{
		MSceneMessage::removeCallback(id);
	}
	sceneCallbackIds.clear();
	SetDetails("");
	Update();
}

void RichPresence::RegisterProjectCallback()
{
	if (projectChangeCallbackId != -1)
	{
#ifdef DEBUG
		MGlobal::displayInfo("Project change callback is already registered.");
#endif // DEBUG
		return;
	}
	projectChangeCallbackId = MEventMessage::addEventCallback("workspaceChanged", OnProjectChange);
}

void RichPresence::RemoveProjectCallback()
{
	if (projectChangeCallbackId == -1)
	{
#ifdef DEBUG
		MGlobal::displayInfo("Project change callback is not registered.");
#endif // DEBUG
		return;
	}
	MStatus status = MEventMessage::removeCallback(projectChangeCallbackId);
	projectChangeCallbackId = -1;
}

void RichPresence::OnProjectChange(void* clientData)
{
	MString projectName;
	MGlobal::executeCommand("workspace -q -active", projectName);
	std::filesystem::path path(projectName.asChar());
	MString dir = path.stem().c_str();

	instance->SetState(dir);
	instance->Update();
}

void RichPresence::OnFileChange(void* clientData)
{
	std::filesystem::path path(MFileIO::currentFile().asChar());
	MString name(("Working on: " + path.filename().string()).c_str());
	instance->SetDetails(name);
	if (instance->resetTimeOnChange)
		instance->RefreshTimestamp();

	instance->Update();
}

void RichPresence::SetResetTimeOnChange(bool reset)
{
	resetTimeOnChange = reset;
}
