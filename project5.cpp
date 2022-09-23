#include <iostream>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>
#include <string.h>
#include <iomanip>
#include <limits.h>
#include <cmath>

/// @brief Estrutura para guardar as informações básicas de cada arquivo válido.
///
/// Guarda nome, extensão, quantidade de linhas vazias, de comentário e código.
struct fileInfo
{
    std::string fileName;
    std::string language;
    unsigned long emptyLines{};
    unsigned long comentLines{};
    unsigned long codeLines{};
};

struct width
{
    int fileName{14};
    int language{14};
    int comment{15};
    int empty{15};
    int code{15};
    int total{7};
};

/// @brief Estrutura para guardar os comandos lidos da linha de comando.
///
/// Guarda o caminho para o diretório ou arquivo alvo e seta flags como true ou false com base nos comandos lidos.
struct line_commands
{
    bool rec = false;              // modo recursivo
    bool usage = false;            //[-h][--help][no line arguments]
    bool incomplete_order = false; // se o comando order for dado de forma imcompleta
    bool wrong_option = false;     // se a opção for inválida
    bool order = false;            // false = ascending [-s]/ true = descending[-S]
    bool file_name_o = false;      // [-s f]
    bool file_type_o = false;      // [-s t]
    bool comment_o = false;        // [-s c]
    bool blank_o = false;          // [-s b]
    bool sloc_o = false;           // [-s s]
    bool total_o = false;          // [-s a]
    std::string caminho;
};

/// @brief Estrutura para guardar o estado em que a linha se encontra.
///
/// Inicialmente a linha se encontra setada para true e sempre que encontrar qualquer char diferente de white space muda o estado da linha.
struct line_states
{
    bool empty = true;
    bool single_comment = false;
    bool multi_comment = false;
    bool code_line_counted = false;
    bool comment_line_counted = false;
    bool quote_mode = false;
    char quote_char = 'e';
};

/// @brief Lê os comandos da linha de comando e preenche a estrutura line_commands.
/// @param commands Estrutura que guarda os comandos da linha de comando.
/// @param argc Quantidade de argumentos a ser lido.
/// @param argv Array de strings com os comandos.
void read_command_lines(line_commands &commands, int argc, char *argv[])
{
    std::string str;
    std::string str2;
    commands.caminho = argv[argc - 1];
    for (int j = 1; j < argc; j++)
    {
        str = std::string(argv[j]);
        if (str == "-h" || str == "--help")
        {
            commands.usage = true;
            break;
        }
        if (str == "-r")
        {
            commands.rec = true;
            break;
        }
        if (str == "-s" || str == "-S")
        {
            if (str == "-S")
            {
                commands.order = true;
            }
            if (j + 1 == argc - 1)
            {
                commands.incomplete_order = true;
                break;
            }
            if (j + 1 < argc)
            {
                str2 = std::string(argv[j + 1]);
                if (str2 == "f")
                {
                    commands.file_name_o = true;
                }
                else if (str2 == "t")
                {
                    commands.file_type_o = true;
                }
                else if (str2 == "c")
                {
                    commands.comment_o = true;
                }
                else if (str2 == "b")
                {
                    commands.blank_o = true;
                }
                else if (str2 == "s")
                {
                    commands.sloc_o = true;
                }
                else if (str2 == "a")
                {
                    commands.total_o = true;
                }
                else
                {
                    commands.wrong_option = true;
                }
            }
            break;
        }
    }
    // std::cout << commands.order << "[com order] " << commands.file_name_o << " [file name]" << commands.file_type_o << " [file type]" << commands.rec << " [rec]" << commands.comment_o << " [comment]" << commands.blank_o << " [blank]" << commands.sloc_o << " [sloc]" << commands.total_o << " [total]"<< commands.usage << " [usage]" << commands.caminho << " [caminho]\n";
}

