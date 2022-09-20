#include <iostream>
#include <vector>
#include <dirent.h>
#include <string>
#include <fstream>
#include <cctype>
#include <string.h>
#include <iomanip>

struct fileInfo
{
    std::string fileName;
    std::string language; // talvez trocar por um enum
    unsigned long emptyLines{};
    unsigned long comentLines{};
    unsigned long codeLines{};
};

//struct run options
//string input_name = ""
//bool recursive_search = false
//sort_e sorting_field = NONE;
//bool ascending = true


// Fuction to print the top of table
void print_Toptable(int x, int m)
{
    m += 1;
    std::cout << "Files processed: " << x << '\n';
    std::cout << std::setfill('-') << std::setw(100 + m) << '\n';

    
    

    std::cout   << "Filename"
                << std::setfill(' ') 
                << std::setw(m) 
                << "Language" 
                << std::setw(m) 
                << "Comments" 
                << std::setw(m) 
                << "Blank" 
                << std::setw(m) 
                << "Code" 
                << std::setw(m) 
                << "All" << '\n';
    
    
    
    std::cout << std::setfill('-') << std::setw(100 + m) << '\n' << std::setfill(' ');
}
// Function to print infos of each file
void print_Infos(fileInfo file, int m)
{
    float total = file.codeLines + file.comentLines + file.emptyLines;
    
    float percentComment = (file.comentLines / total) * 100;

    float percentEmpty = (file.emptyLines / total) * 100;

    float percentCode = (file.codeLines / total) * 100;

    int Commentsize = 0;
    if(percentComment < 9.9){
      Commentsize = 6;
    }
    else if(file.comentLines > 9.9  & file.comentLines < 99.9){
      Commentsize = 8;
    }
    else if(file.comentLines > 99.9){
      Commentsize = 9;
    }
    else{
      Commentsize = 7;
    }

    int emptysize = 0;
    if(percentEmpty < 9.9){
      emptysize = 6;
    }
    else if(file.emptyLines > 9.9 & file.emptyLines < 99.9){
      emptysize = 8;
    }
    else if(file.emptyLines > 99.9){
      emptysize = 9;
    }
    else{
      emptysize = 7;
    }

  int codesize = 0;
    if(percentCode < 9.9){
      codesize = 6;
    }
    else if(file.codeLines > 9.9 & file.codeLines < 99.9){
      codesize = 8;
    }
    else if(file.codeLines > 99.9){
      codesize = 9;
    }
    else{
      codesize = 7;
    }
    

    m += 2;
    std::cout << std::left
              << std::setw(m)
              << file.fileName
              << std::setfill(' ')
              << std::setw(m)
              << file.language
              << std::setw(0);
              
    std::cout
              << file.comentLines
              << " (" << std::setprecision(1) << std::fixed << percentComment
              << std::setfill(' ')
              << std::setw(m - Commentsize) << "%)"
              << std::setw(0)
              << file.emptyLines
              << " (" << std::setprecision(1) << std::fixed << percentEmpty
              << std::setfill(' ')
              << std::setw(m - emptysize) << "%)"
              << std::setw(0)
              << file.codeLines
              << " (" << std::setprecision(1) << std::fixed << percentCode
              << std::setfill(' ')
              << std::setw(m - codesize) << "%)"
              << std::setw(0)
              << file.codeLines + file.emptyLines + file.comentLines
              << std::endl;
              
}

// function to print the bottom of table6
void print_Basetable(int m)
{
    m += 1;
    std::cout << std::setfill('-') << std::setw(100+m) << '\n' << std::setfill(' ');
    std::cout << std::endl;


}

//Function to print the Sum
void print_Sum(int m, int x, int y, int z, int w){
    m += 2;
    std::cout   << std::left
                << std::setw((m * 2))
                << "SUM"
                << std::setfill(' ')
                << std::setw(m)
                << x
                << std::setw(m)
                << y
                << std::setw(m)
                << z
                << std::setw(m)
                << w

    << std::setfill('-') << std::setw(100+m-1) << '\n' << std::setfill(' ')
    << std::endl;
                

}

