/*
 *    sfall
 *    Copyright (C) 2008, 2009, 2010, 2012  The sfall team
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>

#include "..\main.h"
#include "..\FalloutEngine\Fallout2.h"
#include "..\Version.h"

#include "Credits.h"

namespace sfall
{

static DWORD InCredits = 0;
static DWORD CreditsLine = 0;

static const char* ExtraLines[] = {
	"#SFALL " VERSION_STRING,
	"",
	"sfall is free software, licensed under the GPL",
	"Copyright 2008-2017  The sfall team",
	"",
	"@Author",
	"Timeslip",
	"",
	"@Contributors",
	"ravachol",
	"Noid",
	"Glovz",
	"Dream",
	"Ray",
	"Kanhef",
	"KLIMaka",
	"Mash",
	"Helios",
	"Haenlomal",
	"NVShacker",
	"NovaRain",
	"JimTheDinosaur",
	"phobos2077",
	"Tehnokrat",
	"Crafty",
	"Slider2k",
	"Vennor",
	"Oppen",
	"",
	"@Additional thanks to",
	"Nirran",
	"killap",
	"MIB88",
	"Rain man",
	"Continuum",
	"Mr.Stalin",
	"Drobovik",
	"Anyone who has used sfall in their own mods",
	"The bug reporters and feature requesters",
	""
	"",
	"",
	"#FALLOUT 2",
	""
};
static DWORD ExtraLineCount = sizeof(ExtraLines)/4;

static const char* creditsFile = "credits.txt";

static void _stdcall ShowCreditsHook() {
	InCredits = 1;
	CreditsLine = 0;
	__asm {
		mov  eax, creditsFile;
		call fo::funcoffs::credits_;
	}
	InCredits = 0;
}

static DWORD _stdcall CreditsNextLine(char* buf, DWORD* font, DWORD* colour) {
	if (!InCredits || CreditsLine >= ExtraLineCount) return 0;
	const char* line = ExtraLines[CreditsLine++];
	if (strlen(line)) {
		if (line[0] == '#') {
			line++;
			*font = fo::var::name_font;
			*colour = *(BYTE*)0x6A7F01;
		} else if (line[0] == '@') {
			line++;
			*font = fo::var::title_font;
			*colour = fo::var::title_color;
		} else {
			*font = fo::var::name_font;
			*colour = fo::var::name_color;
		}
	}
	strcpy_s(buf, 256, line);
	return 1;
}

// Additional lines will be at the top of CREDITS.TXT contents
static void __declspec(naked) CreditsNextLineHook_Top() {
	__asm {
		pushad;
		push ebx;
		push edx;
		push eax;
		call CreditsNextLine;
		test eax, eax;
		popad;
		jz  fail;
		xor eax, eax;
		inc eax;
		retn;
fail:
		jmp fo::funcoffs::credits_get_next_line_;
	}
}

// Additional lines will be at the bottom of CREDITS.TXT contents
static void __declspec(naked) CreditsNextLineHook_Bottom() {
	__asm {
		pushad;
		call fo::funcoffs::credits_get_next_line_;  // call default function
		test eax, eax;
		popad;
		jnz  morelines;               // if not the end yet, skip custom code
		pushad;
		push ebx;
		push edx;
		push eax;
		call CreditsNextLine;         // otherwise call out function
		test eax, eax;                // if any extra lines left, return 1 (from function), 0 otherwise
		popad;
		jnz  morelines;
		mov  eax, 0x0;
		retn;
morelines:
		mov  eax, 0x1;
		retn;
	}
}

void Credits::init() {
	HookCall(0x480C49, &ShowCreditsHook);
	HookCall(0x43F881, &ShowCreditsHook);
	if (GetConfigInt("Misc", "CreditsAtBottom", 0)) {
		HookCall(0x42CB49, &CreditsNextLineHook_Bottom);
	} else {
		HookCall(0x42CB49, &CreditsNextLineHook_Top);
	}
}

}
