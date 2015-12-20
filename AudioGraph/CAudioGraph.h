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
#include <vector>
#include <map>

#include "AudioGraph.h"
#include "QueryInterface.h"
#include "CAudioGraphNode.h"
#include "CAudioGraphEdge.h"

class CAudioGraphFile;

class CAudioGraph : public IAudioGraph {
public:
	CAudioGraph();

	~CAudioGraph();

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

	//IAudioGraph methods

	/* Returns the ID of this particular graph. */
	LPCSTR STDMETHODCALLTYPE GetID() final {
		return m_ID.c_str();
	}

	/* Returns an arbitrary type string describing this graph. */
	LPCSTR STDMETHODCALLTYPE GetType() final {
		return m_Type.c_str();
	}

	/* Returns the style string of this graph. */
	LPCSTR STDMETHODCALLTYPE GetStyleString() final {
		return m_StyleString.c_str();
	}

	/* Returns the number of nodes associated with this particular graph. */
	UINT STDMETHODCALLTYPE GetNumNodes() final {
		return m_NodeEnum.size();
	}

	/* Retrieves an node associted with this graph by array index. */
	VOID STDMETHODCALLTYPE EnumNode(UINT NodeNum, IAudioGraphNode** ppNode) final;

	/* Retrieves a node based on a given node identifier. */
	VOID STDMETHODCALLTYPE GetNodeByID(LPCSTR ID, IAudioGraphNode** ppNode) final;

	/* Returns the number of edges associated with this particular graph. */
	UINT STDMETHODCALLTYPE GetNumEdges() final {
		return m_EdgeEnum.size();
	}

	/* Retrieves an edge associted with this graph by array index. */
	VOID STDMETHODCALLTYPE EnumEdge(UINT EdgeNum, IAudioGraphEdge** ppEdge) final;

	/* Retrieves an edge based on a given edge identifier. */
	VOID STDMETHODCALLTYPE GetEdgeByID(LPCSTR ID, IAudioGraphEdge** ppEdge) final;

	/* Retrieves the currently active node. */
	VOID STDMETHODCALLTYPE GetCurrentNode(IAudioGraphNode** ppAudioGraphNode) final;

	/* Retrieves the audio graph file that this graph is associated with, if there is one. */
	VOID STDMETHODCALLTYPE GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) final;

	//New methods

	HRESULT Initialize (
		IAudioGraphCallback* pAudioGraphCallback,
		CAudioGraphFile* pAudioGraphFile,
		std::string& Style
	);

	/* To be used by CAudioGraphFile when parsing. */
	VOID CreateNode(std::string& Style);

	/* To be used by CAudioGraphFile when parsing. */
	VOID CreateEdge(std::string& Style);

	/* To be used by CAudioGraphEdge. */
	VOID GetNodeByID(std::string& ID, CAudioGraphNode** ppNode);

	/* Used by CDXAudioWriteCallback. */
	bool IsPlaying() {
		return m_Playing;
	}

	/* Used by CDXAudioWriteCallback. */
	VOID SetPlaying(bool Playing) {
		m_Playing = Playing;
	}

	/* Prepares the graph for playback by creating stream readers. */
	VOID Setup(IMFMediaType* pMediaType);

	/* Closes all streams. */
	VOID Flush();

	/* Fetches a set of samples.  Returns the number of samples written.
	** If any value less than BufferFrames is returned, the graph has finished
	** playing. */
	UINT Process(FLOAT* OutputBuffer, UINT BufferFrames);

private:
	long m_RefCount;

	CComPtr<IAudioGraphCallback> m_Callback;
	CComPtr<CAudioGraphFile> m_File;
	CComPtr<CAudioGraphNode> m_CurrentNode;

	std::string m_ID;
	std::string m_Type;
	std::string m_Initial;
	std::string m_StyleString;
	bool m_Playing;

	std::vector<CComPtr<CAudioGraphNode>> m_NodeEnum;
	std::map<std::string, CComPtr<CAudioGraphNode>> m_NodeMap;
	std::vector<CComPtr<CAudioGraphEdge>> m_EdgeEnum;
	std::map<std::string, CComPtr<CAudioGraphEdge>> m_EdgeMap;

	//IUnknown methods

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IAudioGraph);
		QUERY_INTERFACE_CAST(IUnknown);
		QUERY_INTERFACE_FAIL();
	}
};