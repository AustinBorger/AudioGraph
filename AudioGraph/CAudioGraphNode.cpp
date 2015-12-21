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
#include <propvarutil.h>

#define FILENAME L"CAudioGraphNode.cpp"
#define CHECK_HR(Line) if (FAILED(hr)) { m_Callback->OnObjectFailure(FILENAME, Line, hr); return; }
#define CHECK_HR2(Line) if (FAILED(hr)) { m_Callback->OnObjectFailure(FILENAME, Line, hr); return Written; }

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
		size_t start = off + 4 + strlen(attribute);
		size_t end = Style.find("\"", start);
		size_t length = end - start;
		return Style.substr(start, length);
	};

	m_ID = attribute("id");
	m_AudioFilename = attribute("filename");
	std::string sampleOffsetString = attribute("offset");
	std::string sampleDurationString = attribute("duration");
	std::string terminalString = attribute("terminal");

	// All of these attributes must be defined.
	if (m_ID == "" || m_AudioFilename == "" || sampleOffsetString == "" || sampleDurationString == "") {
		return E_INVALIDARG;
	}

	// Terminal does not need to be defined, but if defined must have a valid value
	if (terminalString == "" || terminalString == "false") {
		m_IsTerminal = false;
	} else if (terminalString == "true") {
		m_IsTerminal = true;
	} else {
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

VOID CAudioGraphNode::Setup(IMFMediaType* pMediaType) {
	HRESULT hr = S_OK;

	// Source: http://stackoverflow.com/questions/10737644/convert-const-char-to-wstring

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, m_AudioFilename.c_str(), m_AudioFilename.size(), NULL, 0);
	std::wstring wFilename(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, m_AudioFilename.c_str(), m_AudioFilename.size(), &wFilename[0], size_needed);

	hr = MFCreateSourceReaderFromURL (
		wFilename.c_str(),
		nullptr,
		&m_Reader
	); CHECK_HR(__LINE__);

	hr = m_Reader->SetStreamSelection (
		MF_SOURCE_READER_ALL_STREAMS,
		FALSE
	); CHECK_HR(__LINE__);

	hr = m_Reader->SetStreamSelection (
		MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		TRUE
	); CHECK_HR(__LINE__);

	hr = m_Reader->SetCurrentMediaType (
		MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		NULL,
		pMediaType
	); CHECK_HR(__LINE__);

	hr = m_Reader->GetCurrentMediaType (
		MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		&m_MediaType
	); CHECK_HR(__LINE__);

	// MSDN Example says to call this again:
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd757929(v=vs.85).aspx
	hr = m_Reader->SetStreamSelection (
		MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		TRUE
	); CHECK_HR(__LINE__);
}

VOID CAudioGraphNode::Flush() {
	m_Sample.Release();
	m_Reader.Release();
	m_MediaType.Release();
}

#include <iostream>

UINT CAudioGraphNode::Process(FLOAT* OutputBuffer, UINT BufferFrames) {
	HRESULT hr = S_OK;
	bool done = false;
	UINT Written = 0;
	LONGLONG BufferDuration = (LONGLONG)((FLOAT(BufferFrames) / FLOAT(44100)) * 1.0E7f);

	while (BufferFrames > 0 && !done && m_SamplePosition < m_SampleOffset + m_SampleDuration) {
		CComPtr<IMFMediaBuffer> Buffer;
		DWORD BufferLength = 0;
		BYTE* pByteBuffer = nullptr;
		LONGLONG SamplePosition = (LONGLONG)(GetTimePosition() * 1.0E7f);
		LONGLONG EndPosition = (LONGLONG)((GetTimeOffset() + GetTimeDuration()) * 1.0E7f);
		LONGLONG SampleTime = 0;
		LONGLONG SampleDuration = 0;
		LONGLONG TimeSkip = 0;
		LONGLONG TimeTruncate = 0;
		UINT SampleSkip = 0;
		UINT SampleTruncate = 0;
		UINT SampleRead = 0;

		hr = m_Sample->GetSampleTime (
			&SampleTime
		); CHECK_HR2(__LINE__);

		m_Sample->GetSampleDuration (
			&SampleDuration
		); CHECK_HR2(__LINE__);

		hr = m_Sample->ConvertToContiguousBuffer (
			&Buffer
		); CHECK_HR2(__LINE__);

		hr = Buffer->GetCurrentLength (
			&BufferLength
		); CHECK_HR2(__LINE__);

		TimeSkip = SamplePosition - SampleTime;
		TimeTruncate = (SamplePosition + BufferDuration > EndPosition) ? (SamplePosition + BufferDuration - EndPosition) : 0;
		SampleSkip = (TimeSkip / 10000000) * 44100;
		SampleTruncate = (TimeTruncate / 10000000) * 44100;
		SampleRead = BufferFrames - SampleTruncate;

		hr = Buffer->Lock (
			&pByteBuffer,
			nullptr,
			nullptr
		); CHECK_HR2(__LINE__);

		pByteBuffer += SampleSkip * sizeof(FLOAT) * 2;

		memcpy_s (
			OutputBuffer,
			BufferFrames * sizeof(FLOAT) * 2,
			pByteBuffer,
			SampleRead * sizeof(FLOAT) * 2
		);

		hr = Buffer->Unlock();
		CHECK_HR2(__LINE__);

		OutputBuffer += SampleRead * 2;
		BufferFrames -= SampleRead;
		Written += SampleRead;
		m_SamplePosition += SampleRead;

		if (SampleTruncate > 0) {
			done = true;
		} else if (BufferFrames > 0) {
			DWORD dwFlags = 0;

			// Read next sample
			m_Reader->ReadSample (
				MF_SOURCE_READER_FIRST_AUDIO_STREAM,
				NULL,
				NULL,
				&dwFlags,
				NULL,
				&m_Sample
			); CHECK_HR2(__LINE__);
		}
	}

	return Written;
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

VOID CAudioGraphNode::Seek() {
	HRESULT hr = S_OK;
	DWORD dwFlags = 0;
	LONGLONG SampleTime = 0;
	LONGLONG SampleDuration = 0;
	LONGLONG DesiredTime = (LONGLONG)(GetTimeOffset() * 1.0E7f); //100-nanosecond units
	PROPVARIANT prop;

	hr = InitPropVariantFromInt64 (
		DesiredTime,
		&prop
	); CHECK_HR(__LINE__);

	hr = m_Reader->SetCurrentPosition (
		GUID_NULL,
		prop
	); CHECK_HR(__LINE__);

	hr = PropVariantClear (
		&prop
	); CHECK_HR(__LINE__);

	do {
		m_Reader->ReadSample (
			MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			NULL,
			NULL,
			&dwFlags,
			NULL,
			&m_Sample
		); CHECK_HR(__LINE__);

		hr = m_Sample->GetSampleTime (
			&SampleTime
		); CHECK_HR(__LINE__);

		m_Sample->GetSampleDuration (
			&SampleDuration
		); CHECK_HR(__LINE__);
	} while (SampleTime + SampleDuration < DesiredTime);

	m_SamplePosition = m_SampleOffset;
}

VOID CAudioGraphNode::GetTransitionEdge(std::string& TransitionString, CAudioGraphEdge** ppAudioGraphEdge) {
	try {
		*ppAudioGraphEdge = m_TransitionMap.at(TransitionString);
	} catch (...) {
		*ppAudioGraphEdge = nullptr;
	}
}