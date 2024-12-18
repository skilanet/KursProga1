Шаблон структуры данных – дерево. Конечная вершина дерева содержит статический массив указателей на объекты
(ограниченный NULL-указателем), промежуточная вершина содержит счетчик вершин дерева и два указателя на правое и
левое поддерево. Значения в дереве не упорядочены. Естественная нумерация значений производится путем обхода дерева
по принципу “левое поддерево – вершина – правое поддерево”. Если при включении указателя в найденный массив последний
переполняется, он становится промежуточной вершиной.

Для заданной двухуровневой структуры данных, содержащей указатели на объекты (или сами объекты) - параметры шаблона,
разработать полный набор операций (добавление, удаление, включение и извлечение по логическому номеру, сортировка,
включение с сохранением порядка, загрузка и сохранение строк в бинарном файле, балансировка – выравнивание размерностей
структур данных нижнего уровня). Предполагается, что операции сравнения хранимых объектов переопределены стандартным
образом (в виде операций <, > и т.д.). Программа должна использовать шаблонный класс с объектами-строками и
реализовывать указанные выше действия над текстом любого объема, загружаемого из файла. Результат по желанию можно
выгрузить в текстовый файл.