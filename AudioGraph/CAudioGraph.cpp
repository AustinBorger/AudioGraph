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

CAudioGraph::CAudioGraph() :
	m_RefCount(1),
	m_Gain(0.0f),
	m_MixVolume(0.0f) 
{ }

CAudioGraph::~CAudioGraph() { }

HRESULT CAudioGraph::Initialize(IAudioGraphCallback* pAudioGraphCallback, LPCSTR Style, IAudioGraphFile* pAudioGraphFile) {
	HRESULT hr = S_OK;

	m_Callback = pAudioGraphCallback;

	m_File = pAudioGraphFile;

	// Parse style string

	m_StyleString = Style;

	return S_OK;
}