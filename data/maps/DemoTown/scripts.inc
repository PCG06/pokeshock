DemoTown_MapScripts::
	.byte 0

DemoTown_Text_Sign_ExpansionDebugMenu:
	.string "Expansion Tips!\p"
	.string "Hold {R_BUTTON} while pressing {START_BUTTON} to open\n"
	.string "the Expansion's overworld debug menu!$"

DemoTown_EventScript_Sign_ExpansionDebugMenu::
	msgbox DemoTown_Text_Sign_ExpansionDebugMenu, MSGBOX_SIGN
	end
