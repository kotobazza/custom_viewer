import os
import json
import xml.etree.ElementTree as ET
import zipfile
import ctypes
import platform
import psutil
from colorama import Fore, Style, init
from prettytable import PrettyTable

init(autoreset=True)

class User:
    def __init__(self, name, age):
        self.name = name
        self.age = age

# Класс для работы с файлами
class FileHandler:
    def create_file(self, filename, content):
        with open(filename, 'w') as f:
            f.write(content)
        print(f"\n{Fore.GREEN}Файл '{filename}' создан и записан.{Style.RESET_ALL}")

    def read_file(self, filename):
        with open(filename, 'r') as f:
            content = f.read()
        print(f"\n{Fore.CYAN}Содержимое файла '{filename}':{Style.RESET_ALL}")
        print(f"{Fore.YELLOW}{'=' * 40}{Style.RESET_ALL}")  # Разделитель
        print(content)
        print(f"{Fore.YELLOW}{'=' * 40}{Style.RESET_ALL}")  # Разделитель

    def delete_file(self, filename):
        confirmation = input(f"{Fore.RED}Вы уверены, что хотите удалить файл '{filename}'? (да/нет): {Style.RESET_ALL}")
        if confirmation.lower() in ['да', 'д', 'yes', 'y']:
            os.remove(filename)
            print(f"\n{Fore.RED}Файл '{filename}' удалён.{Style.RESET_ALL}")
        elif confirmation.lower() in ['нет', 'н', 'no', 'n']:
            print(f"\n{Fore.YELLOW}Удаление файла '{filename}' отменено.{Style.RESET_ALL}")
        else:
            print(f"\n{Fore.YELLOW}Некорректный ввод. Удаление файла '{filename}' отменено.{Style.RESET_ALL}")



class JSONHandler:
    @staticmethod
    def create_json_file(filename, user):
        with open(filename, 'w', encoding='utf-8') as json_file:
            json.dump(user, json_file, default=lambda x: x.__dict__, ensure_ascii=False, indent=4)
        print(f"Файл '{filename}' успешно создан.")

    @staticmethod
    def read_json_file(filename):
        if os.path.exists(filename):
            with open(filename, 'r', encoding='utf-8') as json_file:
                data = json.load(json_file)
                print("Содержимое файла:")
                print(json.dumps(data, ensure_ascii=False, indent=4))
        else:
            print(f"Файл '{filename}' не найден.")

    @staticmethod
    def delete_json_file(filename):
        if os.path.exists(filename):
            os.remove(filename)
            print(f"Файл '{filename}' успешно удален.")
        else:
            print(f"Файл '{filename}' не найден.")

def to_json(obj):
    if isinstance(obj, User):
        result = obj.__dict__
        result["className"] = obj.__class__.__name__
        return result


# Класс для работы с XML
class XMLHandler:
    def create_xml_file(self, filename, root_element):
        root = ET.Element(root_element)
        tree = ET.ElementTree(root)
        tree.write(filename)
        print(f"\nXML файл '{filename}' создан.")

    def add_data_to_xml(self, filename, element_name, data):
        tree = ET.parse(filename)
        root = tree.getroot()
        new_element = ET.SubElement(root, element_name)
        new_element.text = data
        tree.write(filename)
        print(f"\nДанные добавлены в XML файл '{filename}'.")

    def read_xml_file(self, filename):
        tree = ET.parse(filename)
        root = tree.getroot()
        print(f"\nСодержимое XML файла '{filename}':")
        for elem in root:
            print(f"{elem.tag}: {elem.text}")

def correct_size(bts, ending='iB'):
    size = 1024
    for item in ["", "K", "M", "G", "T", "P"]:
        if bts < size:
            return f"{bts:.2f}{item}{ending}"
        bts /= size

