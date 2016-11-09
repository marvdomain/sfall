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

#include "Functions.h"
#include "Wrappers.h"

namespace Wrapper
{

// Fallout2.exe was compiled using WATCOM compiler, which uses Watcom register calling convention.
// In this convention, up to 4 arguments are passed via registers in this order: EAX, EDX, EBX, ECX.

#define _WRAP_WATCOM_CALL0(offs) \
	__asm call FuncOffs::offs

#define _WRAP_WATCOM_CALL1(offs, arg1) \
	__asm mov eax, arg1				   \
	__asm call FuncOffs::offs

#define _WRAP_WATCOM_CALL2(offs, arg1, arg2) \
	__asm mov edx, arg2				   \
	__asm mov eax, arg1				   \
	__asm call FuncOffs::offs

#define _WRAP_WATCOM_CALL3(offs, arg1, arg2, arg3) \
	__asm mov ebx, arg3				   \
	__asm mov edx, arg2				   \
	__asm mov eax, arg1				   \
	__asm call FuncOffs::offs

#define _WRAP_WATCOM_CALL4(offs, arg1, arg2, arg3, arg4) \
	__asm mov ecx, arg4				   \
	__asm mov ebx, arg3				   \
	__asm mov edx, arg2				   \
	__asm mov eax, arg1				   \
	__asm call FuncOffs::offs


// Returns the name of the critter
const char* __stdcall critter_name(TGameObj* critter) {
	_WRAP_WATCOM_CALL1(critter_name_, critter)
}

// Change the name of playable character
void critter_pc_set_name(const char* newName) {
	_WRAP_WATCOM_CALL1(critter_pc_set_name_, newName)
}

bool __stdcall db_access(const char* fileName) {
	_WRAP_WATCOM_CALL1(db_access_, fileName)
}

void __stdcall db_free_file_list(char* * *fileList, DWORD arg2) {
	_WRAP_WATCOM_CALL2(db_free_file_list_, fileList, arg2)
}

int __stdcall db_get_file_list(const char* searchMask, char* * *fileList, DWORD arg3, DWORD arg4) {
	_WRAP_WATCOM_CALL4(db_get_file_list_, searchMask, fileList, arg3, arg4)
}

// prints message to debug.log file
void __declspec(naked) debug_printf(const char* fmt, ...) {
	__asm jmp FuncOffs::debug_printf_
}

// Displays message in main UI console window
void display_print(const char* msg) {
	_WRAP_WATCOM_CALL1(display_print_, msg)
}

void executeProcedure(TProgram* sptr, int procNum) {
	_WRAP_WATCOM_CALL2(executeProcedure_, sptr, procNum)
}

// returns the name of current procedure by program pointer
const char* __stdcall findCurrentProc(TProgram* program) {
	_WRAP_WATCOM_CALL1(findCurrentProc_, program)
}

const char* _stdcall getmsg(DWORD fileAddr, sMessage* result, int messageId) {
	_WRAP_WATCOM_CALL3(getmsg_, fileAddr, result, messageId)
}

// redraws the main game interface windows (useful after changing some data like active hand, etc.)
void intface_redraw() {
	_WRAP_WATCOM_CALL0(intface_redraw_)
}

int __stdcall interpret(TProgram* program, int arg2) {
	_WRAP_WATCOM_CALL2(interpret_, program, arg2)
}

int __stdcall interpretFindProcedure(TProgram* scriptPtr, const char* procName) {
	_WRAP_WATCOM_CALL2(interpretFindProcedure_, scriptPtr, procName)
}

// pops value type from Data stack (must be followed by InterpretPopLong)
DWORD __stdcall interpretPopShort(TProgram* scriptPtr) {
	_WRAP_WATCOM_CALL1(interpretPopShort_, scriptPtr)
}

// pops value from Data stack (must be preceded by InterpretPopShort)
DWORD __stdcall interpretPopLong(TProgram* scriptPtr) {
	_WRAP_WATCOM_CALL1(interpretPopLong_, scriptPtr)
}

// pushes value to Data stack (must be followed by InterpretPushShort)
void __stdcall interpretPushLong(TProgram* scriptPtr, DWORD val) {
	_WRAP_WATCOM_CALL2(interpretPushLong_, scriptPtr, val)
}

// pushes value type to Data stack (must be preceded by InterpretPushLong)
void __stdcall interpretPushShort(TProgram* scriptPtr, DWORD valType) {
	_WRAP_WATCOM_CALL2(interpretPushShort_, scriptPtr, valType)
}

DWORD __stdcall interpretAddString(TProgram* scriptPtr, const char* strval) {
	_WRAP_WATCOM_CALL2(interpretAddString_, scriptPtr, strval)
}

const char* __stdcall interpretGetString(TProgram* scriptPtr, DWORD dataType, DWORD strId) {
	_WRAP_WATCOM_CALL3(interpretGetString_, scriptPtr, dataType, strId)
}

// prints scripting error in debug.log and stops current script execution by performing longjmp
// USE WITH CAUTION
void __declspec(naked) interpretError(const char* fmt, ...) {
	__asm jmp FuncOffs::interpretError_
}

int _stdcall isPartyMember(TGameObj* obj) {
	_WRAP_WATCOM_CALL1(isPartyMember_, obj)
}

int __stdcall item_get_type(TGameObj* item) {
	_WRAP_WATCOM_CALL1(item_get_type_, item)
}

int __stdcall item_m_dec_charges(TGameObj* item) {
	_WRAP_WATCOM_CALL1(item_m_dec_charges_, item) //Returns -1 if the item has no charges
}

TGameObj* __stdcall inven_pid_is_carried_ptr(TGameObj* invenObj, int pid) {
	_WRAP_WATCOM_CALL2(inven_pid_is_carried_ptr_, invenObj, pid)
}

// critter worn item (armor)
TGameObj* __stdcall inven_worn(TGameObj* critter) {
	_WRAP_WATCOM_CALL1(inven_worn_, critter)
}

// item in critter's left hand slot
TGameObj* __stdcall inven_left_hand(TGameObj* critter) {
	_WRAP_WATCOM_CALL1(inven_left_hand_, critter)
}

// item in critter's right hand slot
TGameObj* __stdcall inven_right_hand(TGameObj* critter) {
	_WRAP_WATCOM_CALL1(inven_right_hand_, critter)
}

TProgram* __stdcall loadProgram(const char* fileName) {
	_WRAP_WATCOM_CALL1(loadProgram_, fileName)
}

int __stdcall message_search(DWORD* file, sMessage* msg) {
	_WRAP_WATCOM_CALL2(message_search_, file, msg)
}

TGameObj* __stdcall obj_find_first_at_tile(int elevation, int tileNum) {
	_WRAP_WATCOM_CALL2(obj_find_first_at_tile_, elevation, tileNum)
}

TGameObj* __stdcall obj_find_next_at_tile() {
	_WRAP_WATCOM_CALL0(obj_find_next_at_tile_)
}

int _stdcall partyMemberGetCurLevel(TGameObj* obj) {
	_WRAP_WATCOM_CALL1(partyMemberGetCurLevel_, obj)
}

int _stdcall perk_level(TGameObj* critter, int perkId) {
	_WRAP_WATCOM_CALL2(perk_level, critter, perkId)
}

int proto_ptr(int pid, sProtoBase* *ptrPtr) {
	_WRAP_WATCOM_CALL2(proto_ptr_, pid, ptrPtr)
}

DWORD* __stdcall runProgram(TProgram* progPtr) {
	_WRAP_WATCOM_CALL1(runProgram_, progPtr)
}

// Saves pointer to script object into scriptPtr using scriptID. 
// Returns 0 on success, -1 on failure.
int __stdcall scr_ptr(int scriptId, TScript** scriptPtr) {
	_WRAP_WATCOM_CALL2(scr_ptr_, scriptId, scriptPtr)
}

void skill_get_tags(int* result, int num) {
	_WRAP_WATCOM_CALL2(skill_get_tags_, result, num)
}

void skill_set_tags(int* tags, int num) {
	_WRAP_WATCOM_CALL2(skill_set_tags_, tags, num)
}


int __stdcall stat_level(TGameObj* critter, int statId) {
	_WRAP_WATCOM_CALL2(stat_level_, critter, statId)
}

}
