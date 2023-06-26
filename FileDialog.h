#pragma once
#include <shobjidl.h> 

//opens a windows dialog for choosing a file from the file system
HRESULT OpenFileDialog(char* filename);

//opem a windows dialog for choosing a path for saving
HRESULT SaveFileDialog(char* filename);

//determines if file in given filepath is an acceptible file type
bool AcceptibleFormat(const char* filename);

