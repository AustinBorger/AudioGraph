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

CAudioGraph::CAudioGraph() : 
	m_RefCount(1),
	m_Playing(false)
{ }

CAudioGraph::~CAudioGraph() { }

HRESULT CAudioGraph::Initialize (
	IAudioGraphCallback* pAudioGraphCallback,
	CAudioGraphFile* pAudioGraphFile,
	std::string& Style
) {
	HRESULT hr = S_OK;

	m_Callback = pAudioGraphCallback;
	m_File = pAudioGraphFile;
	m_StyleString = Style;

	// Parse style string

	static const auto attribute = [&Style](LPCSTR attribute) {
		size_t off = Style.find(attribute, 0);
		size_t start = off + 4;
		size_t end = Style.find("\"", start);
		size_t length = end - start;
		return Style.substr(start, length);
	};

	m_ID = attribute("id");
	m_Type = attribute("type");
	m_Initial = attribute("initial");

	// id and initial must be defined, but type is optional.
	if (m_ID == "" || m_Initial == "") {
		return E_INVALIDARG;
	}

	return S_OK;
}

VOID CAudioGraph::CreateNode(std::string& Style) {
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

VOID CAudioGraph::CreateEdge(std::string& Style) {
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

VOID CAudioGraph::GetNodeByID(std::string& ID, CAudioGraphNode** ppNode) {
	try {
		*ppNode = m_NodeMap.at(ID);
	} catch (...) {
		*ppNode = nullptr;
	}
}

VOID CAudioGraph::Setup(IMFMediaType* pMediaType) {
	for (auto Node : m_NodeEnum) {
		Node->Setup(pMediaType);
	}

	m_CurrentNode = m_NodeMap[m_Initial];
	m_CurrentNode->Seek();
}

VOID CAudioGraph::Flush() {
	for (auto Node : m_NodeEnum) {
		Node->Flush();
	}

	m_CurrentNode.Release();
	m_CurrentNode = nullptr;
}

UINT CAudioGraph::Process(FLOAT* OutputBuffer, UINT BufferFrames) {
	UINT Written = 0;
	UINT TotalWritten = 0;
	bool done = false;

	while (BufferFrames > 0 && !done) {
		Written = m_CurrentNode->Process(OutputBuffer, BufferFrames);
		BufferFrames -= Written;
		OutputBuffer += Written * 2;
		TotalWritten += Written;

		// Node has finished playing
		if (BufferFrames > 0) {
			if (m_CurrentNode->IsTerminal() == FALSE) { // Move to the next node
				std::string TransitionString = m_Callback->OnTransition(this, m_CurrentNode);
				CComPtr<CAudioGraphEdge> TransitionEdge;
				m_CurrentNode->GetTransitionEdge(TransitionString, &TransitionEdge);

				if (TransitionEdge != nullptr) {
					m_CurrentNode.Release();
					TransitionEdge->GetTo((IAudioGraphNode**)(&m_CurrentNode));
				} else { // just replay the same node
					m_CurrentNode->Seek();
				}
			} else { // Node is a terminal, stop playing this graph.
				done = true;
			}
		}
	}

	return TotalWritten;
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

VOID CAudioGraph::GetCurrentNode(IAudioGraphNode** ppAudioGraphNode) {
	if (ppAudioGraphNode == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	*ppAudioGraphNode = m_CurrentNode;
}

VOID CAudioGraph::GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) {
	if (ppAudioGraphFile == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	*ppAudioGraphFile = m_File;
}