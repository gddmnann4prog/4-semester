#include<iostream>

#define interface_hi "~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~Virtual memory example~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
#define interface_menu	"Commands:\n"\
						"vini - init virtual memory\n"\
						"vget - get value from virtual memory\n"\
						"vput - put value to virtual memory\n"\
						"save - save data in file\n"\
						"rnd - random filling memory\n"\
						"clf - clear data in file\n"\
						"printf - output file\n"\
						"cls - clear screen\n"\
						"exit \n"
#define interface_vini	"Input size of file in KB\n"\
						"Default amount: 2 KB"
#define interface_vini_success "Virtual memory initialized successfully"
#define interface_err_init "Memory didn't initialized!\nTo initialize memory type command 'vini'"
#define interface_err_init_again "Memory already initialized"
#define interface_index "Input index of value\nRange from 0 to "
#define interface_input_value "Input value"
#define interface_file_created "File has been created"
#define interface_file_not_created "Can't create file"
#define interface_file_opened "File has been opened"
#define interface_file_rnd_start "Putting random values in file..."
#define interface_file_rnd_finish "File has been filled"
#define interface_file_clr_start "Clearing file..."
#define interface_file_clr_finish "File has been cleared"
#define interface_file_output_start "Output file data"
#define interface_saving_data "Saving data..."
#define interface_saved_data "Data saved successfully"
#define interface_closing "Closing..."

using namespace std;

struct Virtual 
{
	static const int NPAGES = 2;
	static const int PAGESIZE = 512;
	FILE* fp;							/*Файловый указатель*/
	int Type;							/*Размер элемента в байтах*/
	int Status[NPAGES];					/*Флаг модификации(Статус страницы)*/
	int Number[NPAGES];					/*Номер страницы*/
	char Page[NPAGES * PAGESIZE];		/*Буфер страницы*/
} RAM;

/*************************************************/
/*choosePage - выбор удаляемой страницы из памяти*/
/*----------------Аргументы----------------------*/
/*memory - виртуальная память*********************/
/*************************************************/
int choosePage(Virtual& memory) 
{
	return 0;
}

/************************************************************/
/*uploadToFile - загрузка страницы виртуальной памяти в файл*/
/*----------------Аргументы---------------------------------*/
/*memory - виртуальная память********************************/
/*index - номер страницы*************************************/
/************************************************************/
void uploadToFile(Virtual& memory, int index)
{
	if (memory.Status[index] == 0) return;

	fseek(memory.fp, memory.Number[index] * RAM.PAGESIZE, SEEK_SET);
	for (int j = 0; j < RAM.PAGESIZE; j++)
	{
		fputc(memory.Page[index * RAM.PAGESIZE + j], memory.fp);
	}
	memory.Status[index] = 0;
}

/***************************************************************/
/*uploadToFile - загрузка страницы виртуальной памяти из файла**/
/*----------------Аргументы------------------------------------*/
/*memory - виртуальная память***********************************/
/*whereToLoad - номер загружаемой страницы в виртуальной паямти*/
/*containerPage - номер загружаемой страницы в файле************/
void downloadFromFile(Virtual& memory, int whereToLoad, int containerPage)
{
	fseek(memory.fp, containerPage * RAM.PAGESIZE, SEEK_SET);
	for (int i = 0; i < RAM.PAGESIZE; i++)
	{
		memory.Page[whereToLoad * RAM.PAGESIZE + i] = fgetc(memory.fp);
	}
	memory.Status[whereToLoad] = 0;
	memory.Number[whereToLoad] = containerPage;
}

/*********************************************************/
/*getElementInTable - взять элемент из виртуальной памяти*/
/*----------------Аргументы------------------------------*/
/*memory - виртуальная память*****************************/
/*index - номер элемента**********************************/
/*********************************************************/
void* getElementInTable(Virtual& memory, int index)
{
	int result = 0;
	int containerPage = index / RAM.PAGESIZE;

	for (int i = 0; i < RAM.NPAGES; i++)
	{
		if (memory.Number[i] == containerPage)
		{
			result = i * RAM.PAGESIZE + index % RAM.PAGESIZE;
			break;
		}
	}

	return &(memory.Page[result]);
}

/*******************************************************/
/*address - взятие адреса элемента в виртуальной памяти*/
/*----------------Аргументы----------------------------*/
/*memory - виртуальная память***************************/
/*index - номер ячейки с элементом**********************/
/*******************************************************/
void* address(Virtual& memory, long index)
{
	int containerPage = index / RAM.PAGESIZE;
	bool isLoaded;

	for (int i = 0; i < RAM.NPAGES; i++)
	{
		if ((isLoaded = (memory.Number[i] == containerPage)) == true)	break;
	}

	if (!isLoaded)
	{
		int whereToLoad = rand() % 2;

		if (memory.Status[whereToLoad] != 0)
			uploadToFile(memory, whereToLoad);

		downloadFromFile(memory, whereToLoad, containerPage);
	}


	return getElementInTable(memory, index);
}

/**************************************************************/
/*vini - инициализатор виртуального массива********************/
/*----------------Аргументы-----------------------------------*/
/*size - размер инициализируемого массива данных в файле(КБ)***/
/*Type - размер элемента в байтах******************************/
/**************************************************************/
Virtual* vini(long size, int Type)
{
	Virtual* newArray = new Virtual();
	int pagesAmount, num;
	char elem;

	if (!newArray)	return NULL;

	pagesAmount = size * 1024 / RAM.PAGESIZE;

	newArray->Type = sizeof(char);

	for (int i = 0; i < RAM.NPAGES; i++)
	{
		newArray->Number[i] = -1;
	}

	/*Open or create file*/
	fopen_s(&(newArray->fp), "temp.dat", "r+b");

	if (newArray->fp == NULL)
	{
		fopen_s(&(newArray->fp), "temp.dat", "w+b");
		if (newArray->fp == NULL)
		{
			cout << interface_file_not_created << endl;
			return NULL;
		}
		cout << interface_file_created << endl;
	}
	cout << interface_file_opened << endl;

	if (fgetc(newArray->fp) == EOF)
	{
		for (int i = 0; i < pagesAmount; i++) {
			for (int j = 0; j < RAM.PAGESIZE; j++) {
				//num = rand();
				elem = 0;
				fprintf(newArray->fp, "%c", elem);
			}
		}
	}

	return newArray;
}

