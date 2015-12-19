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
#include "CAudioGraphNode.h"
#include "CAudioGraphEdge.h"
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

VOID CAudioGraphFile::GetGraphByID(LPCSTR ID, IAudioGraph** ppAudioGraph) {
	if (ppAudioGraph == nullptr) {
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_POINTER);
		return;
	}

	std::string stringID = ID;

	try {
		*ppAudioGraph = m_GraphMap.at(stringID);
	} catch (...) {
		*ppAudioGraph = nullptr;
		m_Callback->OnObjectFailure(FILENAME, __LINE__, E_INVALIDARG);
		return;
	}
}

VOID CAudioGraphFile::Parse() {
	HRESULT hr = S_OK;

	using namespace rapidxml;

	// Source: http://stackoverflow.com/questions/18398167/how-to-copy-a-txt-file-to-a-char-array-in-c

	FILE *f = nullptr; 

	if (_wfopen_s(&f, m_Filename.c_str(), L"rb") != 0) {
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
	std::string style_string;

	static const auto attribute = [&style_string](xml_node<>* node, LPCSTR attribute) {
		if (strcmp(node->first_attribute(attribute)->value(), "") != 0) {
			if (!style_string.empty()) {
				style_string += " ";
			}

			style_string += attribute;
			style_string += " = \"";
			style_string += node->first_attribute(attribute)->value();
			style_string += "\"";
		}
	};

	for (xml_node<>* graph_node = root_node->first_node("Graph"); graph_node; graph_node = graph_node->next_sibling()) {
		style_string = "";

		// Graph attributes: id, type
		attribute(graph_node, "id");
		attribute(graph_node, "type");

		CComPtr<CAudioGraph> Graph = new CAudioGraph();

		hr = Graph->Initialize(m_Callback, style_string.c_str(), this);

		if (SUCCEEDED(hr)) {
			// Add graph to this file
			CComPtr<IAudioGraph> I_Graph = Graph;
			m_GraphEnum.push_back(I_Graph);
			m_GraphMap[I_Graph->GetID()] = I_Graph;
		} else continue;

		for (xml_node<>* vertex_node = graph_node->first_node("Node"); vertex_node; vertex_node = vertex_node->next_sibling()) {
			style_string = "";

			// Node attributes: id, filename, offset, duration
			attribute(vertex_node, "id");
			attribute(vertex_node, "filename");
			attribute(vertex_node, "offset");
			attribute(vertex_node, "duration");

			Graph->CreateNode(style_string.c_str());
		}

		for (xml_node<>* edge_node = graph_node->first_node("Edge"); edge_node; edge_node = edge_node->next_sibling()) {
			style_string = "";

			// Edge attributes: id, trigger, to, from
			attribute(edge_node, "id");
			attribute(edge_node, "trigger");
			attribute(edge_node, "to");
			attribute(edge_node, "from");

			Graph->CreateEdge(style_string.c_str());
		}
	}

	free(content);
}