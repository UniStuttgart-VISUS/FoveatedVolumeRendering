import numpy as np
import matplotlib.pyplot as plt


class window_plotting:
    def __init__(self, width=20, height=20):
        self.width = width
        self.height = height
        self.resolution = (self.width, self.height)
        self.window = np.ndarray(shape=self.resolution, dtype=tuple, order='C')
        self.reset_window(width, height)

    def reset_window(self, width, height):
        self.width = width
        self.height = height
        self.resolution = (self.width, self.height)
        self.window = np.ndarray(shape=self.resolution, dtype=tuple, order='C')

        # init window values
        for x in range(self.width):
            for y in range(self.height):
                self.window[x, y] = (x, y)

    def x_array(self):
        x = []
        for t_x in range(self.width):
            for t_y in range(self.height):
                x += [self.window[t_x, t_y][0]]
        return x

    def y_array(self):
        y = []
        for t_x in range(self.width):
            for t_y in range(self.height):
                y += [self.window[t_x, t_y][1]]
        return y

    def modify_tuples(self, func, *args):
        for x in range(self.width):
            for y in range(self.height):
                self.window[x, y] = func(self.window[x,y], self.width, self.height, *args)


def print_ftp(tp0, tp1):
    if tp0[0] == 0 and tp0[1] == 0:
        print(tp0, tp1)


def tp_mults(v, s):
    return v[0] * s, v[1] * s


def tp_mult(v0, v1):
    return v0[0] * v1[0], v0[1] * v1[1]


def tp_plus(v0, v1):
    return v0[0] + v1[0], v0[1] + v1[1]


def tp_minus(v0, v1):
    return v0[0] - v1[0], v0[1] - v1[1]


def tp_length(v):
    return np.sqrt(pow(v[0], 2) + pow(v[1], 2))


def tp_distance(v0,v1):
    return tp_length(tp_minus(v0, v1))


def harmonic_sinus(t, y, T, phi):
    omega = 2.0 * np.pi / T
    return y * np.sin(omega * t + phi)


def assembled_sinus(x, p):
    if x <= p:
        t = x
        y = p
        T = 4 * p
        if T == 0:
            T = 0.0000001
        phi = 0.0
        y_value = harmonic_sinus(t, y, T, phi)
    else:
        t = x
        y = 1 - p
        T = 4 * (1 - p)
        if T == 0:
            T = 0.0000001
        omega = 2.0 * np.pi / T
        phi = -omega
        y_value = harmonic_sinus(t, y, T, phi) + 1
    return y_value


def identity(tuple_element, width, height):
    return tuple_element


def npos(tuple_element, width, height, point):
    # print_ftp(tuple_element, point)
    conv_x = tuple_element[0] / width
    conv_y = tuple_element[1] / height
    new_x = assembled_sinus(conv_x, point[0])
    new_y = assembled_sinus(conv_y, point[1])

    return width * new_x, height * new_y


def log2d(tuple_element):
    return np.log(tuple_element[0]), np.log(tuple_element[1])


def sqrt2d(tuple_element):
    return np.sqrt(tuple_element[0]), np.sqrt(tuple_element[1])


def pow2d(tuple_element, exponent):
    return np.power(tuple_element[0], exponent), np.power(tuple_element[1], exponent)


def main():
    wp = window_plotting(40, 30)
    point = (0.7, 0.8)
    wp.modify_tuples(npos, point)

    axes = plt.gca()
    axes.set_xlim([-0.2, wp.width])
    axes.set_ylim([-0.2, wp.height])
    axes.set_aspect('equal')

    x_v = wp.x_array()
    y_v = wp.y_array()

    # print(x_v, y_v)

    plt.scatter(x_v, y_v, s=1)

    plt.show()
    return 0


if __name__ == "__main__":
    main()