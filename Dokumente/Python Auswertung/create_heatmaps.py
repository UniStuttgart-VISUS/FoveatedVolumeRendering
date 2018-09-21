import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from ast import literal_eval

msr_path = 'C:/Users/bauer/Desktop/bvrc_rbn/Dokumente/Messungen/'
img_path = msr_path + 'bonsai/Messung_mit_Standard_Transferfunktion/image_st.png'
ms_bonsai_st_path = msr_path + 'bonsai/Messung_mit_Standard_Transferfunktion/ms_data_st.txt'


# Data Object to hold measured data
class MsData():
    def __init__(self, system_time, elapsed_time, kernel_time, mouse_position, grid_position, single_measurement):
        self.system_time = system_time  # String
        self.elapsed_time = elapsed_time  # Integer
        self.kernel_time = kernel_time  # Float
        self.mouse_position = mouse_position  # Tuple of Integer
        self.grid_position = grid_position  # Tuple of Integer
        self.single_measurement = single_measurement  # Boolean


# returns the content of a file
def read_file(file_name):
    with open(file_name, 'r') as f:
        data = f.read()
    return data


# reads measured data and returns Array of MsData Objects containing the data
def read_data(ms_data_path):
    data = read_file(ms_data_path) # read content of file
    data = data.splitlines(False) # data is spitted to lines
    ms_data = []
    for i, line in enumerate(data):
        if i < 8 or i >= data.__len__()-3:
            continue  # skip first 8 lines and last two line ((0,0) and EOM)
        line = line.split('\t')  # split line with \t to get each data
        # print i, line[0], int(line[1]), float(line[2]), literal_eval(line[3]), literal_eval(line[4]), line[5]
        ms_data.append(MsData(line[0], int(line[1]), float(line[2]), literal_eval(line[3]),
                            literal_eval(line[4]), True if int(line[5]) != 1 else False))
    return ms_data


# return three arrays, one for x_pos, one for y_pos and one for kernel time
def axes_from_data(ms_data):
    return [ms_obj.mouse_position[0] for ms_obj in ms_data], [ms_obj.mouse_position[1] for ms_obj in ms_data], \
    [ms_obj.kernel_time for ms_obj in ms_data]


def main():
    # img = mpimg.imread(img_path)
    # imgplot = plt.imshow(img)
    print axes_from_data(read_data(ms_bonsai_st_path))
    pass


if __name__ == '__main__':
    main()