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

#include <string>

#include "CAudioGraphFile.h"
#include "CAudioGraph.h"
#include "rapidxml.hpp"

#define FILENAME L"CAudioGraphFile.cpp"

CAudioGraphFile::CAudioGraphFile() : m_RefCount(1) { }

CAudioGraphFile::~CAudioGraphFile() { }

HRESULT CAudioGraphFile::Initialize(IAudioGraphCallback* pAudioGraphCallback, LPCWSTR Filename) {
	m_Callback = pAudioGraphCallback;

	m_Filename = Filename;

	return S_OK;
}

UINT CAudioGraphFile::GetNumGraphs() {
	return m_GraphEnum.size();
}

VOID CAudioGraphFile::EnumGraph(UINT GraphNum, IAudioGraph** ppAudioGraph) {
	if (ppAudioGraph == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	try {
		*ppAudioGraph = m_GraphEnum.at(GraphNum);
	} catch (...) {
		*ppAudioGraph = nullptr;
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_INVALIDARG);
		return;
	}
}

VOID CAudioGraphFile::GetGraphByID(LPCWSTR ID, IAudioGraph** ppAudioGraph) {
	if (ppAudioGraph == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	std::wstring stringID = ID;

	try {
		*ppAudioGraph = m_GraphMap.at(stringID);
	} catch (...) {
		*ppAudioGraph = nullptr;
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_INVALIDARG);
		return;
	}
}

LPCWSTR CAudioGraphFile::GetFilename() {
	return m_Filename.c_str();
}

VOID CAudioGraphFile::AppendGraph(IAudioGraph* pAudioGraph) {
	CComPtr<IAudioGraph> l_AudioGraph = pAudioGraph;

	m_GraphEnum.push_back(l_AudioGraph);
	
	std::wstring stringID = l_AudioGraph->GetID();

	m_GraphMap[stringID] = l_AudioGraph;
}

VOID CAudioGraphFile::Parse() {
	HRESULT hr = S_OK;

	using namespace rapidxml;

	// Source: http://stackoverflow.com/questions/18398167/how-to-copy-a-txt-file-to-a-char-array-in-c

	FILE *f = _wfopen(m_Filename.c_str(), L"rb");

	if (f == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND));
		return;
	}

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* content = reinterpret_cast<char*>(malloc(fsize + 1));
	fread(content, fsize, 1, f);
	fclose(f);

	content[fsize] = 0;

	// XML Parsing using rapidxml

	xml_document<> document;
	document.parse<0>(content);

	xml_node<>* root_node = document.first_node("AudioGraph");

	for (xml_node<>* graph_node = root_node->first_node("Graph"); graph_node; graph_node = graph_node->next_sibling()) {
		std::string style_string;

		if (strcmp(graph_node->first_attribute("id")->value(), "") != 0) {
			style_string += "id = ";
			style_string += graph_node->first_attribute("id")->value();
			style_string += " ";
		}

		if (strcmp(graph_node->first_attribute("gain")->value(), "") != 0) {
			style_string += "gain = ";
			style_string += graph_node->first_attribute("gain")->value();
			style_string += " ";
		}

		CComPtr<CAudioGraph> Graph = new CAudioGraph();

		hr = Graph->Initialize(m_Callback, style_string.c_str());

		if (SUCCEEDED(hr)) {
			// Add graph to this file
			CComPtr<IAudioGraph> I_Graph = Graph;
			m_GraphEnum.push_back(I_Graph);
			m_GraphMap[I_Graph->GetID()];
		}

		for (xml_node<>* vertex_node = graph_node->first_node("Node"); vertex_node; vertex_node = vertex_node->next_sibling()) {

		}

		for (xml_node<>* edge_node = graph_node->first_node("Edge"); edge_node; edge_node = edge_node->next_sibling()) {

		}
	}

	free(content);
}

VOID CAudioGraphFile::Save() {

}