#pragma once
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <iostream>

class GetImageFromSystem
{
public:

    static GetImageFromSystem& Instance()
    {
        static GetImageFromSystem instance;
        return instance;
    }

    GetImageFromSystem(const GetImageFromSystem&) = delete;
    GetImageFromSystem& operator=(const GetImageFromSystem) = delete;

    // Abre o diálogo de seleção de arquivo
    // filter: string de filtro no formato "Imagens\0*.bmp;*.jpg;*.png\0Todos\0*.*\0"
    void OpenFile(const std::string& filter = "Imagens\0*.bmp;*.jpg;*.png\0")
    {
        ZeroMemory(&ofn, sizeof(ofn));
        ZeroMemory(szFile, sizeof(szFile));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL; // janela pai, NULL = sem janela
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) == TRUE)
        {
            selectedFile = ofn.lpstrFile;

            // Separar pasta e nome do arquivo manualmente (C++14)
            size_t pos = selectedFile.find_last_of("\\/"); // procura o último separador de pasta
            if (pos != std::string::npos)
            {
                folder = selectedFile.substr(0, pos);
                filename = selectedFile.substr(pos + 1);
            }
            else
            {
                folder = "";       // sem pasta
                filename = selectedFile; // apenas nome do arquivo
            }

            fileOpen = true;
            return;
        }
        else {
            fileOpen = false;
            return;
        }
    }

    // Retorna o caminho completo do arquivo selecionado
    std::string GetSelectedFile() const { return selectedFile; }

    // Retorna apenas a pasta onde o arquivo está
    std::string GetFolder() const { return folder; }

    // Retorna apenas o nome do arquivo
    std::string GetFileName() const { return filename; }

    bool FileIsOpen() const { return fileOpen; }

private:
    GetImageFromSystem() = default;
    ~GetImageFromSystem() = default;

    OPENFILENAME ofn{};
    char szFile[260]{};       // buffer para o arquivo selecionado
    std::string selectedFile;
    std::string folder;
    std::string filename;
    bool fileOpen = false;
};
