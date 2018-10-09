import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import matplotlib as mplib
from ast import literal_eval
import matplotlib.gridspec as gridspec

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


# return four arrays, one for x_pos, one for y_pos, one for kernel times and one for paintGL() times
# and takes in an array of MsData objects
def axes_from_data(ms_data):
    return [ms_obj.mouse_position[0] for ms_obj in ms_data], [ms_obj.mouse_position[1] for ms_obj in ms_data], \
    [ms_obj.kernel_time for ms_obj in ms_data], [ms_obj.elapsed_time for ms_obj in ms_data]


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

    # set and adjust colorbar size to size of graph
    cbar = plt.colorbar(sc, fraction=0.027, pad=0.04)

    # set ms to colorbar
    cbar.ax.get_yaxis().labelpad = 15
    cbar.ax.set_ylabel('ms', rotation=270)

    # hide pixel axis
    cur_axes = plt.gca()
    cur_axes.axes.get_xaxis().set_visible(False)
    cur_axes.axes.get_yaxis().set_visible(False)

    '''fig, ax = plt.subplot(figsize=(10,10))
    ax.scatter(x, y, color=colors)

    cax, _ = mplib.colorbar.make_axes(ax)
    cbar = mplib.colorbar.ColorBase(cax, cmap=cmap, norm=normalize)'''

    save_path_name = data_path.split('.')[0] + '_heatmap'
    plt.savefig(save_path_name, dpi=600, bbox_inches='tight')  # save the figure, 600 dpi might be too high

    print 'fig saved to:', save_path_name

    plt.clf()  # clear figure so the method can be called again.


# Plots all values with the picture at the picutre_path at the same index
def plot_all_with_subplots_and_save(picture_path_s, values):
    #set font size
    font = {'family': 'normal',
            'weight': 'bold',
            'size': 30}

    mplib.rc('font', **font)

    total_min = min(min(values[0][2]), min(values[1][2]), min(values[2][2]), min(values[3][2]),
                    min(values[4][2]), min(values[5][2]))

    total_max = max(max(values[0][2]), max(values[1][2]), max(values[2][2]), max(values[3][2]),
                    max(values[4][2]), max(values[5][2]))

    if picture_path_s.__len__() != values.__len__() and picture_path_s.__len__() != 6:
        print('error_1')

    # fig, ax = plt.subplots(nrows=2, ncols=3)  # create subplots
    fig = plt.figure()
    gs = gridspec.GridSpec(nrows=2, ncols=3, wspace=0.0, hspace=0.0, figure=fig)

    color_map = mplib.cm.get_cmap('viridis')  # get cmap viridis

    #stx = ax[0][0]
    #mdcx = ax[0][1]
    #ddcx = ax[0][2]

    #stox = ax[1][0]
    #mdcox = ax[1][1]
    #ddcox = ax[1][2]

    for i in range(6):
        print i
        idx = [0,3,1,4,2,5]
        ax = fig.add_subplot(gs[idx[i]])
        image = mpimg.imread(picture_path_s[i])  # read image_data
        ax.imshow(image)  # plot image
        sc = ax.scatter(x=values[i][0], y=values[i][1], c=values[i][2], s=30,
                          vmin=total_min, vmax=total_max, cmap=color_map)
        ax.axis('off')

    fig.subplots_adjust(wspace=0, hspace=0,right=0.85)
    cbar_ax = fig.add_axes([0.85, 0.15, 0.01, 0.7])
    cbar = fig.colorbar(sc, cax=cbar_ax)
    cbar.ax.set_ylabel('ms', rotation=0)

    # fig.tight_layout()
    fig.show()

    # plt.savefig('C:/Users/bauer/Desktop/bvrc_rbn/Dokumente/Neue_Messungen/Chameleon/hm_wa.png', dpi=600, bbox_inches='tight')  # save the figure, 600 dpi might be too high

    pass


def plot_boxplots(values, labels):
    total_min = min(min(values[0][2]), min(values[1][2]), min(values[2][2]), min(values[3][2]),
                    min(values[4][2]), min(values[5][2]))

    total_max = max(max(values[0][2]), max(values[1][2]), max(values[2][2]), max(values[3][2]),
                    max(values[4][2]), max(values[5][2]))

    fig, ax = plt.subplots(figsize=(4, 3))
    ax.set_xlim(left=total_min-5, right=total_max+5)
    ax.set_xlabel('ms')
    ax.boxplot([v[2] for v in values], vert=False, labels=labels)
    fig.show()
    pass


def main_old():
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