int main(int argc, char *argv[])
{

    DIR *dir;
    struct dirent *diread;

    std::string str;
    std::vector<fileInfo> vetorDeArquivos;

    std::fstream myFile;

    bool empty;
    char *caminho = argv[1]; // se tiver opções a posição do caminho n será estática, sujeito a mudança
    int maior = 0; //Save the 
    int sum_Comments = 0;
    int sum_Blank = 0;
    int sum_Code = 0;
    int sum_All = 0;

    if (argc == 1 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        std::cout << "Usage: " << argv[0] << " [options] <dirname>\n";
        return 1;
    }

    if ((dir = opendir(caminho)) != nullptr) // aqui eu abro o diretório
    {
        while ((diread = readdir(dir)) != nullptr)
        {
            str = std::string(diread->d_name); // turn the c like string(d_name) into a c++ string.

            if (str.size() > maior)
            {
                maior = str.size();
            }
            if (str.size() > 3)
            { // verifica se tem tamanho minimo[tamanho da menor extensão + 1 caractere]
                if (str.find(".cpp") == str.size() - 4 || str.find(".c") == str.size() - 2 || str.find(".hpp") == str.size() - 4 || str.find(".h") == str.size() - 2)
                { // verifica se alguma das extensões foram achadas no final do nome do arquivo
                    // files.push_back(diread->d_name);
                    fileInfo valideFile;
                    valideFile.fileName = str;
                    valideFile.language = str.substr(str.find(".") + 1); // guarda o tipo
                    vetorDeArquivos.push_back(valideFile);
                }
            }
        }
        closedir(dir);
    }
    else
    { // se você não conseguir abrir o diretório apresente esse erro
        perror("opendir");
        return 1;
    }

    print_Toptable(vetorDeArquivos.size(), maior);

    for (auto file : vetorDeArquivos)
    {
        short charNonEmptyPos; // variável para guardar posição do caractere não vazio de cada linha
        short multiLineStartPos;
        short multiLineEndPos;
        short singleLinePos;

        bool modoMultiLinhas = false; // variavel para entrar e sair no modo multi linhas

        // std::cout << file.fileName << "| \n"; // LINHA PARA TESTE, DELETAR NA VERSÃO FINAL
        myFile.open(file.fileName, std::ios::in);
        if (myFile.is_open())
        {
            std::string line;

            while (getline(myFile, line)) // vai iterar sobre cada linha do arquivo
            {
                empty = true; // se não for encontrado caractere diferente do vazio entra na condição que incrementa contador de linhas vazias

                for (int i = 0; i < line.size(); i++) // analisa cada caractere de cada linha
                {
                    if (!isspace(line[i]))
                    { // se char n for white space é linha de código ou comentário
                        empty = false;
                        charNonEmptyPos = i;
                        break;
                    }
                }

                if (empty) // MODO LINHAS VAZIAS
                {
                    file.emptyLines++;
                }
                else // MODO: comentário somente, comentário + codigo, codigo somente.
                {

                    if (!modoMultiLinhas) // MODO MULTI LINHAS
                    {                     // se não está no modo multi linha entra no if para ver se encontra /*

                        multiLineStartPos = line.find("/*");

                        if (multiLineStartPos != std::string::npos)
                        { // se encontrou o /* entra no modo multi linha
                            modoMultiLinhas = true;
                            file.comentLines++;

                            if (charNonEmptyPos < multiLineStartPos)
                            { // se a posição do char n vazio for menor que a do /* quer dizer que tem código antes do comentário
                                file.codeLines++;
                            }
                            continue; // se entrar no modo multi linha não precisa fazer as outras verificações
                        }
                    }
                    else
                    { // se está no modo multi linha entra para ver se encontra */

                        multiLineEndPos = line.find("*/");

                        file.comentLines++;

                        if (multiLineEndPos != std::string::npos)
                        {
                            modoMultiLinhas = false;
                            if (line.size() > multiLineEndPos + 2) // se tem mais caractere depois do /* verifique se é código ou ws
                            {
                                for (int j = multiLineEndPos + 2; j < line.size(); j++)
                                {
                                    if (!isspace(line[j]))
                                    {
                                        file.codeLines++;
                                        break;
                                    }
                                }
                            }
                            // fazer if para saber se tem código depois do termino do comentário
                        }
                        continue; // se está no modo multi linha não precisa fazer outras verificações
                    }

                    singleLinePos = line.find("//"); // posição na linha onde se encontra o comentário

                    if (singleLinePos != std::string::npos) // MODO SINGLE LINE
                    // MODO: comentário somente, comentário + code /*PROBLEMA: está contando o // entre aspas*/
                    {

                        file.comentLines++;

                        if (charNonEmptyPos < singleLinePos) // se não satisfazer essa condição é coment only, se satisfazer é comment + code
                        {
                            file.codeLines++;
                        }
                    }
                    else // MODO: codigo somente
                    {
                        file.codeLines++;
                    }

                    
                }
            }
            sum_Comments = sum_Comments + file.comentLines;
            sum_Blank = sum_Blank + file.emptyLines;
            sum_Code = sum_Code + file.codeLines;
            sum_All = sum_Code + sum_Blank + sum_Comments;

            myFile.close();
            // std::cout << "linhas em branco: " << file.emptyLines << "\n";      // LINHA PARA TESTE, DELETAR NA VERSÃO FINAL
            // std::cout << "linhas de código: " << file.codeLines << "\n";       // LINHA PARA TESTE, DELETAR NA VERSÃO FINAL
            // std::cout << "linhas de comentário: " << file.comentLines << "\n"; // LINHA PARA TESTE, DELETAR NA VERSÃO FINAL
            // std::cout << "linguagem: " << file.language << "\n";               // LINHA PARA TESTE, DELETAR NA VERSÃO FINAL
            print_Infos(file, maior);
            
        }
        
        
    }

    
    print_Basetable(maior);
    print_Sum(maior, sum_Comments, sum_Blank, sum_Code, sum_All);

    return 0;
}
