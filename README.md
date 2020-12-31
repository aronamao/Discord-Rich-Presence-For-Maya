# Autodesk Maya Rich Presence

This plugin will update your status on Discord depending on your scene and project.

Currently works in Maya 2016-2020, it is however only compiled for 2018-2020 as I don't have 2016/2017. If you want to use it in either one you will have to compile it yourself. 2021 support will be added as soon as possible.

<img width="300" alt="Discord_MYJCCDwKbt" src="https://user-images.githubusercontent.com/50831997/59538496-44474380-8efa-11e9-8716-8fa73f144de2.png">

Latest compiled release available [here](https://github.com/ArhasGH/Discord-Rich-Presence-For-Maya/releases/latest)

## Instructions

Simply move the RichPresence.mll to your C:/Users/Name/Documents/maya/Version/plug-ins and enable it in your Plugin Manager.

A menu will be added to your menubar at the top called Rich Presence, in here you can change the settings according to your needs. Note that turning it off and on too many times in a row can lead to it crashing, if that happens, restart maya and everything should work as usual. Also keep in mind that enabling and disabling it can take a while, but that limitation is set by Discord, there's nothing I can do to change that. Your settings will be saved even if maya crashes and the same settings will apply across multiple Maya versions.
If you run into issues you can disable the plug-in in the Plugin Manager, doing so will reset everything. Your Maya may or may not crash when re-enabling it again too quickly, tho I haven't run into this crash for quite a while even when deliberately testing. Aside from that the plugin appears to be stable, at least from my testing over the last 2 years.

