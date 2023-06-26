#include "FileDialog.h"
#include <string>

HRESULT OpenFileDialog(char* filename)
{
	IFileOpenDialog* pFileOpen;

	// Create the FileOpenDialog object
	CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

	// Show the Open dialog box
	HRESULT hr = pFileOpen->Show(NULL);

	// Get the file name from the dialog box
	if (SUCCEEDED(hr))
	{
		IShellItem* pItem;
		pFileOpen->GetResult(&pItem);

		PWSTR pszFilePath;
		pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

		// Convert wide char to multi-byte char
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, filename, bufferSize, NULL, NULL);

		CoTaskMemFree(pszFilePath);

		pItem->Release();

	}
	pFileOpen->Release();

	return hr;
}


HRESULT SaveFileDialog(char* filename)
{
	// Create an instance of the common dialog box
	IFileSaveDialog* pSaveDialog;
	CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pSaveDialog));

	// Set the file types to filter
	COMDLG_FILTERSPEC fileTypes[] = { L"JPEG Image", L"*.jpg", L"PNG Image", L"*.png" };
	pSaveDialog->SetFileTypes(_countof(fileTypes), fileTypes);

	// Set the default file extension
	pSaveDialog->SetDefaultExtension(L"jpg");

	// Show the dialog box
	HRESULT hr = pSaveDialog->Show(NULL);
	if (SUCCEEDED(hr))
	{
		// Get the selected file path
		IShellItem* pItem;
		pSaveDialog->GetResult(&pItem);

		PWSTR pszFilePath;
		pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

		// Convert wide char to multi-byte char
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, filename, bufferSize, NULL, NULL);

		// Cleanup
		CoTaskMemFree(pszFilePath);

		pItem->Release();

	}
	pSaveDialog->Release();

	return hr;
}

bool AcceptibleFormat(const char* filename)
{
	bool isacceptible = true;
	size_t pathlen = strlen(filename);
	const char* nameP = filename + pathlen;
	while (nameP != filename)
	{
		if (*nameP == '.')
		{
			std::string extension(nameP);

			if (extension != ".png" && extension != ".jpg")
			{
				isacceptible = false;
			}
			break;
		}
		--nameP;
	}

	if (nameP == filename)
	{
		isacceptible = false;
	}
	return isacceptible;
}