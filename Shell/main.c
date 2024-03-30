#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>


#define MAX_LENGTH 256
extern char **environ;

char command[MAX_LENGTH];// Хранит введенную команду
char argument[5][MAX_LENGTH];// Хранит аргументы команды
char directory[MAX_LENGTH];// Хранит путь текущей директории
char comment[MAX_LENGTH];
int carg ;


void code_processing(char input[]){
    int i = 0;//Пропускаем прорбелы в начале строки
    while (isspace(input[i]) && input[i] != '\0') {
        i++;
    }
    int j = 0;// Записываем команду
    while (input[i] != '\0' && !isspace(input[i])) {
        command[j++] = input[i++];
    }
    command[j] = '\0';
    //пропускаем пробелы перед аргументом
    while (isspace(input[i]) && input[i] != '\0') {
        i++;
    }
    carg = 0;
    j = 0;//записываем аргумент
    int n = 0;
    while (input[i] != '\0') {
        if(carg < 4){
            if (isspace(input[i])){
                argument[carg][j + 1] = '\0';
                carg++;
                i++;
                j = 0;
            }
            argument[carg][j] = input[i];
            j++;
            i++;
        }
        else{
            comment[n]  = input[i];
            i++;
            n++;
        }

    }
    argument[carg][j] = '\0';
}

void cd_processing(){
    if (argument[0][0] == '\0') {//если нет аргумента узнать текущую директорию
        printf("the current directory: %s\n", directory);
    }
    else {//если не найдена директория соответствующая аргументу сообщить об этом
        if(chdir(argument[0]) != 0) {
            printf("The directory was not found\n");
        }
        else {//изменить дерикторию и переменную среды на указанный в аргументе
            getcwd(directory, sizeof(directory));
            setenv("PWD", directory, 1);
        }
    }
}

void dir_processing(){
    DIR *dir;
    struct dirent *entry;
    char catalog[MAX_LENGTH];
    //при отсутствии аргумента показать содержимое текущей директории
    if (argument[0][0] == '\0') {
        strcpy(catalog, directory);
    }
    else {
        strcpy(catalog, argument[0]);
    }//если указанной дериктории не существует сообщить об этом
    dir = opendir(catalog);
    if (dir == NULL) {
        printf("Invalid directory is specified\n");
    }
    else{//иначе показать содержимое указанной директории
        while ((entry = readdir(dir)) != NULL) {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);
}

void environ_processing(){//вывод переменных среды
    char **env = environ;;
    while (*env != NULL) {
        printf("%s\n", *env);
        env++;
    }
}

void echo_processing() {
    // Проход по каждому элементу массива argument и печать его
    for (int i = 0; i < 5 && argument[i][0] != '\0'; ++i) {
        printf("%s ", argument[i]);
    }
    printf("%s ", comment);
}

void help_processing(){//вывод поддерживаемых комманд
    printf("Supported commands:\n");
    printf("cd <directory>: Change current directory\n");
    printf("clr: Clear the screen\n");
    printf("dir <directory>: List contents of directory\n");
    printf("environ: List all environment variables\n");
    printf("echo <comment>: Display comment\n");
    printf("help: Display this help message\n");
    printf("pause: Pause the shell until Enter is pressed\n");
    printf("quit: Exit the shell\n");
}


void child_processing() {
    pid_t pid = fork();

    if (pid == 0) {
        char* args[5]; // Массив для хранения команды и аргументов


        // Заполнение остальных аргументов
        for (int i = 0; i < carg && argument[i][0] != '\0'; ++i) {
            args[i + 1] = argument[i];
        }
        args[carg + 1] = NULL; // Последний элемент должен быть NULL, как требует execvp
        execvp(command, args);

        // Если execvp вернется, значит, произошла ошибка
        fprintf(stderr, "Command not found: %s\n", command);
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
    } else {
        int status;
        waitpid(pid, &status, 0); // ожидание завершения дочернего процесса
    }
}





int main(int argc, char *argv[]) {
    char input[MAX_LENGTH];// Хранит ввод пользователя

    // Получаем текущий рабочий каталог
    char path[MAX_LENGTH];
    getcwd(path, sizeof(path));
    strcat(path, "/myshell");

    setenv("shell", path, 1);

    getcwd(directory, sizeof(directory));

    // Основной цикл обработки команд
    while (1) {

        //обработка ввода

        printf("myshell: %s> ", directory);//Приглашение командной строки
        fgets(input, sizeof(input), stdin);// Запрос ввода пользователя

        if (strcmp(input, "\n") == 0) {
            continue;//Пропускаем обработку пустой строки
        }

        code_processing(input);


//Определение и исполнение комманды

        if (strcmp(command, "quit") == 0) {//выход из оболочки
            break;
        }
        else if (strcmp(command, "cd") == 0) {//Директория
            cd_processing();
        }
        else if (strcmp(command, "clr") == 0) {//очистить терминал
            system("clear");
        }
        else if (strcmp(command, "dir") == 0) {//Содержимое каталога
            dir_processing();
        }//Показать содержимое среды
        else if (strcmp(command, "environ") == 0) {
            environ_processing();
        }// вывод на экран комментария указанного в аргументе
        else if (strcmp(command, "echo") == 0) {
            echo_processing();
        }//вывод руководства пользователя.
        else if (strcmp(command, "help") == 0) {
            help_processing();
        }//приостановка операций оболочки до нажатия клавиши <Enter>.
        else if (strcmp(command, "pause") == 0) {
            printf("Press Enter to continue...");
            getchar();
        }
            //иначе попытка запустить как дочерний процесс
        else {
            child_processing();
        }
    }


    return 0;
}
