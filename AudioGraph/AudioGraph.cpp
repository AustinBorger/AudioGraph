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

#include <comdef.h>
#include <atlbase.h>
#include <Windows.h>

#include "AudioGraph.h"
#include "CAudioGraphFactory.h"

HRESULT AudioGraphCreateFactory (
	IAudioGraphCallback* pAudioGraphCallback,
	IAudioGraphFactory** ppAudioGraphFactory
) {
	if (pAudioGraphCallback == nullptr || ppAudioGraphFactory == nullptr) {
		return E_POINTER;
	}

	HRESULT hr = S_OK;

	CComPtr<CAudioGraphFactory> Factory = new CAudioGraphFactory();

	hr = Factory->Initialize(pAudioGraphCallback);

	if (FAILED(hr)) {
		*ppAudioGraphFactory = nullptr;
		return hr;
	}

	*ppAudioGraphFactory = Factory;

	return S_OK;
}