/// @brief Printa o usage.
/// @param commmands parametro para a função saber o caminho.
void print_usage(line_commands commmands)
{
    std::cout << "Welcome to sloc, version 1.0, (c) 2022 Luiz and Vilela IMD, UFRN.\n";
    std::cout << "Usage: " << commmands.caminho << " [options] file/directory\n";
    std::cout << "Options:\n";
    std::cout << std::setw(20) << std::left << "    -h/--help"
              << "Display this information.\n";
    std::cout << std::setw(20) << std::left << "    -r"
              << "Look for files recursively in the directory provided..\n";
    std::cout << std::setw(20) << std::left << "    -s c/b/s/a"
              << "Sort table by (c)omments, (b)lank lines, (s)loc, or (a)ll. Default is show files in ordem of appearance.\n";
    std::cout << std::setw(20) << std::left << "    -S c/b/s/a"
              << "Same as above, but in descending order.\n";
    std::cout << "Configuration:\n";
    std::cout << std::setw(20) << std::left << "    config.txt"
              << "Edit this file to add more languages supported by the program.\n";
}

/// @brief Verifica se é um arquivo.
/// @param name_read Nome do arquivo/diretório
/// @return Retorna true se é um arquivo, false caso constrário.
bool is_a_file(std::string name_read)
{
    std::fstream myFile{name_read};
    bool retorno;

    retorno = myFile.is_open();
    myFile.close();

    return retorno;
}

/// @brief Verifica se é uma pasta.
/// @param name_read Nome do arquivo lido.
/// @return Retorna true se é um arquivo, false caso constrário.
bool is_a_dir(std::string name_read)
{
    DIR *dir;
    const char *name_read2 = name_read.c_str();

    if ((dir = opendir(name_read2)) != nullptr)
    {
        closedir(dir);
        return true;
    }
    else
    {
        // std::cout << "cannot open dir\n";
        return false;
    }
}

/// @brief Conta as linhas vazias, de código e comentário de todos arquivos válidos.
/// @param vetorDeArquivos Vetor com todos arquivos válidos
void counting_lines(std::vector<fileInfo> &vetorDeArquivos)
{
    std::string line;

    for (auto &file : vetorDeArquivos)
    {
        line_states line_state;
        std::fstream myFile{file.fileName};
        if (myFile.is_open())
        {
            std::cout << "FILE: [ " << file.fileName << " ]\n";
            while (getline(myFile, line))
            {
                line_state.code_line_counted = false;    // só pode contar uma vez por linha então a cada linha tem que resetar
                line_state.comment_line_counted = false; // só pode contar uma vez por linha então a cada linha tem que resetar
                for (int i = 0; i < line.size(); i++)    // analisa cada caractere de cada linha
                {
                    if (!line_state.quote_mode)                                              // MODO: NOT QUOTE
                    {                                                                        // se não estiver no modo aspas entra para verificar se é comentário ou código
                        if ((line[i] == '"' || line[i] == '"') && !line_state.multi_comment) // verifica se é para entrar no modo aspas
                        {
                            if (!line_state.code_line_counted)
                            {
                                file.codeLines++;
                            }
                            line_state.quote_char = line[i]; // pega qual tipo de aspas foi encontrado primeiro
                            line_state.quote_mode = true;    // entra no modo aspas, n procurando mais por comentários, somente pelo fechamento das aspas
                            line_state.empty = false;
                        }
                        else if (line[i] == '/' && !line_state.multi_comment) // caso n esteja no modo aspas verificar se é comentário
                        {
                            line_state.empty = false;
                            if ((i + 1) < line.size())
                            {
                                if (line[i + 1] == '/')
                                { // MODO: SINGLE COMMENT
                                    if (!line_state.comment_line_counted)
                                    {
                                        file.comentLines++;
                                    }
                                    line_state.empty = false;
                                    break; // conta como comentário, seta a flag de linha vazia para falso e sai da linha
                                }
                                else if (line[i + 1] == '*') // MODO: MULTI COMMENT
                                {
                                    if (!line_state.comment_line_counted)
                                    {
                                        file.comentLines++;
                                        line_state.comment_line_counted = true;
                                    }
                                    line_state.multi_comment = true;
                                    line_state.empty = false;
                                    i++; // verificar se é necessario, talvez deletar
                                }
                            }
                            else
                            { // se encontrar o '/' na ultima posição e ainda n tiver sido contado código na linha, incremente o contador de line code
                                if (!line_state.code_line_counted)
                                {
                                    file.codeLines++;
                                    line_state.code_line_counted = true;
                                    line_state.empty = false;
                                }
                            }
                        }
                        else if (line_state.multi_comment) // verifica se é fechamento de comentário multi linhas
                        {                                  // se vc n estiver em modo multi comment vc pula essa condição
                            line_state.empty = false;
                            if (!line_state.comment_line_counted)
                            { // se entra em uma nova linha em modo multi comentário ligado e a linha de comentário ainda n foi contada, incrementa o contador e liga a flag.
                                file.comentLines++;
                                line_state.comment_line_counted = true;
                            }
                            if (line[i] == '*')
                            {
                                if ((i + 1) < line.size())
                                {
                                    if (line[i + 1] == '/')
                                    {
                                        line_state.multi_comment = false;
                                        line_state.empty = false;
                                        i++; // verificar se é necessário talvez deletar
                                    }
                                }
                            }
                        }
                        else if (!isspace(line[i]))
                        { // caso n seja aspas, nem seja comentário, nem seja espaço vazio é código
                            if (!line_state.code_line_counted)
                            { // conte como linha de código se ainda n foi contado linha de código nessa linha.
                                file.codeLines++;
                                line_state.code_line_counted = true;
                                line_state.empty = false;
                            }
                        }
                    }
                    else // MODO: QUOTE
                    {
                        if (!line_state.code_line_counted) // se entrou em uma nova linha e ainda está no modo aspas conte como código e n conte mais linhas de código nessa linha
                        {
                            file.codeLines++;
                            line_state.code_line_counted = true;
                        }
                        if (line[i] == line_state.quote_char)
                        {
                            line_state.quote_mode = false;
                            line_state.quote_char = 'e';
                        }
                    }
                }
                if (line_state.empty) // se ao terminar de percorrer a linha o modo empty estiver setado para true quer dizer que a linha está vazia, então aumente o contador de linhas vazias
                {
                    file.emptyLines++;
                }
                else if (!line_state.quote_mode)
                { // se estiver no modo aspas n defina o modo vazio como true ao iniciar a nova linha
                    line_state.empty = true;
                }
            }
            myFile.close();
        }
        std::cout << "number of code lines: [ " << file.codeLines << " ]\n";
        std::cout << "number of comment lines: [ " << file.comentLines << " ]\n";
        std::cout << "number of empty lines: [ " << file.emptyLines << " ]\n";
        std::cout << "language: [ " << file.language << " ]\n";
    }
}

