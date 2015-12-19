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

#include "CAudioGraphNode.h"
#include "CAudioGraph.h"
#include "CAudioGraphFile.h"
#include "CAudioGraphEdge.h"

#include <algorithm>

#define FILENAME L"CAudioGraphNode.cpp"

CAudioGraphNode::CAudioGraphNode() : m_RefCount(1) { }

CAudioGraphNode::~CAudioGraphNode() { }

HRESULT CAudioGraphNode::Initialize (
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
		size_t start = off + 4;
		size_t end = Style.find("\"", start);
		size_t length = end - start;
		return Style.substr(start, length);
	};

	m_ID = attribute("id");
	m_AudioFilename = attribute("filename");
	std::string sampleOffsetString = attribute("offset");
	std::string sampleDurationString = attribute("duration");

	// All attributes must be defined.
	if (m_ID == "" || m_AudioFilename == "" || sampleOffsetString == "" || sampleDurationString == "") {
		return E_INVALIDARG;
	}

	try {
		m_SampleOffset = std::atoi(sampleOffsetString.c_str());
		m_SampleDuration = std::atoi(sampleDurationString.c_str());
	} catch (...) {
		return E_INVALIDARG;
	}

	return S_OK;
}

VOID CAudioGraphNode::EnumEdge(UINT EdgeNum, IAudioGraphEdge** ppEdge) {
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

VOID CAudioGraphNode::GetEdgeByID(LPCSTR ID, IAudioGraphEdge** ppEdge) {
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

VOID CAudioGraphNode::GetGraph(IAudioGraph** ppAudioGraph) {
	if (ppAudioGraph == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	*ppAudioGraph = m_Graph;
}

VOID CAudioGraphNode::GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) {
	if (ppAudioGraphFile == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	*ppAudioGraphFile = m_File;
}