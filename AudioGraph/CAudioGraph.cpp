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

#include "CAudioGraph.h"
#include "CAudioGraphFile.h"

#include <algorithm>

#define FILENAME L"CAudioGraph.cpp"

CAudioGraph::CAudioGraph() : m_RefCount(1) { }

CAudioGraph::~CAudioGraph() { }

HRESULT CAudioGraph::Initialize(IAudioGraphCallback* pAudioGraphCallback, LPCSTR Style, CAudioGraphFile* pAudioGraphFile) {
	HRESULT hr = S_OK;

	m_Callback = pAudioGraphCallback;

	m_File = pAudioGraphFile;

	// Parse style string

	m_StyleString = Style;

	return S_OK;
}

VOID CAudioGraph::CreateNode(LPCSTR Style) {
	HRESULT hr = S_OK;

	CComPtr<CAudioGraphNode> Node = new CAudioGraphNode();

	hr = Node->Initialize (
		m_Callback,
		m_File,
		this,
		Style
	);

	if (SUCCEEDED(hr)) {
		m_NodeEnum.push_back(Node);
		m_NodeMap[Node->GetID()] = Node;
	}
}

VOID CAudioGraph::CreateEdge(LPCSTR Style) {
	HRESULT hr = S_OK;

	CComPtr<CAudioGraphEdge> Edge = new CAudioGraphEdge();

	hr = Edge->Initialize (
		m_Callback,
		m_File,
		this,
		Style
	);

	if (SUCCEEDED(hr)) {
		m_EdgeEnum.push_back(Edge);
		m_EdgeMap[Edge->GetID()] = Edge;
	}
}

VOID CAudioGraph::EnumNode(UINT NodeNum, IAudioGraphNode** ppNode) {
	if (ppNode == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	try {
		*ppNode = m_NodeEnum.at(NodeNum);
	} catch (...) {
		*ppNode = nullptr;
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_INVALIDARG);
		return;
	}
}

VOID CAudioGraph::GetNodeByID(LPCSTR ID, IAudioGraphNode** ppNode) {
	if (ppNode == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	std::string stringID = ID;

	try {
		*ppNode = m_NodeMap.at(stringID);
	} catch (...) {
		*ppNode = nullptr;
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_INVALIDARG);
		return;
	}
}

VOID CAudioGraph::EnumEdge(UINT EdgeNum, IAudioGraphEdge** ppEdge) {
	if (ppEdge == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	try {
		*ppEdge = m_EdgeEnum.at(EdgeNum);
	} catch (...) {
		*ppEdge = nullptr;
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_INVALIDARG);
		return;
	}
}

VOID CAudioGraph::GetEdgeByID(LPCSTR ID, IAudioGraphEdge** ppEdge) {
	if (ppEdge == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	std::string stringID = ID;

	try {
		*ppEdge = m_EdgeMap.at(stringID);
	} catch (...) {
		*ppEdge = nullptr;
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_INVALIDARG);
		return;
	}
}

VOID CAudioGraph::GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) {
	if (ppAudioGraphFile == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	*ppAudioGraphFile = m_File;
}