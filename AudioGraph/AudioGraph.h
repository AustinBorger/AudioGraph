/*
** Copyright (C) 2015 Austin Borger <aaborger@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
** API documentation is available here:
**		https://github.com/AustinBorger/AudioGraph
*/

#pragma once

#include <comdef.h>

struct __declspec(uuid("b7fa0e54-41d7-4161-81d6-3036900cfc80")) IAudioGraphCallback : public IUnknown {

};

struct __declspec(uuid("b1f2bb1c-f1da-4f0a-ba3a-b7dbe2a7c824")) IAudioGraph : public IUnknown {

};

/* IAudioGraphFactory provides several APIs to create audio graphs.  It also defines the connection
** between the application and the Windows audio service. */
struct __declspec(uuid("b824c4eb-5a50-4706-8c14-bcc2f207d6ee")) IAudioGraphFactory : public IUnknown {
	virtual VOID STDMETHODCALLTYPE CreateAudioGraphFromFile(LPCWSTR Filename, IAudioGraph** ppAudioGraph) PURE;

	virtual VOID STDMETHODCALLTYPE CreateBlankAudioGraph(IAudioGraph** ppAudioGraph) PURE;
};

#ifndef _AUDIO_GRAPH_EXPORT_TAG
	#ifdef _AUDIO_GRAPH_DLL_PROJECT
		#define _AUDIO_GRAPH_EXPORT_TAG __declspec(dllexport)
	#else
		#define _AUDIO_GRAPH_EXPORT_TAG __declspec(dllimport)
	#endif
#endif

extern "C" HRESULT _AUDIO_GRAPH_EXPORT_TAG AudioGraphCreateFactory (
	IAudioGraphCallback* pAudioGraphCallback,
	IAudioGraphFactory** ppAudioGraphFactory
);