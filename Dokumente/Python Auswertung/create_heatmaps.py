import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import matplotlib as mplib
from ast import literal_eval

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


# return three arrays, one for x_pos, one for y_pos and one for kernel time and takes in an array of MsData objects
def axes_from_data(ms_data):
    return [ms_obj.mouse_position[0] for ms_obj in ms_data], [ms_obj.mouse_position[1] for ms_obj in ms_data], \
    [ms_obj.kernel_time for ms_obj in ms_data]


# plots the picture as background and the data read from a ms data file on top of it as a colormap
# saves the plot the clears it
def plot_and_save(picture_path, data_path, min_sc, max_sc):
    print 'plot_and_save: ', picture_path, data_path

    image = mpimg.imread(picture_path)  # read image_data
    plt.imshow(image)  # plot the image

    # plot the data
    x, y, z = axes_from_data(read_data(data_path))  # get data

    cmap = mplib.cm.get_cmap('viridis')  # get cmap viridis
    # normalize = mplib.colors.Normalize(vmin=min(z), vmax=max(z))  # create a normalization function for z
    # colors = [cmap(normalize(value)) for value in z]  # use normalize function to normalize and map z values to color

    # sc = plt.scatter(x=x, y=y, c=z, s=10, vmin=min(z), vmax=max(z), cmap=cmap)

    # use min max for better comparison
    sc = plt.scatter(x=x, y=y, c=z, s=10, vmin=min_sc, vmax=max_sc, cmap=cmap)
    plt.colorbar(sc)

    '''fig, ax = plt.subplot(figsize=(10,10))
    ax.scatter(x, y, color=colors)

    cax, _ = mplib.colorbar.make_axes(ax)
    cbar = mplib.colorbar.ColorBase(cax, cmap=cmap, norm=normalize)'''

    save_path_name = data_path.split('.')[0] + '_heatmap'
    plt.savefig(save_path_name, dpi=300)  # save the figure

    print 'fig saved to:', save_path_name

    plt.clf()  # clear figure so the method can be called again.


def main():
    msr_path = 'C:/Users/bauer/Desktop/bvrc_rbn/Dokumente/Messungen/'
    dirs = ['bonsai', 'hoatzin', 'E_1353', 'chameleon']
    mmsts = ['Messung_mit_Standard_Transferfunktion', 'Messungen_mit_Transferfunktion_1',
             'Messungen_mit_Transferfunktion_2']

    print 'Begin'

    # bonsai
    for dir in dirs:
        print 'current dir:', dir
        if dir == 'bonsai' or dir == 'E_1353' or dir == 'chameleon':
            for mmst in mmsts:
                print 'current m-type:', mmst
                path = msr_path + dir + '/' + mmst + '/'  # path to the folder the measurements are in
                values1 = axes_from_data(read_data(path + 'ms_data_ddc.txt'))  # first
                values2 = axes_from_data(read_data(path + 'ms_data_mdc.txt'))  # second

                min_v = min(min(values1[2]), min(values2[2]))
                max_v = max(max(values1[2]), max(values2[2]))

                plot_and_save(path + 'image_st.png', path + 'ms_data_ddc.txt', min_v, max_v)
                plot_and_save(path + 'image_st.png', path + 'ms_data_mdc.txt', min_v, max_v)

        if dir == 'hoatzin':
            for mmst in mmsts:
                print 'current m-type:', mmst
                if mmst == mmsts[0]:
                    continue  # skip first one because there is nothing measured
                path = msr_path + '/' + dir + '/' + mmst + '/'  # path to the folder the measurements are in
                values1 = axes_from_data(read_data(path + 'ms_data_ddc.txt'))  # first
                values2 = axes_from_data(read_data(path + 'ms_data_mdc.txt'))  # second

                min_v = min(min(values1[2]), min(values2[2]))
                max_v = max(max(values1[2]), max(values2[2]))

                plot_and_save(path + 'image_st.png', path + 'ms_data_ddc.txt', min_v, max_v)
                plot_and_save(path + 'image_st.png', path + 'ms_data_mdc.txt', min_v, max_v)

    print 'End'
    return


if __name__ == '__main__':
    main()