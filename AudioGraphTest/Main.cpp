#include "AudioGraph.h"
#include "QueryInterface.h"

#include <comdef.h>
#include <atlbase.h>
#include <Windows.h>
#include <string>
#include <iostream>

#pragma comment(lib, "AudioGraph.lib")

#define FILENAME L"Main.cpp"
#define CHECK_HR(Line) if (FAILED(hr)) { OnObjectFailure(FILENAME, Line, hr); return; }

int main() {
	class X : public IAudioGraphCallback {
	public:
		STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
			QUERY_INTERFACE_CAST(IAudioGraphCallback);
			QUERY_INTERFACE_CAST(IUnknown);
			QUERY_INTERFACE_FAIL();
		}

		ULONG STDMETHODCALLTYPE AddRef() {
			return 1;
		}

		ULONG STDMETHODCALLTYPE Release() {
			return 1;
		}

		VOID STDMETHODCALLTYPE OnObjectFailure(LPCWSTR File, UINT Line, HRESULT hr) final {
			_com_error e(hr);
			std::wstring Message;

			Message.append(File);
			Message.append(L" @ Line ");
			Message.append(std::to_wstring(Line));
			Message.append(L":\n\n");
			Message.append(e.ErrorMessage());

			MessageBoxW(NULL, Message.c_str(), L"Object Failure", MB_ICONERROR | MB_OK);

			ExitProcess(hr);
		}

		LPCSTR STDMETHODCALLTYPE OnTransition(IAudioGraph* pAudioGraph, IAudioGraphNode* pNode) {
			return "";
		}

		VOID Run() {
			HRESULT hr = S_OK;

			std::wstring filename;

			std::wcout << "Available demos:" << std::endl;
			std::wcout << "\t" << "example1.xml" << std::endl;

			std::wcout << std::endl;

			std::wcout << "Enter a filename: ";
			std::wcin >> filename;

			hr = AudioGraphCreateFactory (
				this,
				&m_Factory
			); CHECK_HR(__LINE__);

			m_Factory->ParseAudioGraphFile (
				filename.c_str(),
				&m_File
			);

			UINT NumGraphs = m_File->GetNumGraphs();

			std::cout << "NumGraphs = " << NumGraphs << std::endl << std::endl;

			for (UINT i = 0; i < NumGraphs; i++) {
				CComPtr<IAudioGraph> Graph;

				m_File->EnumGraph(i, &Graph);

				std::cout << "Graph Style String = " << Graph->GetStyleString() << std::endl;
				std::cout << "Graph ID = " << Graph->GetID() << std::endl;
				std::cout << "Graph Type = " << Graph->GetType() << std::endl;

				UINT NumNodes = Graph->GetNumNodes();

				std::cout << "Graph NumNodes = " << NumNodes << std::endl;

				for (UINT j = 0; j < NumNodes; j++) {
					CComPtr<IAudioGraphNode> Node;

					Graph->EnumNode(j, &Node);

					std::cout << "Node Style String = " << Node->GetStyleString() << std::endl;
					std::cout << "Node ID = " << Node->GetID() << std::endl;
					std::cout << "Node Filename = " << Node->GetAudioFilename() << std::endl;
					std::cout << "Node Sample Offset = " << Node->GetSampleOffset() << std::endl;
					std::cout << "Node Sample Duration = " << Node->GetSampleDuration() << std::endl;
				}

				m_Factory->QueueAudioGraph(Graph);
			}

			std::cout << std::endl;

			system("pause");
		}

	private:
		CComPtr<IAudioGraphFactory> m_Factory;
		CComPtr<IAudioGraphFile> m_File;
	} x;

	x.Run();

	return 0;
}