def list_drives():
    print("\nЛогические диски:")
    collect_info_dict = dict()
    collect_info_dict['info'] = dict()
    for partition in psutil.disk_partitions():
        try:
            partition_usage = psutil.disk_usage(partition.mountpoint)
        except PermissionError:
            continue
        if 'disk_info' not in collect_info_dict['info']:
            collect_info_dict['info']['disk_info'] = dict()
        if f"'device': {partition.device}" not in collect_info_dict['info']['disk_info']:
            collect_info_dict['info']['disk_info'][partition.device] = dict()
            collect_info_dict['info']['disk_info'][partition.device] = {'file_system': partition.fstype,
                                                                        'size_total': correct_size(
                                                                            partition_usage.total),
                                                                        'size_used': correct_size(
                                                                            partition_usage.used),
                                                                        'size_free': correct_size(
                                                                            partition_usage.free),
                                                                        'percent':
                                                                            f'{partition_usage.percent}'}
    return collect_info_dict

def print_info(dict_info):
    for item in dict_info['info']:
        if item == "disk_info":
            # Создаем таблицу
            table = PrettyTable()
            table.field_names = ["Имя диска", "Файловая система", "Объем диска", "Занято", "Свободно", "Заполненность"]

            for elem in dict_info['info'][item]:
                # Добавляем строки в таблицу
                table.add_row([
                    elem,
                    dict_info['info'][item][elem]['file_system'],
                    dict_info['info'][item][elem]['size_total'],
                    dict_info['info'][item][elem]['size_used'],
                    dict_info['info'][item][elem]['size_free'],
                    f"{dict_info['info'][item][elem]['percent']}%"
                ])

            # Печатаем таблицу
            print(table)

# Функция для работы с ZIP-архивами


def zip_file_func(filenames, zip_filename):
    with zipfile.ZipFile(zip_filename, 'w') as zipf:
        for filename in filenames:
            zipf.write(filename)
            print(f"Файл '{filename}' добавлен в архив '{zip_filename}'.")
    
    # Выводим список всех добавленных файлов
    print("\nВсе файлы были успешно добавлены в архив:")
    print("=" * 50)
    for file in filenames:
        print(f" - {file}")
    print("=" * 50)
    print(f"\nАрхив '{zip_filename}' создан.")
    
def unzip_file(zip_filename):
    with zipfile.ZipFile(zip_filename, 'r') as zipf:
        # Получаем список файлов в архиве
        file_list = zipf.namelist()
        zipf.extractall()  # Извлекаем все файлы
        print(f"\nФайлы из архива '{zip_filename}' разархивированы:")
        print("=" * 50)
        for file in file_list:
            print(f"    + {file}")
        print("=" * 50)

def file_menu(file_handler):
    while True:
        print("\n" + Fore.YELLOW + "--- Меню работы с файлами ---" + Style.RESET_ALL)
        print("1. Создать файл")
        print("2. Прочитать файл")
        print("3. Удалить файл")
        print("0. Вернуться в главное меню")

        choice = input("Введите номер действия: ")

        if choice == '1':
            filename = input("Введите имя файла: ")
            user_input = input("Введите строку для записи в файл: ")
            file_handler.create_file(filename, user_input)
        elif choice == '2':
            filename = input("Введите имя файла для чтения: ")
            if os.path.exists(filename):
                file_handler.read_file(filename)
            else:
                print(f"{Fore.RED}Файл '{filename}' не найден.{Style.RESET_ALL}")
        elif choice == '3':
            filename = input("Введите имя файла для удаления: ")
            if os.path.exists(filename):
                file_handler.delete_file(filename)
            else:
                print(f"{Fore.RED}Файл '{filename}' не найден.{Style.RESET_ALL}")
        elif choice == '0':
            break
        else:
            print(f"{Fore.RED}Неверный выбор, попробуйте снова.{Style.RESET_ALL}")

def json_menu(json_handler):
    
    while True:
        print("\n--- Меню работы с JSON ---")
        print("1. Создать JSON файл")
        print("2. Прочитать JSON файл")
        print("3. Удалить JSON файл")
        print("0. Вернуться в главное меню")

        choice = input("Введите номер действия: ")

        if choice == '1':
            json_filename = input("Введите имя файла: ")
            if not json_filename.endswith('.json'):
                json_filename += '.json'
            name = input("Введите имя: ")
            age = input("Введите возраст: ")
            if(not age.isdigit()):
                while(not age.isdigit()):
                    print(f"Неправильное значение для поля Возраст: {age}")
                    age = input("Введите возраст: ")
            user = User(name, age)  # Создаем объект User
            json_handler.create_json_file(json_filename, user)  # Сериализуем объект в JSON
        elif choice == '2':
            json_filename = input("Введите имя файла: ")
            if not json_filename.endswith('.json'):
                json_filename += '.json'
            json_handler.read_json_file(json_filename)
        elif choice == '3':
            json_filename = input("Введите имя файла: ")
            if not json_filename.endswith('.json'):
                json_filename += '.json'
            json_handler.delete_json_file(json_filename)
        elif choice == '0':
            break
        else:
            print("Неверный выбор, попробуйте снова.")

