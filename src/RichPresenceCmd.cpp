#include "RichPresenceCmd.h"

MStatus RichPresenceCmd::doIt(const MArgList& argList)
{
	MStatus status;
	const MArgDatabase argData(syntax(), argList, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	ptr->BlockUpdates();

	if (argData.isFlagSet("-d"))
	{
		MString details;
		status = argData.getFlagArgument("-d", 0, details);
		ptr->SetDetails(details);
	}
	if (argData.isFlagSet("-st"))
	{
		MString state;
		status = argData.getFlagArgument("-st", 0, state);
		ptr->SetState(state);
	}
	if (argData.isFlagSet("-rt"))
	{
		ptr->RefreshTimestamp();
	}
	if (argData.isFlagSet("-ds"))
	{
		bool displayScene;
		status = argData.getFlagArgument("-ds", 0, displayScene);
		if (status == MStatus::kSuccess)
		{
			if (displayScene)
			{
				ptr->RegisterSceneCallbacks();
				ptr->OnFileChange(nullptr);
			}
			else
			{
				ptr->RemoveSceneCallbacks();
				ptr->SetDetails("");
			}
		}
	}
	if (argData.isFlagSet("-dp"))
	{
		bool displayProject;
		status = argData.getFlagArgument("-dp", 0, displayProject);
		if (status == MStatus::kSuccess)
		{
			if (displayProject)
			{
				ptr->RegisterProjectCallback();
				ptr->OnProjectChange(nullptr);
			}
			else
			{
				ptr->RemoveProjectCallback();
				ptr->SetState("");
			}
		}
	}
	if (argData.isFlagSet("-roc"))
	{
		bool resetTimeOnChange;
		status = argData.getFlagArgument("-roc", 0, resetTimeOnChange);
		if (status == MStatus::kSuccess)
		{
			ptr->SetResetTimeOnChange(resetTimeOnChange);
		}
	}

	ptr->UnblockUpdates();
	ptr->Update();
	return status;
}

MSyntax RichPresenceCmd::NewSyntax()
{
	MSyntax syntax{};
	syntax.setMaxObjects(0);
	syntax.enableEdit(false);
	syntax.addFlag("-d", "-details", MSyntax::kString);
	syntax.addFlag("-st", "-state", MSyntax::kString);
	syntax.addFlag("-ds", "-displayScene", MSyntax::kBoolean);
	syntax.addFlag("-dp", "-displayProject", MSyntax::kBoolean);
	syntax.addFlag("-roc", "-resetTimeOnChange", MSyntax::kBoolean);
	syntax.addFlag("-rt", "-resetTime", MSyntax::kNoArg);
	return syntax;
}
