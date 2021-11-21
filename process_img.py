from PIL import Image

pixel_to_cell = {
    (0,0,0): 0b10000000, #EMPTY
    (255,255,0): 0b01000000, #COPPER
    (255,0,0): 0b00100000, #TAIL
    (0,0,255): 0b00000001 #HEAD
}

IMG_FOLDER = r'.\img\\'
CSV_FOLDER = r'.\csv\\'
CSV_EXTENSION = r'csv'

def to_csv_extension(file_name):
    temp = file_name.split('.')
    temp[-1] = CSV_EXTENSION
    return '.'.join(temp)

if __name__ == '__main__':

    img_name = r'diode.png'

    img = Image.open(IMG_FOLDER + img_name)

    pixels = img.getdata()
    cells = map(lambda p: pixel_to_cell[p], pixels)

    with open(CSV_FOLDER + to_csv_extension(img_name), 'w') as csv_file:
        csv_file.write(f'{img.width} {img.height},\n')
        for cell in cells:
            csv_file.write(f'{cell} ')
