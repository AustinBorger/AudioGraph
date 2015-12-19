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
class CAudioGraphEdge;

class CAudioGraphNode : public IAudioGraphNode {
public:
	CAudioGraphNode();

	~CAudioGraphNode();

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

private:
	long m_RefCount;

	CComPtr<IAudioGraphCallback> m_Callback;
	CComPtr<CAudioGraph> m_Graph;
	CComPtr<CAudioGraphFile> m_File;

	std::string m_ID;
	std::string m_StyleString;
	std::wstring m_AudioFilename;
	FLOAT m_Gain;
	UINT m_SampleOffset;
	UINT m_SampleDuration;

	//IUnknown methods

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IAudioGraphNode);
		QUERY_INTERFACE_CAST(IUnknown);
		QUERY_INTERFACE_FAIL();
	}

	//IAudioGraphNode methods

	/* Returns the ID of this particular node. */
	LPCSTR STDMETHODCALLTYPE GetID() final {
		return m_ID.c_str();
	}

	/* Returns the name of the audio file that this node is streamed from. */
	LPCWSTR STDMETHODCALLTYPE GetAudioFilename() final {
		return m_AudioFilename.c_str();
	}

	/* Returns the number of edges extending from this particular node. */
	UINT STDMETHODCALLTYPE GetNumEdges() final;

	/* Retrieves an edge extending from this node by given array index. */
	VOID STDMETHODCALLTYPE EnumEdge(LONG EdgeNum, IAudioGraphEdge** ppEdge) final;

	/* Retrieves an edge based on a given identifier. */
	VOID STDMETHODCALLTYPE GetEdgeByID(LPCSTR ID, IAudioGraphEdge** ppEdge) final;

	/* Returns the stretch coefficient associated with this particular node. */
	FLOAT STDMETHODCALLTYPE GetStretch() final;

	/* Returns the gain coefficient associated with this particular node. */
	FLOAT STDMETHODCALLTYPE GetGain() final {
		return m_Gain;
	}

	/* Returns the offset this node has from the start of the PCM audio data in the
	** associated file, in samples. */
	UINT STDMETHODCALLTYPE GetSampleOffset() final {
		return m_SampleOffset;
	}

	/* Returns the duration this node will play for, in samples. */
	UINT STDMETHODCALLTYPE GetSampleDuration() final {
		return m_SampleDuration;
	}

	/* Returns the offset this node has from the start of the PCM audio data in the
	** associated file, in seconds. */
	FLOAT STDMETHODCALLTYPE GetTimeOffset() final;

	/* Returns the duration this node will play for, in seconds. */
	FLOAT STDMETHODCALLTYPE GetTimeDuration() final;

	/* Returns this node's formatted style string, which was used to create it. */
	LPCSTR STDMETHODCALLTYPE GetStyleString() final {
		return m_StyleString.c_str();
	}

	/* Retrieves the audio graph that this node is attatched to. */
	VOID STDMETHODCALLTYPE GetGraph(IAudioGraph** ppAudioGraph) final;

	/* Retrieves the audio graph file that this node is associated with, if there is one. */
	VOID STDMETHODCALLTYPE GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) final;
};