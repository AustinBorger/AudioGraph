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

#include "CAudioGraphEdge.h"
#include "CAudioGraph.h"
#include "CAudioGraphFile.h"
#include "CAudioGraphNode.h"

#define FILENAME L"CAudioGraphEdge.cpp"

CAudioGraphEdge::CAudioGraphEdge() : m_RefCount(1) { }

CAudioGraphEdge::~CAudioGraphEdge() { }

HRESULT CAudioGraphEdge::Initialize (
	IAudioGraphCallback* pCallback,
	CAudioGraphFile* pFile,
	CAudioGraph* pGraph,
	std::string& Style
) {
	m_Callback = pCallback;
	m_File = pFile;
	m_Graph = pGraph;
	m_StyleString = Style;

	// Parse style string

	static const auto attribute = [&Style](LPCSTR attribute) {
		size_t off = Style.find(attribute, 0);
		size_t start = off + 4 + strlen(attribute);
		size_t end = Style.find("\"", start);
		size_t length = end - start;
		return Style.substr(start, length);
	};

	m_ID = attribute("id");
	m_Trigger = attribute("trigger");
	std::string toString = attribute("to");
	std::string fromString = attribute("from");
	m_Graph->GetNodeByID(toString, &m_To);
	m_Graph->GetNodeByID(fromString, &m_From);

	// All attributes must be defined.
	if (m_To == nullptr || m_From == nullptr || m_Trigger == "" || m_ID == "") {
		return E_INVALIDARG;
	}

	return S_OK;
}

VOID CAudioGraphEdge::GetFrom(IAudioGraphNode** ppNode) {
	if (ppNode == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	*ppNode = m_From;
}

VOID CAudioGraphEdge::GetTo(IAudioGraphNode** ppNode) {
	if (ppNode == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	*ppNode = m_To;
}

VOID CAudioGraphEdge::GetAudioGraph(IAudioGraph** ppAudioGraph) {
	if (ppAudioGraph == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	*ppAudioGraph = m_Graph;
}

VOID CAudioGraphEdge::GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) {
	if (ppAudioGraphFile == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	*ppAudioGraphFile = m_File;
}