/***************************************************************/
/*vput - записать в виртуальную память значение переменной*******/
/*----------------Аргументы-------------------------------------*/
/*memory - виртуальная память************************************/
/*index - индекс элемента массива данных из файла****************/
/*value - переменная, из которой записывается значение***********/
/****************************************************************/
int vput(Virtual& memory, int index, char& value)
{
	char* valueFromMemory = (char*)address(memory, index);

	*valueFromMemory = value;

	//Finding page with index in virtual memory
	for (int i = 0; i < RAM.NPAGES; i++)
	{
//		if (valueFromMemory < &(memory.Page[i * RAM.PAGESIZE]))
		if (index / RAM.PAGESIZE == memory.Number[i])
			memory.Status[i] = 1;
	}
	

	return 0;
}

/***************************************************************/
/*vget - записать ячейку виртуальной памяти в переменную********/
/*----------------Аргументы------------------------------------*/
/*memory - виртуальная память***********************************/
/*index - индекс элемента массива данных из файла***************/
/*value - переменная, в которую записывается значение***********/
/***************************************************************/
int vget(Virtual& memory, int index, char& value)
{
	char* valueFromMemory = (char*)address(memory, index);

	value = *(valueFromMemory);

	return 0;
}

/*****************************************************/
/*saveMemory - сохрнить всю виртуальную память в файл*/
/*----------------Аргументы--------------------------*/
/*memory - виртуальная память*************************/
/*****************************************************/
void saveMemory(Virtual& memory)
{
	for (int i = 0; i < memory.NPAGES; i++)
	{
		uploadToFile(memory, i);
	}
}

/*********************************/
/*UI - пользовательский интерфейс*/
/*********************************/
void UI()
{
	Virtual* memory = NULL;
	int amount = 2;

	string operation;

	cout << interface_hi << endl;
	do {
		cout << interface_menu << endl;
		cin >> operation;
		if (operation == "vini")
		{
			if (memory != NULL)
			{
				cout << interface_err_init_again << endl << endl;
				continue;
			}
			cout << interface_vini << endl;
			cin >> amount;
			memory = vini(amount, sizeof(char));
			cout << interface_vini_success << endl << endl;
		}
		else if (operation == "vget")
		{
			char value;
			int index = 0;
			if (memory == NULL)
			{
				cout << interface_err_init << endl << endl;
				continue;
			}
			else {
				cout << interface_index << amount * 1024 - 1 << endl;
				cin >> index;
				if (index < 0 || index >= amount * 1024 - 1)
				{
					cout << "Out of rande, try again" << endl << endl;
					continue;
				}
				vget(*memory, index, value);
				cout << "Value in position " << index << " = " << value << endl << endl;
			}
		}
		else if (operation == "vput")
		{
			char value;
			int index = 0;
			if (memory == NULL)
			{
				cout << interface_err_init << endl << endl;
				continue;
			}
			else {
				cout << interface_input_value << endl;
				cin >> value;
				cout << interface_index << amount*1024 << endl;
				cin >> index;
				vput(*memory, index, value);
				cout << "Value " << value << " in position " << index << " has been loaded." << endl << endl;
			}
		}
		else if (operation == "save")
		{
			if (memory != NULL)
			{
				cout << interface_saving_data << endl;
				saveMemory(*memory);
				cout << interface_saved_data << endl << endl;
			}
			else
			{
				cout << interface_err_init << endl << endl;
				continue;
			}
		}
		else if (operation == "rnd")
		{
			if (memory == NULL)
			{
				cout << interface_err_init << endl << endl;
				continue;
			}
			cout << interface_file_rnd_start << endl;
			char c = 0;
			for (int i = 0; i < amount * 1024 / memory->Type; i++)
			{
				c = rand() % 256;
				vput(*memory, i, c);
			}
			cout << interface_file_rnd_finish << endl << endl;
		}
		else if (operation == "clf")
		{
			if (memory == NULL)
			{
				cout << interface_err_init << endl << endl;
				continue;
			}
			cout << interface_file_clr_start << endl;
			char c = 0;
			for (int i = 0; i < amount * 1024 / memory->Type; i++)
			{
				vput(*memory, i, c);
			}
			cout << interface_file_clr_finish << endl << endl;
		}
		else if (operation == "printf")
		{
			if (memory == NULL)
			{
				cout << interface_err_init << endl << endl;
				continue;
			}
			cout << interface_file_output_start << endl;
			char c = 0;
			for (int i = 0; i < amount * 1024 / memory->Type; i++)
			{
				vget(*memory, i, c);
				cout << c;
				if (i == 64) cout << endl;
			}
			cout << endl;
		}
		else if (operation == "cls")
		{
			system("cls");
			cout << interface_hi << endl;
		}
		else if (operation == "exit")
		{
			if (memory != NULL)
			{
				cout << interface_saving_data << endl;
				saveMemory(*memory);
				cout << interface_saved_data << endl;
			}
			cout << interface_closing << endl << endl;
			break;
		}
	} while (1);
}

int main() 
{
	UI();

	return 0;
}