/// @brief Transforma a extensão lida para lower case.
/// @param name_read Nome da extensão.
/// @return Retorna o nome da extensão em lower case.
std::string to_lower_case(std::string name_read)
{
    std::transform(name_read.begin(), name_read.end(), name_read.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); }); // transforma o nomde da extensão do arquivo lido em lowercase

    return name_read;
}

/// @brief Cria dinamicamente a lista de extensões aceitas a partir de um arquivo externo.
/// @return Retorna o vetor com as extensões aceitas.
std::vector<std::string> get_extensions() // lê um arquivo externo para criar dinamicamente a lista de extensões aceitas
{
    std::vector<std::string> extensions;
    std::string extension;
    std::fstream configfile{"config_files/languages.txt"};
    if (configfile.is_open())
    {
        while (getline(configfile, extension))
        {
            extensions.push_back(extension);
        }

        configfile.close();
    }
    return extensions;
}

/// @brief Print the top of the table.
/// @param x
void print_Toptable(int x, width widths)
{
    int total_width = widths.fileName + widths.code + widths.comment + widths.empty + widths.total + widths.language;
    std::cout << "Files processed: " << x << '\n';
    std::cout << std::setw(total_width) << std::setfill('-') << '\n';
    std::cout << std::setfill(' ') << std::setw(widths.fileName) << std::left << "Filename" << std::setw(widths.language) << "Language" << std::setw(widths.comment) << "Comments" << std::setw(widths.empty) << "Blank" << std::setw(widths.code) << "Code" << std::setw(widths.total) << "All";
    std::cout << std::setw(total_width) << std::setfill('-') << '\n';
}

/// @brief Function to print infos of each file
/// @param file
/// @param m

