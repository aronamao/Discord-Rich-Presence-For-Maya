#include "Discord.h"



//initialize function to establish the connection (more or less)
void Discord::Initialize(const char* ID) {
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	Discord_Initialize(ID,&Handle, 1, NULL);
}

//update function needed to update DRP to the desired values, everything that is a const char* is just text and the ints decide whether or not the text is displayed in the first place
void Discord::Update(const char* result,const char* workspace,int name,int project,int time) {
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	if (name == 1) {
		discordPresence.details = result;
	}
	else {
		discordPresence.details = NULL;
	}
	if (time == 1) {
		//time defined in Discord.h
		discordPresence.startTimestamp = eptime;
	}
	else {
		discordPresence.startTimestamp = NULL;
	}
	if (project == 1) {
		discordPresence.state = workspace;
	}
	else {
		discordPresence.state = NULL;
	}
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