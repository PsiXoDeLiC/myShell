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
    char argument[MAX_LENGTH];// Хранит аргументы команды
    char directory[MAX_LENGTH];// Хранит путь текущей директории
    
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
        j = 0;//записываем аргумент
        while (input[i] != '\0' && !isspace(input[i])) {
            argument[j++] = input[i++];
        }
        argument[j] = '\0';
    }
    
    void cd_processing(){
    	if (argument[0] == '\0') {//если нет аргумента узнать текущую директорию 
		printf("the current directory: %s\n", directory);
            }
            else {//если не найдена директория соответствующая аргументу сообщить об этом
		if(chdir(argument) != 0) {
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
    		if (argument[0] == '\0') {
    			strcpy(catalog, directory);
    		}
    		else {
    			strcpy(catalog, argument);
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
    char path[MAX_LENGTH + MAX_LENGTH + 2];
    sprintf(path, "%s/%s", directory, command);
    char *args[] = {argument , NULL};
          setenv("parent", getenv("shell"), 1);

    if (pid == 0) { // 
        execv(path, args);
        // Если execv вернется, значит, произошла ошибка вывести как неопознаннйю команду
        fprintf(stderr, "Command not found \n");
        exit(EXIT_FAILURE);
    } else if (pid < 0) { 
        fprintf(stderr, "Fork failed\n");
    } else { 
        wait(NULL); // ожидание завершения
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
            printf("%s\n", argument);
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