std::string concat_data_percent(int qtd, int total)
{
    std::stringstream buf;
    float percent = 0;
    if (total > 0)
    {
        percent = float(qtd) / float(total) * 100;
    }
    std::string str;
    buf << qtd;
    buf << " (";
    buf << std::fixed << std::setprecision(1) << percent;
    buf << "%)";
    str = buf.str();

    return str;
}
void print_Infos(std::vector<fileInfo> vetorDeArquivos, width widths)
{
    // float percentComment;
    // float percentEmpty;
    // float percentCode;
    std::cout << "\n";
    int total = 0;
    for (auto file : vetorDeArquivos)
    {
        total = file.codeLines + file.comentLines + file.emptyLines;

        std::cout
            << std::setfill(' ')
            << std::left
            << std::setw(widths.fileName)
            << file.fileName
            << std::setw(widths.language)
            << file.language;
        std::cout << std::setw(widths.comment) << concat_data_percent(file.comentLines, total) << std::setw(widths.empty) << concat_data_percent(file.emptyLines, total) << std::setw(widths.code) << concat_data_percent(file.codeLines, total) << std::setw(widths.total) << file.codeLines + file.emptyLines + file.comentLines << '\n';
    }
}

/// @brief Function to print the bottom of table
void print_Basetable(width widths)
{
    int total_width = widths.fileName + widths.code + widths.comment + widths.empty + widths.total + widths.language;
    std::cout << "" << std::setw(total_width) << std::setfill('-') << "" << std::endl;
}

void print_table(std::vector<fileInfo> vetorDeArquivos, width widths)
{
    print_Toptable(vetorDeArquivos.size(), widths);

    print_Infos(vetorDeArquivos, widths);

    print_Basetable(widths);
}
/// @brief Verifica se o arquivo possui uma extensão válida.
/// @param name Nome do arquivo sendo analisado.
/// @param extensions Vetor com extensões aceitas.
/// @return Se for arquivo válido retorna a posição onde o "." da extensão foi encontrado, caso contrário retorne 0 para dizer que não é um arquivo válido.
short is_target_file(std::string name, std::vector<std::string> extensions)
{

    short position;

    name = to_lower_case(name);

    for (auto extension : extensions)
    {

        position = name.find(extension); // indice em que extensão foi achado no nome do arquivo lido

        if ((position == name.size() - extension.size()) && position != std::string::npos) // verifica se a extensão foi achada e se ela está no final do nome do arquivo
        {
            return position; // retorna posição para fazer corte da extensão e também confirmar se é válida
        }
    }
    return 0; // se não for válida 0
}

/// @brief Lê arquivos do diretório e adiciona os arquivos válidos.
/// @param caminhoStr Caminho do diretório.
/// @param vetorDeArquivos Vetor com arquivos válidos.
/// @param commands Estrutura com comandos, necessário para saber se é recursivo ou não.
/// @param extensions Vetor com extensões aceitas.
/// @return // Retorna o vetor com arquivos válidos.
std::vector<fileInfo> add_target_files(std::string caminhoStr, std::vector<fileInfo> vetorDeArquivos, line_commands commands, std::vector<std::string> extensions, int &maior)
{
    std::string name_read;
    const char *caminho = caminhoStr.c_str();
    fileInfo valideFile;
    DIR *dir;
    struct dirent *diread;
    char file_path[PATH_MAX + 1];

    if ((dir = opendir(caminho)) != nullptr) /// aqui eu abro o diretório
    {
        while ((diread = readdir(dir)) != nullptr)
        {
            name_read = std::string(diread->d_name);
            strcpy(file_path, caminho);
            strcat(file_path, "/");
            strcat(file_path, name_read.c_str());

            if (name_read == "." || name_read == "..")
            {
                continue;
            }
            else if (is_target_file(name_read, extensions))
            {

                valideFile.fileName = file_path;
                valideFile.language = to_lower_case(name_read.substr(is_target_file(name_read, extensions) + 1)); // pensar em uma maneira de n ter que usar is_target duas vezes
                vetorDeArquivos.push_back(valideFile);
                if (std::string(file_path).size() > maior)
                {
                    maior = std::string(file_path).size() + 2;
                }
            }
            else if (is_a_dir(file_path) && commands.rec)
            {
                vetorDeArquivos = add_target_files(file_path, vetorDeArquivos, commands, extensions, maior);
            }
        }
        closedir(dir);
        // return vetorDeArquivos;
    }
    return vetorDeArquivos;
    // else
    // { // se você não conseguir abrir o diretório apresente esse erro
    //     std::cout << " Sorry, unable to open target " << caminho << "\n";
    //     return vetorDeArquivos;
    // }
}

