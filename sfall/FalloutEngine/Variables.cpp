/*
* sfall
* Copyright (C) 2008-2016 The sfall team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Variables.h"


// Pointers to engine global variables
namespace VarPtr
{

// defines const pointer to a variable (pointer is constant, but value can be changed)
#define VARDEFN(type, name, addrConst)	\
	type* const name = reinterpret_cast<type*>(VARPTR##addrConst);


VARDEFN(long, pc_trait, _pc_trait) // 2 of them
VARDEFN(DWORD, aiInfoList, _aiInfoList)
VARDEFN(DWORD, ambient_light, _ambient_light)
VARDEFN(sArt, art, _art)
VARDEFN(DWORD, art_name, _art_name)
VARDEFN(DWORD, art_vault_guy_num, _art_vault_guy_num)
VARDEFN(DWORD, art_vault_person_nums, _art_vault_person_nums)
VARDEFN(DWORD, bckgnd, _bckgnd)
VARDEFN(DWORD, black_palette, _black_palette)
VARDEFN(DWORD, bottom_line, _bottom_line)
VARDEFN(DWORD, btable, _btable)
VARDEFN(DWORD, btncnt, _btncnt)
VARDEFN(DWORD, CarCurrArea, _CarCurrArea)
VARDEFN(DWORD, cmap, _cmap)
VARDEFN(DWORD, colorTable, _colorTable)
VARDEFN(DWORD, combat_free_move, _combat_free_move)
VARDEFN(DWORD, combat_list, _combat_list)
VARDEFN(DWORD, combat_state, _combat_state)
VARDEFN(DWORD, combat_turn_running, _combat_turn_running)
VARDEFN(DWORD, combatNumTurns, _combatNumTurns)
VARDEFN(CritStruct, crit_succ_eff, _crit_succ_eff)  // static array
VARDEFN(DWORD, critter_db_handle, _critter_db_handle)
VARDEFN(DWORD, critterClearObj, _critterClearObj)
VARDEFN(DWORD, crnt_func, _crnt_func)
VARDEFN(DWORD, curr_font_num, _curr_font_num)
VARDEFN(DWORD, curr_pc_stat, _curr_pc_stat)
VARDEFN(DWORD, curr_stack, _curr_stack)
VARDEFN(DWORD, cursor_line, _cursor_line)
VARDEFN(DWORD, dialog_target, _dialog_target)
VARDEFN(DWORD, dialog_target_is_party, _dialog_target_is_party)
VARDEFN(DWORD, drugInfoList, _drugInfoList)
VARDEFN(DWORD, edit_win, _edit_win)
VARDEFN(DWORD, Educated, _Educated)
VARDEFN(DWORD, Experience_, _Experience_)
VARDEFN(DWORD, fallout_game_time, _fallout_game_time)
VARDEFN(DWORD, flptr, _flptr)
VARDEFN(DWORD, folder_card_desc, _folder_card_desc)
VARDEFN(DWORD, folder_card_fid, _folder_card_fid)
VARDEFN(DWORD, folder_card_title, _folder_card_title)
VARDEFN(DWORD, folder_card_title2, _folder_card_title2)
VARDEFN(DWORD, frame_time, _frame_time)
VARDEFN(char, free_perk, _free_perk)
VARDEFN(int*, game_global_vars, _game_global_vars)
VARDEFN(DWORD, game_user_wants_to_quit, _game_user_wants_to_quit)
VARDEFN(DWORD, gcsd, _gcsd)
VARDEFN(DWORD, gdBarterMod, _gdBarterMod)
VARDEFN(DWORD, gdNumOptions, _gdNumOptions)
VARDEFN(DWORD, gIsSteal, _gIsSteal)
VARDEFN(DWORD, glblmode, _glblmode)
VARDEFN(DWORD, gmouse_current_cursor, _gmouse_current_cursor)
VARDEFN(DWORD, gmovie_played_list, _gmovie_played_list)
VARDEFN(BYTE, GreenColor, _GreenColor)
VARDEFN(DWORD, gsound_initialized, _gsound_initialized)
VARDEFN(DWORD, hit_location_penalty, _hit_location_penalty)
VARDEFN(DWORD, holo_flag, _holo_flag)
VARDEFN(DWORD, holopages, _holopages)
VARDEFN(DWORD, hot_line_count, _hot_line_count)
VARDEFN(DWORD, i_fid, _i_fid)
VARDEFN(DWORD, i_lhand, _i_lhand)
VARDEFN(DWORD, i_rhand, _i_rhand)
VARDEFN(DWORD, i_wid, _i_wid)
VARDEFN(DWORD, i_worn, _i_worn)
VARDEFN(DWORD, In_WorldMap, _In_WorldMap)
VARDEFN(DWORD, info_line, _info_line)
VARDEFN(DWORD, interfaceWindow, _interfaceWindow)
VARDEFN(DWORD, intfaceEnabled, _intfaceEnabled)
VARDEFN(DWORD, intotal, _intotal)
VARDEFN(TGameObj*, inven_dude, _inven_dude)
VARDEFN(DWORD, inven_pid, _inven_pid)
VARDEFN(DWORD, inven_scroll_dn_bid, _inven_scroll_dn_bid)
VARDEFN(DWORD, inven_scroll_up_bid, _inven_scroll_up_bid)
VARDEFN(DWORD, inventry_message_file, _inventry_message_file)
VARDEFN(DWORD, itemButtonItems, _itemButtonItems)
VARDEFN(DWORD, itemCurrentItem, _itemCurrentItem) // 0 - left, 1 - right
VARDEFN(DWORD, kb_lock_flags, _kb_lock_flags)
VARDEFN(DWORD, last_buttons, _last_buttons)
VARDEFN(DWORD, last_button_winID, _last_button_winID)
VARDEFN(DWORD, last_level, _last_level)
VARDEFN(DWORD, Level_, _Level_)
VARDEFN(DWORD, Lifegiver, _Lifegiver)
VARDEFN(DWORD, list_com, _list_com)
VARDEFN(DWORD, list_total, _list_total)
VARDEFN(DWORD, loadingGame, _loadingGame)
VARDEFN(DWORD, LSData, _LSData)
VARDEFN(DWORD, lsgwin, _lsgwin)
VARDEFN(DWORD, main_ctd, _main_ctd)
VARDEFN(DWORD, main_window, _main_window)
VARDEFN(DWORD, map_elevation, _map_elevation)
VARDEFN(int*,  map_global_vars, _map_global_vars)
VARDEFN(DWORD, master_db_handle, _master_db_handle)
VARDEFN(DWORD, max, _max)
VARDEFN(DWORD, maxScriptNum, _maxScriptNum)
VARDEFN(DWORD, Meet_Frank_Horrigan, _Meet_Frank_Horrigan)
VARDEFN(DWORD, mouse_hotx, _mouse_hotx)
VARDEFN(DWORD, mouse_hoty, _mouse_hoty)
VARDEFN(DWORD, mouse_is_hidden, _mouse_is_hidden)
VARDEFN(DWORD, mouse_x_, _mouse_x_)
VARDEFN(DWORD, mouse_y, _mouse_y)
VARDEFN(DWORD, mouse_y_, _mouse_y_)
VARDEFN(DWORD, Mutate_, _Mutate_)
VARDEFN(DWORD, name_color, _name_color)
VARDEFN(DWORD, name_font, _name_font)
VARDEFN(DWORD, name_sort_list, _name_sort_list)
VARDEFN(int, num_game_global_vars, _num_game_global_vars)
VARDEFN(int, num_map_global_vars, _num_map_global_vars)
VARDEFN(TGameObj*, obj_dude, _obj_dude)
VARDEFN(DWORD, objectTable, _objectTable)
VARDEFN(DWORD, objItemOutlineState, _objItemOutlineState)
VARDEFN(DWORD, optionRect, _optionRect)
VARDEFN(DWORD, outlined_object, _outlined_object)
VARDEFN(DWORD, partyMemberAIOptions, _partyMemberAIOptions)
VARDEFN(DWORD, partyMemberCount, _partyMemberCount)
VARDEFN(DWORD, partyMemberLevelUpInfoList, _partyMemberLevelUpInfoList)
VARDEFN(DWORD*, partyMemberList, _partyMemberList) // each struct - 4 integers, first integer - objPtr
VARDEFN(DWORD, partyMemberMaxCount, _partyMemberMaxCount)
VARDEFN(DWORD, partyMemberPidList, _partyMemberPidList)
VARDEFN(char*, patches, _patches)
VARDEFN(sPath*, paths, _paths)  // array
VARDEFN(CritStruct, pc_crit_succ_eff, _pc_crit_succ_eff)  // 
VARDEFN(DWORD, pc_kill_counts, _pc_kill_counts)
VARDEFN(char, pc_name, _pc_name)
VARDEFN(DWORD, pc_proto, _pc_proto)
VARDEFN(DWORD, perk_data, _perk_data)
VARDEFN(int*, perkLevelDataList, _perkLevelDataList)
VARDEFN(DWORD, pip_win, _pip_win)
VARDEFN(DWORD, pipboy_message_file, _pipboy_message_file)
VARDEFN(DWORD, pipmesg, _pipmesg)
VARDEFN(DWORD, preload_list_index, _preload_list_index)
VARDEFN(const char*, procTableStrs, _procTableStrs)  // table of procId (from define.h) => procName map
VARDEFN(DWORD, proto_main_msg_file, _proto_main_msg_file)
VARDEFN(DWORD, ptable, _ptable)
VARDEFN(DWORD, pud, _pud)
VARDEFN(DWORD, queue, _queue)
VARDEFN(DWORD, quick_done, _quick_done)
VARDEFN(DWORD, read_callback, _read_callback)
VARDEFN(BYTE, RedColor, _RedColor)
VARDEFN(DWORD, retvals, _retvals)
VARDEFN(DWORD, scr_size, _scr_size)
VARDEFN(DWORD, scriptListInfo, _scriptListInfo)
VARDEFN(SkillInfo, skill_data, _skill_data)
VARDEFN(DWORD, slot_cursor, _slot_cursor)
VARDEFN(DWORD, sneak_working, _sneak_working) // DWORD var 
VARDEFN(DWORD, square, _square)
VARDEFN(DWORD*, squares, _squares)
VARDEFN(DWORD, stack, _stack)
VARDEFN(DWORD, stack_offset, _stack_offset)
VARDEFN(DWORD, stat_data, _stat_data)
VARDEFN(DWORD, stat_flag, _stat_flag)
VARDEFN(DWORD, Tag_, _Tag_)
VARDEFN(DWORD, tag_skill, _tag_skill)
VARDEFN(DWORD, target_curr_stack, _target_curr_stack)
VARDEFN(DWORD, target_pud, _target_pud)
VARDEFN(DWORD, target_stack, _target_stack)
VARDEFN(DWORD, target_stack_offset, _target_stack_offset)
VARDEFN(DWORD, target_str, _target_str)
VARDEFN(DWORD, target_xpos, _target_xpos)
VARDEFN(DWORD, target_ypos, _target_ypos)
VARDEFN(DWORD, text_char_width, _text_char_width)
VARDEFN(DWORD, text_height, _text_height)
VARDEFN(DWORD, text_max, _text_max)
VARDEFN(DWORD, text_mono_width, _text_mono_width)
VARDEFN(DWORD, text_spacing, _text_spacing)
VARDEFN(DWORD, text_to_buf, _text_to_buf)
VARDEFN(DWORD, text_width, _text_width)
VARDEFN(DWORD, title_color, _title_color)
VARDEFN(DWORD, title_font, _title_font)
VARDEFN(DWORD, trait_data, _trait_data)
VARDEFN(DWORD, view_page, _view_page)
VARDEFN(DWORD, wd_obj, _wd_obj)
VARDEFN(DWORD, wmAreaInfoList, _wmAreaInfoList)
VARDEFN(DWORD, wmLastRndTime, _wmLastRndTime)
VARDEFN(DWORD, wmWorldOffsetX, _wmWorldOffsetX)
VARDEFN(DWORD, wmWorldOffsetY, _wmWorldOffsetY)
VARDEFN(DWORD, world_xpos, _world_xpos)
VARDEFN(DWORD, world_ypos, _world_ypos)
VARDEFN(DWORD, WorldMapCurrArea, _WorldMapCurrArea)
VARDEFN(BYTE, YellowColor, _YellowColor)

}