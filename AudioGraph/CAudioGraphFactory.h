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
**		https://github.com/AustinBorger/AudioGraph
*/

#pragma once

#include <comdef.h>
#include <atlbase.h>
#include <Windows.h>

#include "AudioGraph.h"
#include "QueryInterface.h"
#include "DXAudio.h"
#include "CDXAudioWriteCallback.h"
#include "CAudioGraphFile.h"

class CAudioGraphFactory : public IAudioGraphFactory {
public:
	CAudioGraphFactory();

	~CAudioGraphFactory();

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

	HRESULT Initialize(IAudioGraphCallback* pAudioGraphCallback);

private:
	long m_RefCount;

	CComPtr<IAudioGraphCallback> m_Callback;
	CComPtr<IDXAudioStream> m_Stream;
	CComPtr<CDXAudioWriteCallback> m_WriteCallback;

	// Memory blocks below ensure that objects are sequential to the factory.
	BYTE _memblockWriteCallback[sizeof(CDXAudioWriteCallback)];

	//IUnknown methods

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IAudioGraphFactory);
		QUERY_INTERFACE_CAST(IUnknown);
		QUERY_INTERFACE_FAIL();
	}

	//IAudioGraphFactory methods

	/* Parses an XML file defining a set of audio graphs. */
	VOID STDMETHODCALLTYPE ParseAudioGraphFile(LPCWSTR Filename, IAudioGraphFile** ppAudioGraphFile) final;

	/* Creates a blank XML file defining a set of audio graphs. */
	VOID STDMETHODCALLTYPE CreateAudioGraphFile(LPCWSTR Filename, IAudioGraphFile** ppAudioGraphFile) final;

	/* Creates a blank audio graph. */
	VOID STDMETHODCALLTYPE CreateAudioGraph(LPCSTR Style, IAudioGraph** ppAudioGraph) final;

	/* Places an audio graph in the playback queue. */
	VOID STDMETHODCALLTYPE QueueAudioGraph(IAudioGraph* pAudioGraph) final;
};