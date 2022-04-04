#include <maya/MFnPlugin.h>
#include <maya/MFileIO.h>
#include "drpCmd.h"
#include "Discord.h"


Discord* g_Discord;
MStringArray test;
MString mayaVer = MGlobal::mayaVersion();

MString projectName;
MString workspace;

nlohmann::json Settings;

std::string path = "C:/ProgramData/DRPMaya/settings.json";


void read() {
	std::ifstream file(path);
	file >> Settings;
	file.close();
}

void write(bool value = true, bool name = true, bool project = true, bool time = true, bool secret = false) {
	std::ofstream file(path);
	Settings = {
			{"PresenceEnabled", value},
			{"DisplayFileName", name},
			{"DisplayProjectName", project},
			{"DisplayTime", time},
			{"Secret", secret}
	};
	file << std::setw(4) << Settings;
	file.close();
}

//function for calling the Discord Update
void update() {
	//Check if name and/or project are uninitialized and grab the values from the config if that's the case
	read();
	// define the project name
	MStatus status = MGlobal::executeCommand(MString("workspace -q -active"), workspace);
	// if the name is supposed to be displayed generate it
	if (Settings["DisplayFileName"]) {
		//grab the current file directory
		std::string fileName = MFileIO::currentFile().asChar();
		std::size_t botDirPos = fileName.find_last_of("/") + 1;
		std::string file = fileName.substr(botDirPos, fileName.length());
		std::string ext = ".ma";
		if (file.find(".ma") != std::string::npos || file.find(".mb") != std::string::npos) {
			ext = "";
		}
		std::string result = "Working on " + file + ext;
		//pass the name,project and whether or not they are to be displayed
		g_Discord->Update(result.c_str(), workspace.asChar(), Settings);
	}
	// if the name is not supposed to be displayed just pass NULL
	else {

		g_Discord->Update(NULL, workspace.asChar(), Settings);
	}
}


// just calling the initialization
void initialize() {
	const char* ID = NULL;
	if (mayaVer == "2016") {
		ID = "589191141132468345";
	}
	else if (mayaVer == "2017") {
		ID = "794218711342383154";
	}
	else if (mayaVer == "2018") {
		ID = "578912672628604948";
	}
	else if (mayaVer == "2019") {
		ID = "585149869098926108";
	}
	else if (mayaVer == "2020") {
		ID = "654424985363808286";
	}
	else if (mayaVer == "2022") {
		ID = "824375466566746193";
	}
	else if (mayaVer == "2023") {
		ID = "960249266192912534";
	}
	const char* version = mayaVer.asChar();
	g_Discord->Initialize(ID);
}
// just calling the Shutdown function
void shutDown() {
	g_Discord->ShutDown();
}

void compareProject() {
	MGlobal::executeCommand(MString("workspace -q -	active"), projectName);
	if (workspace != projectName) {
		update();
	}
}

const char* createMenuItem(std::string first, bool value, std::string second) {
	return ("menuItem -label " + first + " -cb " + std::string(value ? "1" : "0") + "-c " + second).c_str();
}

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	// add the command
	MFnPlugin fnPlugin(obj, "Arhas", "1.0", "Any");
	status = fnPlugin.registerCommand("drpEnable", drp::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	//add the menu at the top by using MEL Commands
	MString menu;
	MGlobal::executeCommand("menu -p MayaWindow -l \"Rich Presence\" RP", menu);

	//add workaround Callback for project Change by comparing the old project name to the new one
	MCallbackId projectChangeCallback = MEventMessage::addEventCallback("workspaceChanged", (MMessage::MBasicFunction)compareProject);
	//add Callback for when you make a new/open a file to update Discord
	MCallbackId newFileCallback = MSceneMessage::addCallback(MSceneMessage::kAfterNew, (MMessage::MBasicFunction)update);
	MCallbackId openFileCallback = MSceneMessage::addCallback(MSceneMessage::kAfterOpen, (MMessage::MBasicFunction)update);

	//create the directory and the config and set default values if non-existant
	if (CreateDirectory("C:/ProgramData/DRPMaya", NULL)) {
		write();
	}

	//if the directory already exists grab the old settings
	else if (ERROR_ALREADY_EXISTS == GetLastError()) {
		read();
	}
	//add menu items with a checkbox in the right state and have it call the drpEnable command if toggled

	if (Settings["PresenceEnabled"] == true) {
		initialize();
		update();
	}
	MGlobal::executeCommand(createMenuItem("Enable", Settings["PresenceEnabled"].get<bool>(), "\"drpEnable\" Enable"));
	MGlobal::executeCommand(createMenuItem("\"Show file name\"", Settings["DisplayFileName"].get<bool>(), "\"drpEnable\" Name"));
	MGlobal::executeCommand(createMenuItem("\"Show project name\"", Settings["DisplayProjectName"].get<bool>(), "\"drpEnable\" Project"));
	MGlobal::executeCommand(createMenuItem("\"Show time spent\"", Settings["DisplayTime"].get<bool>(), "\"drpEnable\" Time"));
	MGlobal::executeCommand(createMenuItem("\"Secret Mode\"", Settings["Secret"].get<bool>(), "\"drpEnable\" Secret"));
	return MS::kSuccess;
}
MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	//shut down DRP
	shutDown();

	//remove the command
	MFnPlugin fnPlugin(obj);
	status = fnPlugin.deregisterCommand("drpEnable");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	//remove the menu at the top
	MGlobal::executeCommand("deleteUI RP");

	//close the file if currently open
	std::ifstream file("C:/ProgramData/DRPMaya/settings.json");
	if (file.is_open()) {
		file.close();
	}
	//delete the file and directory
	remove("C:/ProgramData/DRPMaya/settings.json");
	_rmdir("C:/ProgramData/DRPMaya");

	return MS::kSuccess;
}

MStatus drp::doIt(const MArgList& argList)
{
	MStatus status;
	int value;
	int name;
	int project;
	int time;
	int secret;

	//query the current state of the checkboxes and store them in the according variable
	MGlobal::executeCommand("menuItem -q -cb Enable", value);
	MGlobal::executeCommand("menuItem -q -cb Name", name);
	MGlobal::executeCommand("menuItem -q -cb Project", project);
	MGlobal::executeCommand("menuItem -q -cb Time", time);
	MGlobal::executeCommand("menuItem -q -cb Secret", secret);

	//if the value is being set to off it should shutdown DRP
	if (value == 0) {
		shutDown();
		write(value, name, project, time, secret);

	}

	//if the value is being set to on it should start it and set it to the current values and store the current values in the config
	else {
		initialize();

		write(value, name, project, time, secret);
		update();
	}

	return MS::kSuccess;
}