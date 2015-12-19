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

class CAudioGraphFile : public IAudioGraphFile {
public:
	CAudioGraphFile();

	~CAudioGraphFile();

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

	HRESULT Initialize(IAudioGraphCallback* pAudioGraphCallback, LPCWSTR Filename);

	VOID Parse();

private:
	long m_RefCount;

	CComPtr<IAudioGraphCallback> m_Callback;
	std::wstring m_Filename;
	std::vector<CComPtr<IAudioGraph>> m_GraphEnum;
	std::map<std::string, CComPtr<IAudioGraph>> m_GraphMap;

	//IUnknown methods

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IAudioGraphFile);
		QUERY_INTERFACE_CAST(IUnknown);
		QUERY_INTERFACE_FAIL();
	}

	//IAudioGraphFile methods

	/* Returns the number of graphs contained in this file. */
	UINT STDMETHODCALLTYPE GetNumGraphs() final;

	/* Retrieves a graph based on the given array index. */
	VOID STDMETHODCALLTYPE EnumGraph(UINT GraphNum, IAudioGraph** ppAudioGraph) final;

	/* Retrieves a graph based on a given graph identifier. */
	VOID STDMETHODCALLTYPE GetGraphByID(LPCSTR ID, IAudioGraph** ppAudioGraph) final;

	/* Returns the object's filename. */
	LPCWSTR STDMETHODCALLTYPE GetFilename() final;

	/* Appends an existing audio graph to the file. */
	VOID STDMETHODCALLTYPE AppendGraph(IAudioGraph* pAudioGraph) final;

	/* Saves the file to disk in XML format. */
	VOID STDMETHODCALLTYPE Save() final;
};