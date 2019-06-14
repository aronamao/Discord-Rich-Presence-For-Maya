#include "drpCmd.h"
static MCallbackId projectChangeCallback;
static MCallbackId newFileCallback;
static MCallbackId openFileCallback;


drp::drp()
{
}

//make sure to remove the callbacks to not break maya
drp::~drp()
{
	MMessage::removeCallback(projectChangeCallback);
	MMessage::removeCallback(newFileCallback);
	if (MGlobal::mayaVersion() == "2019") {
		MMessage::removeCallback(openFileCallback);
	}
}

void * drp::creator()
{
	return new drp();
}
