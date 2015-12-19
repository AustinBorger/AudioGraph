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
**		https://github.com/AustinBorger/DXAudio
*/

#pragma once

#include <comdef.h>
#include <atlbase.h>
#include <Windows.h>
#include <string>

#include "AudioGraph.h"
#include "QueryInterface.h"

class CAudioGraph;
class CAudioGraphFile;
class CAudioGraphNode;

class CAudioGraphEdge : public IAudioGraphEdge {
public:
	CAudioGraphEdge();

	~CAudioGraphEdge();

	//IUnknown methods

	ULONG STDMETHODCALLTYPE AddRef() {
		return ++m_RefCount;
	}

	ULONG STDMETHODCALLTYPE Release() {
		m_RefCount--;

		if (m_RefCount <= 0) {
			delete this;
			return 0;
		}

		return m_RefCount;
	}

	HRESULT Initialize (
		IAudioGraphCallback* pCallback,
		CAudioGraphFile* pFile,
		CAudioGraph* pGraph,
		CAudioGraphNode* pFrom,
		CAudioGraphNode* pTo,
		LPCSTR Style
	);

private:
	long m_RefCount;

	CComPtr<CAudioGraph> m_Graph;
	CComPtr<CAudioGraphFile> m_File;
	CComPtr<CAudioGraphNode> m_From;
	CComPtr<CAudioGraphNode> m_To;
	CComPtr<IAudioGraphCallback> m_Callback;

	std::string m_ID;
	std::string m_Trigger;
	std::string m_StyleString;

	//IUnknown methods

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IAudioGraphEdge);
		QUERY_INTERFACE_CAST(IUnknown);
		QUERY_INTERFACE_FAIL();
	}

	//IAudioGraphEdge methods

	/* Returns the ID of this particular edge. */
	LPCSTR STDMETHODCALLTYPE GetID() final {
		return m_ID.c_str();
	}

	/* Retrieves the source node of this particular edge. */
	VOID STDMETHODCALLTYPE GetFrom(IAudioGraphNode** ppNode) final;

	/* Retrieves the destination node of this particular edge. */
	VOID STDMETHODCALLTYPE GetTo(IAudioGraphNode** ppNode) final;

	/* Returns the trigger string associated with this edge. */
	LPCSTR STDMETHODCALLTYPE GetTrigger() final {
		return m_Trigger.c_str();
	}

	/* Returns this edge's formatted style string, which was used to create it. */
	LPCSTR STDMETHODCALLTYPE GetStyleString() final {
		return m_StyleString.c_str();
	}

	/* Retrieves the audio graph that this edge is attached to. */
	VOID STDMETHODCALLTYPE GetAudioGraph(IAudioGraph** ppAudioGraph) final;

	/* Retrieves the audio graph file that this edge is associated with, if there is one. */
	VOID STDMETHODCALLTYPE GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) final;
};