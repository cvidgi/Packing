Задача:
Имеется набор коробок, необходимо его как можно плотнее уложить в паллету размерами 800 x 1200, минимизируя высоту (А значит и объём).
Предложенный алгоритм укладывает коробки в паллету, максимизируя плотность укладки, с помощью генетического алгоритма. Поворачивать коробку разрешено только меняя ширину и длину местами.

Гены и первая популяция:
Структура гена состоит из box_id - id коробки, которую описывает ген; index - индекс, на котором мы хотим поставить коробку (Про расстановку в конце), к которой относится ген; type - номер угла, ближе к которому мы хотим поставить коробку: к (0, 0, 0) или (0, W, L) и rotation - ориентации коробки (в случае 0 перебираются оба варианта и выбирается лучший). Изначально параметры index генов одного генотипа составляют случайную перестановку, rotation = 0, type случайный. Кроме того имеется несколько специально подобранных генов. 

Скрещивание:
В скрещивании двух генотипов для каждого гена потомка равновероятно выбирается ген одного из родителей (Кроме параметра index).
Перестановка параметров index равновероятно выбирается из перестановок родителей. Это сделано для сохранения корректности перестановки потомка.

Мутации:
Часть генов генотипа случайно меняется.

Расстановка:
Расстановка в данной версии алгоритма выполняется самым простым в плане реализации способом. Имеется 2 сета углов - в первом находятся углы, которые могут быть интересны для type = 0, во втором для type = 1. Для очередной коробки перебираются углы из подходящего сета, выполняется попытка поставить в этот угол коробку и проверка, что такое размещение корректно (Простой проверкой пустоты пересечения со всеми уже поставленными коробками). В силу упорядоченности сета, первый подходящий угол будет ближайшим для (0, 0, 0)/(0, W, L).
