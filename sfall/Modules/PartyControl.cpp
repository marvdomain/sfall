/*
*    sfall
*    Copyright (C) 2008-2016  The sfall team
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

#include <algorithm>
#include <vector>

#include "..\main.h"
#include "..\FalloutEngine\Fallout2.h"
#include "LoadGameHook.h"

#include "PartyControl.h"

namespace sfall
{

bool isControllingNPC = false;

static DWORD controlMode;
static std::vector<WORD> allowedCritterPids;
static int delayedExperience;

static struct DudeState {
	fo::GameObject* obj_dude = nullptr;
	DWORD art_vault_guy_num;
	long traits[2];
	char pc_name[sizeof(fo::var::pc_name)];
	DWORD last_level;
	DWORD Level;
	DWORD Experience;
	char free_perk;
	DWORD unspent_skill_points;
	//DWORD map_elevation;
	DWORD sneak_working;
	//DWORD sneak_queue_time;
	DWORD itemCurrentItem;
	fo::ItemButtonItem itemButtonItems[2];
	long perkLevelDataList[fo::PERK_count];
	//DWORD drug_gvar[6];
	//DWORD jet_gvar;
	long tag_skill[4];
	//DWORD bbox_sneak;
} realDude;

// saves the state of PC before moving control to NPC
static void SaveRealDudeState() {
	realDude.obj_dude = fo::var::obj_dude;
	realDude.art_vault_guy_num = fo::var::art_vault_guy_num;
	realDude.itemCurrentItem = fo::var::itemCurrentItem;
	memcpy(realDude.itemButtonItems, fo::var::itemButtonItems, sizeof(fo::ItemButtonItem) * 2);
	memcpy(realDude.traits, fo::var::pc_trait, sizeof(long) * 2);
	memcpy(realDude.perkLevelDataList, fo::var::perkLevelDataList, sizeof(DWORD) * fo::PERK_count);
	strcpy_s(realDude.pc_name, sizeof(fo::var::pc_name), fo::var::pc_name);
	realDude.Level = fo::var::Level_;
	realDude.last_level = fo::var::last_level;
	realDude.Experience = fo::var::Experience_;
	realDude.free_perk = fo::var::free_perk;
	realDude.unspent_skill_points = fo::var::curr_pc_stat[0];
	//real_map_elevation = fo::var::map_elevation;
	realDude.sneak_working = fo::var::sneak_working;
	fo::SkillGetTags(realDude.tag_skill, 4);
}

// take control of the NPC
static void SetCurrentDude(fo::GameObject* npc) {
	// remove skill tags
	long tagSkill[4];
	std::fill(std::begin(tagSkill), std::end(tagSkill), -1);
	fo::SkillSetTags(tagSkill, 4);

	// reset traits
	fo::var::pc_trait[0] = fo::var::pc_trait[1] = -1;

	// reset perks
	for (int i = 0; i < fo::PERK_count; i++) {
		fo::var::perkLevelDataList[i] = 0;
	}

	// change character name
	fo::func::critter_pc_set_name(fo::func::critter_name(npc));

	// change level
	int level = fo::func::isPartyMember(npc) 
		? fo::func::partyMemberGetCurLevel(npc) 
		: 0;

	fo::var::Level_ = level;
	fo::var::last_level = level;

	// reset other stats
	fo::var::Experience_ = 0;
	fo::var::free_perk = 0;
	fo::var::curr_pc_stat[0] = 0;
	fo::var::sneak_working = 0;

	// deduce active hand by weapon anim code
	char critterAnim = (npc->artFid & 0xF000) >> 12; // current weapon as seen in hands
	if (fo::AnimCodeByWeapon(fo::func::inven_left_hand(npc)) == critterAnim) { // definitely left hand..
		fo::var::itemCurrentItem = 0;
	} else {
		fo::var::itemCurrentItem = 1;
	}

	// switch main dude_obj pointers - this should be done last!
	fo::var::obj_dude = npc;
	fo::var::inven_dude = npc;
	fo::var::inven_pid = npc->protoId;
	fo::var::art_vault_guy_num = npc->artFid & 0xFFF;

	isControllingNPC = true;
	delayedExperience = 0;

	fo::func::intface_redraw();
}

// restores the real dude state
static void RestoreRealDudeState() {
	assert(realDude.obj_dude != nullptr);

	fo::var::obj_dude = realDude.obj_dude;
	fo::var::inven_dude = realDude.obj_dude;
	fo::var::inven_pid = realDude.obj_dude->protoId;
	fo::var::art_vault_guy_num = realDude.art_vault_guy_num;

	fo::var::itemCurrentItem = realDude.itemCurrentItem;
	memcpy(fo::var::itemButtonItems, realDude.itemButtonItems, sizeof(DWORD) * 6 * 2);
	memcpy(fo::var::pc_trait, realDude.traits, sizeof(long) * 2);
	memcpy(fo::var::perkLevelDataList, realDude.perkLevelDataList, sizeof(DWORD) * fo::PERK_count);
	strcpy_s(fo::var::pc_name, sizeof(fo::var::pc_name), realDude.pc_name);
	fo::var::Level_ = realDude.Level;
	fo::var::last_level = realDude.last_level;
	fo::var::Experience_ = realDude.Experience;
	fo::var::free_perk = realDude.free_perk;
	fo::var::curr_pc_stat[0] = realDude.unspent_skill_points;
	//realDude.map_elevation = fo::var::map_elevation; -- why save elevation?
	fo::var::sneak_working = realDude.sneak_working;
	fo::SkillSetTags(realDude.tag_skill, 4);

	if (delayedExperience > 0) {
		fo::func::stat_pc_add_experience(delayedExperience);
	}

	fo::func::intface_redraw();

	isControllingNPC = false;
}

static void __stdcall DisplayCantDoThat() {
	fo::func::display_print(fo::GetMessageStr(&fo::var::proto_main_msg_file, 675)); // I Can't do that
}

// 1 skip handler, -1 don't skip
int __stdcall PartyControl::SwitchHandHook(fo::GameObject* item) {
	// don't allow to use the weapon, if no art exist for it
	if (fo::func::item_get_type(item) == fo::ItemType::item_type_weapon && isControllingNPC) {
		int fId = fo::var::obj_dude->artFid;
		long weaponCode = fo::AnimCodeByWeapon(item);
		fId = (fId & 0xffff0fff) | (weaponCode << 12);
		if (!fo::func::art_exists(fId)) {
			DisplayCantDoThat();
			return 1;
		}
	}
	return -1;
}

static void __declspec(naked) stat_pc_add_experience_hook() {
	__asm {
		cmp  isControllingNPC, 0
		je   skip
		add  delayedExperience, esi
		retn
skip:
		xchg esi, eax
		jmp  fo::funcoffs::stat_pc_add_experience_
	}
}

// prevents using sneak when controlling NPCs
static void __declspec(naked) pc_flag_toggle_hook() {
	__asm {
		cmp  isControllingNPC, 0
		je   end
		call DisplayCantDoThat
		retn
end:
		call  fo::funcoffs::pc_flag_toggle_
		retn
	}
}

void __stdcall PartyControlReset() {
	if (realDude.obj_dude != nullptr && isControllingNPC) {
		RestoreRealDudeState();
	}
}

bool PartyControl::IsNpcControlled() {
	return isControllingNPC;
}

void PartyControl::SwitchToCritter(fo::GameObject* critter) {
	if (isControllingNPC) {
		RestoreRealDudeState();
	} else {
		SaveRealDudeState();
	}
	if (critter != nullptr && critter != realDude.obj_dude) {
		SetCurrentDude(critter);
	}
}

fo::GameObject* PartyControl::RealDudeObject() {
	return realDude.obj_dude != nullptr
		? realDude.obj_dude
		: fo::var::obj_dude;
}

void PartyControl::init() {
	LoadGameHook::OnGameReset() += PartyControlReset;

	HookCall(0x454218, stat_pc_add_experience_hook); // call inside op_give_exp_points_hook
	HookCalls(pc_flag_toggle_hook, { 0x4124F1, 0x41279A });
}

}
