DemoTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DemoTown_OnTransition
	.byte 0

DemoTown_OnTransition:
	setrespawn HEAL_LOCATION_DEMO_TOWN
	end

DemoTown_Text_Sign_ExpansionDebugMenu:
	.string "Expansion Tips!\p"
	.string "Hold {R_BUTTON} while pressing {START_BUTTON} to open\n"
	.string "the Expansion's overworld debug menu!$"

DemoTown_EventScript_Sign_ExpansionDebugMenu::
	msgbox DemoTown_Text_Sign_ExpansionDebugMenu, MSGBOX_SIGN
	end
