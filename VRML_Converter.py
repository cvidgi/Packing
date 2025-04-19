import csv


def csv_to_vrml(csv_file, vrml_file):
    with open(csv_file, 'r') as file:
        reader = csv.reader(file)
        boxes = list(reader)[1:]

    with open(vrml_file, 'w') as file:
        file.write("#VRML V2.0 utf8\n")

        for box in boxes:
            id, x1, y1, z1, x2, y2, z2 = map(float, box)
            x1 = x1 / 100
            y1 = y1 / 100
            z1 = z1 / 100
            x2 = x2 / 100
            y2 = y2 / 100
            z2 = z2 / 100

            # Находим центр и размеры коробки
            center_x = (x1 + x2) / 2
            center_y = (y1 + y2) / 2
            center_z = (z1 + z2) / 2

            size_x = abs(x2 - x1)
            size_y = abs(y2 - y1)
            size_z = abs(z2 - z1)

            # Записываем коробку в файл VRML
            file.write(f"Transform {{\n")
            file.write(f"  translation {center_x} {center_y} {center_z}\n")
            file.write(f"  children [\n")
            file.write(f"    Shape {{\n")
            file.write(f"      geometry Box {{ size {size_x} {size_y} {size_z} }}\n")
            file.write(f"      appearance Appearance {{\n")
            file.write(f"        material Material {{ diffuseColor 0.8 0.5 0.5 }}\n")  # Цвет коробки
            file.write(f"      }}\n")
            file.write(f"    }}\n")
            file.write(f"  ]\n")
            file.write(f"}}\n")


for i in range(1, 437):
    csv_to_vrml('Output/' + str(i) + '.csv', 'VRMLs/' + str(i) + '.wrl')

