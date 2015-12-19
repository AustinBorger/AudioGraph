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

#include "AudioGraph.h"
#include "QueryInterface.h"

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

	//New methods

	HRESULT Initialize(IAudioGraphCallback* pAudioGraphCallback, LPCSTR Style);

private:
	long m_RefCount;

	//IUnknown methods

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IAudioGraph);
		QUERY_INTERFACE_CAST(IUnknown);
		QUERY_INTERFACE_FAIL();
	}

	//IAudioGraph methods

	/* Returns the ID of this particular graph. */
	LPCSTR STDMETHODCALLTYPE GetID() final;

	/* Returns an arbitrary type string describing this graph. */
	LPCSTR STDMETHODCALLTYPE GetType() final;

	/* Returns the style string of this graph. */
	LPCSTR STDMETHODCALLTYPE GetStyleString() final;

	/* Returns the gain of this graph. */
	FLOAT STDMETHODCALLTYPE GetGain() final;

	/* Creates a node that will be associated with this graph.  Style is a string listing the
	** attributes of the node, in XML syntax. */
	VOID STDMETHODCALLTYPE CreateNode(LPCSTR Style, IAudioGraphNode** ppNode) final;

	/* Creates an edge that will be associated with this graph.  Style is a string listing the
	** attributes of the edge, in XML syntax. */
	VOID STDMETHODCALLTYPE CreateEdge(LPCSTR Style, IAudioGraphEdge** ppEdge) final;

	/* Removes a node associated with this graph.  Note that the application is still
	** expected to release any references to the node. */
	VOID STDMETHODCALLTYPE RemoveNode(IAudioGraphNode* pNode) final;

	/* Removes an edge associated with this graph.  Note that the application is still
	** expected to release any references to the edge. */
	VOID STDMETHODCALLTYPE RemoveEdge(IAudioGraphEdge* pEdge) final;

	/* Retrieves a node based on a given node identifier. */
	VOID STDMETHODCALLTYPE GetNodeByID(LPCSTR ID, IAudioGraphNode** ppNode) final;

	/* Retrieves an edge based on a given edge identifier. */
	VOID STDMETHODCALLTYPE GetEdgeByID(LPCSTR ID, IAudioGraphEdge** ppEdge) final;

	/* Gets the mix gain of this graph.  The mix gain is used for mixing/fading between different
	** audio graphs, where multiple graphs are playing concurrently. */
	FLOAT STDMETHODCALLTYPE GetMixVolume() final;

	/* Sets the mix gain of this graph.  The mix gain is used for mixing/fading between different
	** audio graphs, where multiple graphs are playing concurrently. */
	VOID STDMETHODCALLTYPE SetMixVolume(FLOAT Volume) final;
};