def xml_menu(xml_handler, file_handler):
    while True:
        print("\n--- Меню работы с XML ---")
        print("1. Создать XML файл")
        print("2. Добавить данные в XML файл")
        print("3. Прочитать XML файл")
        print("4. Удалить XML файл")
        print("0. Вернуться в главное меню")

        choice = input("Введите номер действия: ")
        xml_filename = "data.xml"

        if choice == '1':
            xml_handler.create_xml_file(xml_filename, "root")
        elif choice == '2':
            xml_data = input("Введите данные для добавления в XML: ")
            xml_handler.add_data_to_xml(xml_filename, "data", xml_data)
        elif choice == '3':
            xml_handler.read_xml_file(xml_filename)
        elif choice == '4':
            file_handler.delete_file(xml_filename)
        elif choice == '0':
            break
        else:
            print("Неверный выбор, попробуйте снова.")


def zip_menu(file_handler):
    while True:
        print("\n--- Меню работы с ZIP-архивами ---")
        print("1. Создать ZIP архив")
        print("2. Разархивировать ZIP архив")
        print("0. Вернуться в главное меню")

        choice = input("Введите номер действия: ")

        if choice == '1':
            zip_filename = input("Введите имя для создаваемого ZIP архива (например, 'archive.zip'): ")
            if not zip_filename.endswith('.zip'):
                zip_filename += '.zip'  # Добавляем расширение .zip, если его нет
            
            filenames = []
            while True:
                file_to_zip = input("Введите имя файла для добавления в архив (или 'стоп' для завершения): ")
                if file_to_zip.lower() == 'стоп':
                    break
                if os.path.isfile(file_to_zip):
                    file_handler.create_file(file_to_zip, "Это тестовый файл для архивации.")
                    filenames.append(file_to_zip)
                else:
                    print(f"Файл '{file_to_zip}' не найден. Пожалуйста, попробуйте снова.")

            if filenames:
                zip_file_func(filenames, zip_filename)
                zip_size = os.path.getsize(zip_filename) / (1024 ** 2)  # Размер в МБ
                print(f"Размер архива '{zip_filename}': {zip_size:.2f} МБ")
                for file in filenames:
                    file_handler.delete_file(file)
            else:
                print("Не было добавлено ни одного файла в архив.")
                
        elif choice == '2':
            zip_filename = input("Введите имя ZIP архива для разархивирования: ")
            unzip_file(zip_filename)
        elif choice == '0':
            break
        else:
            print("Неверный выбор, попробуйте снова.")

def main():
    file_handler = FileHandler()
    json_handler = JSONHandler()
    xml_handler = XMLHandler()

    while True:
        print("\n" + Fore.YELLOW + "--- Главное меню ---" + Style.RESET_ALL)
        print("1. Вывести информацию о логических дисках")
        print("2. Работа с файлами")
        print("3. Работа с JSON")
        print("4. Работа с XML")
        print("5. Работа с ZIP-архивами")
        print("0. Выход")

        choice = input("Введите номер действия: ")

        if choice == '1':
            dict_info = list_drives() 
            print_info(dict_info)
        elif choice == '2':
            file_menu(file_handler)
        elif choice == '3':
            json_menu(json_handler)
        elif choice == '4':
            xml_menu(xml_handler, file_handler)
        elif choice == '5':
            zip_menu(file_handler)
        elif choice == '0':
            print(f"{Fore.GREEN}Выход из программы.{Style.RESET_ALL}")
            break
        else:
            print(f"{Fore.RED}Неверный выбор, попробуйте снова.{Style.RESET_ALL}")

if __name__ == "__main__":
    main()