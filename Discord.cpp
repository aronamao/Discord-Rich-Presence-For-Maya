#include "Discord.h"



//initialize function to establish the connection (more or less)
void Discord::Initialize(const char* ID) {
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	Discord_Initialize(ID,&Handle, 1, NULL);
}

//update function needed to update DRP to the desired values
void Discord::Update(const char* result, const char* workspace, nlohmann::json Settings) {
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	if (Settings["Secret"]) {
		discordPresence.details = "Working on a secret";
		discordPresence.state = NULL;

	}
	else {
		discordPresence.details = (Settings["DisplayFileName"] ? result : NULL);
		discordPresence.state = (Settings["DisplayProjectName"] ? workspace : NULL);
	}
	discordPresence.startTimestamp = (Settings["DisplayTime"] ? eptime : NULL);
	//endTimestamp set to NULL to have the time go up indefinetly
	discordPresence.endTimestamp = NULL;
	discordPresence.largeImageKey = "lrgicon";
	//discordPresence.largeImageText = "None";
	Discord_UpdatePresence(&discordPresence);
}

//simple function to stop DRP
void Discord::ShutDown() {
	Discord_Shutdown();
}