/// @brief Cria lista de arquivos válidos.
/// @param commands Estrutura com comandos passados na linha de comando. Verifica se é necessário entrar no modo recursivo e o caminho.
/// @param vetorDeArquivos Vetor com arquivos válidos.
/// @param extensions Vetor com extensões aceitas pelo programa.
void creating_list_of_valid_files(line_commands commands, std::vector<fileInfo> &vetorDeArquivos, std::vector<std::string> extensions, int &maior)
{
    if (is_a_dir(commands.caminho))
    {
        vetorDeArquivos = add_target_files(commands.caminho, vetorDeArquivos, commands, extensions, maior);
        for (auto file : vetorDeArquivos)
        {
            std::cout << file.fileName << "\n";
        }
        if (vetorDeArquivos.size() == 0)
        {
            std::cout << "Sorry, unable to find any supported source file inside directory " << commands.caminho << "\n";
        }
    }
    else if (is_a_file(commands.caminho))
    {
        if (is_target_file(commands.caminho, extensions))
        {
            fileInfo valideFile;
            valideFile.fileName = std::string(commands.caminho);
            maior = std::string(commands.caminho).size() + 2;
            valideFile.language = std::string(commands.caminho).substr(is_target_file(commands.caminho, extensions) + 1); // pensar em uma maneira de n ter que usar is_target duas vezes
            vetorDeArquivos.push_back(valideFile);
            std::cout << "Nome do arquivo é: " << vetorDeArquivos[0].fileName << "\n";
        }
        else
        {
            std::cout << " Sorry, file " << commands.caminho << " not supported at this time.\n";
        }
    }
    else
    {
        std::cout << " Sorry, unable to open target " << commands.caminho << "\n";
    }
}

void getting_widths(width &widths, std::vector<fileInfo> vetorDeArquivos)
{
    std::stringstream stringObject;
    std::string sizeCodeLinesStr;
    std::string sizeCommentLinesStr;
    std::string sizeEmptyLinesStr;
    std::string total;
    for (auto file : vetorDeArquivos)
    {
        total = std::to_string(file.codeLines + file.comentLines + file.emptyLines).size();
        if (file.fileName.size() > widths.fileName)
        {
            widths.fileName = file.fileName.size() + 6;
        }
        if (std::to_string(file.codeLines).size() + 14 > widths.code)
        {
            widths.code = std::to_string(file.codeLines).size() + 14;
        }
        if (std::to_string(file.comentLines).size() + 14 > widths.comment)
        {
            widths.comment = std::to_string(file.comentLines).size() + 14;
        }
        if (std::to_string(file.emptyLines).size() + 14 > widths.empty)
        {
            widths.empty = std::to_string(file.emptyLines).size() + 14;
        }
        if (total.size() > widths.total)
        {
            widths.total = total.size() + 6;
        }
        if (file.language.size() + 6 > widths.language)
        {
            widths.language = file.language.size() + 6;
        }
    }
}

int main(int argc, char *argv[])
{
    std::vector<std::string> extensions;   // lista dinamicamente criada de extensões aceitas
    std::vector<fileInfo> vetorDeArquivos; // lista de arquivos para ser preenchido com os arquivos válidos
    line_commands commands;                // struct com flags e caminho ou arquivo alvo
    int maior = 0;
    width widths;
    extensions = get_extensions(); // função que cria a lista de extensões

    read_command_lines(commands, argc, argv); // lê os comandos da linha de comando e preenche o struct commands com as flags e o caminho ou arquivo alvo

    if (commands.incomplete_order)
    {
        std::cout << "Missing indication of sorting field.\n";
        return 1;
    }

    if (commands.wrong_option)
    {
        std::cout << "Unrecognized sorting field provided.\n";
        return 1;
    }

    if (commands.usage || argc == 1) // apresenta o usage e encerra o programa se nenhum argumento for inserido ou as flags -h ou --help forem inseridas
    {
        print_usage(commands);
        return 1;
    }

    creating_list_of_valid_files(commands, vetorDeArquivos, extensions, maior); // cria uma lista de arquivos válidos e apresenta erros quando encontrados

    counting_lines(vetorDeArquivos);

    getting_widths(widths, vetorDeArquivos);

    std::cout << "code width[" << widths.code << "]\n";
    std::cout << "comment width[" << widths.comment << "]\n";
    std::cout << "empty width[" << widths.empty << "]\n";
    std::cout << "file name width[" << widths.fileName << "]\n";

    print_table(vetorDeArquivos, widths);

    return 0;
}