def main_old_2():
    # path to the folder with the measurements
    msr_path = 'C:/Users/bauer/Desktop/bvrc_rbn/Dokumente/Neue_Messungen/Chameleon/'

    print 'Begin'

    st_values = axes_from_data(read_data(msr_path + 'ms_data_st_chameleon.txt'))
    st_ors_values = axes_from_data(read_data(msr_path + 'ms_data_st_rORS_chameleon.txt'))
    mdc_values = axes_from_data(read_data(msr_path + 'ms_data_mdc_chameleon.txt'))
    mdc_ors_values = axes_from_data(read_data(msr_path + 'ms_data_mdc_rORS_chameleon.txt'))
    ddc_values = axes_from_data(read_data(msr_path + 'ms_data_ddc_chameleon.txt'))
    ddc_ors_values = axes_from_data(read_data(msr_path + 'ms_data_ddc_rORS_chameleon.txt'))

    total_min = min(min(st_values[2]), min(st_ors_values[2]), min(mdc_values[2]), min(mdc_ors_values[2]),
                    min(ddc_values[2]), min(ddc_ors_values[2]))

    total_max = max(max(st_values[2]), max(st_ors_values[2]), max(mdc_values[2]), max(mdc_ors_values[2]),
                    max(ddc_values[2]), max(ddc_ors_values[2]))

    plot_and_save(msr_path + 'st.png', msr_path + 'ms_data_st_chameleon.txt', total_min, total_max)
    plot_and_save(msr_path + 'st_ors.png', msr_path + 'ms_data_st_rORS_chameleon.txt', total_min, total_max)
    plot_and_save(msr_path + 'mdc.png', msr_path + 'ms_data_mdc_chameleon.txt', total_min, total_max)
    plot_and_save(msr_path + 'mdc_ors.png', msr_path + 'ms_data_mdc_rORS_chameleon.txt', total_min, total_max)
    plot_and_save(msr_path + 'ddc.png', msr_path + 'ms_data_ddc_chameleon.txt', total_min, total_max)
    plot_and_save(msr_path + 'ddc_ors.png', msr_path + 'ms_data_ddc_rORS_chameleon.txt', total_min, total_max)

    print 'End'
    pass

msr_path = 'C:/Users/bauer/Desktop/bvrc_rbn/Dokumente/Neue_Messungen/Chameleon/'

st_values = axes_from_data(read_data(msr_path + 'ms_data_st_chameleon.txt'))
st_ors_values = axes_from_data(read_data(msr_path + 'ms_data_st_rORS_chameleon.txt'))
mdc_values = axes_from_data(read_data(msr_path + 'ms_data_mdc_chameleon.txt'))
mdc_ors_values = axes_from_data(read_data(msr_path + 'ms_data_mdc_rORS_chameleon.txt'))
ddc_values = axes_from_data(read_data(msr_path + 'ms_data_ddc_chameleon.txt'))
ddc_ors_values = axes_from_data(read_data(msr_path + 'ms_data_ddc_rORS_chameleon.txt'))

vs = [st_values, st_ors_values, mdc_values, mdc_ors_values, ddc_values, ddc_ors_values]
pps = [msr_path + 'st.png', msr_path + 'st_ors.png', msr_path + 'mdc.png',
                     msr_path + 'mdc_ors.png', msr_path + 'ddc.png', msr_path + 'ddc_ors.png']


def print_averages():
    print 'Kernel averages:'
    for i, v in enumerate(vs):
        print np.average(v[2])


def print_overhead():
    print 'Overhead average:'
    for i, v in enumerate(vs):
        print np.average([v[3][j] - v[2][j] for j, _ in enumerate(v[3])])


def print_sums():
    print 'total time paintGL():'
    for i, v in enumerate(vs):
        print np.sum(v[3])


def print_varianz():
    print 'varianz der kernel zeiten:'
    for i, v in enumerate(vs):
        print np.var(v[2])


def print_all():
    print_averages()
    print_overhead()
    print_sums()
    print_varianz()


def main():
    # path to the folder with the measurements


    print 'Begin'

    # plot_all_with_subplots_and_save(picture_paths, values)

    labels = ['Standard ohne v. S.a.r.', 'Standard mit v. S.a.r.', 'MDC ohne v. S.a.r.',
              'MDC mit v. S.a.r', 'DDC ohne v. S.a.r', 'DDC mit v. S.a.r.']

    # plot_boxplots(vs, labels)

    print_all()

    print 'End'
    pass


if __name__ == '__main__':
    main()